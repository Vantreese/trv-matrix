#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esunlib.h>

#include "terradefs.h"


unsigned short Zero = (unsigned short) 0, One = (unsigned short) 1;

void destroyterraindata(terrain_t *ter, unsigned short ypts)
{
	unsigned short
		yloop;

	if (ypts == (unsigned short) 0)
	{
		ypts = ter->ypts;
	}
	if (ter->altw.elevations)
	{
		for (yloop = Zero; yloop < ypts; ++yloop)
		{
			free(ter->altw.elevations[yloop]);
		}
		free(ter->altw.elevations);
		ter->altw.elevations = NULL;
	}
}

short **createterraindata(terrain_t *ter, unsigned short ypts)
{
	unsigned short
		xloop, yloop;

	if (ypts == Zero)
	{
		ypts = ter->ypts;
	}
	/* Allocate ter->ypts, rather than ypts, so that blank data may be written properly when necessary.
	 */
	ter->altw.elevations = calloc(ter->ypts, sizeof(short*));
	for (yloop = Zero; yloop < ypts; ++yloop)
	{
		ter->altw.elevations[yloop] = calloc(ter->xpts, sizeof(short));
		for (xloop = Zero; xloop < ter->xpts; ++xloop)
		{
			ter->altw.elevations[yloop][xloop] = (short) -32768;
		}
	}
	return ter->altw.elevations;
}

terrain_t *initterrain(terrain_t *ter)
{
	terrain_t
		*rter = ter;

	if (!ter)
	{
		rter = calloc(1, sizeof(terrain_t));
	}
	else
	{
		rter->size = rter->size_pad = Zero;
		rter->xpts = rter->xpts_pad = Zero;
		rter->ypts = rter->ypts_pad = Zero;
		rter->scal_x = rter->scal_y = rter->scal_z = rter->crad = (float) 0.0;
		rter->crvm = 0;
		rter->altw.scale = rter->altw.base = Zero;
		rter->altw.elevations = NULL;
	}
	return rter;
}

int readterrainhead(FILE *fter, terrain_t *ter)
{
	char
		terragen[8], terrain[8], code[4];
	size_t
		ReadStat;


	rewind(fter);
	ReadStat = fread(terragen, 8, 1, fter);
	ReadStat = fread(terrain, 8, 1, fter);
	if (strncmp(terragen, "TERRAGEN", 8)
			&& strncmp(terrain, "TERRAIN ", 8))
	{
		fputs("Not a Terragen-Terrain data file\n", stderr);
		return 1;
	}

	ReadStat = fread(code, 4, 1, fter);
	while (strncmp(code, "ALTW", 4)
			&& strncmp(code, "EOF ", 4)
			&& ReadStat > 0)
	{
		if (!strncmp(code, "SIZE", 4))
		{
			fread(&(ter->size), 2, 1, fter);
			fread(&(ter->size_pad), 2, 1, fter);
		}
		if (!strncmp(code, "XPTS", 4))
		{
			fread(&(ter->xpts), 2, 1, fter);
			fread(&(ter->xpts_pad), 2, 1, fter);
		}
		if (!strncmp(code, "YPTS", 4))
		{
			fread(&(ter->ypts), 2, 1, fter);
			fread(&(ter->ypts_pad), 2, 1, fter);
		}
		if (!strncmp(code, "SCAL", 4))
		{
			fread(&(ter->scal_x), 4, 1, fter);
			fread(&(ter->scal_y), 4, 1, fter);
			fread(&(ter->scal_z), 4, 1, fter);
		}
		if (!strncmp(code, "CRAD", 4))
		{
			fread(&(ter->crad), 4, 1, fter);
		}
		if (!strncmp(code, "CRVM", 4))
		{
			fread(&(ter->crvm), 4, 1, fter);
		}
		ReadStat = fread(code, 4, 1, fter);
	}
	if (!ter->xpts) ter->xpts = ter->size + One;
	if (!ter->ypts) ter->ypts = ter->size + One;
	if (!strncmp(code, "ALTW", 4))
	{
		fread(&(ter->altw.scale), 2, 1, fter);
		fread(&(ter->altw.base), 2, 1, fter);
		return 0;
	}
	return 1;
}

short *readterrainrow(FILE *fter, unsigned short xpts)
{
	short *elevations = NULL;

	elevations = calloc(xpts, sizeof(short));
	fread(elevations, 2, xpts, fter);
	return elevations;
}

