/*
 * Start of BMP section.
 */

/*
 * Todo:
 * createBitmapImage() needs a colortable parameter aColors, with possibly biClrUsed,
 * Need better mapping to color table.
 * Need mechanism to upgrade images.
 * Need color counter so that may create a color table and smaller images.
 *
 * The padding mechanism of a 4-bit image is not fully understood.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmpdefs.h"


int ipow2( int p )
{
	int i = 1;

	if ((p < 0) || (p > 30))
	{
		return -1;
	}
	return (i << p);
}


/*
 * For reference: a *.BMP format.

BITMAPFILEHEADER	bmfh;
BITMAPINFOHEADER	bmih;
RGBQUAD			aColors[];
BYTE			aBitmapBits[];

 */

int readBitmapFileHeader( BITMAPIMAGE *bm, FILE *fp )
{
	size_t
		size;
	char
		BMPtype[2] = {'B', 'M'}, *ThisType;
	int
		iRet = 0;
	BITMAPFILEHEADER
		*bmfh = &(bm->bmfh);


	rewind(fp);
	size = fread(&(bmfh->bfType), sizeof(UINT), 1, fp);
	if (size != 1)
	{
		fprintf(stderr, "Error reading file for bfType, size = %d\n", size);
		return 1;
	}
	if (memcmp(&(bmfh->bfType), BMPtype, 2))
	{
		ThisType = (char*) &(bmfh->bfType);
		fprintf(stderr, "Not a BMP-type image (%c%c)\n", ThisType[0], ThisType[1]);
		return 1;
	}
	size = fread(&(bmfh->bfSize), sizeof(DWORD), 1, fp);
	if (size != 1)
	{
		fputs("Error reading file for bfSize\n", stderr);
		return 1;
	}
	size = fread(&(bmfh->bfReserved1), sizeof(UINT), 1, fp);
	if (size != 1)
	{
		fputs("Error reading file for Reserved1\n", stderr);
		return 1;
	}
	size = fread(&(bmfh->bfReserved2), sizeof(UINT), 1, fp);
	if (size != 1)
	{
		fputs("Error reading file for Reserved2\n", stderr);
		return 1;
	}
	if (bmfh->bfReserved1 != 0)
	{
		fprintf(stderr, "Warning!  Reserved1 has value %d\n", bmfh->bfReserved1);
		iRet = -1;
	}
	if (bmfh->bfReserved2 != 0)
	{
		fprintf(stderr, "Warning!  Reserved2 has value %d\n", bmfh->bfReserved2);
		iRet = -1;
	}
	size = fread(&(bmfh->bfOffBits), sizeof(DWORD), 1, fp);
	if (size != 1)
	{
		fputs("Error reading file for OffBits\n", stderr);
		return 1;
	}

	return iRet;
}

int readBitmapInfoHeader( BITMAPIMAGE *bm, FILE *fp )
{
	int
		iRet = 0;
	size_t
		size;
	BITMAPINFOHEADER
		*bmiHeader = &(bm->bmi.bmiHeader);


	size = fread(&(bmiHeader->biSize), sizeof(DWORD), 1, fp);
	if (bmiHeader->biSize == 12)
	{
		bmiHeader->biWidth = 0;
		bmiHeader->biHeight = 0;
		size = fread(&(bmiHeader->biWidth), sizeof(WORD), 1, fp);
		size = fread(&(bmiHeader->biHeight), sizeof(WORD), 1, fp);
	}
	else
	{
		size = fread(&(bmiHeader->biWidth), sizeof(LONG), 1, fp);
		size = fread(&(bmiHeader->biHeight), sizeof(LONG), 1, fp);
	}
	size = fread(&(bmiHeader->biPlanes), sizeof(WORD), 1, fp);
	size = fread(&(bmiHeader->biBitCount), sizeof(WORD), 1, fp);
	if (bmiHeader->biSize != 12)
	{
		size = fread(&(bmiHeader->biCompression), sizeof(DWORD), 1,
				fp);
		switch (bmiHeader->biCompression)
		{
		case BI_RGB:
			break;
		case BI_RLE8:
			fputs("biCompression is BI_RLE8 (Not supported)\n", stderr);
			break;
		case BI_RLE4:
			fputs("biCompression is BI_RLE4 (Not supported)\n", stderr);
			break;
		default:
			fprintf(stderr, "biCompression is not recognized (%d)\n", bmiHeader->biCompression);
			iRet = -1;
			break;
		}
		size = fread(&(bmiHeader->biSizeImage), sizeof(DWORD), 1, fp);
		size = fread(&(bmiHeader->biXPelsPerMeter), sizeof(LONG), 1, fp);
		size = fread(&(bmiHeader->biYPelsPerMeter), sizeof(LONG), 1, fp);
		size = fread(&(bmiHeader->biClrUsed), sizeof(DWORD), 1, fp);
		size = fread(&(bmiHeader->biClrImportant), sizeof(DWORD), 1, fp);
	}
	else
	{
		bmiHeader->biCompression = 0;
		bmiHeader->biSizeImage = 0;
		bmiHeader->biXPelsPerMeter = 0;
		bmiHeader->biYPelsPerMeter = 0;
		bmiHeader->biClrUsed = 0;
		bmiHeader->biClrImportant = 0;
	}

	return iRet;
}

