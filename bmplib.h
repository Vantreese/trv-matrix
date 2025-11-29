#ifndef _BMPLIB_H_
#define _BMPLIB_H_

#include <stdio.h>
#include "bmpdefs.h"


/*
 * For reference: a *.BMP format.
 *
 * BITMAPFILEHEADER    bmfh;
 * BITMAPINFOHEADER    bmih;
 * RGBQUAD             aColors[];
 * BYTE                aBitmapBits[];
 *
 */



extern int readBitmapFileHeader( BITMAPIMAGE *bm, FILE *fp );
extern int readBitmapInfoHeader( BITMAPIMAGE *bm, FILE *fp );
extern int readBitmapInfoColors( BITMAPIMAGE *bm, FILE *fp );
extern int readBitmapRow( BITMAPIMAGE *bm, LONG row, FILE *fp );
extern int readBitmapImage( BITMAPIMAGE *bm, FILE *fp );

extern int writeBitmapFileHeader( BITMAPIMAGE *bm, FILE *fp );
extern int writeBitmapInfoHeader( BITMAPIMAGE *bm, FILE *fp );
extern int writeBitmapInfoColors( BITMAPIMAGE *bm, FILE *fp );
extern int writeBitmapRow( BITMAPIMAGE *bm, LONG row, FILE *fp );
extern int writeBitmapImage( BITMAPIMAGE *bm, FILE *fp );

extern int dumpBitmap( BITMAPIMAGE *bm );

extern LONG getBitmapWidth( BITMAPIMAGE *bm );
extern LONG getBitmapHeight( BITMAPIMAGE *bm );
extern WORD getBitmapBitCount( BITMAPIMAGE *bm );
extern BYTE *getBitmapBits( BITMAPIMAGE *bm );
extern BITMAPIMAGE *createBitmapImage( LONG biWidth, LONG biHeight, WORD biBitCount, LONG biXPelsPerMeter, LONG biYPelsPerMeter, BYTE *aBitmapBits );
extern void destroyBitmapImage(BITMAPIMAGE *bm);
extern LONG getBitmapColorIndex( BITMAPIMAGE *bm, RGBQUAD pix );
extern RGBQUAD getBitmapPix( BITMAPIMAGE *bm, LONG pos );
extern RGBQUAD getBitmapXY( BITMAPIMAGE *bm, LONG x, LONG y );
extern void putBitmapPix( BITMAPIMAGE *bm, LONG pos, RGBQUAD pix );
extern void putBitmapXY( BITMAPIMAGE *bm, LONG x, LONG y, RGBQUAD pix );

extern void setBitmapPelsPerMeter( BITMAPIMAGE *bm, LONG biXPelsPerMeter, LONG biYPelsPerMeter );
extern RGBQUAD setpix(BYTE red, BYTE green, BYTE blue);
extern double getrealred(RGBQUAD pix);
extern double getrealgreen(RGBQUAD pix);
extern double getrealblue(RGBQUAD pix);


#endif
