#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>

struct s_file
{
	char filename[4096];
	int hFile;
	unsigned char *bMap;
	struct stat sb;
};

int open_file(char *lpFileName, struct s_file *sFile);
int mapview_file(struct s_file *sFile);
int open_and_map(char *lpFileName, struct s_file *sFile);
void clean_file(struct s_file *sFile);

#endif // __FILE_H__