int readBitmapInfoColors( BITMAPIMAGE *bm, FILE *fp )
{
	int
		iRet = 0;
	BITMAPINFO
		*bmi = &(bm->bmi);
	DWORD
		biClrUsed = bm->bmi.bmiHeader.biClrUsed, loop;
	size_t
		size;


	if (biClrUsed == 0)
	{
		biClrUsed = (bm->bmfh.bfOffBits-(bm->bmi.bmiHeader.biSize+14))/4;
		// biClrUsed = ipow2(bmi->bmiHeader.biBitCount);
		bm->bmi.bmiHeader.biClrUsed = biClrUsed;
	}
	if ((biClrUsed == 0) || (biClrUsed > 256))
	{
		bmi->bmiColors = NULL;
		return iRet;
	}
	bmi->bmiColors = calloc(biClrUsed, sizeof(RGBQUAD));
	for (loop = 0; loop < biClrUsed; ++loop)
	{
		size = fread(&(bmi->bmiColors[loop]), sizeof(RGBQUAD), 1, fp);
	}
	return iRet;
}

int readBitmapRow( BITMAPIMAGE *bm, LONG row, FILE *fp )
{
	int
		iStat = 0, iRet = 0, modder;
	BYTE
		*aBitmapBits, pad[3];
	LONG
		biWidth = bm->bmi.bmiHeader.biWidth;
	RGBTRIPLE
		*aBitmapRGB;



	switch (bm->bmi.bmiHeader.biBitCount)
	{
	case 24:
		aBitmapRGB = (RGBTRIPLE*) &(bm->aBitmapBits[3*biWidth*row]);
 		iStat = fread(aBitmapRGB, 3, biWidth, fp);
		modder = (3 * biWidth) % 4;
		if (modder > 0) fread(pad, 1, (4 - modder), fp);
		break;
	case 8:
		aBitmapBits = (BYTE*) &(bm->aBitmapBits[biWidth*row]);
		iStat = fread(aBitmapBits, 1, biWidth, fp);
		modder = biWidth % 4;
		if (modder > 0) fread(pad, 1, (4 - modder), fp);
		break;
	case 4:
		aBitmapBits = (BYTE*) &(bm->aBitmapBits[(biWidth+1)/2*row]);
		iStat = 2*fread(aBitmapBits, 1, (biWidth+1)/2, fp);
		modder = ((biWidth+1)/2)%4;
		if (modder > 0) fread(pad, 1, (4 - modder), fp);
		break;
	case 1:
		aBitmapBits = (BYTE*) &(bm->aBitmapBits[(biWidth+7)/8*row]);
 		iStat = 8*fread(aBitmapBits, 1, (biWidth+7)/8, fp);
		modder = ((biWidth+7)/8)%4;
		if (modder > 0) fread(pad, 1, (4 - modder), fp);
		break;
	default:
		aBitmapBits = (BYTE*) &(bm->aBitmapBits[biWidth*row]);
 		iStat = fread(aBitmapBits, 1, biWidth, fp);
		modder = biWidth%4;
		if (modder > 0) fread(pad, 1, (4 - modder), fp);
		break;
	}


	if (iStat < biWidth)
	{
		fprintf(stderr, "Error! Expecting row size %d, but got %d\n", biWidth, iStat);
		iRet = 1;
	}
	return iRet;
}

