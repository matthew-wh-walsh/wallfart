/** 
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
 */

/** This file is part of WallFArt.
 *
 *  wallfart_data.c: the vanilla C WallFArt core 
 *   I was trying to make this program run as quickly as possible without
 *   any asm; hence the exorbitant (ab)use of macros.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <errno.h>

#include "wallfart_complex.h"
#include "wallfart_colour.h"
#include "wallfart_types.h"
#include "wallfart_data.h"

#ifndef ABS
#define ABS(n) 	(n < 0) ? n * -1 : n
#endif

/**
 * to test colour rules set to 1 
 */
#define OUTSIDERULETEST 0
#define INSIDERULETEST  0
/**
 * then specify what to test
 */
int outside_rule_num  = 0;
int inside_rule_num = 4;
#define VERBOSE_COLOUR  0   /* to see what's being used */

/* if KISS set, only one rule applied to wallfart (inside and out)  */
#define KISS 0
//#define KISS 1

static WallFArt *WallFArt_alloc(int width, int height);
static WallFArt *WallFArt_realloc(WallFArt *f, int width, int height);
static WallFArt *WallFArt_evaluate(WallFArt *f);

WallFArt *WallFArt_new(WallFArt  	*f,
		     WallFArt_types 	type,
		     int 		width,
		     int		height, 
		     WallFArt_complex  	startC, 
		     WallFArt_complex  	Z,	
		     WallFArt_complex  	modifier,  
		     double		side,
		     ca_rule		*car)
{
	WallFArt *_f = f;

	if (_f == NULL) { /* everything's new */

		_f = WallFArt_alloc(width, height);
		_f->car = NULL;
	}

	if (car == NULL) { 
		_f->car =new_car();
		init_ca_rule(_f->car);
	} else {
		_f->car = car;
	}

	if (type == RANDOMTYPE) { type = PICK_A_RANDOM_TYPE(); }
	_f->type = type;  
	_f->width = width; 
	_f->height = height;	
	_f->side = side;
	complex_assign(_f->startC, startC);
	complex_assign(_f->Z, Z);
	complex_assign(_f->modifier, modifier);

	return (WallFArt_evaluate(_f));
}

/***********************************************************************
 */
