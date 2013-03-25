#include "bmp.h"

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
      int a = (x + (y * bmp->width)) * 3;
      p->b = bmp->data[a + 0];
      p->g = bmp->data[a + 1];
      p->r = bmp->data[a + 2];
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
