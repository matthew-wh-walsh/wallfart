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
 *  Using no particular logic, I started playing with the basic Mandelbrot 
 *	set and kept some variations that I thought resulted in a 'nifty' 
 *	'neat' or just a plain, gosh darned, 'keen' graphic... groovy baby.
 *
 */
#ifndef _WALLFART_TYPES_H_
#define _WALLFART_TYPES_H_

#include <stdlib.h>   /* for rand() && RAND_MAX */
#include "wallfart_complex.h" 

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * Except for Mandelbrot, these type names are all of my own invention 
 */
typedef enum { 
	TEST,
	MANDELBROT,
	MANDELCUBE, 
	MANDELQUAD, 
	MANDELQUINT, 
	MANDELHEXNOT, 
	STARFISH3,
	STARFISH,
	STARSHINE,
	SIMPLEC, 
	FUZZY, 
	FUZZY2,
	MONET,
	TRIANGLE,
	STRETCH,
	REVERSE, 
	BUG,
	FACE,
	ZSQ_HALFC_Z,  /* My imagination had left me by this one */
	RANDOMTYPE    /* so I stopped. */
} WallFArt_types;

/*
 *  ''RANDOMTYPE'' is both a type and a measure; again, groovy.
 */
#define PICK_A_RANDOM_TYPE() \
	((int)((RANDOMTYPE)*(((float)rand())/((float)RAND_MAX))))

/* test type: currently an eight legged beastie: I like it. 	*/
/* so I'm Keeping it around */
#define DO_TEST(complexZ, complexC) {				\
	WallFArt_complex tmp1, tmp2;				\
	complex_cube(tmp1, complexZ);				\
	complex_cube(tmp2, complexC);				\
	complex_add(complexZ, tmp1, tmp2); 			\
	complex_cube(tmp1, complexZ);				\
	complex_add(complexZ, tmp1, complexC);}

/* Z = Z^2 + C */
#define DO_MANDELBROT(complexZ, complexC) {			\
	WallFArt_complex tmp1;					\
	complex_square(tmp1, complexZ);				\
	complex_add(complexZ, tmp1, complexC); 			}

/* Z = Z^3 + C */
#define DO_MANDELCUBE(complexZ, complexC) { 			\
	WallFArt_complex tmp1; 		 			\
	complex_cube(tmp1, complexZ);				\
	complex_add(complexZ, tmp1, complexC); 			}

/* Z = Z^4 + C */
#define DO_MANDELQUAD(complexZ, complexC) {			\
	WallFArt_complex tmp1, tmp2;				\
	complex_square(tmp1, complexZ);				\
	complex_square(tmp2, tmp1);				\
	complex_add(complexZ, tmp2, complexC); }

/* Z = Z^5 + C */
#define DO_MANDELQUINT(complexZ, complexC) {			\
	WallFArt_complex tmp1, tmp2;				\
	complex_cube(tmp1, complexZ);				\
	complex_square(tmp2, tmp1);				\
	complex_add(complexZ, tmp2, complexC); }

/*	NO.. NO more cube quad quint etc... so, 'HEXNOT'	*/
/* TODO 							*/
/*  hmm, this is odd, I don't understand, tmp1 is never defined */
/*    and yet things work consistently.. some sort of voodoo 
      magic no doubt.   					*/
#define DO_MANDELHEXNOT(complexZ, complexC) {			\
	WallFArt_complex tmp1, tmp2;				\
	complex_product(tmp2, tmp1, complexZ);			\
	complex_square(tmp1, complexZ);				\
	complex_sub(complexZ, tmp2, complexC); }

/* ((z^2+c^2)^2)+c == 3 legged 					*/
#define DO_STARFISH3(complexZ, complexC) {			\
	WallFArt_complex tmp1, tmp2;				\
	complex_square(tmp1, complexZ);				\
	complex_square(tmp2, complexC);				\
	complex_add(complexZ, tmp1, tmp2); 			\
	complex_square(tmp1, complexZ);				\
	complex_add(complexZ, tmp1, complexC);}

/* ((z^3+c^3)^2)+c == 5 legged 					*/
#define DO_STARFISH(complexZ, complexC) {			\
	WallFArt_complex tmp1, tmp2;				\
	complex_cube(tmp1, complexZ);				\
	complex_cube(tmp2, complexC);				\
	complex_add(complexZ, tmp1, tmp2); 			\
	complex_square(tmp1, complexZ);				\
	complex_add(complexZ, tmp1, complexC);}

/* Z = (Z*(1.0+0.1i))+(C^5) 					*/
#define DO_STARSHINE(complexZ, complexC) {			\
	WallFArt_complex tmp1, tmp2, modifier;			\
	complex_init(modifier, 1.0, 0.1);			\
	complex_product(tmp2, modifier, complexZ);		\
	complex_cube(tmp1, complexC);				\
	complex_square(tmp1, tmp1);				\
	complex_add(complexZ, tmp1, tmp2);}

