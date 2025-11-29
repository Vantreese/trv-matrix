#include <fcntl.h>
#include <string.h>
#include <esunlib.h>

#include "matrix.h"


/*	!	!	!	*/
#define matrix_NOP	0

#define matrix_ADD	20010001
#define matrix_DET	20010002
#define matrix_DIV	20010013
#define matrix_DOT	20010012
#define matrix_GAMMA	20081031
#define matrix_GEN	20010006
#define matrix_GXTRANS	20010019
#define matrix_HI	20010007
#define matrix_IMPORT	20010016
#define matrix_INV	20010003
#define matrix_LO	20010009
#define matrix_MUI	20010011
#define matrix_MUL	20010004
#define matrix_NORM	20050804
#define matrix_PGM	20010014
#define matrix_PGM12	20060218
#define matrix_PGM16	20010015
#define matrix_RANGE	20010020
#define matrix_REV	20010017
#define matrix_SCALE	20010021
#define matrix_SUB	20010005
#define matrix_T	20010010
#define matrix_TRANS	20010018
#define matrix_UNF	20010008


extern Public_t	Public;
extern char *GZipVersion;


int matrix_OP = matrix_NOP;


void matrix_errormsg()
{
	fputs("Try one of these:\n", stderr);
	fprintf(stderr, "%s -add <A_matrix> <B_matrix> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -det <A_matrix>\n", Public.Name);
	fprintf(stderr, "%s -div <A_matrix> <B_matrix> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -dot <A_matrix> <B_matrix> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -gamma <A_matrix> <scalar> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -gen <height> <width> <max> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -gxtrans <A_matrix> <x1> <y1> <x2> <y2> <x3> <y3> <xp1> <yp1> <xp2> <yp2> <xp3> <yp3> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -hi <A_matrix>\n", Public.Name);
	fprintf(stderr, "%s -import <A_data> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -inv <A_matrix> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -lo <A_matrix>\n", Public.Name);
	fprintf(stderr, "%s -mui <A_matrix> <B_matrix> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -mul <A_matrix> <B_matrix> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -norm <A_matrix> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -pgm <A_matrix> [<PGM_image>]\n", Public.Name);
	fprintf(stderr, "%s -pgm12 <A_matrix> [<PGM_image>]\n", Public.Name);
	fprintf(stderr, "%s -pgm16 <A_matrix> [<PGM_image>]\n", Public.Name);
	fprintf(stderr, "%s -range <A_matrix>\n", Public.Name);
	fprintf(stderr, "%s -rev <A_matrix> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -scale <A_matrix> <scalar> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -sub <A_matrix> <B_matrix> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -t <A_matrix> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -trans <A_matrix> <i1> <j1> <i2> <j2> <i3> <j3> <ip1> <jp1> <ip2> <jp2> <ip3> <jp3> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -unf <height> <width> <value> [<OUT_matrix>]\n", Public.Name);
	fprintf(stderr, "%s -ver\n", Public.Name);
}
void matrix_what()
{
	fputs(Public.Name, stderr);
	fputs(": I'm not sure what to do.\n", stderr);
}
Matrix_t *matrix_open(char *filename)
{
	Matrix_t *A;
	A = GetMatrix(filename);
	if (!A)
	{
		exit(1);
	}
	return A;
}


