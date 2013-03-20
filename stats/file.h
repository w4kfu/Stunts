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

struct file
{
	char filename[4096];
	int hFile;
	char *bMap;
	struct stat sb;
};

int open_file(char *lpFileName, struct file *sFile);
int mapview_file(struct file *sFile);
int open_and_map(char *lpFileName, struct file *sFile);
void clean_file(struct file *sFile);

#endif // __FILE_H__
