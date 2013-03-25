#include "bmp.h"
#include <string.h>

unsigned int palette[] ={0x000000, 0x0000A8, 0x00A800, 0x00A8A8, 0xA80000, 0xA800A8, 0xA85400, 0xA8A8A8, 
	0x545454, 0x5454FC, 0x54FC54, 0x54FCFC, 0xFC5454, 0xFC54FC, 0xFCFC54, 0xFCFCFC, 
	0xFCFCFC, 0xDCDCDC, 0xCCCCCC, 0xC0C0C0, 0xB0B0B0, 0xA4A4A4, 0x989898, 0x888888, 
	0x7C7C7C, 0x707070, 0x606060, 0x545454, 0x484848, 0x383838, 0x2C2C2C, 0x202020, 
	0xFCD8D8, 0xFCB8B8, 0xFC9C9C, 0xFC7C7C, 0xFC5C5C, 0xFC4040, 0xFC2020, 0xFC0000, 
	0xE40000, 0xCC0000, 0xB40000, 0x9C0000, 0x840000, 0x700000, 0x580000, 0x400000, 
	0xFCE8D8, 0xFCDCC0, 0xFCD4AC, 0xFCC894, 0xFCC080, 0xFCB868, 0xFCAC54, 0xFCA43C, 
	0xFC9C28, 0xE08820, 0xC4781C, 0xA86414, 0x905410, 0x744008, 0x583004, 0x402000, 
	0xFCFCD8, 0xFCFCB8, 0xFCFC9C, 0xFCFC7C, 0xFCF85C, 0xFCF440, 0xFCF420, 0xFCF400, 
	0xE4D800, 0xCCC000, 0xB4A400, 0x9C8C00, 0x847400, 0x6C5800, 0x544000, 0x402800, 
	0xF8FCD8, 0xF4FCB8, 0xE8FC9C, 0xE0FC7C, 0xD0FC5C, 0xC4FC40, 0xB4FC20, 0xA0FC00, 
	0x90E400, 0x80CC00, 0x74B400, 0x609C00, 0x508400, 0x447000, 0x345800, 0x284000, 
	0xD8FCD8, 0x9CFC9C, 0x90EC90, 0x84E084, 0x78D078, 0x70C46C, 0x64B864, 0x58A858, 
	0x509C4C, 0x449040, 0x388034, 0x2C742C, 0x246820, 0x185814, 0x0C4C08, 0x044000, 
	0xD8FCFC, 0xB8FCFC, 0x9CFCFC, 0x7CFCF8, 0x5CFCFC, 0x40FCFC, 0x20FCFC, 0x00FCFC, 
	0x00E4E4, 0x00CCCC, 0x00B4B4, 0x009C9C, 0x008484, 0x007070, 0x005858, 0x004040, 
	0xD8ECFC, 0xB8E0FC, 0x9CD4FC, 0x7CC8FC, 0x5CBCFC, 0x40B0FC, 0x009CFC, 0x008CE4, 
	0x0080D0, 0x0074BC, 0x0064A8, 0x005890, 0x004C7C, 0x003C68, 0x003054, 0x002440, 
	0xD8D8FC, 0xB8BCFC, 0x9C9CFC, 0x7C80FC, 0x5C60FC, 0x4040FC, 0x0004FC, 0x0000E4, 
	0x0000D0, 0x0000BC, 0x0000A8, 0x000090, 0x00007C, 0x000068, 0x000054, 0x000040, 
	0xF0D8FC, 0xE4B8FC, 0xD89CFC, 0xD07CFC, 0xC85CFC, 0xBC40FC, 0xB420FC, 0xA800FC, 
	0x9800E4, 0x8000CC, 0x7400B4, 0x60009C, 0x500084, 0x440070, 0x340058, 0x280040, 
	0xFCD8FC, 0xFCB8FC, 0xFC9CFC, 0xFC7CFC, 0xFC5CFC, 0xFC40FC, 0xFC20FC, 0xE000E4, 
	0xCC00CC, 0xB800B8, 0xA400A4, 0x900090, 0x7C007C, 0x680068, 0x540054, 0x400040, 
	0xFCE8DC, 0xF0D4C4, 0xE4C4AC, 0xD8B498, 0xCCA080, 0xC0906C, 0xB48054, 0xAC7040, 
	0x9C6438, 0x8C5C34, 0x80542C, 0x704C28, 0x604020, 0x54381C, 0x443014, 0x382810, 
	0xFCD8CC, 0xF8CCB8, 0xF4C0A8, 0xF0B494, 0xECA884, 0xE89C74, 0xE49464, 0xE08C58, 
	0xD8804C, 0xD47840, 0xC86C34, 0xC0602C, 0xB45424, 0xA8481C, 0x9C3C14, 0x94300C, 
	0xF4C0A8, 0xF0BCA0, 0xF0B89C, 0xF0B494, 0xECB090, 0xECAC88, 0xECA884, 0xE8A480, 
	0xE8A078, 0xE89C74, 0xE4986C, 0xE49468, 0xE49464, 0xFC9C9C, 0xFC9494, 0xFC9090, 
	0xFC8C8C, 0xFC8484, 0xFC8080, 0xFC7C7C, 0xD8B498, 0xD0AC8C, 0xCCA484, 0xC89C7C, 
	0xC49474, 0xC0906C, 0xC0C0C0, 0xBCBCBC, 0xB8B8B8, 0xB4B4B4, 0xB0B0B0, 0xFFFFFF};

