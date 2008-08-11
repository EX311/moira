/*
 * bmplib.c : bmp reading library
 *            only little endian machine supported
 *
 * Copyright(C) 2002 holelee
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fb-util.h"

#define BI_RGB 0
#define BI_RLE4 1
#define BI_RLE8 2
#define BI_BITFIELD 3

struct palette_s
{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char filter;
};

struct bmphandle_s
{
	/* bmp header contents */
	int filesize;
	int reserved;
	int dataoffset;
	int headersize;
	int width;
	int height;
	short nplanes;
	short bpp;
	int compression;
	int bitmapsize;
	int hres;
	int vres;
	int ncolors;
	int importantcolors;

	/* palette, pixel data, getpixel function pointer */
	int npalette;
	int bytes_per_line;
	struct palette_s *palette;
	unsigned char *data;
	unsigned char *encodeddata;
	struct color (*getpixel)(bmphandle_t, int, int);
	unsigned bsize_blue, bsize_green, bsize_red;
	unsigned boffset_blue, boffset_green, boffset_red;
};

static struct color getpixel_32bpp(bmphandle_t bh, int x, int y)
{
	struct color ret;
	unsigned *pdata;
	unsigned *mask = (unsigned *)(bh->palette);
	int offset = (bh->height-y)*bh->bytes_per_line + (x<<2);

	pdata = (unsigned *)(bh->data + offset);

	ret.b = ((*pdata & mask[2]) >> bh->boffset_blue) << (8-bh->bsize_blue);
	ret.g = ((*pdata & mask[1]) >> bh->boffset_green) << (8-bh->bsize_green);
	ret.r = ((*pdata & mask[0]) >> bh->boffset_red) << (8-bh->bsize_red);

	return ret;
}

static struct color getpixel_24bpp(bmphandle_t bh, int x, int y)
{
	struct color ret;
	unsigned char *pdata;
	int offset = (bh->height-y)*bh->bytes_per_line + x*3;

	pdata = bh->data + offset;
	ret.b = *pdata;
	ret.g = *(pdata+1);
	ret.r = *(pdata+2);

	return ret;
}

static struct color getpixel_16bpp(bmphandle_t bh, int x, int y)
{
	/* BI_RGB case */
	struct color ret;
	unsigned short *pdata;
	unsigned *mask = (unsigned *)(bh->palette);
	int offset = (bh->height-y)*bh->bytes_per_line + (x<<1);

	pdata = (unsigned short *)(bh->data + offset);
	ret.b = ((*pdata & mask[2]) >> bh->boffset_blue) << (8-bh->bsize_blue);
	ret.g = ((*pdata & mask[1]) >> bh->boffset_green) << (8-bh->bsize_green);
	ret.r = ((*pdata & mask[0]) >> bh->boffset_red) << (8-bh->bsize_red);

	return ret;
}

static struct color getpixel_8bpp(bmphandle_t bh, int x, int y)
{
	struct color ret;
	unsigned char *pdata;
	int offset = (bh->height-y)*bh->bytes_per_line + x;
	int pixel8;

	pdata = bh->data + offset;
	pixel8 = *pdata;
	/* palette lookup */
	ret.b = bh->palette[pixel8].blue;
	ret.g = bh->palette[pixel8].green;
	ret.r = bh->palette[pixel8].red;

	return ret;
}

static struct color getpixel_4bpp(bmphandle_t bh, int x, int y)
{
	struct color ret;
	unsigned char *pdata;
	int offset = (bh->height-y)*bh->bytes_per_line + (x>>1);
	int pixel4;
	int boffset;

	pdata = bh->data + offset;
	boffset = (x & 0x01)<<2;

	pixel4 = (*pdata & (0xF0U >> boffset)) >> (4-boffset);

	/* palette lookup */
	ret.b = bh->palette[pixel4].blue;
	ret.g = bh->palette[pixel4].green;
	ret.r = bh->palette[pixel4].red;

	return ret;
}

static struct color getpixel_1bpp(bmphandle_t bh, int x, int y)
{
	struct color ret;
	unsigned char *pdata;
	int offset = (bh->height-y)*bh->bytes_per_line + (x/8);
	int pixel1;
	int boffset;

