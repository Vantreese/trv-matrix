#ifndef _BMPDEFS_H_
#define _BMPDEFS_H_

/*
 * Start of BMP section.
 */



#define BI_RGB  0
#define BI_RLE8 1
#define BI_RLE4 2

/*
 * Defined by Windows OS (windows.h, etc)
 */

typedef unsigned char	BYTE;
typedef unsigned long	DWORD;
typedef unsigned short	UINT;
/* Previously "UINT = unsigned int", however UINT represents a 16-bit
 * unsigned integer and "unsigned int" recently was upgraded to 32-bit
 * unsigned integer.
 */
typedef unsigned short	WORD;
typedef long		LONG;

typedef struct tagRGBTRIPLE
{
	BYTE
		rgbBlue, rgbGreen, rgbRed;
} RGBTRIPLE;

typedef struct tagRGBQUAD
{
	BYTE
		rgbBlue, rgbGreen, rgbRed, rgbReserved;
} RGBQUAD;


typedef struct tagBITMAPFILEHEADER
{
	UINT
		bfType;
	DWORD
		bfSize;
	UINT
		bfReserved1, bfReserved2;
	DWORD
		bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	DWORD
		biSize;
	LONG
		biWidth, biHeight;
	WORD
		biPlanes, biBitCount;
	DWORD
		biCompression, biSizeImage;
	LONG
		biXPelsPerMeter, biYPelsPerMeter;
	DWORD
		biClrUsed, biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER
		bmiHeader;
	RGBQUAD
		*bmiColors;
} BITMAPINFO;

typedef struct tagBITMAPIMAGE
{
	BITMAPFILEHEADER
		bmfh;
	BITMAPINFO
		bmi;
	BYTE
		*aBitmapBits;
} BITMAPIMAGE;

#define BM_INCHESPERMETER 39.37

#endif
