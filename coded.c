#include <string.h>
#include <sys/stat.h>
#include <esunlib.h>

#define ANTI_MOD 2.0
#define MODULAR  16777216.0
#define IMODULAR 16777216
#define ROUND_TO 1.0

Dln_t		*Message = NULL;
Matrix_t	*C = NULL, *D = NULL, *E = NULL, *M = NULL;

double Modulo(double n, double mod)
{
	double nn = si_round(n, ROUND_TO);

	if ((nn > mod) || (nn < 0.0))
		nn = (nn / mod - trunc((nn / mod))) * mod;
	if (nn < 0.0)
		nn += mod;
	return nn;
}
double Moduloa(double n, double mod)
{
	double nn = trunc(n + 0.25);

	if ((nn > mod) || (nn < 0.0))
		nn = (nn / mod - trunc((nn / mod))) * mod;
	if (nn < 0.0)
		nn += mod;
	return nn;
}

/*
double Modulo(double n, double mod)
{
	Int64 nn = (Int64) n, mmod = (Int64) mod;

	return ((double) (nn % mmod));
}
*/

int main(int argc, char *argv[])
{
	char szCfgFile[FILENAME_MAX], CfgTok[] = "=\r\n", *sz1, *sz2;
	char szCfgDir[FILENAME_MAX], szCfgName[FILENAME_MAX];
	char szLine[FILENAME_MAX], szKeyDir[FILENAME_MAX] = ".", szKey[FILENAME_MAX];
	char szMatrixDir[FILENAME_MAX];
	Dln_t *dln;
	double epsilon = ROUND_TO / 2.0;
	double Det;
	FILE *fpCFG, *fpmtx, *fpI = NULL, *fpO = stdout;
	int cc = 'e';
	struct stat matrixstat;
	Int64 mu, ie, id;
	int modded = 0;
	double Mod, d;
	size_t i, j;

	SetAllEnv(argc, argv);
	if (!getenv("HOME")) strcpy(szMatrixDir, ".");
	else strcpy(szMatrixDir, getenv("HOME"));
	strcatc(szMatrixDir, PathC);
	strcat(szMatrixDir, ".matrix");
	splitname(argv[0], NULL, szCfgName, NULL);
	fullname(szCfgFile, szMatrixDir, szCfgName, "cfg");
	if (stat(szMatrixDir, &matrixstat) == -1)
	{
		/* mkdir(szMatrixDir, 0700); */
	}
	if (fpCFG = fopen(szCfgFile, "r"))
	{
		while (fgets(szLine, FILENAME_MAX, fpCFG))
		{
			sz1 = strtok(szLine, CfgTok);
			sz2 = strtok(NULL, CfgTok);
			if (!strcmp(sz1, "EKeyDir"))
			{
				strcpy(szKeyDir, sz2);
			}
			if (!strcmp(sz1, "EKey"))
			{
				strcpy(szKey, sz2);
			}
		}
		fclose(fpCFG);
		strcpy(szLine, szKeyDir);
		strcatc(szLine, EndPathC);
		strcat(szLine, szKey);
		if (fpmtx = fopen(szLine, "rb"))
		{
			E = fgetMatrix(NULL, fpmtx);
			fclose(fpmtx);
		}
	}
	if (!E)
	{
		Query(m_sz, szKey, 0, "", "Enter the name of the Encryption Key:");
		if (strcmp(szKey, ""))
		{
			strcpy(szLine, szKey);
			if (fpmtx = fopen(szLine, "rb"))
			{
				E = fgetMatrix(NULL, fpmtx);
				fclose(fpmtx);
			}
			if (!E)
			{
				strcat(szLine, ".key");
			}
			if (fpmtx = fopen(szLine, "rb"))
			{
				E = fgetMatrix(NULL, fpmtx);
				fclose(fpmtx);
			}
			if (!E)
			{
				strcpy(szLine, szKeyDir);
				strcatc(szLine, EndPathC);
				strcat(szLine, szKey);
				if (fpmtx = fopen(szLine, "rb"))
				{
					E = fgetMatrix(NULL, fpmtx);
					fclose(fpmtx);
				}
				else
				{
					/* Error opening file */
				}
			}
		}
	}
	if (!E)
	{
		fputs("No Encryption Key available.\n", stdout);
		return 1;
	}
	D = mtx_Inverse(NULL, E, &Det);
	if (!D)
	{
		fputs("Decryption Key cannot be derived by this Encryption Key.\n", stdout);
		mtx_Unlink(E, E->a, E->i);
		return 2;
	}
	Det = si_round(Det, ROUND_TO);

	/* Beginning of added part */
	if (fEQ(epsilon, 1.0, Modulo(Det, ANTI_MOD)) && (Det < MODULAR))
	{
		modded = 1;
		Mod = Modulo(Det, MODULAR);
		ie = (Int64) Mod;
		euclidean64(IMODULAR, ie, &mu, &id);
		d = (double) id;
		for (i = 0; i < D->i; ++i)
		{
			for (j = 0; j < D->j; ++j)
			{
				D->a[i][j] *= Det * d;
				D->a[i][j] = Modulo(D->a[i][j], MODULAR);
			}
		}
	}
	/* End of added part */

	if (argc > 1)
	{
		strcpy(szLine, argv[1]);
		switch (szLine[0])
		{
		case 'e':
		case 'E':
			cc = 'e';
			break;
		case 'd':
		case 'D':
			cc = 'd';
			break;
		default:
			cc = ChooseC("+edx", 'e', "Encrypt or Decrypt or eXit?");
			break;
		}
	}
	else
	{
		cc = ChooseC("+edx", 'e', "Encrypt or Decrypt or eXit?");
	}
	if (cc == 'x')
	{
		mtx_O(E, E->i, E->j);
		mtx_O(D, D->i, D->j);
		mtx_Unlink(E, E->a, E->i);
		mtx_Unlink(D, D->a, D->i);
		return 0;
	}
	if (argc > 2)
	{
		fpI = fopen(argv[2], "rb");
	}
	else
	{
		Query(m_sz, szLine, 0, "", "Incoming file (return to exit):");
		if (strcmp(szLine, ""))
		{
			fpI = fopen(szLine, "rb");
		}
	}
	if (!fpI)
	{
		mtx_O(E, E->i, E->j);
		mtx_O(D, D->i, D->j);
		mtx_Unlink(E, E->a, E->i);
		mtx_Unlink(D, D->a, D->i);
		return 0;
	}
	if (argc > 3)
	{
		if (cc == 'e')
		{
			fpO = fopen(argv[3], "w");
		}
		else
		{
			fpO = fopen(argv[3], "wb");
		}
	}
	else
	{
		Query(m_sz, szLine, 0, "", "Outgoing file (return for stdout):");
		if (strcmp(szLine, ""))
		{
			if (cc == 'e')
			{
				fpO = fopen(szLine, "w");
			}
			else
			{
				fpO = fopen(szLine, "wb");
			}
		}
	}
	switch (cc)
	{
	case 'e':
		dln = fgetsec(NULL, E->i, fpI);
		M = mtx_Encode(15, dln);
		C = mtx_Multiply(NULL, M, E);
		if (modded)
		{
			for (i = 0; i < C->i; ++i)
			{
				for (j = 0; j < C->j; ++j)
				{
					C->a[i][j] = Modulo(C->a[i][j], MODULAR);
				}
			}
		}
		fputMatrixTryInt("I", epsilon, C, fpO);
		/*
		dln = mtx_Decode(C);
		fputsec(dln, fpO);
		*/
		mtx_O(M, M->i, M->j);
		break;
	case 'd':
		/*
		dln = fgetsec(NULL, D->i, fpI);
		C = mtx_Encode(0, dln);
		*/
		C = fgetMatrix(NULL, fpI);
		M = mtx_Multiply(NULL, C, D);
		if (!M)
		{
			fputs("Coded message is not compatible with this Encryption Key.\n", stdout);
			break;
		}
		if (modded)
		{
			for (i = 0; i < M->i; ++i)
			{
				for (j = 0; j < M->j; ++j)
				{/* This section has rounding problems, and has been using Moduloa().  */
					M->a[i][j] = Moduloa(M->a[i][j], MODULAR);
				}
			}
		}
		dln = mtx_Decode(M);
		fputsec(dln, fpO);
		mtx_O(M, M->i, M->j);
		break;
	}
	fclose(fpI);
	fclose(fpO);
	mtx_O(E, E->i, E->j);
	mtx_O(D, D->i, D->j);
	mtx_Unlink(E, E->a, E->i);
	mtx_Unlink(D, D->a, D->i);
	return 0;
}