int readBitmapImage( BITMAPIMAGE *bm, FILE *fp )
{
	int
		iRet = 0;
	LONG
		arraysize, datasize, loop;
	BYTE
		*aBitmapBits;


	arraysize = bm->bmi.bmiHeader.biWidth * bm->bmi.bmiHeader.biHeight;
	datasize = arraysize * ((bm->bmi.bmiHeader.biBitCount + 7) / 8);
	aBitmapBits = calloc(datasize, sizeof(BYTE));
	bm->aBitmapBits = (BYTE*) aBitmapBits;
	for (loop = 0; loop < bm->bmi.bmiHeader.biHeight; ++loop)
	{
		readBitmapRow(bm, loop, fp);
	}
	return iRet;
}






/* Write BMP Images
 */


int writeBitmapFileHeader( BITMAPIMAGE *bm, FILE *fp )
{
	size_t
		size;
	char
		BMPtype[2] = {'B', 'M'};
	int
		iRet = 0;
	BITMAPFILEHEADER
		*bmfh = &(bm->bmfh);
	BITMAPINFOHEADER
		*bmih = &(bm->bmi.bmiHeader);
	LONG
		arraysize, datasize;


	size = fwrite(BMPtype, 2, 1, fp);
	if (size != 1)
	{
		fprintf(stderr, "Error writing file for bfType, size = %d\n", size);
		return 1;
	}
	arraysize = bmih->biWidth * bmih->biHeight;
	datasize = arraysize * ((bmih->biBitCount + 7) / 8);
	bmfh->bfSize = datasize + 54;
	if (bm->bmi.bmiHeader.biBitCount <= 8)
	{
		bmfh->bfSize += bm->bmi.bmiHeader.biClrUsed * 4;
	}
	size = fwrite(&(bmfh->bfSize), sizeof(DWORD), 1, fp);
	if (size != 1)
	{
		fputs("Error writing file for bfSize\n", stderr);
		return 1;
	}
	bmfh->bfReserved1 = bmfh->bfReserved2 = (UINT) 0;
	size = fwrite(&(bmfh->bfReserved1), sizeof(UINT), 1, fp);
	if (size != 1)
	{
		fputs("Error writing file for Reserved1\n", stderr);
		return 1;
	}
	size = fwrite(&(bmfh->bfReserved2), sizeof(UINT), 1, fp);
	if (size != 1)
	{
		fputs("Error writing file for Reserved2\n", stderr);
		return 1;
	}
	bmfh->bfOffBits = 54;
	if (bm->bmi.bmiHeader.biBitCount <= 8)
	{
		bmfh->bfOffBits += bm->bmi.bmiHeader.biClrUsed * 4;
	}
	size = fwrite(&(bmfh->bfOffBits), sizeof(DWORD), 1, fp);
	if (size != 1)
	{
		fputs("Error writing file for OffBits\n", stderr);
		return 1;
	}

	return iRet;
}

int writeBitmapInfoHeader( BITMAPIMAGE *bm, FILE *fp )
{
	int
		iRet = 0;
	size_t
		size;
	BITMAPINFOHEADER
		*bmiHeader = &(bm->bmi.bmiHeader);


	/* Choices for bmiHeader->biSize are 12 and 40.
	 */
	bmiHeader->biSize = 40;
	bmiHeader->biSizeImage = bmiHeader->biWidth*bmiHeader->biHeight*((bmiHeader->biBitCount+7)/8);
	size = fwrite(&(bmiHeader->biSize), sizeof(DWORD), 1, fp);
	if (bmiHeader->biSize == 12)
	{
		size = fwrite(&(bmiHeader->biWidth), sizeof(WORD), 1, fp);
		size = fwrite(&(bmiHeader->biHeight), sizeof(WORD), 1, fp);
	}
	else
	{
		size = fwrite(&(bmiHeader->biWidth), sizeof(LONG), 1, fp);
		size = fwrite(&(bmiHeader->biHeight), sizeof(LONG), 1, fp);
	}
	bmiHeader->biPlanes = (WORD) 1;
	size = fwrite(&(bmiHeader->biPlanes), sizeof(WORD), 1, fp);
	size = fwrite(&(bmiHeader->biBitCount), sizeof(WORD), 1, fp);
	if (bmiHeader->biSize != 12)
	{
		bmiHeader->biCompression = BI_RGB;
		size = fwrite(&(bmiHeader->biCompression), sizeof(DWORD), 1, fp);
		switch (bmiHeader->biCompression)
		{
		case BI_RGB:
			break;
		case BI_RLE8:
			fputs("biCompression is BI_RLE8 (Not supported)\n", stderr);
			break;
		case BI_RLE4:
			fputs("biCompression is BI_RLE4 (Not supported)\n", stderr);
			break;
		default:
			fprintf(stderr, "biCompression is not recognized (%d)\n", bmiHeader->biCompression);
			iRet = -1;
			break;
		}
		size = fwrite(&(bmiHeader->biSizeImage), sizeof(DWORD), 1, fp);
		size = fwrite(&(bmiHeader->biXPelsPerMeter), sizeof(LONG), 1, fp);
		size = fwrite(&(bmiHeader->biYPelsPerMeter), sizeof(LONG), 1, fp);
		size = fwrite(&(bmiHeader->biClrUsed), sizeof(DWORD), 1, fp);
		size = fwrite(&(bmiHeader->biClrImportant), sizeof(DWORD), 1, fp);
	}
	else
	{
		/*
		bmiHeader->biCompression = 0;
		bmiHeader->biSizeImage = 0;
		bmiHeader->biXPelsPerMeter = 0;
		bmiHeader->biYPelsPerMeter = 0;
		bmiHeader->biClrUsed = 0;
		bmiHeader->biClrImportant = 0;
		*/
	}

	return iRet;
}