static WallFArt *WallFArt_evaluate(WallFArt *f)
{
	int x, y, idx;

  /*** RGB variables 
   */
	int n_channels  = 3; 	/* RGB, no alpha */
	int rowstride   = n_channels * f->width;
	unsigned char  *pos, *pixels;
	unsigned char   red, green, blue;
	f->n_channels   = n_channels;
	f->rowstride    = rowstride;

  /*** WallFArt variables
   */   WallFArt_complex start, Z, modify;
	f->gap 	    = f->side/(double)f->width;
	f->max_iter = 0;
	f->min_iter = MAX_ITERATIONS;
	f->max.real = 0; f->max.imag = 0;
	f->min.real = ARBITRARY_MAX; f->min.imag = ARBITRARY_MAX;

   	pixels = f->rgb_data; 	/* !RGBA TODO I wish I had done RGBA */
	idx = 0;		/*	from the start  */
	start.real = f->startC.real;
	for (x=0; x<f->width; x++) {
		start.real += f->gap;
		start.imag = f->startC.imag;
		for (y=0; y<f->height; y++) {
			start.imag += f->gap;
			complex_assign(Z, f->Z);
			complex_assign(modify, f->modifier);
			f->iter[idx] = 0;
		/* now things get really ugly... */
			do {
				switch(f->type) {
					case TEST:
						DO_TEST(Z, start);
						break;
					case MANDELBROT:
						DO_MANDELBROT(Z, start);
						break;
					case MANDELCUBE:
						DO_MANDELCUBE(Z, start);
						break;
					case MANDELQUAD:
						DO_MANDELQUAD(Z, start);
						break;
					case MANDELQUINT:
						DO_MANDELQUINT(Z, start);
						break;
					case MANDELHEXNOT:
						DO_MANDELHEXNOT(Z, start);
						break;
					case SIMPLEC:
						DO_SIMPLEC(Z, start);
						break;
					case STARSHINE:
						DO_STARSHINE(Z, start);
						break;
					case MONET:
						DO_MONET(Z, start);
						break;
					case REVERSE:
						DO_REVERSE(Z, start);
						break;
					case TRIANGLE:
						DO_TRIANGLE(Z, start);
						break;
					case STRETCH:
						DO_STRETCH(Z, start);
						break;
					case FUZZY:
						DO_FUZZY(Z, start);
						break;
					case FUZZY2:
						DO_FUZZY2(Z, start);
						break;
					case STARFISH3: 
						DO_STARFISH3(Z, start);
						break;
					case STARFISH:
						DO_STARFISH(Z, start);
						break;
					case FACE:
						DO_FACE(Z, start);
						break;
					case BUG:
						DO_BUG(Z, start);
						break;
					case ZSQ_HALFC_Z:
						DO_ZSQ_HALFC_Z(Z, start);
						break;
					default:
						DO_MANDELBROT(Z, start);
						break;
				}
			} while ((f->iter[idx]++ < MAX_ITERATIONS) &&
				(IN_RANGE(Z)));

		/*** keep track of min/max values 
			TODO I should do something with them */
			if (f->iter[idx] < f->min_iter) {
				if (f->iter[idx] != 0)
					f->min_iter = f->iter[idx];
			}
			if (f->iter[idx] > f->max_iter) {
				if (f->iter[idx] < MAX_ITERATIONS) 
					f->max_iter = f->iter[idx];
			}
			if (Z.real > f->max.real)
				f->max.real = Z.real;
			if (Z.imag > f->max.imag)
				f->max.imag = Z.imag;
			if (Z.real < f->min.real)
				f->min.real = Z.real;
			if (Z.imag < f->min.imag)
				f->min.imag = Z.imag;

		 /*** save the values
 		  */
		   	f->slice[idx].real = Z.real;
			f->slice[idx].imag = Z.imag;

		 /*** the RGB data buffer 
		  */
		 	if (f->iter[idx] != 0) {
				APPLY_OUTSIDE_COLOUR();
			} else { 
				if (KISS) {
					APPLY_OUTSIDE_COLOUR();
				} else {
					APPLY_INSIDE_COLOUR();
				}
			}
			pos = pixels + y * rowstride + x * n_channels;
			pos[0] = red; pos[1] = green; pos[2] = blue;
			idx++;
		}
	  }
	return f;
}

/* TODO should call this when saving and produce a big 
 	screen sized png rather than scaling the graphic up */
WallFArt *WallFArt_resize(WallFArt *f, int wid, int hig)
{
	SIZE_CHECK(wid, hig);

	WallFArt_types tt = f->type;
	double tside = f->side;
	ca_rule *tcar = new_car();
	copy_ca_rule(tcar, f->car);
#if 0
	ca_rule *tcar = NULL;
	if (f->car != NULL) {
		tcar = new_car();
		copy_ca_rule(tcar, f->car);
	}
#endif 
	WallFArt_complex tc, tz, tm;
	complex_assign(tc, f->startC);
	complex_assign(tz, f->Z);
	complex_assign(tm, f->modifier);

	f = WallFArt_realloc(f, wid, hig);
	f->type = tt;
	f->width = wid;
	f->height = hig;
	complex_assign(f->startC, tc);
	complex_assign(f->Z, tz);
	complex_assign(f->modifier, tm);
	f->side = tside;
	f->car = tcar; 
	// WallFArt_free(f);
	return (WallFArt_evaluate(f));
}