int readterrain(FILE *fter, terrain_t *ter)
{
	unsigned short
		yloop;


	if (!fter || !ter)
	{
		fputs("NULL data in readterrain\n", stderr);
		return 1;
	}
	if (readterrainhead(fter, ter) > 0)
	{
		return 1;
	}
	ter->altw.elevations = calloc(ter->ypts, sizeof(short*));
	for (yloop = Zero; yloop < ter->ypts; ++yloop)
	{
		ter->altw.elevations[yloop] = readterrainrow(fter, ter->xpts);
	}

	return 0;
}

int writeterrainhead(FILE *fter, terrain_t *ter)
{
	fwrite("TERRAGEN", 8, 1, fter);
	fwrite("TERRAIN ", 8, 1, fter);
	fwrite("SIZE", 4, 1, fter);
	fwrite(&(ter->size), 2, 1, fter);
	fwrite(&(ter->size_pad), 2, 1, fter);
	if ((ter->size != ter->xpts - 1) || (ter->size != ter->ypts - 1))
	{
		fwrite("XPTS", 4, 1, fter);
		fwrite(&(ter->xpts), 2, 1, fter);
		fwrite(&(ter->xpts_pad), 2, 1, fter);
		fwrite("YPTS", 4, 1, fter);
		fwrite(&(ter->ypts), 2, 1, fter);
		fwrite(&(ter->ypts_pad), 2, 1, fter);
	}
	fwrite("SCAL", 4, 1, fter);
	fwrite(&(ter->scal_x), 4, 1, fter);
	fwrite(&(ter->scal_y), 4, 1, fter);
	fwrite(&(ter->scal_z), 4, 1, fter);
	fwrite("CRAD", 4, 1, fter);
	fwrite(&(ter->crad), 4, 1, fter);
	fwrite("CRVM", 4, 1, fter);
	fwrite(&(ter->crvm), 4, 1, fter);
	fwrite("ALTW", 4, 1, fter);
	fwrite(&(ter->altw.scale), 2, 1, fter);
	fwrite(&(ter->altw.base), 2, 1, fter);
	return 0;
}

int writeterrainrow(FILE *fter, unsigned short xpts, short *elevations)
{
	fwrite(elevations, 2, xpts, fter);
	return 0;
}

int writeterrain(FILE *fter, terrain_t *ter)
{
	unsigned short
		xloop, yloop;
	short
		el0[4097];

	for (xloop = 0; xloop < ter->xpts; ++xloop)
	{
		el0[xloop] = -32768;
	}
	writeterrainhead(fter, ter);
	for (yloop = 0; yloop < ter->ypts; ++yloop)
	{
		if (ter->altw.elevations[yloop])
		{
			writeterrainrow(fter, ter->xpts, ter->altw.elevations[yloop]);
		}
		else
		{
			writeterrainrow(fter, ter->xpts, el0);
		}
	}
	fwrite("EOF ", 4, 1, fter);
	fflush(fter);
	return 0;
}

int writeterraineof(FILE *fter)
{
	int
		ierror;

	ierror = fwrite("EOF ", 4, 1, fter);
	if (!ierror) return 1;
	fflush(fter);
	return 0;
}

unsigned short getterrainsize(terrain_t *ter)
{
	return ter->size;
}

unsigned short getterrainxpts(terrain_t *ter)
{
	return ter->xpts;
}

unsigned short getterrainypts(terrain_t *ter)
{
	return ter->ypts;
}

float getterrainzscale(terrain_t *ter)
{
	return ter->scal_z;
}

float getterrainradius(terrain_t *ter)
{
	return ter->crad;
}

short getterrainheightscale(terrain_t *ter)
{
	return ter->altw.scale;
}

short getterrainbaseheight(terrain_t *ter)
{
	return ter->altw.base;
}

void putterraindims(terrain_t *ter, unsigned short xpts, unsigned short ypts)
{
	unsigned short
		size;

	size = (xpts > ypts ? xpts : ypts) - 1;
	if (size < 128) size = 128;
	if ((size > 128) && (size < 256)) size = 256;
	if ((size > 256) && (size < 512)) size = 512;
	if ((size > 512) && (size < 1024)) size = 1024;
	if ((size > 1024) && (size < 2048)) size = 2048;
	if (size > 2048) size = 4096;
	if (xpts > 4097) xpts = 4097;
	if (ypts > 4097) ypts = 4097;
	ter->size = size;
	ter->xpts = (xpts < size + 1 ? size + 1 : xpts);
	ter->ypts = (ypts < size + 1 ? size + 1 : ypts);
	ter->size_pad = ter->xpts_pad = ter->ypts_pad = Zero;
}

void putterrainscale(terrain_t *ter, float scal)
{
	ter->scal_x = ter->scal_y = ter->scal_z = scal;
}

void putterrainradius(terrain_t *ter, float crad)
{
	ter->crad = crad;
}

