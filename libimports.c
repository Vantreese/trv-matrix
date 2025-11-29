#include <string.h>
#include <sys/stat.h>
#include <esunlib.h>

#include "matrix.h"
#include "bmplib.h"
#include "terralib.h"

Matrix_t *importBitmap(FILE *fp)
{
	Matrix_t *A;
	int istat;
	BITMAPIMAGE bm;
	LONG bmwidth, bmheight;
	int iloop, jloop;
	double value, r, g, b;
	RGBQUAD pix;

	istat = readBitmapFileHeader(&bm, fp);
	if (istat > 0)
	{
		return NULL;
	}
	istat = readBitmapInfoHeader(&bm, fp);
	if (istat > 0)
	{
		return NULL;
	}
	istat = readBitmapInfoColors(&bm, fp);
	if (istat > 0)
	{
		return NULL;
	}
	istat = readBitmapImage(&bm, fp);
	if (istat > 0)
	{
		return NULL;
	}

	bmwidth = getBitmapWidth(&bm);
	bmheight = getBitmapHeight(&bm);

	A = mtx_Create(NULL, bmheight, bmwidth);
	for (iloop = 1; iloop <= bmheight; ++iloop)
	{
		for (jloop = 1; jloop <= bmwidth; ++jloop)
		{
			pix = getBitmapXY(&bm, (LONG)(jloop-1), (LONG)(iloop-1));
			r = getrealred(pix);
			g = getrealgreen(pix);
			b = getrealblue(pix);
			value = gray_601_1(r, g, b);
			mtx_In(A, iloop, jloop, value);
		}
	}

	destroyBitmapImage(&bm);
	return A;
}

Matrix_t *importTerragen(FILE *fp)
{
	Matrix_t *A;
	int istat;
	terrain_t ter;
	short xpts, ypts;
	int iloop, jloop;
	double value;

	initterrain(&ter);
	istat = readterrain(fp, &ter);
	if (istat > 0)
	{
		return NULL;
	}
	xpts = getterrainxpts(&ter);
	ypts = getterrainypts(&ter);
	A = mtx_Create(NULL, ypts, xpts);
	for (iloop = 1; iloop <= ypts; ++iloop)
	{
		for (jloop = 1; jloop <= xpts; ++jloop)
		{
			value = getterrainmeterxy(&ter, (jloop-1), (ypts-iloop));
			mtx_In(A, iloop, jloop, value);
		}
	}
	return A;
}

#define MagicKeySize 16
Matrix_t *GetMatrix(char *Param)
{
	FILE *in1 = NULL;
	Matrix_t *A;
	char *filename;
	int istat, pfile = 0;
	char MagicKey[MagicKeySize];
	size_t keysize;
	struct stat instat;

	filename = calloc(strlen(Param)+5, sizeof(char));
	strcpy(filename, Param);
	if (fileexist(filename) != -1)
	{
		in1 = fopen(filename, "rb");
	}
	if (!in1)
	{
		in1 = GZcat(filename);
		if (in1) pfile = 1;
	}
	if (!in1)
	{
		strcpy(filename, Param);
		strcat(filename, ".mtx");
		if (fileexist(filename) != -1)
		{
			in1 = fopen(filename, "rb");
		}
	}
	if (!in1)
	{
		in1 = GZcat(filename);
		if (in1) pfile = 1;
	}
	if (!in1)
	{
		strcpy(filename, Param);
		strcat(filename, ".txt");
		if (fileexist(filename) != -1)
		{
			in1 = fopen(filename, "rb");
		}
	}
	if (!in1)
	{
		in1 = GZcat(filename);
		if (in1) pfile = 1;
	}
	if (!in1)
	{
		fprintf(stderr, "Error in %s\n", Param);
		return NULL;
	}
	keysize = fread(MagicKey, 1, MagicKeySize, in1);
	if (pfile)
	{
		pclose(in1);
		in1 = GZcat(filename);
	}
	else rewind(in1);
	A = fgetMatrix(NULL, in1);
	if (!A)
	{
		if (pfile)
		{
			pclose(in1);
			in1 = GZcat(filename);
		}
		else rewind(in1);
		/* Probably not a Matrix format, try another format */
		if (0)
		{
		}
		else if ((keysize >= 2) && !strncmp(MagicKey, "BM", 2))
		{
			A = importBitmap(in1);
		}
		else if ((keysize >= 2) && !strncmp(MagicKey, "P4", 2))
		{
			A = importPBM(in1);
		}
		else if ((keysize >= 2) && !strncmp(MagicKey, "P5", 2))
		{
			A = importPGM(in1);
		}
		else if ((keysize >= 2) && !strncmp(MagicKey, "P6", 2))
		{
			A = importPPM(in1);
		}
		else if ((keysize >= 16) && !strncmp(MagicKey, "TERRAGENTERRAIN ", 16))
		{
			A = importTerragen(in1);
		}
		else
		{
			/* Unrecognized format */
		}
	}
	if (pfile) pclose(in1);
	else fclose(in1);
	free(filename);
	if (!A)
	{
		fprintf(stderr, "Could not load matrix %s\n", Param);
		return NULL;
	}
	return A;
}


