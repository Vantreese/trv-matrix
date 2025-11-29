#include <math.h>
#include <esunlib.h>

double fmin(double a, double b)
{
	return (a < b ? a : b);
}
double fmax(double a, double b)
{
	return (a > b ? a : b);
}
double fmin4(double a, double b, double c, double d)
{
	return fmin(fmin(a, b), fmin(c, d));
}
double fmax4(double a, double b, double c, double d)
{
	return fmax(fmax(a, b), fmax(c, d));
}

int inrange(Matrix_t *M, int i, int j)
{
	return (((i > 0) && (i <= mtx_Height(M)) && (j > 0) && (j <= mtx_Width(M))) ? 1 : 0);
}

double matrix_outpt(Matrix_t *M, double i, double j)
{
	int i1 = (int) floor(i);
	int j1 = (int) floor(j);
	int i2 = (int) ceil(i);
	int j2 = (int) ceil(j);
	double ilen = i-floor(i);
	double jlen = j-floor(j);
	double v = 0.0;

	if (inrange(M, i1, j1))
	{
		v += mtx_Out(M, i1, j1)*(1.0-ilen)*(1.0-jlen);
	}
	if (inrange(M, i1, j2) && (jlen > 0.0))
	{
		v += mtx_Out(M, i1, j2)*(1.0-ilen)*jlen;
	}
	if (inrange(M, i2, j1) && (ilen > 0.0))
	{
		v += mtx_Out(M, i2, j1)*ilen*(1.0-jlen);
	}
	if (inrange(M, i2, j2) && (ilen > 0.0) && (jlen > 0.0))
	{
		v += mtx_Out(M, i2, j2)*ilen*jlen;
	}
	return v;
}

/*
 * Return -1 if line AB does not cross pixel (ix, iy)
 * Return 0 if line AB crosses pixel (ix, iy)
 * Return 1 if one point of AB is within pixel (ix, iy)
 * Return 2 if two points of AB are within pixel (ix, iy)
 * Output cross points (xa, ya) and (xb, yb) on border of pixel or within pixel.
 */
/*
int crossrange(int ix, int iy, double Ax, double Ay, double Bx, double By, double *xa, double *ya, double *xb, double *yb)
{
	int pointA = 0, pointB = 0;
	int ret = -1;
	double x1 = (double) ix, y1 = (double) iy, x2 = (double) (ix+1.0), y2 = (double) (iy+1.0);
	double dx = (Bx-Ax), dy = (By-Ay);
	double dxp, dyp;

	*xa = *ya = 0.0;
	*xb = *yb = 0.0;

	if (insidepix(x1, y1, x2, y2, Ax, Ay))
	{
		pointA = 1;
		*xa = Ax;
		*ya = Ay;
	}
	if (insidepix(x1, y1, x2, y2, Bx, By))
	{
		pointB = 1;
		*xb = Bx;
		*yb = By;
	}
	if (pointA || pointB)
	{
		ret = pointA+pointB;
		if (pointA && pointB) return ret;
	}
	if (!pointA)
	{
		if (dx == 0.0)
		{
		}
		else
		{
			dxp = x2-Bx;
			dyp = dy/dx*dxp;
		}
	}
}
*/

/*
 * Transform a set (or a single) pixels into a single pixel with a single value.
 * Input:  (Ax, Ay), (Bx, By), (Cx, Cy), (Dx, Dy) -- 4 corners of a transformed pixel specified in original coordinates.
 * crossrange() is too difficult to develop, forced to simplify it with a lesser quality function.
 * Taking the average of the area covered by (lox, loy), (hix, hiy) will have to do for now, seems good and it's easy and fast.
 */
double transformpixeldata(double Ax, double Ay, double Bx, double By, double Cx, double Cy, double Dx, double Dy, Matrix_t *M)
{
	double lox, loy, hix, hiy, dlox, dloy, dhix, dhiy;
	int ilox, iloy, ihix, ihiy;
	int ilimit = mtx_Height(M), jlimit = mtx_Width(M);

	int xloop, yloop;
	double Area, Value = 0.0;
	double xlen, ylen;

	double iloop, jloop;


	lox = fmin4(Ax, Bx, Cx, Dx);
	hix = fmax4(Ax, Bx, Cx, Dx);

	loy = fmin4(Ay, By, Cy, Dy);
	hiy = fmax4(Ay, By, Cy, Dy);

	Area = (hix-lox)*(hiy-loy);

	dlox = floor(lox);
	dloy = floor(loy);
	dhix = ceil(hix);
	dhiy = ceil(hiy);
	ilox = (int) dlox;
	iloy = (int) dloy;
	ihix = (int) dhix;
	ihiy = (int) dhiy;



	iloop = loy;
	for (yloop = iloy; yloop < ihiy; ++yloop)
	{
		if ((yloop == iloy) && (yloop == ihiy-1))
		{
			ylen = hiy-loy;
		}
		else if (yloop == iloy)
		{
			ylen = 1.0-(loy-dloy);
		}
		else if (yloop == ihiy-1)
		{
			ylen = 1.0-(dhiy-hiy);
		}
		else
		{
			ylen = 1.0;
		}
		jloop = lox;
		for (xloop = ilox; xloop < ihix; ++xloop)
		{
			if ((xloop == ilox) && (xloop == ihix-1))
			{
				xlen = hix-lox;
			}
			else if (xloop == ilox)
			{
				xlen = 1.0-(lox-dlox);
			}
			else if (xloop == ihix-1)
			{
				xlen = 1.0-(dhix-hix);
			}
			else
			{
				xlen = 1.0;
			}

			Value += matrix_outpt(M, (iloop+1.0), (jloop+1.0))*xlen*ylen;
			jloop = floor(jloop+1.0);
		}
		iloop = floor(iloop+1.0);
	}
	Value /= Area;

	return Value;
}

