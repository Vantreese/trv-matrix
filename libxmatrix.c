#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <esunlib.h>

#include "matrix.h"
//#include "bmplib.h"
//#include "terralib.h"



char *GZipVersion = NULL;



int genmtxrnd(int m)
{
	unsigned int rr;

	mtx_random(&rr, sizeof(unsigned int));
	return ((rr & 0x7fffffff) % m);
}

long long genmtxrnd64(long long m)
{
	unsigned long long rr;

	mtx_random(&rr, sizeof(unsigned long long));
	return ((rr & 0x7fffffffffffffff) % m);
}

int fgettok(FILE *fp, char buf[], size_t bufsize, char atok[])
{
	char *cp;
	int cc, ic=0;
	size_t l1;

	if (feof(fp)) return -1;
	for (l1 = 0; l1 < bufsize; ++l1)
	{
		cc = getc(fp);
		if (cc == -1)
		{
			buf[ic] = '\0';
			return ic;
		}
		cp = strchr(atok, cc);
		if (cp)
		{
			buf[ic] = '\0';
			return ic;
		}
		buf[ic++] = (char) cc;
	}
	return ic;
}

int fgetchr(FILE *fp, int chr)
{
	int cc;

	do
	{
		cc = getc(fp);
	} while ((cc != chr) && (cc != -1));
	return cc;
}

int fileexist(char *filename)
{
	struct stat filestat;

	if (!filename) return -1;

	return stat(filename, &filestat);
}

void getGZipVersion(void)
{
	char Line[BUFSIZ], *sz;
	FILE *pp = popen("gzip -V 2>&1", "r");

	if (!pp) return;

	sz = fgets(Line, BUFSIZ, pp);
	if (sz) GZipVersion = strdup(Line);
	pclose(pp);
}

int GZstat(char *filename)
{
	int istat;
	char gzfilename[FILENAME_MAX];

	if (!GZipVersion) return -1;
	if (!filename) return -1;

//	istat = fileexist(filename);
//	if (istat == -1)
//	{
		sprintf(gzfilename, "%s.gz", filename);
		istat = fileexist(gzfilename);
//	}
	return istat;
}

int GUnZip(char *filename)
{
	int istat = 0;
	char gzcmd[BUFSIZ];

	if (!GZipVersion) return 1;
	if (!filename) return 1;
	if (GZstat(filename) == -1) return 1;

	sprintf(gzcmd, "gzip -d %s\n", filename);
	istat = system(gzcmd);
	return istat;
}

int GZip(char *filename)
{
	int istat = 0;
	char gzcmd[BUFSIZ];

	if (!GZipVersion) return 1;
	if (!filename) return 1;
	if (GZstat(filename) == -1) return 1;

	sprintf(gzcmd, "gzip %s\n", filename);
	istat = system(gzcmd);
	return istat;
}

char *GZipList(char *filename)
{
	char Line[BUFSIZ] = {'\0'}, *gzout = NULL;
	char gzcmd[BUFSIZ];
	FILE *gzfile;

	if (!GZipVersion) return NULL;
	if (!filename) return NULL;
	if (GZstat(filename) == -1) return NULL;

	sprintf(gzcmd, "gzip -l %s", filename);
	gzfile = popen(gzcmd, "rt");
	if (!gzfile) return NULL;
	fgets(Line, BUFSIZ, gzfile);
	fgets(Line, BUFSIZ, gzfile);
	pclose(gzfile);
	if (strlen(Line) > 0)
	{
		gzout = strdup(Line);
	}
	return gzout;
}

FILE *GZcat(char *filename)
{
	FILE *cat = NULL;
	char gzcmd[BUFSIZ];
	char *gzlist;

	if (!GZipVersion) return NULL;
	if (!filename) return NULL;
	if (GZstat(filename) == -1) return NULL;

	gzlist = GZipList(filename);
	if (gzlist)
	{
		free(gzlist);
		sprintf(gzcmd, "gzip -cd %s", filename);
		cat = popen(gzcmd, "rb");
	}
	return cat;
}

Matrix_t *importPBM(FILE *fp)
{
	Matrix_t *A;
	int istat;
	char Line[80] = {'\0'};
	char WhiteSP[] = " \r\n\t";
	int width, height;
	int iloop, jloop;
	byte pixvalue;
	double value;

	istat = fgettok(fp, Line, 80, WhiteSP);
	if ((istat == -1) || strcmp(Line, "P4")) return NULL;

	do
	{
		if (Line[0] == '#') istat = fgetchr(fp, '\n');
		while ((istat = fgettok(fp, Line, 80, WhiteSP)) == 0);
		if (istat == -1) return NULL;
	} while (Line[0] == '#');
	width = atoi(Line);

	do
	{
		if (Line[0] == '#') istat = fgetchr(fp, '\n');
		while ((istat = fgettok(fp, Line, 80, WhiteSP)) == 0);
		if (istat == -1) return NULL;
	} while (Line[0] == '#');
	height = atoi(Line);

	A = mtx_Create(NULL, height, width);
	for (iloop = 1; iloop <= height; ++iloop)
	{
		for (jloop = 1; jloop <= width; ++jloop)
		{
			switch ((jloop-1)%8)
			{
			case 0:
				fread(&(pixvalue.c), 1, 1, fp);
				value = (double) pixvalue.bit.b7;
				break;
			case 1:
				value = (double) pixvalue.bit.b6;
				break;
			case 2:
				value = (double) pixvalue.bit.b5;
				break;
			case 3:
				value = (double) pixvalue.bit.b4;
				break;
			case 4:
				value = (double) pixvalue.bit.b3;
				break;
			case 5:
				value = (double) pixvalue.bit.b2;
				break;
			case 6:
				value = (double) pixvalue.bit.b1;
				break;
			case 7:
				value = (double) pixvalue.bit.b0;
				break;
			}
			mtx_In(A, iloop, jloop, (1.0-value));
		}
	}

	return A;
}