/* */
WallFArt *WallFArt_repaint(WallFArt *f)
{
	int x, y, idx = 0;
	int n_channels  = 3; 	/* RGB, no alpha */
	int rowstride   = n_channels * f->width;
	unsigned char  *pos, *pixels;
	unsigned char   red, green, blue;
	f->n_channels   = n_channels;
	f->rowstride    = rowstride;

	init_ca_ruleO(f->car); /* random colour scheme */

#if OUTSIDERULETEST
	outside_rule_num = outside_rule_num; /* doesn't that look silly */
#else
	int next_o_rule = (int)(((float)OUTSIDE_RULES * (float)rand()/(float)RAND_MAX));
	if (next_o_rule == outside_rule_num)
		outside_rule_num = ++next_o_rule % OUTSIDE_RULES;
	else 
		outside_rule_num = next_o_rule;
#endif

#if VERBOSE_COLOUR
	printf("\n<<<<<OUTSIDE>>>> colour : %d\n", outside_rule_num);
#endif

   	pixels = f->rgb_data;
	for(x=0; x<f->width; x++) {
		for(y=0; y<f->height; y++) {
		 	if (f->iter[idx] != 0) {
    				APPLY_OUTSIDE_COLOUR();
			} else {
				if (KISS) {
    				APPLY_OUTSIDE_COLOUR();
				} else {
				APPLY_INSIDE_COLOUR();
			/* this would be a nice spot to include an image */
				}
			}
			pos = pixels + y * rowstride + x * n_channels;
			pos[0] = red; pos[1] = green; pos[2] = blue;
			idx++;
		}
	}
	return f;
}

/**
 *   the last 3 multipliers of the car struct apply 
 *	to introduce randomness	
 */
WallFArt *WallFArt_repaint_inside(WallFArt *f)
{
	int x, y, idx = 0;
	int n_channels  = 3; 	/* RGB, no alpha */
	int rowstride   = n_channels * f->width;
	unsigned char  *pos, *pixels;
	unsigned char   red, green, blue;
	f->n_channels   = n_channels;
	f->rowstride    = rowstride;

	init_ca_ruleI(f->car); /* random change */

#if INSIDERULETEST 
	inside_rule_num = inside_rule_num; /* again with the sillyness */
#else
	/* I don't like doing the same scheme twice */
	int next_i_rule = (int)((float)(INSIDE_RULES) * (float)rand()/(float)RAND_MAX);
	if (next_i_rule == inside_rule_num)
		inside_rule_num = ++next_i_rule % INSIDE_RULES;
	else 
		inside_rule_num = next_i_rule;
#endif
#if VERBOSE_COLOUR
	printf("\n>>>>>INSIDE<<<<< colour it: %d\n", inside_rule_num);
#endif

   	pixels = f->rgb_data;
	for(x=0; x<f->width; x++) {
		for(y=0; y<f->height; y++) {
		 	if (f->iter[idx] != 0) {
    				APPLY_OUTSIDE_COLOUR();
			} else { /* set red, green && blue */
				if (KISS) {
				APPLY_OUTSIDE_COLOUR();
				} else {
				APPLY_INSIDE_COLOUR();
				// image framed by a fractal 
				}
			}
			pos = pixels + y * rowstride + x * n_channels;
			pos[0] = red; pos[1] = green; pos[2] = blue;
			idx++;
		}
	}
	return f;
}


/***************************************************************************
 */
#define CALLOC_ERROR_MSG "Calloc failed exiting with error: %d\n", errno
#define MALLOC_ERROR_MSG "Malloc failed exiting with error: %d\n", errno
#define RGB_ERROR_MSG "Calloc failed with error: %d. (No RGB buf. Exit.)\n", errno

