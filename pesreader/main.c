#include <stdio.h>
#include <ctype.h>
#include "arg.h"
#include "../uncomp/uncomp.h"
#include "bmp.h"

void read_pes(unsigned char *buf);
void hex_dump(void *data, int size);

struct s_pes
{
	char name[5];
	unsigned int offset;
};

struct s_headerimg
{
	unsigned short width;
	unsigned short height;
	unsigned short unknow1;
	unsigned short unknow2;
	unsigned short posx;
	unsigned short posy;
	unsigned char pad[4];
};

int checkpacktype(struct s_conf *conf)
{
	unsigned int uncomp_size;

	if (*(conf->fpes.bMap) != 0x82)
	{
		fprintf(stderr, "[-] Wrong pack type\n");
		return 0;
	}
	uncomp_size = (*(conf->fpes.bMap + 1)) | (*(conf->fpes.bMap + 2) << 0x8) | (*(conf->fpes.bMap + 3) << 0x10);
	if (uncomp_size & 0xF)
		uncomp_size = (uncomp_size >> 4) + 1;
	else
		uncomp_size >>= 4;
	if (!uncomp_size)
	{
		fprintf(stderr, "[-] Wrong pack type\n");
		return 0;
	}
	return 1;
}

/* seg037:000A ; int __cdecl __far ComputeRealSize(int OffsetData, int SegData) */

unsigned int computenewsize(unsigned char *buf, struct s_pes *p, unsigned int nb_entry)
{
	unsigned int i;
	int new_size = 0;
	unsigned height, width;
	unsigned char *pbuf = NULL;

	new_size += nb_entry * 8;
	new_size += 0x10;
	for (i = 0; i < nb_entry; i++)
	{
		pbuf = buf + p[i].offset;
		width = *(unsigned short*)pbuf;
		height = *(unsigned short*)(pbuf + 2);
		new_size += width * height * 8;
		new_size += 0x10;
	}
	new_size += 0x10;
	return new_size;
}


void dump_to_file(char *filename, unsigned char *buf, size_t size)
{
	int fd;

        fd = open(filename, O_WRONLY | O_CREAT, 0644);
        if (fd == -1)
        {
                perror("open()");
                exit(0);
        }
	write(fd, buf, size);
	close(fd);
}

/* seg012:7582 ; int __cdecl __far sub_35FA2(int OffsetUncompData, int SegUncompData, int OffsetResData, int SegResData) */

void expandpes(unsigned char *buf, struct s_pes *p, unsigned int nb_entry)
{
	unsigned int new_size = 0;
	unsigned char *buf_out = NULL;
	unsigned char *sbuf_out = NULL;
	unsigned char *res_out = NULL;
	unsigned char *offset_buf = NULL;
	unsigned char *data_buf = NULL;
	unsigned int offset, i, j, k, l;
	char actual;
	struct s_headerimg himg;

	/* 	+4 : Size file
	   	+2 : nb_entry
		+nb_entry * 8 : name[4] + offset[4]
	*/
	new_size = computenewsize(buf + 4 + 2 + nb_entry * 8, p, nb_entry);
	if (!(buf_out = malloc(sizeof (char) * new_size)))
	{
		perror("malloc()");
		return;
	}

	/* save ptr for output buffer */
	res_out = buf_out;
	memset(buf_out, 0, sizeof (char) * new_size);

	/* update new size */
	*(unsigned int*)buf_out = new_size;
	buf += 4;
	buf_out += 4;

	/* update number of resources entry */
	*(unsigned short*)buf_out = *(unsigned short*)buf;
	buf_out += 2;
	buf += 2;

	/* copy name of resources */
	memcpy(buf_out, buf, (nb_entry) * 4);
	buf += (nb_entry * 4);
	buf_out += (nb_entry * 4);

	/* save ptr to offset table */
	offset_buf = buf_out;
	buf += (nb_entry * 4);
	buf_out += (nb_entry * 4);
	offset = 0;
	for (i = 0; i < nb_entry; i++)
	{
		// Update offset in offset table
		*(unsigned int*)(offset_buf + 4 * i) = offset;
		buf_out = (offset_buf + 4 * nb_entry) + offset;

		// Get Actual image data
		data_buf = buf + p[i].offset;


		/* get image header */
		memcpy(&himg, data_buf, sizeof (struct s_headerimg));
		
		/* Update next Offset */
		offset = offset + (himg.width * himg.height * 8 + 0x10);

		/* save image header */
		memcpy(buf_out, data_buf, 0x10);
	
		/* update width */
		*(unsigned short*)buf_out = himg.width * 0x8;

		buf_out += 0x10;
		data_buf += 0x10;
	
		sbuf_out = buf_out;
		for (j = 0; j < 4; j++)
		{
			if (himg.pad[j] & 0xF)
			{
				buf_out = sbuf_out;
				for (k = 0; k < (unsigned int)(himg.width * himg.height); k++)
				{
					actual = *data_buf++;
					for (l = 0; l < 8; l++)
					{
						if (actual & 0x80)
							*buf_out |= (himg.pad[j] & 0xF);
						buf_out++;
						actual <<= 1;
					}
				}
			}
			else
				break;
		}
	}
	hex_dump(res_out, new_size);
	dump_to_file("real.pes", res_out, new_size);
	read_pes(res_out);
}

/* seg012:76D6 ; int __cdecl __far sub_360F6(int OffsetUncompData, int SegUncompData, int OffsetResData, int SegResData) */