Matrix_t *importPGM(FILE *fp)
{
	Matrix_t *A;
	int istat;
	char Line[80] = {'\0'};
	char WhiteSP[] = " \r\n\t";
	int width, height, maxvalue;
	int iloop, jloop;
	int pixvalue = 0;
	double value;

	istat = fgettok(fp, Line, 80, WhiteSP);
	if ((istat == -1) || strcmp(Line, "P5")) return NULL;

	do
	{
		if (Line[0] == '#') istat = fgetchr(fp, '\n');
		while ((istat = fgettok(fp, Line, 80, WhiteSP)) == 0);
		if (istat == -1) return NULL;
	} while (Line[0] == '#');
	width = atoi(Line);

	do
	{
		if (Line[0] == '#') istat = fgetchr(fp, '\n');
		while ((istat = fgettok(fp, Line, 80, WhiteSP)) == 0);
		if (istat == -1) return NULL;
	} while (Line[0] == '#');
	height = atoi(Line);

	do
	{
		if (Line[0] == '#') istat = fgetchr(fp, '\n');
		while ((istat = fgettok(fp, Line, 80, WhiteSP)) == 0);
		if (istat == -1) return NULL;
	} while (Line[0] == '#');
	maxvalue = atoi(Line);

	A = mtx_Create(NULL, height, width);
	for (iloop = 1; iloop <= height; ++iloop)
	{
		for (jloop = 1; jloop <= width; ++jloop)
		{
			if (maxvalue > 255)
			{
				pixvalue = fgetBE(fp);
			}
			else
			{
				fread(&pixvalue, 1, 1, fp);
			}
			value = (double) pixvalue;
			value /= (double) maxvalue;
			mtx_In(A, iloop, jloop, value);
		}
	}

	return A;
}

Matrix_t *importPPM(FILE *fp)
{
	Matrix_t *A;
	int istat;
	char Line[80] = {'\0'};
	char WhiteSP[] = " \r\n\t";
	int width, height, maxvalue;
	int iloop, jloop;
	int r = 0, g = 0, b = 0;
	double dr, dg, db;

	istat = fgettok(fp, Line, 80, WhiteSP);
	if ((istat == -1) || strcmp(Line, "P6")) return NULL;

	do
	{
		if (Line[0] == '#') istat = fgetchr(fp, '\n');
		while ((istat = fgettok(fp, Line, 80, WhiteSP)) == 0);
		if (istat == -1) return NULL;
	} while (Line[0] == '#');
	width = atoi(Line);

	do
	{
		if (Line[0] == '#') istat = fgetchr(fp, '\n');
		while ((istat = fgettok(fp, Line, 80, WhiteSP)) == 0);
		if (istat == -1) return NULL;
	} while (Line[0] == '#');
	height = atoi(Line);

	do
	{
		if (Line[0] == '#') istat = fgetchr(fp, '\n');
		while ((istat = fgettok(fp, Line, 80, WhiteSP)) == 0);
		if (istat == -1) return NULL;
	} while (Line[0] == '#');
	maxvalue = atoi(Line);

	A = mtx_Create(NULL, height, width);
	for (iloop = 1; iloop <= height; ++iloop)
	{
		for (jloop = 1; jloop <= width; ++jloop)
		{
			if (maxvalue > 255)
			{
				r = fgetBE(fp);
				g = fgetBE(fp);
				b = fgetBE(fp);
			}
			else
			{
				fread(&r, 1, 1, fp);
				fread(&g, 1, 1, fp);
				fread(&b, 1, 1, fp);
			}
			dr = (double) r;
			dr /= (double) maxvalue;
			dg = (double) g;
			dg /= (double) maxvalue;
			db = (double) b;
			db /= (double) maxvalue;

			mtx_In(A, iloop, jloop, gray_601_1(dr, dg, db));
		}
	}

	return A;
}

void PutMatrixToString(Matrix_t *C, char *arg)
{
	if (arg)
	{
		fprintf(stdout, "%s: ", arg);
	}
	fputMatrix("a", C, stdout);
}