BMP *bmp_create(int x, int y)
{
	BMP *b = NULL;

	b = malloc(sizeof(BMP));
	if (!b)
	{
		perror("[-] malloc()");
		exit(EXIT_FAILURE);
	}
	b->width  = x;
	b->height = y;
	b->data   = (unsigned char *)malloc((x*y)*3);
	return (b);
}

void bmp_getcolor(BMP *bmp, int x, int y, pixel *p)
{
  	if((x < bmp->width) && (y < bmp->height))
    	{
      		int a = (x + (y * bmp->width));
		memcpy(p, &palette[bmp->data[a]], 3);
    	}
}

void	make_bmpfileheader(long bitsize, bmpFHEAD *filehead)
{
  	filehead->bfType = 0x4d42;	/* 'MB'; */
  	filehead->bfSize = sizeof(bmpFHEAD) - 2 + sizeof(bmpIHEAD) + bitsize; /* -2 due to byte alignment */
  	filehead->bfReserved1 = 0;
  	filehead->bfReserved2 = 0;
  	filehead->bfOffBits = sizeof(bmpFHEAD)-2 + sizeof(bmpIHEAD);	/* -2 due to bytealignment */
}

void	make_bmpinfoheader(BMP *bmp, long bitsize, bmpIHEAD *infohead)
{
  	infohead->biSize = sizeof (bmpIHEAD);
  	infohead->biWidth = bmp->width;
  	infohead->biHeight = bmp->height;
  	infohead->biPlanes = 1;
  	infohead->biBitCount = 24; /* 24bit */
  	infohead->biCompression = 0;  /* RGB */
  	infohead->biSizeImage = bitsize;
  	infohead->biXPelsPerMeter = 2800;
  	infohead->biYPelsPerMeter = 2800;
  	infohead->biClrUsed = 0;
  	infohead->biClrImportant = 0;
}

int bmp_save(BMP *bmp, char *filename)
{
	bmpFHEAD filehead; /* File Header */
	bmpIHEAD infohead; /* Info Header */
	unsigned char swp;
	long bitsize;
	int x, y, a;
	pixel p;
	FILE *fp;

	fp = fopen(filename, "wb");
	if (!bmp->data || !fp)
	{
		fclose (fp);
		return (0);
	}
	bitsize  = bmp->width * bmp->height * 3;
	bitsize += bitsize % 4; /* and round to next 4 bytes (see byte alignement in compiler options!) */
	make_bmpfileheader(bitsize, &filehead);

	fwrite(&filehead.bfType,sizeof(filehead.bfType), 1, fp);
	fwrite(&filehead.bfSize,sizeof(filehead.bfSize), 1, fp);
	fwrite(&filehead.bfReserved1,sizeof(filehead.bfReserved1), 1, fp);
	fwrite(&filehead.bfReserved2,sizeof(filehead.bfReserved2), 1, fp);
	fwrite(&filehead.bfOffBits, sizeof(filehead.bfOffBits),1, fp);

	make_bmpinfoheader(bmp, bitsize, &infohead);

	fwrite(&infohead, sizeof (infohead), 1, fp);

	/* BITMAP */
	for(y = 0; y < bmp->height; y++)
	{
		for(x = 0; x < bmp->width; x++)
		{
			bmp_getcolor(bmp, x,y, &p);
		  	fwrite (&p, sizeof (p), 1, fp);
		}
		/* Add padding bytes */
		swp = 0;
		for (a = 0; a < (4 - ((3 * bmp->width ) % 4)) % 4; a++)
			fwrite (&swp, sizeof(char), 1, fp);
	}
	fclose (fp);
	return 1;
}