static WallFArt *WallFArt_alloc(int width, int height)
{

	extern WallFArt *WallFArt_free(WallFArt *f);
	extern int errno;
	int n_channels = 3; /* !RGBA */
	WallFArt *f = NULL;

	f = (WallFArt *)malloc(sizeof(WallFArt));

	if (f) {
	    f->iter = (byte *)calloc((width*height), sizeof(byte));
	    	if (f->iter) {
    			f->slice = 
			 (WallFArt_complex *)calloc((width*height), 
					sizeof(WallFArt_complex));
	    	} else {
			fprintf(stderr, CALLOC_ERROR_MSG);
			WallFArt_free(f);
			exit(errno);
		}

		if (f->slice) {
			f->rgb_data = (byte *)calloc
			   ((width * height * n_channels), sizeof(unsigned char));

			if (f->rgb_data) {
				return f;

			} else {
				fprintf(stderr, RGB_ERROR_MSG);
				WallFArt_free(f);
				exit(errno);
			}

		} else {
			fprintf(stderr, CALLOC_ERROR_MSG);
			WallFArt_free(f);
			exit(errno);
		}

	} else {
		fprintf(stderr, MALLOC_ERROR_MSG);
		WallFArt_free(f);
		exit(errno);
	}
}

/**  
 *	realloc(): can't save -> free() -> [c|m]alloc() again 
 */
static WallFArt *WallFArt_realloc(WallFArt *f, int width, int height)
{
	extern WallFArt *WallFArt_free(WallFArt *f);
	extern int errno;
	int n_channels = 3; /* !RGBA */

	f = (WallFArt *)realloc(f, sizeof(WallFArt));

	if (f) {
	    f->iter = (byte *)realloc(f->iter, width*height*sizeof(byte));
	    	if (f->iter) {
    		     f->slice = 
	(WallFArt_complex *)realloc(f->slice, width*height*sizeof(WallFArt_complex));
	    	} else {
		     fprintf(stderr, CALLOC_ERROR_MSG);
			WallFArt_free(f);
			exit(errno);
		}
		if (f->slice) {
		     f->rgb_data = 
		     	(byte *)realloc(f->rgb_data, width*height*n_channels*sizeof(byte));

			if (f->rgb_data) {
				return f;

			} else {
				fprintf(stderr, RGB_ERROR_MSG);
				WallFArt_free(f);
				exit(errno);
			}

		} else {
		     fprintf(stderr, CALLOC_ERROR_MSG);
			WallFArt_free(f);
			exit(errno);
		}

	} else {
	     fprintf(stderr, MALLOC_ERROR_MSG);
		WallFArt_free(f);
		exit(errno);
	}
}

WallFArt *WallFArt_free(WallFArt *f)
{
	 if (f) {
		if (f->iter) 
		    free(f->iter);
		if (f->slice) 
		    free(f->slice);
		if (f->rgb_data) 
		    free(f->rgb_data);
		if (f->car) 
		    free(f->car);
	     free(f);
	  f = NULL;
	}
    return f;
}

float random_float(const float min, const float max)
{
	if (min == max) return min;
	else if (min < max)
		return (max - min) * (float)(rand()/(float)RAND_MAX) + min;
	else return 0.0;
}

