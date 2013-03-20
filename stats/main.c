#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
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

void hex_dump(FILE *fout, void *data, int size);

void print_dos_info(FILE *fout, struct dos_header *dh)
{
	fprintf(fout, "--- DOS HEADER INFO ---\n\n");
	fprintf(fout, "* e_magic    = 0x%04X\n", dh->e_magic);
	fprintf(fout, "* e_cblp     = 0x%04X\n", dh->e_cblp);
	fprintf(fout, "* e_cp       = 0x%04X\n", dh->e_cp);
	fprintf(fout, "* e_crlc     = 0x%04X\n", dh->e_crlc);
	fprintf(fout, "* e_cparhdr  = 0x%04X\n", dh->e_cparhdr);
	fprintf(fout, "* e_minalloc = 0x%04X\n", dh->e_minalloc);
	fprintf(fout, "* e_maxalloc = 0x%04X\n", dh->e_maxalloc);
	fprintf(fout, "* e_ss       = 0x%04X\n", dh->e_ss);
	fprintf(fout, "* e_sp       = 0x%04X\n", dh->e_sp);
	fprintf(fout, "* e_csum     = 0x%04X\n", dh->e_csum);
	fprintf(fout, "* e_ip       = 0x%04X\n", dh->e_ip);
	fprintf(fout, "* e_cs       = 0x%04X\n", dh->e_cs);
	fprintf(fout, "* e_lfarlc   = 0x%04X\n", dh->e_lfarlc);
	fprintf(fout, "* e_ovno     = 0x%04X\n", dh->e_ovno);
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

void exec_decompressor(struct file *sFile)
{
	char *argv[] = {"./bin/uncomp", "-i", sFile->filename, "-o", "/tmp/out", NULL};
  	pid_t child;
	int status;

	if ((child = fork()) == 0)
	{
		execv("./bin/uncomp", argv);
		exit(0);
	}
	do 
	{
    		waitpid(child, &status, 0);
    		if(status == -1) 
		{
        		perror("Error during wait()");
        		abort();
    		}
	} while (status > 0);
}

void parse_type2(FILE *fout, struct file *sFile)
{
	unsigned int i;
	unsigned short nb_entry;
	unsigned char **name_entry = NULL;
	unsigned char *buf = NULL;

	buf = sFile->bMap;
	buf += 4;
	nb_entry = *(unsigned short*)(buf);
	buf += 2;
	fprintf(fout, "NbEntry : %X (%d)\n\n", nb_entry, nb_entry);
	name_entry = malloc(sizeof (char*) * nb_entry);
	if (!name_entry)
		return;
	for (i = 0; i < nb_entry; i++)
	{
		name_entry[i] = malloc(sizeof (char) * 5);
		if (!name_entry[i])
			break;
		memset(name_entry[i], 0, 5);
		strncpy(name_entry[i], buf, 4);
		buf += 4;
		fprintf(fout, "* 0x%02X (%d) : %s\n", (i + 1), i + 1, name_entry[i]);
	}
	for (i = 0; i < nb_entry; i++)
	{
		if (name_entry[i])
			free(name_entry[i]);
	}
	free(name_entry);

}

void print_info_type_02(FILE *fout, struct file *sFile)
{
	unsigned int uncomp_size = 0;
	unsigned int real_size = 0;
	unsigned int treelevels = 0;
	struct file sFileuncomp;

	fprintf(fout, "--- (TYPE 02) Variable Length Compression ---\n\n");
	uncomp_size = *(sFile->bMap + 1) | (*(sFile->bMap + 2) << 0x8) | (*(sFile->bMap + 3) << 0x10);
	treelevels = *(sFile->bMap + 4);
	fprintf(fout, "* Uncompressed Size : 0x%08X (%d)\n", uncomp_size, uncomp_size);
	fprintf(fout, "* Huffman Binary Tree Depths : %X (%d)\n", treelevels, treelevels);
	exec_decompressor(sFile);
	if (open_and_map("/tmp/out", &sFileuncomp) == 0)
	{
		clean_file(&sFileuncomp);
		fprintf(stderr, "[-] open_and_map failed : %s\n", "/tmp/out");
	}
	real_size = *(unsigned int*)(sFileuncomp.bMap);
	if (real_size == uncomp_size)
	{
		fprintf(fout, "\n[+] Decompression OK!\n\n");
		parse_type2(fout, &sFileuncomp);
	}
	else
		fprintf(fout, "\n[-] Real size not found in new header !\n\n");
	fprintf(fout, "\n\nhexdump :\n\n");
	hex_dump(fout, sFileuncomp.bMap, 0x40);
	clean_file(&sFileuncomp);
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

void hex_dump(FILE *fout, void *data, int size)
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
            fprintf(fout, "\t[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
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
        fprintf(fout, "\t[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}
