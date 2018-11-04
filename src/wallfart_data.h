/***
 *         WallFArt := 'Wallpaper Fractal Art'
 *		        ^^^^      ^       ^^^
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  The fundamental WFD (WallFArt Data Core ((sounds impressive, huh) ;-)
 */
#ifndef _WALLFART_DATA_H_
#define _WALLFART_DATA_H_

#include "wallfart_complex.h"
#include "wallfart_colour.h"
#include "wallfart_types.h"

#ifdef __cplusplus  
extern "C" {	   
#endif

#ifndef boolean
#define boolean unsigned char 
#endif
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef byte
#define byte unsigned char
#endif
/* FIXME foolish 8 bit limit;
  silly, isn't it? with today's hardware and all... */
#define MAX_ITERATIONS 255

#define IN_RANGE(Z) (complex_sqrsum((Z)) <= 4.0)
#define ARBITRARY_MAX	99999999
#define ARBITRARY_MIN	0

#define MIN_WID		48
#define MIN_HIG		48
#define SIZE_CHECK(w, h)			\
	if ((w) < MIN_WID) w = MIN_WID;		\
	if ((h) < MIN_HIG) h = MIN_HIG;

/*****************************************************************************/

typedef struct _WallFArt WallFArt;
struct _WallFArt {

   	WallFArt_types 	type;	  /* WallFArt_types.h */
	int 		width;
	int 		height;

   	byte 		*iter;    /* 8 bit iterative values */
	int 		min_iter; /*  don't actually use these... yet */
	int 		max_iter;
	WallFArt_complex 	startC;
	WallFArt_complex 	Z;
	double 		side;	  /*  for zoom */
	double 		gap;	  /* 'distance' between points 	   */

    	WallFArt_complex	min;  /* don't actually use these     */
	WallFArt_complex	max;  /*  but I still might some day  */
	WallFArt_complex	modifier; /***/
	WallFArt_complex	*slice;   /***/

	/* TODO I wish I'd used RGBA */
    	byte 		*rgb_data;
	int 		n_channels;
	int		rowstride;

	/* from wallfart_colour */
 	ca_rule 	*car; 
	//ca_rule	*inside_car; /* when *iter == 0  */
	/* I divided up car so first 6 random multipliers apply to outside
		and the last 3 to the inside */
};

/*****************************************************************************
 */
WallFArt *WallFArt_new(WallFArt  	*f,
		     WallFArt_types 	type,
		     int 		width,
		     int		height, 
		     WallFArt_complex  	startC, 
		     WallFArt_complex  	Z,	
		     WallFArt_complex  	modifier,  
		     double		side,
		     ca_rule		*car);
		     /* ca_rule	*inside_car); */

WallFArt *WallFArt_free			(WallFArt *f);
WallFArt *WallFArt_resize		(WallFArt *f, int wid, int hig);
WallFArt *WallFArt_repaint		(WallFArt *f);
WallFArt *WallFArt_repaint_inside	(WallFArt *f);
void WallFArt_dump			(WallFArt *f);

WallFArt *WallFArt_data_generate
   (WallFArt_types type, int width, int height, ca_rule *car, boolean randomly);
void WallFArt_data_get_vars
   (WallFArt_types type, WallFArt_complex *start, WallFArt_complex *z, double *side, boolean randomly);

#ifdef __cplusplus
}
#endif

#endif /* _WALLFART_DATA_H_ */