/* a wee bit ugly but, it works */
void WallFArt_data_get_vars(WallFArt_types type, 
			WallFArt_complex *start, WallFArt_complex *z, 
			double *side, boolean randomly)
{
	if (randomly) {
	    switch(type) {
	    	case TEST:
			start->real = (double)random_float(-1.3, -0.3);
			start->imag = (double)random_float(-0.9, -0.3);
			z->real = (double)random_float(-0.5, 0.6);
			z->imag = (double)random_float(-0.4, 0.7);
			*side 	= (double)random_float(0.7, 3.3);
			break;
	    	case SIMPLEC:
			start->real = (double)random_float(-1.3, -0.3);
			start->imag = (double)random_float(-0.9, -0.3);
			z->real = (double)random_float(-0.5, 0.6);
			z->imag = (double)random_float(-0.4, 0.7);
			*side 	= (double)random_float(1.0, 3.3);
			break;
	    	case MANDELBROT:
			start->real = (double)random_float(-1.7, 0.1);
			start->imag = (double)random_float(-1.4, -0.1);
			z->real     = (double)random_float(0.1, 0.38);
			z->imag     = (double)random_float(0.4, 0.7);
			*side 	    = (double)random_float(0.002, 6.4);
			break;
	    	case STARSHINE:
			start->real = (double)random_float(-3.0, -0.89);
			start->imag = (double)random_float(-2.5, -0.93);
			z->real = (double)random_float(0.537, 0.548);
			z->imag = (double)random_float(-0.15, -0.14);
			*side 	= (double)random_float(3.2, 4.8); 
			break;
	    	case MANDELCUBE:
			start->real = (double)random_float(0.17, 0.2);
			start->imag = (double)random_float(-1.1, -1.0);
			z->real     = (double)random_float(-0.1, 0.4);
			z->imag     = (double)random_float(-0.4, -0.2);
			*side 	    = (double)random_float(0.09, 0.2);
			break;
	    	case MANDELQUAD:
			start->real = (double)random_float(-1.7, -1.5);
			start->imag = (double)random_float(-1.9, -1.8);
			z->real     = (double)random_float(1.0, 1.1);
			z->imag     = (double)random_float(0.1, 0.3);
			*side 	    = (double)random_float(2.15, 2.2);
			break;
	    	case MANDELQUINT:
			start->real = (double)random_float(-1.7, -1.3);
			start->imag = (double)random_float(0.1, 0.2);
			z->real     = (double)random_float(-0.8, -0.5);
			z->imag     = (double)random_float(0.0, 0.2);
			*side 	    = (double)random_float(0.8, 2.5);
			break;
	    	case MANDELHEXNOT:
			start->real = (double)random_float(0.2, 0.5);
			start->imag = (double)random_float(-1.5, 0.8);
			z->real     = (double)random_float(-0.1, 0.5);
			z->imag     = (double)random_float(-0.9, 0.1);
			*side 	    = (double)random_float(0.8, 3.0);
			break;
	    	case FACE:
			start->real = (double)random_float(-0.9, -0.8);
			start->imag = (double)random_float(-1.2, -1.1);
			z->real     = (double)random_float(-0.97, -0.95);
			z->imag     = (double)random_float(0.3, 0.6);
			*side 	    = (double)random_float(1.4, 1.6);
			break;
	    	case MONET:
			start->real = (double)random_float(-0.4, -0.3);
			start->imag = (double)random_float(-1.0, -0.8);
			z->real = (double)random_float(-0.6, 0.8);
			z->imag = (double)random_float(-0.4, 0.5);
			*side 	= (double)random_float(0.1, 4.5); 
			break;
	    	case REVERSE:
			start->real = (double)random_float(0.4, 0.5);
			start->imag = (double)random_float(0.4, 0.5);
			z->real     = (double)random_float(-0.2, 0.5);
			z->imag     = (double)random_float(-0.2, 0.4);
			*side 	    = (double)random_float(0.1, 0.5);
			break;
	    	case TRIANGLE:
			start->real = (double)random_float(-3.5, -1.3);
			start->imag = (double)random_float(-3.0, -1.4);
			z->real     = (double)random_float(-1.3, 1.3);
			z->imag     = (double)random_float(-1.1, 1.3);
			*side 	    = (double)random_float(4.4, 9.5);
			break;
	    	case STRETCH:
			start->real = (double)random_float(-2.2, 0.8);
			start->imag = (double)random_float(-2.9, -1.0);
			z->real     = (double)random_float(-0.8, 0.8);
			z->imag     = (double)random_float(-0.3, 1.2);
			*side 	= (double)random_float(2.8, 9.3);
			break;
	    	case FUZZY:
			start->real = (double)random_float(-3.0, -2.4);
			start->imag = (double)random_float(-2.2, -1.9);
			z->real     = (double)random_float(-0.2, 0.0);
			z->imag     = (double)random_float(-0.6, 0.0);
			*side 	    = (double)random_float(5.0, 7.6);
			break;
		case FUZZY2:
			start->real = (double)random_float(-2.0, -1.6);
			start->imag = (double)random_float(-1.5, -1.1);
			z->real     = (double)random_float(1.1, 1.3);
			z->imag     = (double)random_float(-0.1, 0.1);
			*side 	    = (double)random_float(3.3, 4.7);
			break;
	    	case STARFISH3:
			start->real = (double)random_float(-0.9, -0.8);
			start->imag = (double)random_float(-0.4, -0.3);
			z->real     = (double)random_float(0.2, 0.3);
			z->imag     = (double)random_float(0.2, 0.3);
			*side 	    = (double)random_float(0.2, 0.5);
			break;
	    	case STARFISH:
			start->real = (double)random_float(-0.006, -0.005);
			start->imag = (double)random_float(-0.75, -0.70);
			z->real     = (double)random_float(0.0, 0.2);
			z->imag     = (double)random_float(-0.5, -0.4);
			*side 	    = (double)random_float(0.16, 0.77);
			break;
	    	case BUG:
			start->real = (double)random_float(-2.3, 0.1);
			start->imag = (double)random_float(-3.3, 0.1);
			z->real     = (double)random_float(-0.5, 0.8);
			z->imag     = (double)random_float(-0.2, 0.1);
			*side 	    = (double)random_float(3.5, 9.7);
			break;
	    	case ZSQ_HALFC_Z:
			start->real = (double)random_float(-4.4, -0.3);
			start->imag = (double)random_float(-1.9, -1.5);
			z->real     = (double)random_float(-1.1, 0.1);
			z->imag     = (double)random_float(-0.5, -0.2);
			*side       = (double)random_float(3.1, 8.1);
			//start->real = (double)gen_rand_float(1900, 3000, -1000);
			//start->imag = (double)gen_rand_float( 800, 1500, -1000);
			//z->real     = (double)gen_rand_float( 500, 1000,  1000);
			//z->imag     = (double)gen_rand_float(   0,  255,  1000);
			//*side       = (double)gen_rand_float( 150,  190,   100);
			break;
		default: /* assume Mandelbrot */
			start->real = -2.1; start->imag = -1.4;
			z->real = 0; z->imag = 0;
			*side = 3.2;
			break;
	   }
/* OK, so it's _very_ ugly but, it works */
	} else { 
	   switch(type) {
	   	case TEST:
			start->real = -2.4; start->imag = -1.4;
			z->real = 0.0; z->imag = 0.0;
			*side = 5.0;
			break;
	   	case MANDELBROT:
			//start->real = -2.15; start->imag = -1.2;
			start->real = -2.6; start->imag = -1.2;
			z->real = 0; z->imag = 0;
			// *side = 3.2;
			*side = 4.2; /* 16x9 */
			break;
		case MANDELCUBE:
			//start->real = -1.92; start->imag = -1.42;
			start->real = -2.45; start->imag = -1.4;
			z->real = 0; z->imag = 0;
			//*side = 5.0;
			//*side = 3.7;
			*side       = (double)random_float(3.0, 6.1);
			break;
		case MANDELQUAD:
			start->real = -1.8; start->imag = -1.3;
			z->real = 0; z->imag = 0;
			//*side = 4.2;  /* 3.6 */
			*side       = (double)random_float(3.0, 6.0);
			break;
		case MANDELQUINT:
			//start->real = -2.6; start->imag = -2.0;
			start->real = -2.6; start->imag = -1.5;
			z->real = 0; z->imag = 0.2;
		//	*side = 5.3;
			*side       = (double)random_float(3.0, 6.0);
			break;
	    	case MANDELHEXNOT:
			//start->real = -2.7; start->imag = -2.1;
			start->real = -2.7; start->imag = -1.5;
			z->real = 0; z->imag = 0;
			//*side = 5.5;
			*side       = (double)random_float(3.0, 6.0);
			break;
	   	case SIMPLEC:
			//start->real = -0.576; start->imag = -0.433;
			z->real = 0.0; z->imag = 0.0;
			start->real = -0.976; start->imag = -0.56;
			//z->real = 0.3; z->imag = 0.1;
			// *side = 1.159;
			//*side = 2.0; /* 16/9 */
			*side       = (double)random_float(1.0, 6.0);
			break;
	   	case STARSHINE:
			start->real = -1.76; start->imag = -1.36;
			z->real = 0.537; z->imag = -0.148;
		//	*side = 4.6;
			*side       = (double)random_float(3.0, 6.0);
			break;
	   	case MONET:
			//start->real = -2.747; start->imag = -2.05;
			start->real = -2.6; start->imag = -1.5;
			z->real = 0.3; z->imag = 0.1;
			//*side = 5.494; /* 4.88 */
			*side       = (double)random_float(3.0, 6.0);
			break;
	   	case FACE:
			start->real = -2.75; start->imag = -1.88;
			z->real = -0.77; z->imag = 0.47;
			//*side = 6.5;
			//*side = 5.5;
			*side       = (double)random_float(3.0, 6.0);
			break;
	    	case REVERSE:
			//start->real = -0.8; start->imag = -1.1;
			start->real = -1.3; start->imag = -1.1;
			z->real = 0; z->imag = 0;
			//*side = 4.0;
			*side       = (double)random_float(3.0, 6.0);
			break;
	    	case TRIANGLE:
			start->real = -1.92; start->imag = -1.78;
			z->real = -0.2; z->imag = 0.2;
			//*side = 6.7;
			*side       = (double)random_float(5.0, 9.0);
			break;
	    	case STRETCH:
			//start->real = -3.1; start->imag = -2.7;
			start->real = -3.5; start->imag = -2.0;
			z->real = 0.3; z->imag = -0.2;
			//*side = 6.9;
			*side       = (double)random_float(3.0, 8.0);
			break;
	    	case FUZZY:
			//start->real = -2.65; start->imag = -2.011;
			start->real = -3.2; start->imag = -2.0;
			z->real = 0; z->imag = 0;
			//*side = 7.2;
			//*side = 5.4;
			*side       = (double)random_float(3.0, 8.0);
			break;
		case FUZZY2:
			//start->real = -2.24; start->imag = -3.34;
			start->real = -4.24; start->imag = -2.0;
			z->real = 0; z->imag = 0;
			//*side = 7.8;
			*side       = (double)random_float(3.3, 9.9);
			break;
	    	case STARFISH3:
			start->real = -2.7; start->imag = -1.3;
			z->real = 0.0; z->imag = 0.0;
			//*side = 3.9;
			*side       = (double)random_float(3.0, 7.0);
			break;
		case STARFISH:
			start->real = -3.1; start->imag = -1.4;
			z->real = 0.0; z->imag = 0.0;
			//*side = 5.7;
			//*side = 4.44;
			*side       = (double)random_float(3.0, 7.0);
			break;
	   	case BUG:
			start->real = -2.26; start->imag = -2.55;
			z->real = 0.1; z->imag = 0.0;
			//*side = 6.6; /* 8.8 9x16 */
			*side       = (double)random_float(3.0, 9.9);
			break;
	   	case ZSQ_HALFC_Z:
			//start->real = -5.8; start->imag = -2.9;
			start->real = -3.9; start->imag = -2.2;
			z->real = 0.0; z->imag = 0.0;
			//*side = 7.5;
			//*side = 5.0;
			*side       = (double)random_float(3.0, 9.9);
			break;
		default:
			start->real = -2.1; start->imag = -1.4;
			z->real = 0; z->imag = 0;
			*side = 3.2;
			break;
		}
	}
}