/* Z = ((Z*(1.0+0.1i)) + C^2)  					*/
#define DO_SIMPLEC(complexZ, complexC) {			\
	WallFArt_complex tmp1, tmp2, modifier;			\
	complex_init(modifier, 1.0, 0.1);			\
	complex_product(tmp2, modifier, complexZ);		\
	complex_square(tmp1, complexC);				\
	complex_add(complexZ, tmp1, tmp2);}

/* lots of noise Z = ((((C-(0+0i))*Z)-C) + (Z^2))     		*/
/* unlike the Grinch, I _can_ stand the noise...  		*/
#define DO_FUZZY(complexZ, complexC) {				\
	WallFArt_complex tmp1, tmp2, modifier;			\
	complex_init(modifier, .0 , 0);				\
	complex_sub(tmp1, complexC, modifier);			\
	complex_product(tmp2, tmp1, complexZ);			\
	complex_sub(tmp1, tmp2, complexC);			\
	complex_square(tmp2, complexZ);				\
	complex_add(complexZ, tmp2, tmp1);}

/* somewhat different noise, kinda peanut shaped. I like it.	*/
#define DO_FUZZY2(complexZ, complexC) {				\
	WallFArt_complex tmp1, tmp2, modifier;			\
	complex_init(modifier, 1.0 , 0.0);			\
	complex_sub(tmp1, complexC, modifier);			\
	complex_product(tmp2, tmp1, complexZ);			\
	complex_sub(tmp1, tmp2, complexC);			\
	complex_square(tmp2, complexZ);				\
	complex_add(complexZ, tmp2, tmp1);}

/* while playing with this, a graphic that reminded me of a Monet
print I used to have manifested itself on my screen; 		*/
#define DO_MONET(complexZ, complexC) {				\
	WallFArt_complex tmp1, tmp2;;				\
	complex_product(tmp2, tmp1, complexZ);			\
	complex_cube(tmp1, complexZ);				\
	complex_sub(tmp2, tmp1, tmp2);				\
	complex_sub(complexZ, tmp2, complexC);}

/* TRIANGLE  Z^5+Z^2+((0.5+0.5i) - C)   */
/* for some reason, I like triangles: Bit of a fixation actually. */
#define DO_TRIANGLE(complexZ, complexC) {			\
	WallFArt_complex tmp1, tmp2, modifier;			\
	complex_init(modifier,0.5, 0.5);			\
	complex_square(tmp1, complexZ);				\
	complex_cube(tmp2, tmp1);				\
	complex_sub(tmp1, modifier, complexC);			\
	complex_add(tmp2, tmp1, tmp2);				\
	complex_product(complexZ, tmp1, complexZ);}


/* Z = ((0.5+-0.2i) - C) * ((Z^5+Z^2) - C) 			*/
#define DO_STRETCH(complexZ, complexC) {			\
	WallFArt_complex tmp1, tmp2, modifier;			\
	complex_init(modifier, 0.5, 0.2);			\
	complex_square(tmp1, complexZ);				\
	complex_cube(tmp2, tmp1);				\
	complex_add(tmp2, tmp1, tmp2);				\
	complex_sub(complexZ, tmp2, complexC);			\
	complex_sub(tmp1, modifier, complexC);			\
	complex_product(complexZ, tmp1, complexZ);}

/* Z =  Z - ((Z * (0+0i)) + Z^2)	 			*/
#define DO_REVERSE(complexZ, complexC) {			\
	WallFArt_complex tmp1, tmp2;				\
	tmp1.real=0;tmp1.imag=0;				\
	complex_product(tmp2, tmp1, complexZ);			\
	complex_square(tmp1, complexZ);				\
	complex_add(tmp2, tmp1, tmp2);				\
	complex_sub(complexZ, tmp2, complexC);}

/* Z = C * (Z * (1-Z)) 						*/
#define DO_BUG(complexZ, complexC) {				\
	WallFArt_complex tmp1, tmp2, modifier;			\
	complex_init(modifier, 1.0, 0.0);			\
	complex_sub(tmp1, modifier, complexZ); 			\
	complex_product(tmp2, tmp1, complexZ);			\
	complex_product(complexZ, complexC, tmp2);}

/* (Z^3+C^3) + C == (I see faces everywhere) 			*/
#define DO_FACE(complexZ, complexC) {				\
	WallFArt_complex tmp1, tmp2;				\
	complex_cube(tmp1, complexZ); 				\
	complex_cube(tmp2, complexC);				\
	complex_add(complexZ, tmp1, tmp2); 			\
	complex_add(complexZ, complexZ, complexC);}

/* z = z^2 + 0.5c + z 						*/
/* z=z^2+c; c=c*0.5+z 						*/
#define DO_ZSQ_HALFC_Z(complexZ, complexC) {			\
	WallFArt_complex tmpz, tmpc, modifier;			\
	complex_init(modifier, 0.5, 0.0); 			\
	complex_square(tmpz, complexZ); 			\
	complex_product(tmpc, complexC, modifier);		\
	complex_add(tmpc, tmpc, complexZ);			\
	complex_add(complexZ, tmpz, tmpc);}

#ifdef __cplusplus
}
#endif

#endif /* _WALLFART_TYPES_H_ */