void PutMatrix(char *Param, Matrix_t *C, char *arg)
{
	FILE	*outf;

	if (!Param) outf = fopen("matrix.mtx", "wb");
	else outf = fopen(Param, "wb");
	if (!outf)
	{
		fprintf(stderr, "Error in %s\n", Param);
		return;
	}
	if (!Param)
	{
		PutMatrixToString(C, arg);
	}
	fputMatrix("b", C, outf);
	fclose(outf);
}

Matrix_t *matrix_Transpose(Matrix_t *A)
{
	Matrix_t
		*C;
	size_t
		i, j;

	C = mtx_Create(NULL, A->j, A->i);
	if (C)
	{
		for (i = 0; i < C->i; ++i)
		{
			for (j = 0; j < C->j; ++j)
			{
				C->a[i][j] = A->a[j][i];
			}
		}
	}
	return C;
}

Matrix_t *matrix_Dot(Matrix_t *A, Matrix_t *B)
{
	Matrix_t
		*C;
	size_t
		i, j;

	if (A->i != B->i) return NULL;
	if (A->j != B->j) return NULL;
	C = mtx_Create(NULL, A->i, A->j);
	if (C)
	{
		for (i = 0; i < C->i; ++i)
		{
			for (j = 0; j < C->j; ++j)
			{
				C->a[i][j] = A->a[i][j]*B->a[i][j];
			}
		}
	}
	return C;
}

Matrix_t *matrix_Reverse(Matrix_t *A)
{
	Matrix_t
		*C;
	double
		lo, hi;
	int
		iloop, jloop, i, j;

	mtx_Range(A, &lo, &hi);
	C = mtx_Create(NULL, A->i, A->j);
	for (iloop = 1; iloop <= A->i; ++iloop)
	{
		for (jloop = 1; jloop <= A->j; ++jloop)
		{
			mtx_In(C, iloop, jloop, (hi-mtx_Out(A, iloop, jloop)+lo));
		}
	}
	return C;
}

Matrix_t *matrix_Normal(Matrix_t *A)
{
	Matrix_t *C;
	double lo, hi;
	int iloop, jloop, i, j;

	mtx_Range(A, &lo, &hi);
	C = mtx_Create(NULL, A->i, A->j);
	for (iloop = 1; iloop <= A->i; ++iloop)
	{
		for (jloop = 1; jloop <= A->j; ++jloop)
		{
			mtx_In(C, iloop, jloop, ((mtx_Out(A, iloop, jloop)-lo)/(hi-lo)));
		}
	}
	return C;
}

void exportPGM(FILE *fp, Matrix_t *A)
{
	int i, j;
	int iloop, jloop;
	double lo = 0.0, hi = 1.0;
	double pgmdvalue;
	int pgmvalue;
	double hivalue = 256.0;
	double testvalue;

	i = mtx_Height(A);
	j = mtx_Width(A);
	fprintf(fp, "P5\n%d %d\n255\n", j, i);
	for (iloop = 1; iloop <= i; ++iloop)
	{
		for (jloop = 1; jloop <= j; ++jloop)
		{
			testvalue = mtx_Out(A, iloop, jloop) * hivalue;
			pgmdvalue = (testvalue >= hivalue) ? (hivalue - 1.0) : (testvalue);
			pgmvalue = (int) pgmdvalue;
			fputc(pgmvalue, fp);
		}
	}
}


void exportPGM2(FILE *fp, Matrix_t *A)
{
	int i, j;
	int iloop, jloop;
	double lo = 0.0, hi = 1.0;
	double pgmdvalue;
	int pgmvalue;
	double hivalue = 65536.0;
	double testvalue;

	i = mtx_Height(A);
	j = mtx_Width(A);
	fprintf(fp, "P5\n%d %d\n65535\n", j, i);
	for (iloop = 1; iloop <= i; ++iloop)
	{
		for (jloop = 1; jloop <= j; ++jloop)
		{
			testvalue = mtx_Out(A, iloop, jloop) * hivalue;
			pgmdvalue = (testvalue >= hivalue) ? (hivalue - 1.0) : (testvalue);
			pgmvalue = (int) pgmdvalue;
			fputBE(pgmvalue, fp);
		}
	}
}


void exportPGM12(FILE *fp, Matrix_t *A)
{
	int i, j;
	int iloop, jloop;
	double lo = 0.0, hi = 1.0;
	double pgmdvalue;
	int pgmvalue;
	double hivalue = 4096.0;
	double testvalue;

	i = mtx_Height(A);
	j = mtx_Width(A);
	fprintf(fp, "P5\n%d %d\n4095\n", j, i);
	for (iloop = 1; iloop <= i; ++iloop)
	{
		for (jloop = 1; jloop <= j; ++jloop)
		{
			testvalue = mtx_Out(A, iloop, jloop) * hivalue;
			pgmdvalue = (testvalue >= hivalue) ? (hivalue - 1.0) : (testvalue);
			pgmvalue = (int) pgmdvalue;
			fputBE(pgmvalue, fp);
		}
	}
}