/***
 */
WallFArt *WallFArt_data_generate
	(WallFArt_types type, int width, int height, ca_rule *car, boolean randomly)
{
	WallFArt_complex startC, Z, modifier;
	double side;

	SIZE_CHECK(width, height);

	if (car == NULL) {
	  car = new_car();
	  init_ca_rule(car);
	}

	if (type == RANDOMTYPE) {
		type = PICK_A_RANDOM_TYPE(); 
	} 

	switch(type) {
		case TEST:
			//complex_init(modifier, 1.5, 0);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case STARSHINE:
			//complex_init(modifier, 1.5, 0);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case SIMPLEC:
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case MONET:
			//complex_init(modifier, 1.5, 0);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case FACE:
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case MANDELBROT:
			complex_clr(modifier);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case MANDELCUBE:
			complex_clr(modifier);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;	
		case MANDELQUAD:
			complex_clr(modifier);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;	
		case MANDELQUINT:
			complex_clr(modifier);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;	
		case MANDELHEXNOT:
			complex_clr(modifier);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case REVERSE:
			complex_clr(modifier);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case TRIANGLE:
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case STRETCH:
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case FUZZY:
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case FUZZY2:
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case STARFISH3:
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case STARFISH:
			// complex_init(modifier, -1.0, 0.2);
			complex_init(modifier, -0.0, 0.0);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case BUG:
			//complex_init(modifier, 1.0, 0);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		case ZSQ_HALFC_Z: /* z = z^2 + 0.5c *z */
			//complex_init(modifier, 0.5, 0);
			WallFArt_data_get_vars(type, &startC, &Z, &side, randomly);
			break;
		default:
			complex_clr(modifier);
			complex_init(Z, -0.0, 0.0);
			complex_init(startC, -1.0, -0.9);
			side = 2.3;  
			break;
	}
	return 
	(WallFArt_new(NULL, type, width, height, startC, Z, modifier, side, car));
}