int writeBitmapInfoColors( BITMAPIMAGE *bm, FILE *fp )
{
	int
		iRet = 0;
	DWORD
		biClrUsed = bm->bmi.bmiHeader.biClrUsed, loop;
	size_t
		size;
	BITMAPINFO
		*bmi = &(bm->bmi);


	if (biClrUsed == 0)
	{
		biClrUsed = ipow2(bmi->bmiHeader.biBitCount);
	}
	if ((biClrUsed == 0) || (biClrUsed > 256))
	{
		bmi->bmiColors = NULL;
		return iRet;
	}
	for (loop = 0; (loop < biClrUsed) && (bmi->bmiColors); ++loop)
	{
		size = fwrite(&(bmi->bmiColors[loop]), sizeof(RGBQUAD), 1, fp);
	}
	return iRet;
}

int writeBitmapRow( BITMAPIMAGE *bm, LONG row, FILE *fp )
{
	int
		iStat = 0, iRet = 0;
	BYTE
		*aBitmapBits, pad[3] = {'\0', '\0', '\0'};
	int
		modder;
	LONG
		l1;
	RGBTRIPLE
		*aBitmapRGB;
	LONG
		biWidth = bm->bmi.bmiHeader.biWidth;


	switch (bm->bmi.bmiHeader.biBitCount)
	{
	case 24:
		aBitmapRGB = (RGBTRIPLE*) &(bm->aBitmapBits[3*biWidth*row]);
		iStat = fwrite(aBitmapRGB, 3, biWidth, fp);
		modder = (3 * biWidth) % 4;
		if (modder > 0) fwrite(pad, 1, (4 - modder), fp);
		break;
	case 8:
		aBitmapBits = (BYTE*) &(bm->aBitmapBits[biWidth*row]);
		iStat = fwrite(aBitmapBits, 1, biWidth, fp);
		modder = biWidth % 4;
		if (modder > 0) fwrite(pad, 1, (4 - modder), fp);
		break;
	case 4:
		aBitmapBits = (BYTE*) &(bm->aBitmapBits[(biWidth+1)/2*row]);
		iStat = fwrite(aBitmapBits, 1, (biWidth+1)/2, fp);
		modder = ((biWidth+1)/2)%4;
		if (modder > 0) fwrite(pad, 1, (4 - modder), fp);
		break;
	case 1:
		aBitmapBits = (BYTE*) &(bm->aBitmapBits[(biWidth+7)/8*row]);
 		iStat = 8 * fwrite(aBitmapBits, 1, (biWidth+7)/8, fp);
		modder = ((biWidth+7)/8)%4;
		if (modder > 0) fwrite(pad, 1, (4 - modder), fp);
		break;
	default:
		aBitmapBits = (BYTE*) &(bm->aBitmapBits[biWidth*row]);
 		iStat = fwrite(aBitmapBits, 1, biWidth, fp);
		modder = biWidth%4;
		if (modder > 0) fwrite(pad, 1, (4 - modder), fp);
		break;
	}

	return iRet;
}

