#include <esunlib.h>
#include "matrix.h"


int main(int argc, char *argv[])
{
	double dr;
	long long m;
	long long rr;
	int *ia, *ja;
	Matrix_t *A = NULL;
	size_t n, i, j, ii, jj, k, kk;

	if (argc > 1)
	{
		if (!strcmp(argv[1], "-help"))
		{
			fprintf(stderr, "%s <size> <high> <loops>\n", argv[0]);
			return 1;
		}
		n = atoi(argv[1]);
	}
	else
		n = 8;

	if (argc > 2)
		m = atoll(argv[2]);
	else
		m = 4;

	if (argc > 3)
		kk = atoi(argv[3]);
	else
		kk = 3;

	A = mtx_I(NULL, n);
	ia = calloc(n, sizeof(int));
	ja = calloc(n, sizeof(int));
	for (k = 1; k <= kk; ++k)
	{
		for (i = 0; i < n; ++i)
		{
			ia[i] = ja[i] = 0;
		}
		for (i = 0; i < n; ++i)
		{
			if ((n > 2) && (i == n - 2))
			{
				jj = n;
				for (ii = 0; (ii < n) && (jj == n); ++ii)
				{
					if (!ia[ii] && !ja[ii])
					{/* Found common. */
						do
						{
							jj = genmtxrnd(n);
						} while (ja[jj] || (jj == ii));
					}
				}
				--ii;
				if (jj == n)
				{
					do
					{
						ii = genmtxrnd(n);
					} while (ia[ii]);
					do
					{
						jj = genmtxrnd(n);
					} while (ja[jj] || (jj == ii));
				}
			}
			else
			{
				do
				{
					ii = genmtxrnd(n);
				} while (ia[ii]);
				do
				{
					jj = genmtxrnd(n);
				} while (ja[jj] || (jj == ii));
			}
			ia[ii] = ja[jj] = 1;
			do
			{
				rr = genmtxrnd64(m);
			} while (rr == 0);
			for (j = 0; j < n; ++j)
			{
				dr = (double) rr;
				A->a[ii][j] += dr * A->a[jj][j];
			}
		}
	}
	free(ia);
	free(ja);
	fputMatrixTryInt("I", 0.5, A, stdout);
	return 0;
}
