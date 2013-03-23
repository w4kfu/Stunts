#include <stdio.h>
#include <ctype.h>
#include "arg.h"
#include "../uncomp/uncomp.h"

void hex_dump(void *data, int size);

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

void rle(unsigned char *buf)
{
	unsigned int size = 0;
	unsigned int len = 0;

	size = (*buf) | (*(buf + 1) << 0x8) | (*(buf + 2) << 0x10);
	buf += 3;
	printf("Size = %X\n", size);
	buf++;
	len = (*buf);
	printf("Len = %X\n", len);

}

void decomp(struct s_conf *conf)
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

}

int main(int argc, char **argv)
{
        struct s_conf conf = {0};

        parse_opt(argc, argv, &conf);
        check_opt(&conf, argv[0]);
	if (checkpacktype(&conf))
	{
		decomp(&conf);

	}
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
                    ((unsigned int)p-(unsigned int)data));
        }
        c = *p;
        if (isprint(c) == 0)
        {
            c = '.';
        }
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);
        if (n % 16 == 0)
        {
            printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        }
        else if (n % 8 == 0)
        {
            strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
            strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
        }
        p++;
    }
    if (strlen(hexstr) > 0)
    {
        printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}