int writeBitmapImage( BITMAPIMAGE *bm, FILE *fp )
{
	int
		iRet = 0;
	LONG
		l1;


	for (l1 = 0; l1 < bm->bmi.bmiHeader.biHeight; ++l1)
	{
		writeBitmapRow(bm, l1, fp);
	}
	return iRet;
}


int dumpBitmap( BITMAPIMAGE *bm )
{
	char
		*bfType;
	double
		d1, inchespermeter = BM_INCHESPERMETER;
	int
		iRet = 0;
	FILE
		*fpdump = stderr;

	bfType = (char*) &(bm->bmfh.bfType);
	fprintf(fpdump, "bmfh.bfType = \"%c%c\"\n", bfType[0], bfType[1]);
	fprintf(fpdump, "bmfh.bfSize = %d\n", bm->bmfh.bfSize);
	fprintf(fpdump, "bmfh.bfReserved1 = %d\n", bm->bmfh.bfReserved1);
	fprintf(fpdump, "bmfh.bfReserved2 = %d\n", bm->bmfh.bfReserved2);
	fprintf(fpdump, "bmfh.bfOffBits = %d\n", bm->bmfh.bfOffBits);

	fprintf(fpdump, "bmi.bmiHeader.biSize = %d\n", bm->bmi.bmiHeader.biSize);
	fprintf(fpdump, "bmi.bmiHeader.biWidth = %d\n", bm->bmi.bmiHeader.biWidth);
	fprintf(fpdump, "bmi.bmiHeader.biHeight = %d\n", bm->bmi.bmiHeader.biHeight);
	fprintf(fpdump, "bmi.bmiHeader.biPlanes = %d\n", bm->bmi.bmiHeader.biPlanes);
	fprintf(fpdump, "bmi.bmiHeader.biBitCount = %d\n", bm->bmi.bmiHeader.biBitCount);
	fprintf(fpdump, "bmi.bmiHeader.biCompression = %d", bm->bmi.bmiHeader.biCompression);
	switch (bm->bmi.bmiHeader.biCompression)
	{
	case BI_RGB:
		if (bm->bmi.bmiHeader.biBitCount != 24)
		{
			fputs(" (Gray or raw)\n", fpdump);
		}
		else
		{
			fputs(" (BI_RGB)\n", fpdump);
		}
		break;
	case BI_RLE8:
		fputs(" (BI_RLE8 (Not supported))\n", fpdump);
		break;
	case BI_RLE4:
		fputs(" (BI_RLE4 (Not supported))\n", fpdump);
		break;
	default:
		fputs(" (Unknown compression!)\n", fpdump);
		break;
	}
	fprintf(fpdump, "bmi.bmiHeader.biSizeImage = %d\n", bm->bmi.bmiHeader.biSizeImage);
	d1 = (double) bm->bmi.bmiHeader.biXPelsPerMeter / inchespermeter;
	fprintf(fpdump, "bmi.bmiHeader.biXPelsPerMeter = %d (%.1f DPI)\n", bm->bmi.bmiHeader.biXPelsPerMeter, d1);
	d1 = (double) bm->bmi.bmiHeader.biYPelsPerMeter / inchespermeter;
	fprintf(fpdump, "bmi.bmiHeader.biYPelsPerMeter = %d (%.1f DPI)\n", bm->bmi.bmiHeader.biYPelsPerMeter, d1);
	fprintf(fpdump, "bmi.bmiHeader.biClrUsed = %d\n", bm->bmi.bmiHeader.biClrUsed);
	fprintf(fpdump, "bmi.bmiHeader.biClrImportant = %d\n", bm->bmi.bmiHeader.biClrImportant);

	if (bm->bmi.bmiColors)
	{
		fprintf(fpdump, "bmi.bmiColors has %d colors\n", bm->bmi.bmiHeader.biClrUsed);
	}

	if (bm->aBitmapBits)
	{
		fputs("aBitmapBits contains image data\n", fpdump);
	}

	return iRet;
}

LONG getBitmapWidth( BITMAPIMAGE *bm )
{
	if (bm)
		return bm->bmi.bmiHeader.biWidth;
	else
		return -1;
}

LONG getBitmapHeight( BITMAPIMAGE *bm )
{
	if (bm)
		return bm->bmi.bmiHeader.biHeight;
	else
		return -1;
}

