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
 * A long time ago, in a galaxy far, far away, when I first started making 
 *	fractals, I timed some operations comparing these macros with a 
 *	builtin complex number type and these macros were faster.
 *	Doesn't matter much these days, if I were to work on this anymore, 
 *	I would.switch to some internal library representation of complex
 *	numbers (<complex>||<complex.h>)
 */
#ifndef _WALLFART_COMPLEX_H_
#define _WALLFART_COMPLEX_H_

#include <math.h> /* for sqrt() */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _WallFArt_complex WallFArt_complex;
    struct _WallFArt_complex {
	double real;
	double imag;
    };

#define complex_init(cn,r,i)  	cn.real=r; cn.imag=i
#define complex_clr(cn) 	complex_init(cn,0,0)
#define complex_assign(cn, n)				\
	complex_init(cn,n.real, n.imag)

#define complex_compare(cn1, cn2)			\
	((cn1.real==cn2.real) && (cn1.imag==cn2.imag)) 

/** SIZE: (a+bi) = sqrt(a^2 + b^2) 	 	       */
#define complex_sqrsum(cn)				\
	(cn.real*cn.real+cn.imag*cn.imag)
#define complex_size(cn)				\
	sqrt(complex_sqrsum(cn))

/** SUM: (a+bi) + (c+di) = (a+c) + (b+d)i		*/
#define complex_add(result, c1, c2)			\
	result.real = ((c1).real+(c2).real);		\
	result.imag = ((c1).imag+(c2).imag)
#define complex_sum complex_add

/** DIFFERENCE: (a+bi) - (c+di) = (a+c) - (b+d)i	*/
#define complex_sub(result, c1, c2)			\
	result.real = ((c1).real-(c2).real);		\
	result.imag = ((c1).imag-(c2).imag)
#define complex_diff complex_sub

/** PRODUCT: (a+bi) * (c+di) = (ac-bd) + (ad+bc)i	*/
#define complex_product(result, c1, c2)			\
	result.real = c1.real*c2.real-c1.imag*c2.imag;	\
	result.imag = c1.real*c2.imag+c1.imag*c2.real
#define complex_mult complex_product

/** SQUARE: (a+bi)^2 = (a*a - b*b) + (2ab)i		*/ 
#define complex_square(result, cn)			\
	complex_product(result, cn, cn)

/** CUBE: (a+bi)^3 = a(a*a-3(b*b))+b(3(a*a)-(b*b)i	*/ 
#define complex_cube(result, cn) 			\
	result.real = 					\
	cn.real*(cn.real*cn.real-(3*(cn.imag*cn.imag)));\
	result.imag =					\
	cn.imag*(3*(cn.real*cn.real)-(cn.imag*cn.imag));

/** no DIV? Nope, not today */

#ifdef __cplusplus
}
#endif

#endif /* _WALLFART_COMPLEX_H_ */
