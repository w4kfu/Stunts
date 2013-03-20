#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "file.h"

struct dos_header
{
	unsigned short e_magic;
	unsigned short e_cblp;
	unsigned short e_cp;
	unsigned short e_crlc;
	unsigned short e_cparhdr;
	unsigned short e_minalloc;
	unsigned short e_maxalloc;
	unsigned short e_ss;
	unsigned short e_sp;
	unsigned short e_csum;
	unsigned short e_ip;
	unsigned short e_cs;
	unsigned short e_lfarlc;
	unsigned short e_ovno;
};

struct type_02_header
{
	unsigned char type;
	unsigned char size[3];
	unsigned char treedepth;
};

void print_dos_info(FILE *fout, struct dos_header *dh)
{
	fprintf(fout, " --- DOS HEADER INFO ---\n");
	fprintf(fout, "* e_magic    = %04X\n", dh->e_magic);
	fprintf(fout, "* e_cblp     = %04X\n", dh->e_cblp);
	fprintf(fout, "* e_cp       = %04X\n", dh->e_cp);
	fprintf(fout, "* e_crlc     = %04X\n", dh->e_crlc);
	fprintf(fout, "* e_cparhdr  = %04X\n", dh->e_cparhdr);
	fprintf(fout, "* e_minalloc = %04X\n", dh->e_minalloc);
	fprintf(fout, "* e_maxalloc = %04X\n", dh->e_maxalloc);
	fprintf(fout, "* e_ss       = %04X\n", dh->e_ss);
	fprintf(fout, "* e_sp       = %04X\n", dh->e_sp);
	fprintf(fout, "* e_csum     = %04X\n", dh->e_csum);
	fprintf(fout, "* e_ip       = %04X\n", dh->e_ip);
	fprintf(fout, "* e_cs       = %04X\n", dh->e_cs);
	fprintf(fout, "* e_lfarlc   = %04X\n", dh->e_lfarlc);
	fprintf(fout, "* e_ovno     = %04X\n", dh->e_ovno);
}

int test_dos_header(struct file *sFile)
{
	struct dos_header *dh = NULL;

	if ((unsigned int)sFile->sb.st_size > sizeof (struct dos_header))
	{
		dh = (struct dos_header*)sFile->bMap;
		if (dh->e_magic == 0x5A4D)
			return 1;
	}
	return 0;
}

int test_type_02(struct file *sFile)
{
	if ((unsigned int)sFile->sb.st_size > sizeof (struct type_02_header))
	{
		if (*sFile->bMap == 0x02)
			return 1;
	}
	return 0;
}

void print_info_type_02(FILE *fout, struct file *sFile)
{
	unsigned int uncomp_size = 0;
	unsigned int treelevels = 0;

	fprintf(fout, " --- (TYPE 02) Variable Length Compression ---\n");
	uncomp_size = *(sFile->bMap + 1) | (*(sFile->bMap + 2) << 0x8) | (*(sFile->bMap + 3) << 0x10);
	treelevels = *(sFile->bMap + 4);
	fprintf(fout, "* Uncompressed Size : %08X (%d)\n", uncomp_size, uncomp_size);
	fprintf(fout, "* Huffman Binary Tree Depths : %X\n", treelevels);
}

void print_info_file(FILE *fout, struct file *sFile)
{
	fprintf(fout, "### %s\n\n", sFile->filename);
	fprintf(fout, "* Size : 0x%08lX (%ld)\n\n", sFile->sb.st_size, sFile->sb.st_size);
	if (test_dos_header(sFile))
		print_dos_info(fout, (struct dos_header*)sFile->bMap);
	if (test_type_02(sFile))
		print_info_type_02(fout, sFile);
	fprintf(fout, "\n");
}
 
int main(int argc, char **argv)
{
	DIR *rep;
        struct dirent *ent;
    	struct file sFile;
	char filename[4096];
	FILE *fout;

	if (argc != 2)
	{
		fprintf(stderr, "Usage : %s <4DSD_directory>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	rep = opendir(argv[1]);
	if (rep == NULL)
	{
		perror("opendir()");
		exit(EXIT_FAILURE);
	}
	fout = fopen("stats.md", "w");
	if (!fout)
	{
		perror("fopen()");
		exit(EXIT_FAILURE);
	}
        while ((ent = readdir(rep)) != NULL)
        {
		if (ent->d_type == 8)
		{
			if (strlen(argv[1]) + strlen(ent->d_name) + 1 + 1 < 4096)
			{
				strcpy(filename, argv[1]);
				if (filename[strlen(filename) - 1] != '/')
					strcat(filename, "/");
				strcat(filename, ent->d_name);
    				if (open_and_map(filename, &sFile) == 0)
    				{
        				clean_file(&sFile);
        				fprintf(stderr, "[-] open_and_map failed : %s\n", filename);
    				}
				else
				{
					print_info_file(fout, &sFile);
					clean_file(&sFile);
				}
			}
		}
        }
        closedir(rep);
	fclose(fout);
    	return 0;
}