	pdata = bh->data + offset;
	boffset = x%8;

	pixel1 = (*pdata & (0x80U >> boffset)) >> (7-boffset);

	/* palette lookup */
	ret.b = bh->palette[pixel1].blue;
	ret.g = bh->palette[pixel1].green;
	ret.r = bh->palette[pixel1].red;

	return ret;
}

static int bmp_readheader(FILE *fp, bmphandle_t bh)
{
	int size;
	int remnant;
	unsigned char ID[2];

	/* check ID */
	ID[0] = fgetc(fp);
	ID[1] = fgetc(fp);
	if(ID[0] != 'B' || ID[1] != 'M')
		return -1;
	/* Does not support other IDs such as IC */

	/* reading header */
	size = fread(&bh->filesize, 1, 52, fp);
	if(size != 52)
		return -1;

	bh->npalette = (bh->dataoffset-54)>>2;
	/* dword boundary on line end */
	size = bh->width*bh->bpp/8;
	remnant = size%4;
	if(remnant == 0)
		bh->bytes_per_line = size;
	else
		bh->bytes_per_line = size + (4-remnant);

	return 0;
}

static int bmp_readpalette(FILE *fp, bmphandle_t bh)
{
	int size;
	/* alread fp point where palette is */
	size = fread(bh->palette, sizeof(struct palette_s), bh->npalette, fp);
	if(size != bh->npalette)
		return -1;

	return 0;
}

static void rle8_decoding(bmphandle_t bh)
{
	int i, j;
	unsigned char *pdata = bh->data;
	unsigned char *pend = pdata + bh->width*bh->height;
	unsigned char *pedata = bh->encodeddata;
	unsigned char c;

	while(1)
	{
		if(pdata >= pend)
			break;
		c = *pedata++;
		if(c == 0) /* escape */
		{
			c = *pedata++;
			if(c == 0) /* end of line */
			{
				i = pdata - bh->data;
				i %= bh->width;
				for(; i < bh->width; i++)
					pdata++;
			} else if(c == 1) /* end of bitmap */
			{
				return;
			} else if(c == 2) /* delta */
			{
				j = *pedata++; /* right */
				i = *pedata++; /* down */
				i = j+i*bh->width;
				while(0 < i--)
					pdata++;
			} else /* absolute mode */
			{
				c = *pedata++;
				while(0 < c--)
					*pdata++ = *pedata++;
				/* word boundary */
				while((unsigned)pedata & 0x01)
					pedata++;
			}
		}
		else
		{
			while(c--)
				*pdata++ = *pedata;
			pedata++;
		}
	}
}
static int bmp_readdata(FILE *fp, bmphandle_t bh)
{
	fseek(fp, bh->dataoffset, SEEK_SET);

	if(bh->compression == BI_RGB || bh->compression == BI_BITFIELD)
	{
		bh->data = (unsigned char *)malloc(bh->bitmapsize);
		fread(bh->data, 1, bh->bitmapsize, fp);
	}
	else
	{
		bh->encodeddata = (unsigned char *)malloc(bh->bitmapsize);
		bh->data = (unsigned char *)malloc(bh->width*bh->height*bh->bpp/8);
		memset(bh->encodeddata, 0, bh->bitmapsize);
		fread(bh->encodeddata, 1, bh->bitmapsize, fp);
		if(bh->compression == BI_RLE4)
			return -1; /* rle4_decoding is not supported ! */
		else
			rle8_decoding(bh);
	}

	return 0;
}

static void calculate_boffset(bmphandle_t bh)
{
	int i;
	unsigned *mask = (unsigned *)(bh->palette);
	unsigned temp;

	/* red */
	temp = mask[0];
	for(i = 0; i < 32; i++)
	{
		if(temp & 0x01)
			break;
		temp >>= 1;
	}
	bh->boffset_red = i;
	for(i = 0; i < 32; i++)
	{
		if(temp & 0x800000UL)
			break;
		temp <<= 1;
	}
	bh->bsize_red = 32-i;

	/* green */
	temp = mask[1];
	for(i = 0; i < 32; i++)
	{
		if(temp & 0x01)
			break;
		temp >>= 1;
	}
	bh->boffset_green = i;
	for(i = 0; i < 32; i++)
	{
		if(temp & 0x800000UL)
			break;
		temp <<= 1;
	}
	bh->bsize_green = 32-i;

	/* blue */
	temp = mask[2];
	for(i = 0; i < 32; i++)
	{
		if(temp & 0x01)
			break;
		temp >>= 1;
	}
	bh->boffset_blue = i;
	for(i = 0; i < 32; i++)
	{
		if(temp & 0x800000UL)
			break;
		temp <<= 1;
	}
	bh->bsize_blue = 32-i;
}