WORD getBitmapBitCount( BITMAPIMAGE *bm )
{
	if (!bm)
	{
		return (WORD) 0;
	}
	return bm->bmi.bmiHeader.biBitCount;
}

BYTE *getBitmapBits( BITMAPIMAGE *bm )
{
	if (!bm)
	{
		return NULL;
	}
	return bm->aBitmapBits;
}

BITMAPIMAGE *createBitmapImage( LONG biWidth, LONG biHeight,
		WORD biBitCount,
		LONG biXPelsPerMeter, LONG biYPelsPerMeter,
		BYTE *aBitmapBits )
{
	BITMAPIMAGE
		*bm = NULL;
	char
		bfType[2] = {'B', 'M'};
	LONG
		arraysize, datasize;
	DWORD
		biClrUsed, l1;


	arraysize = biWidth * biHeight;
	datasize = arraysize * ((biBitCount + 7) / 8);

	bm = calloc(1, sizeof(BITMAPIMAGE));
	memcpy(&(bm->bmfh.bfType), bfType, 2);
	bm->bmfh.bfSize = (DWORD) (datasize + 54);
	bm->bmfh.bfOffBits = (DWORD) 54;

	bm->bmi.bmiHeader.biSize = (DWORD) 40;
	bm->bmi.bmiHeader.biWidth = biWidth;
	bm->bmi.bmiHeader.biHeight = biHeight;
	bm->bmi.bmiHeader.biPlanes = (WORD) 1;
	bm->bmi.bmiHeader.biBitCount = biBitCount;
	bm->bmi.bmiHeader.biCompression = (DWORD) BI_RGB;
	bm->bmi.bmiHeader.biSizeImage = (DWORD) datasize;
	bm->bmi.bmiHeader.biXPelsPerMeter = biXPelsPerMeter;
	bm->bmi.bmiHeader.biYPelsPerMeter = biYPelsPerMeter;
	if (biBitCount <= 8)
	{
		biClrUsed = (DWORD) ipow2(biBitCount);
		bm->bmi.bmiHeader.biClrUsed = biClrUsed;
		bm->bmi.bmiColors = calloc(biClrUsed, sizeof(RGBQUAD));
		bm->bmfh.bfSize += bm->bmi.bmiHeader.biClrUsed * 4;
		bm->bmfh.bfOffBits += bm->bmi.bmiHeader.biClrUsed * 4;
		for (l1 = 0; l1 < biClrUsed; ++l1)
		{
			bm->bmi.bmiColors[l1].rgbBlue = (BYTE) l1;
			bm->bmi.bmiColors[l1].rgbGreen = (BYTE) l1;
			bm->bmi.bmiColors[l1].rgbRed = (BYTE) l1;
			bm->bmi.bmiColors[l1].rgbReserved = (BYTE) 0;
		}
	}
	else
	{
		bm->bmi.bmiHeader.biClrUsed = (DWORD) 0;
	}
	bm->bmi.bmiHeader.biClrImportant = (DWORD) 0;

	bm->aBitmapBits = calloc(datasize, sizeof(BYTE));
	if (aBitmapBits)
	{
		memcpy(bm->aBitmapBits, aBitmapBits, datasize);
	}

	return bm;
}

void destroyBitmapImage(BITMAPIMAGE *bm)
{
	if (bm->aBitmapBits)
	{
		free(bm->aBitmapBits);
	}
	if (bm->bmi.bmiColors)
	{
		free(bm->bmi.bmiColors);
	}
}

LONG getBitmapColorIndex( BITMAPIMAGE *bm, RGBQUAD pix )
{
	DWORD
		l1;


	l1 = bm->bmi.bmiHeader.biClrUsed;
	if (l1 == 0)
	{
		if ((pix.rgbRed == pix.rgbGreen)
				&& (pix.rgbGreen == pix.rgbBlue))
		{
			return pix.rgbRed;
		}
		return -1;
	}
	for (l1 = 0; l1 < bm->bmi.bmiHeader.biClrUsed; ++l1)
	{
		if ((bm->bmi.bmiColors[l1].rgbRed == pix.rgbRed)
			&& (bm->bmi.bmiColors[l1].rgbGreen == pix.rgbGreen)
			&& (bm->bmi.bmiColors[l1].rgbBlue == pix.rgbBlue))
		{
			return l1;
		}
	}
	return -1;
}