void unflip(unsigned char *buf, unsigned int size)
{
	unsigned int nb_entry;
	unsigned char *abuf = NULL;
	unsigned int i, j, x, y;
	struct s_pes *p = NULL;
	unsigned char *pout = NULL;
	unsigned char *sbuf = NULL;
	unsigned char val = 0;
	struct s_headerimg *himg;

	(void)size;
	sbuf = buf;
	/* forgive size file */
	buf = buf + 4;
	nb_entry = *(unsigned short*)buf;
	buf = buf + 2;
	p = malloc(sizeof (struct s_pes) * nb_entry);
	if (!p)
	{
		perror("malloc()");
		return;
	}
	/* fill name and offset of each entry */
	for (i = 0; i < nb_entry; i++)
	{
		memcpy(p[i].name, buf + (i * 4), 4);
		memcpy(&p[i].offset, (char*)(buf + (i * 4) + (nb_entry * 4)), 4);
	}
	/* buf = data offset */
	buf += (nb_entry * 8);
	for (i = 0; i < nb_entry; i++)
	{
		abuf = buf + p[i].offset;
		if (!(*(abuf + 0xF) & 0xF0))
		{
			val = *(abuf + 0xE) >> 4;
			if ((val & 0x0F))
			{	
				himg = (struct s_headerimg*)abuf;
				abuf += 0x10;
				if (!(pout = malloc(sizeof (char) * himg->width * himg->height)))
				{
					perror("malloc");
					return;
				}
				for (j = 0; j < 4; j++)
				{
					if ((val & 0x1))
					{
						for (y = 0; y < himg->height; y++)
						{
							for (x = 0; x < himg->width; x++)
							{
								*pout++ = abuf[y + x * himg->height];
							}
						}
						pout = pout - (himg->width * himg->height);
						/* update bit flipped */
						memcpy(abuf, pout, (himg->width * himg->height));
						abuf = abuf + (himg->width * himg->height);	
					}
					val >>= 1;
				}
				free(pout);
			}
		}	
	}
	expandpes(sbuf, p, nb_entry);
}

void readtobmp(unsigned char *buf, char *name)
{
  	BMP b;
  	char output_name[500];

	b.width = *(unsigned short*)buf;
	buf += 2;
	b.height = *(unsigned short*)buf;
	buf += 2;
	buf += 12;
	b.data = buf;
	strcpy(output_name, "./extract/");
  	strcat(output_name, name);
  	strcat(output_name, ".bmp");
  	bmp_save(&b, output_name);
}

void read_pes(unsigned char *buf)
{
	struct s_pes *p = NULL;
	unsigned short nb_entry = 0;
	unsigned int i;

	buf += 4;
	nb_entry = *(unsigned short*)buf;
	buf += 2;
	printf("nb_entry = %X\n", nb_entry);
	p = malloc(sizeof (struct s_pes) * nb_entry);
	if (!p)
	{
		perror("malloc()");
		return;
	}
	for (i = 0; i < nb_entry; i++)
	{
		memcpy(p[i].name, buf + (i * 4), 4);
		memcpy(&p[i].offset, (char*)(buf + (i * 4) + (nb_entry * 4)), 4);
	}
	buf += (nb_entry * 8);
	for (i = 0; i < nb_entry; i++)
	{
		printf("Entry : %d, Name : %s, Offset %08X, Size : %08X\n", i, p[i].name, p[i].offset, p[i + 1].offset - p[i].offset);
		hex_dump(buf + p[i].offset, p[i + 1].offset - p[i].offset);
		readtobmp(buf + p[i].offset, p[i].name);
	}
}

int main(int argc, char **argv)
{
        struct s_conf conf = {0};

        parse_opt(argc, argv, &conf);
        check_opt(&conf, argv[0]);
	//if (checkpacktype(&conf))
	//{
		unflip(conf.fpes.bMap, conf.fpes.sb.st_size);

	//}
        clean(&conf);
        return 0;
}

void hex_dump(void *data, int size)
{
    unsigned char *p = (unsigned char*)data;
    unsigned char c;
    int n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[16 * 3 + 5] = {0};
    char charstr[16 * 1 + 5] = {0};

    for(n = 1; n <= size; n++)
    {
        if (n % 16 == 1)
        {
                snprintf(addrstr, sizeof(addrstr), "%.4x",
                    ((unsigned int)p - (unsigned int)data));
        }
        c = *p;
        if (isprint(c) == 0)
        {
            c = '.';
        }
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr) - strlen(hexstr) - 1);
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr) - strlen(charstr) - 1);
        if (n % 16 == 0)
        {
            printf("[%8.8s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        }
        else if (n % 8 == 0)
        {
            strncat(hexstr, "  ", sizeof(hexstr) - strlen(hexstr) - 1);
            strncat(charstr, " ", sizeof(charstr) - strlen(charstr) - 1);
        }
        p++;
    }
    if (strlen(hexstr) > 0)
    {
        printf("[%8.8s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}

/*void decomp(struct s_conf *conf)
{
	unsigned char *buf = NULL;
	unsigned int final_len = 0;
	struct s_comp chuff;

	buf = conf->fpes.bMap;
	buf++;
	final_len = (*buf) | (*(buf + 1) << 0x8) | (*(buf + 2) << 0x10);
	buf += 3;
	printf("final_len = %X\n", final_len);
	memcpy(&chuff, buf, 0x4);
	printf("TYPE = %X\n", chuff.type);
	printf("SIZE = %X\n", chuff.size);
        huff(buf, conf->fpes.sb.st_size, &chuff);
        if (chuff.tree)
        {
                uncomp(&chuff, (buf + conf->fpes.sb.st_size - 4));
                if (!chuff.buf_out)
                {
                        fprintf(stderr, "[-] Error decompression !\n");
                        return;
                }
        }
        else
        {
                fprintf(stderr, "[-] No Huffman tree !\n");
                return;
        }
	hex_dump(chuff.buf_out, chuff.size);
	rle(chuff.buf_out, chuff.size);
}*/

