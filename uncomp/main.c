#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void hex_dump(void *data, int size);

unsigned int craftescape(char *buf, size_t escapelen, unsigned short **escape1, unsigned short **escape2)
{
	unsigned char val;
	size_t i;
	unsigned int escape;
	unsigned int alphabet_length = 0x46;


	if (!(*escape1 = malloc(sizeof (unsigned short) * escapelen)))
	{
		perror("malloc()");
		exit(EXIT_FAILURE);
	}
	memset(*escape1, 0, sizeof (unsigned short) * escapelen);
	if (!(*escape2 = malloc(sizeof (unsigned short) * escapelen)))
	{
		perror("malloc()");
		exit(EXIT_FAILURE);
	}
	memset(*escape2, 0, sizeof (unsigned short) * escapelen);
	escape = 0;
	for (i = 0; i < escapelen; i++)
	{
		(*escape1)[i] = alphabet_length;
		escape *= 2;
		(*escape1)[i] -= escape;
		val = *buf++;	
		escape += val;
		alphabet_length += val;
		(*escape2)[i] = escape;
	}
	return alphabet_length - 0x46;
}

void craft_symbols_widths(char *buf, size_t escapelen, unsigned char *alphabet, unsigned char *symbols, unsigned char *widths)
{
	unsigned char symbolwidth;
	unsigned char symbolcount = 0x80;
	size_t i, j, k, escape;

	if (escapelen >= 8)
		escapelen = 8;

	escape = 1;
	for (i = 0, j = 0; escape <= escapelen; escape++, symbolcount >>= 1)
	{
		symbolwidth = *buf++;
		for ( ; symbolwidth > 0; symbolwidth--, j++)
		{
			for (k = symbolcount; k; k--, i++)
			{
				symbols[i] = alphabet[j];
				widths[i] = escape;
			}
		}
	}
	for (; i < 0x100; i++)
		widths[i] = 0x40;

}

void uncomp(char *buf, size_t size)
{
	unsigned int uncomp_size = 0;
	unsigned char type;
	unsigned char escapelen;
	unsigned int alphabet_length;
	unsigned short *escape1 = NULL;
	unsigned short *escape2 = NULL;
	unsigned char widths[0x100] = {0};
	unsigned char symbols[0x100] = {0};
	unsigned char *alphabet = NULL;
	char *buf_escape = NULL;

	printf("RealSize   = 0x%08X\n", size);
	type = *buf++;
	printf("Type = %02X\n", type);
	uncomp_size = (*buf) | ((*buf + 1) << 0x8) | ((*buf + 2) << 0x10);
	buf += 3;
	printf("uncomp_size = %02X\n", uncomp_size);
	escapelen = *buf++;
	buf_escape = buf;
	printf("EscapeLen = %02X\n", escapelen);
	alphabet_length = craftescape(buf, escapelen, &escape1, &escape2);
	buf += escapelen;
	printf("AlphabetLength = %X\n", alphabet_length);
	printf("escape1 :\n");
	hex_dump(escape1, escapelen * 2);
	printf("escape2 :\n");
	hex_dump(escape2, escapelen * 2);
	if (!(alphabet = malloc(sizeof (char) * alphabet_length)))
	{
		perror("malloc()");
		exit(EXIT_FAILURE);
	}
	memcpy(alphabet, buf, alphabet_length);
	buf += alphabet_length;
	printf("alphabet :\n");
	hex_dump(alphabet, alphabet_length);
	craft_symbols_widths(buf_escape, escapelen, alphabet, symbols, widths);
	printf("symbols :\n");
	hex_dump(symbols, 0x100);
	printf("widths :\n");
	hex_dump(widths, 0x100);
}

int main(int argc, char **argv)
{
        int fd;
        struct stat st;
        char *buf = NULL;

	if (argc != 2)
	{
		fprintf(stderr, "%s <file>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

        fd = open(argv[1], O_RDONLY);
        if (fd == -1)
        {
                perror("open()");
                exit(EXIT_FAILURE);
        }
        if (fstat(fd, &st) == -1)
        {
                perror("fstat()");
                exit(EXIT_FAILURE);
        }
        if ((buf = malloc(sizeof (char) * st.st_size)) == NULL)
        {
                perror("malloc()");
                exit(EXIT_FAILURE);
        }
        if (read(fd, buf, st.st_size) != st.st_size)
        {
                perror("read()");
                goto clean;
        }
	uncomp(buf, st.st_size);
clean:
        free(buf);
        close(fd);
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
        if (isalnum(c) == 0)
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