Matrix_t *matrix_Transform(Matrix_t *M, double x1, double y1, double x2, double y2, double x3, double y3, double xp1, double yp1, double xp2, double yp2, double xp3, double yp3, int height, int width)
{
	Matrix_t *Out = mtx_Create(NULL, height, width);
	Matrix_t *Q = mtx_Create(NULL, 3, 3), *Qp = mtx_Create(NULL, 3, 3), *TransInv, *Qpi;
	Matrix_t *X1 = mtx_Create(NULL, 3, 1), *XP1 = mtx_Create(NULL, 3, 1);
	Matrix_t *X2 = mtx_Create(NULL, 3, 1), *XP2 = mtx_Create(NULL, 3, 1);
	Matrix_t *X3 = mtx_Create(NULL, 3, 1), *XP3 = mtx_Create(NULL, 3, 1);
	Matrix_t *X4 = mtx_Create(NULL, 3, 1), *XP4 = mtx_Create(NULL, 3, 1);
	double v;
	int iloop, jloop;


	mtx_In(X1, 3, 1, 1.0);
	mtx_In(X2, 3, 1, 1.0);
	mtx_In(X3, 3, 1, 1.0);
	mtx_In(X4, 3, 1, 1.0);

	mtx_In(Q, 1, 1, x1);
	mtx_In(Q, 2, 1, y1);
	mtx_In(Q, 3, 1, 1.0);
	mtx_In(Q, 1, 2, x2);
	mtx_In(Q, 2, 2, y2);
	mtx_In(Q, 3, 2, 1.0);
	mtx_In(Q, 1, 3, x3);
	mtx_In(Q, 2, 3, y3);
	mtx_In(Q, 3, 3, 1.0);

	mtx_In(Qp, 1, 1, xp1);
	mtx_In(Qp, 2, 1, yp1);
	mtx_In(Qp, 3, 1, 1.0);
	mtx_In(Qp, 1, 2, xp2);
	mtx_In(Qp, 2, 2, yp2);
	mtx_In(Qp, 3, 2, 1.0);
	mtx_In(Qp, 1, 3, xp3);
	mtx_In(Qp, 2, 3, yp3);
	mtx_In(Qp, 3, 3, 1.0);

	Qpi = mtx_Inverse(NULL, Qp, &v);
	TransInv = mtx_Multiply(NULL, Q, Qpi);

	for (iloop = 0; iloop < height; ++iloop)
	{
		for (jloop = 0; jloop < width; ++jloop)
		{
			mtx_In(X1, 1, 1, (double) jloop);
			mtx_In(X1, 2, 1, (double) iloop);
			mtx_In(X2, 1, 1, (double) (jloop+1));
			mtx_In(X2, 2, 1, (double) iloop);
			mtx_In(X3, 1, 1, (double) (jloop+1));
			mtx_In(X3, 2, 1, (double) (iloop+1));
			mtx_In(X4, 1, 1, (double) jloop);
			mtx_In(X4, 2, 1, (double) (iloop+1));

			mtx_Multiply(XP1, TransInv, X1);
			mtx_Multiply(XP2, TransInv, X2);
			mtx_Multiply(XP3, TransInv, X3);
			mtx_Multiply(XP4, TransInv, X4);

			v = transformpixeldata(
					mtx_Out(XP1, 1, 1), mtx_Out(XP1, 2, 1),
					mtx_Out(XP2, 1, 1), mtx_Out(XP2, 2, 1),
					mtx_Out(XP3, 1, 1), mtx_Out(XP3, 2, 1),
					mtx_Out(XP4, 1, 1), mtx_Out(XP4, 2, 1),
					M);
			mtx_In(Out, iloop+1, jloop+1, v);
		}
	}
	return Out;
}