RGBQUAD getBitmapPix( BITMAPIMAGE *bm, LONG pos )
{
	BYTE
		*aBitmapBits = bm->aBitmapBits;
	RGBQUAD
		pix, *bmiColors = bm->bmi.bmiColors;


	switch (bm->bmi.bmiHeader.biBitCount)
	{
		case 24:
			memcpy(&pix, &(aBitmapBits[3*pos]), 3);
			pix.rgbReserved = (BYTE) 0;
			break;
		case 8:
			if (bmiColors)
			{
				memcpy(&pix, &(bmiColors[aBitmapBits[pos]]), 4);
			}
			else
			{
				pix.rgbRed = pix.rgbGreen = pix.rgbBlue = aBitmapBits[pos];
			}
			break;
		case 4:
			if (bmiColors)
			{
				if (pos%2 == 0)
				{
					memcpy(&pix, &(bmiColors[(aBitmapBits[pos/2]>>4)&0xf]), 4);
				}
				else
				{
					memcpy(&pix, &(bmiColors[(aBitmapBits[pos/2])&0xf]), 4);
				}
			}
			else
			{
				if (pos%2 == 0)
				{
					pix.rgbRed = pix.rgbGreen = pix.rgbBlue = aBitmapBits[pos/2]&0xf;
				}
				else
				{
					pix.rgbRed = pix.rgbGreen = pix.rgbBlue = (aBitmapBits[pos/2]>>4)&0xf;
				}
			}
			break;
		default:
			if (bmiColors)
			{
				memcpy(&pix, &(bmiColors[aBitmapBits[pos]]), 4);
			}
			else
			{
				pix.rgbRed = pix.rgbGreen = pix.rgbBlue = aBitmapBits[pos];
			}
			break;
	}
	return pix;
}

void putBitmapPix( BITMAPIMAGE *bm, LONG pos, RGBQUAD pix )
{
	BYTE
		*aBitmapBits = bm->aBitmapBits;
	LONG
		index;
	RGBQUAD
		*bmiColors = bm->bmi.bmiColors;


	pix.rgbReserved = (BYTE) 0;
	switch (bm->bmi.bmiHeader.biBitCount)
	{
		case 24:
			memcpy(&(aBitmapBits[3 * pos]), &pix, 3);
			break;
		case 8:
			index = getBitmapColorIndex(bm, pix);
			if (index != -1)
			{
				aBitmapBits[pos] = index;
			}
			break;
		default:
			index = getBitmapColorIndex(bm, pix);
			if (index != -1)
			{
				aBitmapBits[pos] = index;
			}
			break;
	}
}


RGBQUAD getBitmapXY( BITMAPIMAGE *bm, LONG x, LONG y )
{
	BYTE
		*aBitmapBits = bm->aBitmapBits;
	LONG
		pos;
	RGBQUAD
		pix, *bmiColors = bm->bmi.bmiColors;


	pos = (bm->bmi.bmiHeader.biHeight - y - 1) * bm->bmi.bmiHeader.biWidth + x;
	pix = getBitmapPix(bm, pos);
	return pix;
}

void putBitmapXY( BITMAPIMAGE *bm, LONG x, LONG y, RGBQUAD pix )
{
	BYTE
		*aBitmapBits = bm->aBitmapBits;
	LONG
		index, pos;
	RGBQUAD
		*bmiColors = bm->bmi.bmiColors;


	pos = (bm->bmi.bmiHeader.biHeight - y - 1) * bm->bmi.bmiHeader.biWidth + x;
	putBitmapPix(bm, pos, pix);
}

void setBitmapPelsPerMeter( BITMAPIMAGE *bm,
		LONG biXPelsPerMeter, LONG biYPelsPerMeter )
{
	bm->bmi.bmiHeader.biXPelsPerMeter = biXPelsPerMeter;
	bm->bmi.bmiHeader.biYPelsPerMeter = biYPelsPerMeter;
}

RGBQUAD setpix(BYTE red, BYTE green, BYTE blue)
{
	RGBQUAD
		pix = {red, green, blue, 0};

	return pix;
}

double getrealred(RGBQUAD pix)
{
	double
		d = (double) pix.rgbRed;

	return (d/255.0);
}

double getrealgreen(RGBQUAD pix)
{
	double
		d = (double) pix.rgbGreen;

	return (d/255.0);
}

double getrealblue(RGBQUAD pix)
{
	double
		d = (double) pix.rgbBlue;

	return (d/255.0);
}