int main( char argc, char *argv[] ) {
	char **Param = &(argv[0]);
	int iParam = argc;
	int m, r;
	Matrix_t *A1, *A, *B, *B1, *C;
	size_t i, j, i1, j1;
	double e1, e2, Det, u;
	/* New variables for PGM */
	FILE *pgmout;
	/* End new variables */


	SetAllEnv(argc, argv);
	getGZipVersion();



	if (argc == 1)
	{
		matrix_errormsg();
		return 1;
	}
	Param = &(Param[1]);
	--iParam;
	if (!strcmp(Param[0], "-add"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_ADD;
	}
	else if (!strcmp(Param[0], "-det"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_DET;
	}
	else if (!strcmp(Param[0], "-div"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_DIV;
	}
	if (!strcmp(Param[0], "-dot"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_DOT;
	}
	else if (!strcmp(Param[0], "-gamma"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_GAMMA;
	}
	else if (!strcmp(Param[0], "-gen"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_GEN;
	}
	else if (!strcmp(Param[0], "-gxtrans"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_GXTRANS;
	}
	else if (!strcmp(Param[0], "-hi"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_HI;
	}
	else if (!strcmp(Param[0], "-import"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_IMPORT;
	}
	else if (!strcmp(Param[0], "-inv"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_INV;
	}
	else if (!strcmp(Param[0], "-lo"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_LO;
	}
	else if (!strcmp(Param[0], "-mui"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_MUI;
	}
	else if (!strcmp(Param[0], "-mul"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_MUL;
	}
	else if (!strcmp(Param[0], "-norm"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_NORM;
	}
	else if (!strcmp(Param[0], "-pgm"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_PGM;
	}
	else if (!strcmp(Param[0], "-pgm12"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_PGM12;
	}
	else if (!strcmp(Param[0], "-pgm16"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_PGM16;
	}
	else if (!strcmp(Param[0], "-range"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_RANGE;
	}
	else if (!strcmp(Param[0], "-rev"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_REV;
	}
	else if (!strcmp(Param[0], "-scale"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_SCALE;
	}
	else if (!strcmp(Param[0], "-sub"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_SUB;
	}
	else if (!strcmp(Param[0], "-t"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_T;
	}
	else if (!strcmp(Param[0], "-trans"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_TRANS;
	}
	else if (!strcmp(Param[0], "-unf"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_UNF;
	}
	else if (!strcmp(Param[0], "-ver"))
	{
		Param = &(Param[1]);
		--iParam;
		matrix_OP = matrix_NOP;
		fputs(_Matrix_Version_, stderr);
		fputs(_EagleSun_Library_Version_, stderr);
		if (GZipVersion)
		{
			fputs(GZipVersion, stderr);
		}
		return 0;
	}
	else if (Param[0][0] == '-')
	{
		Param = &(Param[1]);
		--iParam;
		matrix_what();
		matrix_errormsg();
		return 1;
	}


/*
 * Read matrix A
 */
	switch (matrix_OP)
	{
	case matrix_NOP:
		if (!iParam)
		{
			matrix_errormsg();
			return 1;
		}
		A = matrix_open(Param[0]);
		PutMatrixToString(A, Param[0]);
		break;
	case matrix_GEN:
	case matrix_UNF:
		 break;
	default:
		if (!iParam)
		{
			matrix_errormsg();
			return 1;
		}
		A = matrix_open(Param[0]);
	}


/*
 * Read/write other matrices
 */
	switch (matrix_OP)
	{
	case matrix_NOP:
		if (iParam >= 2)
		{
			PutMatrix(Param[1], A, Param[0]);
		}
		break;
	case matrix_ADD:
		if (iParam >= 2)
		{
			B = GetMatrix(Param[1]);
			if (!B)
			{
				return 1;
			}
		}
		else
		{
			matrix_errormsg();
			return 1;
		}
		C = mtx_Add(NULL, A, B);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 2)
			{
				PutMatrix(Param[2], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_DET:
		C = mtx_Inverse(A, A, &Det);
		fprintf(stdout, "%s: %.16e\n", &(argv[1][1]), Det);
		break;
	case matrix_DIV:
		if (iParam >= 2)
		{
			B = GetMatrix(Param[1]);
			if (!B)
			{
				return 1;
			}
		}
		else
		{
			matrix_errormsg();
			return 1;
		}
		B1 = mtx_Inverse(NULL, B, &Det);
		C = mtx_Multiply(NULL, A, B1);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 2)
			{
				PutMatrix(Param[2], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_DOT:
		if (iParam >= 2)
		{
			B = GetMatrix(Param[1]);
			if (!B)
			{
				return 1;
			}
		}
		else
		{
			matrix_errormsg();
			return 1;
		}
		C = matrix_Dot(A, B);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 2)
			{
				PutMatrix(Param[2], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_GAMMA:
		u = atof(Param[1]);
		if (u > 0.0)
		{
			for (i = 0; i < A->i; ++i)
			{
				for (j = 0; j < A->j; ++j)
				{
					A->a[i][j] = pow(A->a[i][j], 1.0/u);
				}
			}
			if (iParam > 2)
			{
				PutMatrix(Param[2], A, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, A, &(argv[1][1]));
			}
		}
		else
		{
			fputs("Gamma value must be larger than 0.0\n", stderr);
		}
		break;
	case matrix_GEN:
		if (iParam >= 1) i = atoi(Param[0]);
		else i = 8;
		if (i < 1) i = 8;
		if (iParam >= 2) j = atoi(Param[1]);
		else j = i;
		if (j < 1) j = i;
		if (iParam >= 3) m = atoi(Param[2]);
		else m = 256;
		if (m < 1) m = 256;
		C = mtx_Create(NULL, i, j);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			for (i1 = 0; i1 < i; ++i1)
			{
				for (j1 = 0; j1 < j; ++j1)
				{
					r = genmtxrnd(m);
					C->a[i1][j1] = (double) r;
				}
			}
			mtx_TryInt(1.0, C);
			if (iParam > 3)
			{
				PutMatrix(Param[3], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_GXTRANS:
		if (iParam > 12)
		{
			if (iParam > 14)
			{
				/*
				 * matrix_Transform() was designed for x-y graphics coordinants,
				 * but matrices operate in i-j coordinate system.
				 */
				C = matrix_Transform(A,
					atof(Param[1]), atof(Param[2]),
					atof(Param[3]), atof(Param[4]),
					atof(Param[5]), atof(Param[6]),
					atof(Param[7]), atof(Param[8]),
					atof(Param[9]), atof(Param[10]),
					atof(Param[11]), atof(Param[12]),
					atoi(Param[13]), atoi(Param[14]));
			}
			else
			{
				C = matrix_Transform(A,
					atof(Param[1]), atof(Param[2]),
					atof(Param[3]), atof(Param[4]),
					atof(Param[5]), atof(Param[6]),
					atof(Param[7]), atof(Param[8]),
					atof(Param[9]), atof(Param[10]),
					atof(Param[11]), atof(Param[12]),
					mtx_Height(A), mtx_Width(A));
			}
		}
		else
		{
			matrix_errormsg();
			return 1;
		}
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 15)
			{
				PutMatrix(Param[15], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_HI:
		e1 = A->a[0][0];
		for (i = 0; i < A->i; ++i)
		{
			for (j = 0; j < A->j; ++j)
			{
				e1 = (A->a[i][j] > e1 ? A->a[i][j] : e1);
			}
		}
		fprintf(stdout, "%s: %.16e\n", &(argv[1][1]), e1);
		break;
	case matrix_IMPORT:
		if (iParam >= 2)
		{
			PutMatrix(Param[1], A, Param[0]);
		}
		else
		{
			PutMatrix(NULL, A, Param[0]);
		}
		break;
	case matrix_INV:
		C = mtx_Inverse(NULL, A, &Det);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 1)
			{
				PutMatrix(Param[1], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_LO:
		e1 = A->a[0][0];
		for (i = 0; i < A->i; ++i)
		{
			for (j = 0; j < A->j; ++j)
			{
				e1 = (A->a[i][j] < e1 ? A->a[i][j] : e1);
			}
		}
		fprintf(stdout, "%s: %.16e\n", &(argv[1][1]), e1);
		break;
	case matrix_MUI:
		if (iParam >= 2)
		{
			B = GetMatrix(Param[1]);
			if (!B)
			{
				return 1;
			}
		}
		else
		{
			matrix_errormsg();
			return 1;
		}
		A1 = mtx_Inverse(NULL, A, &Det);
		C = mtx_Multiply(NULL, A1, B);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 2)
			{
				PutMatrix(Param[2], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_MUL:
		if (iParam >= 2)
		{
			B = GetMatrix(Param[1]);
			if (!B)
			{
				return 1;
			}
		}
		else
		{
			matrix_errormsg();
			return 1;
		}
		C = mtx_Multiply(NULL, A, B);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 2)
			{
				PutMatrix(Param[2], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_NORM:
		C = matrix_Normal(A);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 1)
			{
				PutMatrix(Param[1], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_PGM:
		if (iParam >= 2)
		{
			pgmout = fopen(Param[1], "wb");
			if (!pgmout)
			{
				fprintf(stderr,"Can't create %s\n", Param[1]);
				return 1;
			}
		}
		else
		{
			pgmout = stdout;
			setmode(fileno(pgmout), O_BINARY);
		}
		exportPGM(pgmout, A);
		fclose(pgmout);
		break;
	case matrix_PGM12:
		if (iParam >= 2)
		{
			pgmout = fopen(Param[1], "wb");
			if (!pgmout)
			{
				fprintf(stderr,"Can't create %s\n", Param[1]);
				return 1;
			}
		}
		else
		{
			pgmout = stdout;
			setmode(fileno(pgmout), O_BINARY);
		}
		exportPGM12(pgmout, A);
		fclose(pgmout);
		break;
	case matrix_PGM16:
		if (iParam >= 2)
		{
			pgmout = fopen(Param[1], "wb");
			if (!pgmout)
			{
				fprintf(stderr,"Can't create %s\n", Param[1]);
				return 1;
			}
		}
		else
		{
			pgmout = stdout;
			setmode(fileno(pgmout), O_BINARY);
		}
		exportPGM2(pgmout, A);
		fclose(pgmout);
		break;
	case matrix_RANGE:
		e1 = e2 = A->a[0][0];
		for (i = 0; i < A->i; ++i)
		{
			for (j = 0; j < A->j; ++j)
			{
				e1 = (A->a[i][j] < e1 ? A->a[i][j] : e1);
				e2 = (A->a[i][j] > e2 ? A->a[i][j] : e2);
			}
		}
		fprintf(stdout, "%s: %.16e, %.16e\n", &(argv[1][1]), e1, e2);
		break;
	case matrix_REV:
		C = matrix_Reverse(A);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 1)
			{
				PutMatrix(Param[1], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_SCALE:
		u = atof(Param[1]);
		for (i = 0; i < A->i; ++i)
		{
			for (j = 0; j < A->j; ++j)
			{
				A->a[i][j] *= u;
			}
		}
		if (iParam > 2)
		{
			PutMatrix(Param[2], A, &(argv[1][1]));
		}
		else
		{
			PutMatrix(NULL, A, &(argv[1][1]));
		}
		break;
	case matrix_SUB:
		if (iParam >= 2)
		{
			B = GetMatrix(Param[1]);
			if (!B)
			{
				return 1;
			}
		}
		else
		{
			matrix_errormsg();
			return 1;
		}
		C = mtx_Subtract(NULL, A, B);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 2)
			{
				PutMatrix(Param[2], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_T:
		C = matrix_Transpose(A);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 1)
			{
				PutMatrix(Param[1], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_TRANS:
		if (iParam > 12)
		{
			if (iParam > 14)
			{
				/*
				 * matrix_Transform() was designed for x-y graphics coordinants,
				 * but matrices operate in i-j coordinate system.
				 */
				C = matrix_Transform(A,
					atof(Param[2])-1.0, atof(Param[1])-1.0,
					atof(Param[4])-1.0, atof(Param[3])-1.0,
					atof(Param[6])-1.0, atof(Param[5])-1.0,
					atof(Param[8])-1.0, atof(Param[7])-1.0,
					atof(Param[10])-1.0, atof(Param[9])-1.0,
					atof(Param[12])-1.0, atof(Param[11])-1.0,
					atoi(Param[13]), atoi(Param[14]));
			}
			else
			{
				C = matrix_Transform(A,
					atof(Param[2])-1.0, atof(Param[1])-1.0,
					atof(Param[4])-1.0, atof(Param[3])-1.0,
					atof(Param[6])-1.0, atof(Param[5])-1.0,
					atof(Param[8])-1.0, atof(Param[7])-1.0,
					atof(Param[10])-1.0, atof(Param[9])-1.0,
					atof(Param[12])-1.0, atof(Param[11])-1.0,
					mtx_Height(A), mtx_Width(A));
			}
		}
		else
		{
			matrix_errormsg();
			return 1;
		}
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			if (iParam > 15)
			{
				PutMatrix(Param[15], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	case matrix_UNF:
		if (iParam >= 1) i = atoi(Param[0]);
		else i = 8;
		if (i <= 0) i = 8;
		if (iParam >= 2) j = atoi(Param[1]);
		else j = i;
		if (j <= 0) j = i;
		if (iParam >= 3) u = atof(Param[2]);
		else u = 0.0;
		C = mtx_Create(NULL, i, j);
		if (!C)
		{
			fputs("Could not create matrix C\n", stderr);
		}
		else
		{
			for (i1 = 0; i1 < i; ++i1)
			{
				for (j1 = 0; j1 < j; ++j1)
				{
					C->a[i1][j1] = u;
				}
			}
			if (iParam > 3)
			{
				PutMatrix(Param[3], C, &(argv[1][1]));
			}
			else
			{
				PutMatrix(NULL, C, &(argv[1][1]));
			}
		}
		break;
	}

	return 0;
}
