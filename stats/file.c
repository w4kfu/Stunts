#include "file.h"

int open_file(char *lpFileName, struct file *sFile)
{
	sFile->hFile = open(lpFileName, O_RDONLY);
	if (sFile->hFile == -1)
	{
		perror("open()");
		return 0;
	}
        if (fstat(sFile->hFile, &sFile->sb) == -1)
        {
                perror("fstat()");
		return 0;
        }
	strncpy(sFile->filename, lpFileName, 4096);
	return 1;
}

int mapview_file(struct file *sFile)
{
	if ((sFile->bMap = mmap (NULL, sFile->sb.st_size, PROT_READ, MAP_PRIVATE,
				sFile->hFile, 0)) == MAP_FAILED)
	{
		perror("mmap()");
		return 0;
	}
	return 1;
}

int open_and_map(char *lpFileName, struct file *sFile)
{
	if (open_file(lpFileName, sFile) == 0)
		return 0;
	if (mapview_file(sFile) == 0)
		return 0;
	return 1;
}

void clean_file(struct file *sFile)
{
	close(sFile->hFile);
	munmap(sFile->bMap, sFile->sb.st_size);
}