bmphandle_t bmp_open(const char *filename)
{
	bmphandle_t bh;
	FILE *fp;

	fp = fopen(filename, "r+b");
	if(fp == NULL)
		return NULL;

	bh = (bmphandle_t)malloc(sizeof(*bh));
	memset(bh, 0, sizeof(*bh));	/* I don't like calloc */
	if(bmp_readheader(fp, bh))
		goto error;

	if(bh->npalette != 0)
	{
		bh->palette = (struct palette_s *)malloc(sizeof(struct palette_s)*bh->npalette);
		memset(bh->palette, 0, sizeof(struct palette_s)*bh->npalette);
		if(bmp_readpalette(fp, bh))
			goto error;
	}

	if(bmp_readdata(fp, bh))
		goto error;

	switch(bh->bpp)
	{
	case 1:
		bh->getpixel = getpixel_1bpp;
		break;
	case 4:
		bh->getpixel = getpixel_4bpp;
		break;
	case 8:
		bh->getpixel = getpixel_8bpp;
		break;
	case 16:
		bh->getpixel = getpixel_16bpp;
		if(bh->compression == BI_RGB)
		{
			unsigned *mask;
			if(bh->palette != NULL) /* something wrong */
				goto error;
			mask = (unsigned *)malloc(sizeof(unsigned)*3);
			mask[2] = 0x001F; /* blue mask */
			mask[1] = 0x03E0; /* green mask */
			mask[0] = 0x7C00; /* red mask */
			bh->palette = (struct palette_s *)mask;
			bh->boffset_blue = 0;
			bh->boffset_green = 5;
			bh->boffset_red = 10;
			bh->bsize_blue = 5;
			bh->bsize_green = 5;
			bh->bsize_red = 5;
		} else	/* BI_BITFIELD */
		{
			if(bh->palette == NULL) /* something wrong */
				goto error;
			calculate_boffset(bh);
		}
		break;
	case 24:
		bh->getpixel = getpixel_24bpp;
		break;
	case 32:
		bh->getpixel = getpixel_32bpp;
		if(bh->compression == BI_RGB)
		{
			unsigned *mask;
			if(bh->palette != NULL) /* something wrong */
				goto error;

			mask = (unsigned *)malloc(sizeof(unsigned)*3);
			mask[2] = 0x000000FF; /* blue mask */
			mask[1] = 0x0000FF00; /* green mask */
			mask[0] = 0x00FF0000; /* red mask */
			bh->palette = (struct palette_s *)mask;
			bh->boffset_blue = 0;
			bh->boffset_green = 8;
			bh->boffset_red = 16;
			bh->bsize_blue = 8;
			bh->bsize_green = 8;
			bh->bsize_red = 8;
		} else /* BI_BITFILED */
		{
			if(bh->palette == NULL) /* something wrong */
				goto error;
			calculate_boffset(bh);
		}
		break;
	default:
		goto error;
	}

	fclose(fp);
	return bh;
error:
	bmp_close(bh);
	fclose(fp);
	return NULL;
}

void bmp_close(bmphandle_t bh)
{
	if(bh->palette)
		free(bh->palette);
	if(bh->encodeddata)
		free(bh->encodeddata);
	if(bh->data)
		free(bh->data);

	free(bh);
}

int bmp_width(bmphandle_t bh)
{
	return bh->width;
}

int bmp_height(bmphandle_t bh)
{
	return bh->height;
}

struct color bmp_getpixel(bmphandle_t bh, int x, int y)
{
	return bh->getpixel(bh, x, y);
}
