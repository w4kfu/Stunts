#include <stdio.h>
#include <stdlib.h>
#include "arg.h"
#include "file.h"
#include "../uncomp/tree.h"
#include "../uncomp/uncomp.h"

void print_dos_header(struct dos_header *dh)
{
	printf("[+] Dos Header Info\n");
	printf("\te_magic    = %04X	// Magic Number\n", dh->e_magic);
	printf("\te_cblp     = %04X	// Bytes on last page of file\n", dh->e_cblp);
	printf("\te_cp       = %04X	// Pages in file\n", dh->e_cp);
	printf("\te_crlc     = %04X	// Relocations\n", dh->e_crlc);
	printf("\te_cparhdr  = %04X	// Size Of header in paragraphs\n", dh->e_cparhdr);
	printf("\te_minalloc = %04X	// Minimum extra paragraphs needed\n", dh->e_minalloc);
	printf("\te_maxalloc = %04X	// Maximum extra paragraphs needed\n", dh->e_maxalloc);
	printf("\te_ss       = %04X	// Initial (relative) SS value\n", dh->e_ss);
	printf("\te_sp       = %04X	// Initial SP value\n", dh->e_sp);
	printf("\te_csum     = %04X	// Checksum\n", dh->e_csum);
	printf("\te_ip       = %04X	// Initial IP value\n", dh->e_ip);
	printf("\te_cs       = %04X	// Initial (relative) CS value\n", dh->e_cs);
	printf("\te_lfarlc   = %04X	// File offset of relocation table\n", dh->e_lfarlc);
	printf("\te_ovno     = %04X	// Overlay number\n", dh->e_ovno);
	printf("\n");
}

int checkfiles(struct s_conf *conf)
{
	if (conf->dh->e_magic != 0x5A4D)
	{
		fprintf(stderr, "[-] Wrong DOS Header for %s\n", conf->fhdr.filename);
		return 0; 
	}
	if (conf->ccmn.type != 0x02)
	{
		fprintf(stderr, "[-] File not compressed / Wrong type for %s\n", conf->fcmn.filename);
		return 0; 
	}
	if (conf->cdif.type != 0x02)
	{
		fprintf(stderr, "[-] File not compressed / Wrong type for %s\n", conf->fdif.filename);
		return 0; 
	}
	if (conf->ccod.type != 0x02)
	{
		fprintf(stderr, "[-] File not compressed / Wrong type for %s\n", conf->fcod.filename);
		return 0; 
	}
	return 1;
}

void compute_offset(unsigned short *a, unsigned short *b, unsigned long c)
{
	unsigned short d;

	c += *a;
	d = c / 0x10;
	*b += d;
	*a = c - (d << 4);
}

/* This part start at loc_10BC0 in file sub_10B1A.asm */
void apply_diff(struct s_comp *cmn, struct s_comp *dif)
{
	unsigned short actual_val;
	unsigned short offset = 0;
	unsigned short segment = 0;
	unsigned char *buf;
	unsigned int pos = 0;

	segment -= 0x1000;
	offset--;
	compute_offset(&offset, &segment, 0);
	buf = dif->buf_out;
	while (buf < (dif->buf_out + dif->size))
	{
		actual_val = *(unsigned short*)buf;
		buf += 2;
		compute_offset(&offset, &segment, actual_val & 0x7FFF);
		pos = segment << 4 | offset;
		*(cmn->buf_out + pos) = *buf++;
		*(cmn->buf_out + pos + 1) = *buf++;
		if ((actual_val & 0x8000) != 0)
		{
			*(cmn->buf_out + pos + 2) = *buf++;
			*(cmn->buf_out + pos + 3) = *buf++;	
		}
	}
}

/* all the following code can be found inside sub_10B1A.asm */
void info_new_exec(struct s_conf *conf)
{
	unsigned int exec_size = 0;
	unsigned int header_size = 0;
	unsigned int sum_cmn_cod = 0;
	int fd_out;
	unsigned int i;

	exec_size = conf->dh->e_cp * 512;
	if (conf->dh->e_cblp > 0)
		exec_size -= (512 - conf->dh->e_cblp);
	header_size = conf->dh->e_cparhdr * 16;
	sum_cmn_cod = conf->ccmn.size + conf->ccod.size;
	printf("EXEC_SIZE = %08X\n", exec_size);
	printf("Header Size = %08X\n", header_size);
	printf("Sum CMN + CODE = %08X\n", sum_cmn_cod);

	/* Maybe check the sum and exist if it doesn't match ... need more test */

	if ((fd_out = open("game.exe", O_WRONLY | O_CREAT, 0644)) == -1)
	{	
		perror("open()");
		return;
	}
	write(fd_out, conf->dh, sizeof (struct dos_header));
	/* write padding */
	for (i = 0; i < header_size - sizeof (struct dos_header); i++)
		write(fd_out, "\x00", 1);

	/* work with *.CMN */
	huff((unsigned char*)conf->fcmn.bMap, conf->fcmn.sb.st_size, &conf->ccmn);
	if (conf->ccmn.tree)
        {
		uncomp(&conf->ccmn, (unsigned char*)(conf->fcmn.bMap + conf->fcmn.sb.st_size));
		if (!conf->ccmn.buf_out)
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

	/* work with *.DIF */
	huff((unsigned char*)conf->fdif.bMap, conf->fdif.sb.st_size, &conf->cdif);
	if (conf->cdif.tree)
        {
		uncomp(&conf->cdif, (unsigned char*)(conf->fdif.bMap + conf->fdif.sb.st_size));
		if (!conf->cdif.buf_out)
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

	/* Now we have *.CMN and *.DIF uncompressed, we can apply the diff on the cmn data */
	apply_diff(&conf->ccmn, &conf->cdif);
	write(fd_out, conf->ccmn.buf_out, conf->ccmn.size);
	free(conf->ccmn.buf_out);
	free(conf->cdif.buf_out);

	/* work with *.COD */
	huff((unsigned char*)conf->fcod.bMap, conf->fcod.sb.st_size, &conf->ccod);
	if (conf->ccod.tree)
        {
		uncomp(&conf->ccod, (unsigned char*)(conf->fcod.bMap + conf->fcod.sb.st_size));
		if (conf->ccod.buf_out)
		{
			write(fd_out, conf->ccod.buf_out, conf->ccod.size);	
			free(conf->ccod.buf_out);
		}
		else
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
	close(fd_out);
}

void craftexec(struct s_conf *conf)
{
	if (!checkfiles(conf))
		return;
	print_dos_header(conf->dh);
	printf("[+] Compressed File Informations\n");
	printf("\t(%s)  : CompressedSize = 0x%08lX, RealSize = 0x%08X\n", conf->fcmn.filename, conf->fcmn.sb.st_size, conf->ccmn.size);
	printf("\t(%s) : CompressedSize = 0x%08lX, RealSize = 0x%08X\n", conf->fdif.filename, conf->fdif.sb.st_size, conf->cdif.size);
	printf("\t(%s) : CompressedSize = 0x%08lX, RealSize = 0x%08X\n", conf->fcod.filename, conf->fcod.sb.st_size, conf->ccod.size);
	printf("[+] New executable Informations\n");
	info_new_exec(conf);
}

int main(int argc, char **argv)
{
        struct s_conf conf = {0};

        parse_opt(argc, argv, &conf);
        check_opt(&conf, argv[0]);
	craftexec(&conf);
	clean(&conf);
	return 0;
}
