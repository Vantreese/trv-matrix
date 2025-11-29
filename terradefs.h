#ifndef _terradefs_h_
#define _terradefs_h_

typedef struct
{
	short
		scale, base, **elevations;
} altw_t;

typedef struct
{
	unsigned short
		size, size_pad, xpts, xpts_pad, ypts, ypts_pad;
	float
		scal_x, scal_y, scal_z, crad;
	unsigned int
		crvm;
	altw_t
		altw;
} terrain_t;

#endif
