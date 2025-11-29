#ifndef _Matrix_H_
#define _Matrix_H_

#define _Matrix_Version_ "Matrix 2014-10-06 10:32\n  Copyright (c) Terry Ron Vantreese\n  2001, 2002, 2003, 2004, 2005, 2006, 2008, 2010, 2014\n"

#include <stdio.h>

// 601-1: 2.9891165792705754e-01  5.8661071874886872e-01  1.1447762332407385e-01
#define gray_601_1(r, g, b)	(2.9891165792705754e-01*(r)+5.8661071874886872e-01*(g)+1.1447762332407385e-01*(b))

// 709:  2.1264934272065303e-01  7.1516913570590368e-01  7.2181521573443333e-02
#define gray_709(r, g, b)	(2.1264934272065303e-01*(r)+7.1516913570590368e-01*(g)+7.2181521573443333e-02*(b))

// ITU:  2.2201465283108743e-01  7.0665521742369075e-01  7.1330129745222015e-02
#define gray_itu(r, g, b)	(2.2201465283108743e-01*(r)+7.0665521742369075e-01*(g)+7.1330129745222015e-02*(b))



typedef union
{
	unsigned char c;
	struct _bit
	{
		unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
	} bit;
} byte;


extern unsigned char *mtx_random(void *v, int rndsize);
extern int genmtxrnd(int m);
extern long long genmtxrnd64(long long m);
extern int fgettok(FILE *fp, char buf[], size_t bufsize, char atok[]);
extern int fgetchr(FILE *fp, int chr);
extern void getGZipVersion(void);
extern FILE *GZcat(char *filename);
extern Matrix_t *importBitmap(FILE *fp);
extern Matrix_t *importPBM(FILE *fp);
extern Matrix_t *importPGM(FILE *fp);
extern Matrix_t *importPPM(FILE *fp);
extern Matrix_t *importTerragen(FILE *fp);
extern Matrix_t *GetMatrix(char *Param);
extern void PutMatrix(char *Param, Matrix_t *C, char *arg);
extern void PutMatrixToString(Matrix_t *C, char *arg);
extern Matrix_t *matrix_Transpose(Matrix_t *A);
extern Matrix_t *matrix_Dot(Matrix_t *A, Matrix_t *B);
extern Matrix_t *matrix_Reverse(Matrix_t *A);
extern Matrix_t *matrix_Normal(Matrix_t *A);
extern void exportPGM(FILE *fp, Matrix_t *A);
extern void exportPGM2(FILE *fp, Matrix_t *A);
extern Matrix_t *matrix_Transform(Matrix_t *M,
		double x1, double y1, double x2, double y2, double x3, double y3,
		double xp1, double yp1, double xp2, double yp2, double xp3, double yp3,
		int height, int width);

#endif
