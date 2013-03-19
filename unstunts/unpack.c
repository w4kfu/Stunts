#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct bitmap
{
	char *pbuf;
        unsigned short buf;
        unsigned char count;
};

void hex_dump(void *data, int size);
int getbit(struct bitmap *p);
void unpack(char *buf_stunt);

int getbit(struct bitmap *p) 
{
    int b;

    b = p->buf & 1;
    if(--p->count == 0)
    {
        p->buf = *(unsigned short*)p->pbuf;
	p->pbuf += 2;
        p->count= 0x10;
    }
    else
        p->buf >>= 1;
    return b;
}

void unpack(char *buf_stunt)
{
	short len;
	short span;
	struct bitmap bits;
	char data[0x4000];
	char *p = NULL;

	p = data;
	bits.count = 0x10;
	bits.pbuf = buf_stunt;
	bits.buf = *(unsigned short*)bits.pbuf;
	bits.pbuf += 2;
	for (;;)
	{
		if (p - data > 0x4000)
		{
			printf("ALLO !?\n");
			exit(0);
		}
		if (getbit(&bits))
		{
			*p++ = *bits.pbuf++;
			continue;
		}
		if (!getbit(&bits))
		{
			len = getbit(&bits) << 1;
			len |= getbit(&bits);
			len += 2;
			span = *bits.pbuf++;
			span |= 0xFF00;
		}
		else
		{
			span = *bits.pbuf++;
			len = *bits.pbuf++;
			span |= ((len >> 3) | 0xE0) << 0x8;
            		len = (len & 0x07);
			if (!len)
			{
				len = *bits.pbuf++;
				if (len == 0)
				{
					printf("END \n");
					/* break; */
					continue;
				}
				if (len == 1)
				{
					printf("Must memcpy ?!!!\n");
					break;
					continue;
				}
				len++;
			}
			else
				len += 2;
		}
		for (; len > 0; len--, p++)
			*p = *(p + span);
	}
	hex_dump(data, p - data);
}

int main(void)
{
	int fd;
	struct stat st;
	char *buf_stunt = NULL;

	fd = open("STUNTS.COM", O_RDONLY);
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
	printf("SizeFile = %lX\n", st.st_size);
	if ((buf_stunt = malloc(sizeof (char) * st.st_size)) == NULL)
	{
		perror("malloc()");
		exit(EXIT_FAILURE);
	}
	if (read(fd, buf_stunt, st.st_size) != st.st_size)
	{
		perror("read()");
		goto clean;
	}
	/* Remove COM header */
	unpack(buf_stunt + 0x20);
clean:
	free(buf_stunt);
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
                snprintf(addrstr, sizeof(addrstr), "%.4x", p - (unsigned char*)data);
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