void putterrainheightscale(terrain_t *ter, short scale)
{
	ter->altw.scale = scale;
}

void putterrainbaseheight(terrain_t *ter, short base)
{
	ter->altw.base = base;
}

int isterraincurved(terrain_t *ter)
{
	return ter->crvm;
}

void maketerraincurved(terrain_t *ter)
{
	ter->crvm = 1;
}

void maketerrainflat(terrain_t *ter)
{
	ter->crvm = 0;
}

short getterraindataxy(terrain_t *ter, unsigned short x, unsigned short y)
{
	return ter->altw.elevations[y][x];
}

void putterraindataxy(terrain_t *ter, unsigned short x, unsigned short y, short elevations)
{
	ter->altw.elevations[y][x] = elevations;
}

double getterrainmeterxy(terrain_t *ter, unsigned short x, unsigned short y)
{
	static double
		ht;
	double
		base, scale, el;

	base = (double)ter->altw.base;
	scale = (double)ter->altw.scale;
	el = (double)ter->altw.elevations[y][x];
	ht = (base+scale*el/65536.0)*ter->scal_z;
	return ht;
}

void putterrainmeterxy(terrain_t *ter, unsigned short x, unsigned short y, double ht)
{
	static double
		htconst, base, scale;
	static int
		el;

	base = (double)ter->altw.base;
	scale =(double)ter->altw.scale;
	htconst = 65536.0/scale;
	ht /= (double)ter->scal_z;
	el = (int) si_round(htconst*(ht-base), 1.0);
	ter->altw.elevations[y][x] = (short) ((el > -32768) && (el < 32768) ? el : -32768);
}

terrain_t *copyterrain(terrain_t *copy0, terrain_t *ter)
{
	terrain_t
		*copy;
	unsigned short
		x, y;
	short
		el;


	copy = initterrain(copy0);
	putterraindims(copy, getterrainxpts(ter), getterrainypts(ter));
	putterrainscale(copy, getterrainzscale(ter));
	if (isterraincurved(ter))
	{
		maketerraincurved(copy);
	}
	else
	{
		maketerrainflat(copy);
	}
	putterrainradius(copy, getterrainradius(ter));
	putterrainbaseheight(copy, getterrainbaseheight(ter));
	putterrainheightscale(copy, getterrainheightscale(ter));
	createterraindata(copy, 0);
	for (y = 0; y < getterrainypts(ter); ++y)
	{
		for (x = 0; x < getterrainxpts(ter); ++x)
		{
			el = getterraindataxy(ter, x, y);
			putterraindataxy(copy, x, y, el);
		}
	}
	return copy;
}



/*
 * minht, maxt are in meters; will convert to terragen units here.
 * Does not set any value in terrain objects.
 */
int setheightscale(short *htscale0, short *baseht0, double minht, double maxht, double zscale)
{
	short
		htscale, baseht;
	int
		ilo, ihi;
	double
		dhtscale, dbaseht;

	minht /= zscale;
	maxht /= zscale;
	htscale = (short) floor((maxht-minht))+1;
	baseht = (short) floor((minht+maxht)/2.0);
	dhtscale = (double) htscale;
	dbaseht = (double) baseht;
	ilo = (int) si_round(65536.0/dhtscale*(minht-dbaseht), 1.0);
	ihi = (int) si_round(65536.0/dhtscale*(maxht-dbaseht), 1.0);
	if ((ilo < -32768) || (ihi > 32767))
	{
		++baseht;
		ilo = (int) si_round(65536.0/htscale*(minht-baseht), 1.0);
		ihi = (int) si_round(65536.0/htscale*(maxht-baseht), 1.0);
	}
	if ((ilo < -32768) || (ihi > 32767))
	{
		--baseht;
		++htscale;
		ilo = (int) si_round(65536.0/htscale*(minht-baseht), 1.0);
		ihi = (int) si_round(65536.0/htscale*(maxht-baseht), 1.0);
	}
	if ((ilo < -32768) || (ihi > 32767))
	{
		++baseht;
		ilo = (int) si_round(65536.0/htscale*(minht-baseht), 1.0);
		ihi = (int) si_round(65536.0/htscale*(maxht-baseht), 1.0);
	}
fprintf(stderr, "range = (%d, %d)\n", ilo, ihi);
fprintf(stderr, "zscale = %f, htscale = %d, baseheight = %d\n", zscale, htscale, baseht);
	if ((ilo < -32768) || (ihi > 32767))
	{
		return 1;
	}
	else
	{
		*htscale0 = htscale;
		*baseht0 = baseht;
		return 0;
	}
}