/***************************************************************/
static const char *names[] = {
	"TEST", "MANDELBROT", "MANDELCUBE", "MANDELQUAD", 
	"MANDELQUINT", "MANDELHEXNOT", "STARFISH3", "STARFISH",
	"STARSHINE", "SIMPLEC", "FUZZY", "FUZZY2", "MONET",
	"TRIANGLE", "STRETCH", "REVERSE", "BUG", "FACE",
	"ZSQ_HALFC_Z", NULL};

void WallFArt_dump(WallFArt *f)
{
#if !defined ABS
#define ABS(n)	(((n) < 0) ? (-(n)) : n)
#endif

#define print_complex(cn) { 				\
	printf("%2.8f", (cn).real);			\
	char *sign = (((cn).imag < 0) ? "-" : "+");	\
	printf(" %s ", sign);				\
	printf("%2.8f i", ABS((cn).imag));		}

#define NL printf("\n")

	printf("\n    **** BASIC:\n");
	printf("\t Type:\t\t\t%s (%d)\n", names[f->type],f->type);
	printf("\t Width:\t\t\t%d\n", f->width);
	printf("\t Height:\t\t%d\n", f->height);
	printf("\n");
	printf("\t Min Iterations:\t%d\n", f->min_iter);
	printf("\t Max Iterations:\t%d\n", f->max_iter);
	printf("    **** DETAILS:\n");
	printf("\t Start Corner:\t\t");print_complex(f->startC); NL;
	printf("\t Z:\t\t\t");print_complex(f->Z); NL;
	printf("\t modifier:\t\t");print_complex(f->modifier); NL;
	printf("\t Side:\t\t\t%2.8f\n", f->side);
	printf("\t Gap:\t\t\t%2.8f\n", f->gap);
	printf("\t Minimum:\t\t");print_complex(f->min); NL;
	printf("\t Maximum:\t\t");print_complex(f->max); NL;
	printf("\n");
}
