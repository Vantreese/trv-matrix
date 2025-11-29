#ifndef _terralib_h_
#define _terralib_h_

#include <stdio.h>
#include "terradefs.h"

extern void destroyterraindata(terrain_t *ter, unsigned short ypts);
extern short **createterraindata(terrain_t *ter, unsigned short ypts);
extern terrain_t *copyterrain(terrain_t *copy0, terrain_t *ter);
extern terrain_t *initterrain(terrain_t *ter);
extern int readterrainhead(FILE *fter, terrain_t *ter);
extern short *readterrainrow(FILE *fter, unsigned short xpts);
extern int readterrain(FILE *fter, terrain_t *ter);
extern int writeterrainhead(FILE *fter, terrain_t *ter);
extern int writeterrainrow(FILE *fter, unsigned short xpts, short *elevations);
extern int writeterrain(FILE *fter, terrain_t *ter);
extern int writeterraineof(FILE *fter);

extern unsigned short getterrainsize(terrain_t *ter);
extern unsigned short getterrainxpts(terrain_t *ter);
extern unsigned short getterrainypts(terrain_t *ter);
extern float getterrainzscale(terrain_t *ter);
extern float getterrainradius(terrain_t *ter);
extern short getterrainheightscale(terrain_t *ter);
extern short getterrainbaseheight(terrain_t *ter);
extern short getterraindataxy(terrain_t *ter, unsigned short x, unsigned short y);
extern double getterrainmeterxy(terrain_t *ter, unsigned short x, unsigned short y);

extern void putterraindims(terrain_t *ter, unsigned short xpts, unsigned short ypts);
extern void putterrainscale(terrain_t *ter, float scal);
extern void putterrainradius(terrain_t *ter, float crad);
extern void putterrainheightscale(terrain_t *ter, short scale);
extern void putterrainbaseheight(terrain_t *ter, short base);
extern void putterraindataxy(terrain_t *ter, unsigned short x, unsigned short y, short elevations);

extern int isterraincurved(terrain_t *ter);
extern void maketerraincurved(terrain_t *ter);
extern void maketerrainflat(terrain_t *ter);
extern void putterrainmeterxy(terrain_t *ter, unsigned short x, unsigned short y, double ht);

extern int setheightscale(short *htscale0, short *baseht0, double minht, double maxht, double zscale);

#endif
