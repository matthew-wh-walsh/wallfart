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
 * wallfart_colour.h
 *    Meaning the verb, 'colour'; ie, to colour, with colour.
 *	a collection of 36 (17 + 19) of what I called 
 *	'colour appication rules' which are applied via 
 *	the use of something I called ... wait for it... 
 *	a 'colour application rule'... 
 *	Now` doesn't that make a whole lot of sense.
 *	Now` that doesn't make a whole lot of sense.
 * 	NOTE TO SELF:
 *  THERE IS A FLOATING POINT EXCEPTION WAITING TO HAPPEN IN HERE SOMEWHERE 
 */
#ifndef _WALLFART_COLOUR_H_
#define _WALLFART_COLOUR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define EIGHT_BIT_COLOUR 255

typedef unsigned short 	ca_mask; /* colour application mask */
typedef enum {
	RIT = 1 << 0,	/* red iterative 	*/
	RRE = 1 << 1,   /* red real 		*/
	RIM = 1 << 2,   /* red imaginary 	*/
	GIT = 1 << 3,	/* green iterative	*/
	GRE = 1 << 4,	/* green real		*/
	GIM = 1 << 5,	/* etc... 		*/
	BIT = 1 << 6,
	BRE = 1 << 7,
	BIM = 1 << 8
} ColourApplicationFlags;

#define NUM_FLAGS 9
#define MASK_MAX 	0x01ff	 /*  == 1 1111 1111  */
#define RANDMASK()\
	(int)((float)(MASK_MAX+1.0) * rand()/(float)RAND_MAX)

#define USE_RIT(msk)	((msk) & RIT)
#define USE_RRE(msk)	((msk) & RRE)
#define USE_RIM(msk)	((msk) & RIM)
#define USE_GIT(msk)	((msk) & GIT)
#define USE_GRE(msk)	((msk) & GRE)
#define USE_GIM(msk)	((msk) & GIM)
#define USE_BIT(msk)	((msk) & BIT)
#define USE_BRE(msk)	((msk) & BRE)
#define USE_BIM(msk)	((msk) & BIM)

/* throw in some additional multipliers for more of a ''randomness'' feel
 */
#define MAX_MULT EIGHT_BIT_COLOUR
#define RANDMULT()\
	((int)((((float)MAX_MULT) * rand())/((float)RAND_MAX)))

/* struct colour-application-rule: an attempt to make ''pseudo-random''
 *	feel more... well, more random.
 */
typedef struct _ca_rule {
	ca_mask mask;		 /* what combo of values to use */
	int     mult[NUM_FLAGS]; /* what to multiply them by */
} ca_rule;

/*  A new_car: "C'mon down!..."
 */
#define new_car() (ca_rule *)malloc(sizeof(ca_rule))

#define init_ca_rule(car) {			\
	int x;					\
	for (x=0; x<NUM_FLAGS; x++) {		\
		car->mult[x] = RANDMULT();	\
	}					\
	car->mask = RANDMASK();			} 

/* I divided things up so the first 6 multipliers apply to outside 
    hence the ..._ruleO() and ..._ruleI() */
#define init_ca_ruleO(car) {			\
	int x;					\
	for (x=0; x<6; x++) {			\
		car->mult[x] = RANDMULT();	\
	}					\
	car->mask = RANDMASK();			} 

/* the last 3 multipliers apply to inside */
#define init_ca_ruleI(car) {			\
	int x;					\
	for (x=6; x<NUM_FLAGS; x++) {		\
		car->mult[x] = RANDMULT();	\
	}					}

/* some more, perhaps useful, macros */
#define copy_ca_rule(dst, src) {		\
	int x;					\
	for (x=0; x<NUM_FLAGS; x++) {		\
		dst->mult[x] = src->mult[x];	\
	} dst->mask = src->mask; 		}

#define get_gradient(value)\
	((value * idx)/(f->width * f->height))
#define unch unsigned char
#define abSolute(x) (x<0) ? (x * -1) : x

/***
 * this was my first idea on how to colour randomly..
 *	the start of it all... makes me feel all warm 'n fuzzy
 */
#define apply_ca_rule0(red, green, blue, car, f, idx, x, y) \
   red    = ((USE_RIT(car->mask)) ? (f->iter[idx] +       car->mult[0]) : 0); 	\
   red   += ((USE_RRE(car->mask)) ? (f->slice[idx].real * car->mult[1]) : 0); 	\
   red   += ((USE_RIM(car->mask)) ? (f->slice[idx].imag * car->mult[2]) : 0); 	\
   red   %= EIGHT_BIT_COLOUR;						   	\
   green  = ((USE_GIT(car->mask)) ? (f->iter[idx] +       car->mult[1]) : 0); 	\
   green += ((USE_GRE(car->mask)) ? (f->slice[idx].real * car->mult[2]) : 0); 	\
   green += ((USE_GIM(car->mask)) ? (f->slice[idx].imag * car->mult[3]) : 0); 	\
   green %= EIGHT_BIT_COLOUR;						   	\
   blue   = ((USE_BIT(car->mask)) ? (f->iter[idx] +       car->mult[2]) : 0); 	\
   blue  += ((USE_BRE(car->mask)) ? (f->slice[idx].real * car->mult[3]) : 0); 	\
   blue  += ((USE_BIM(car->mask)) ? (f->slice[idx].imag * car->mult[4]) : 0); 	\
   blue  %= EIGHT_BIT_COLOUR;

/*** 
 * but it didn't feel random enough so I kept at it...  
 */
#define OR_THEM(x, y) ((unch)x | (unch)y)

#define apply_ca_rule1(red, green, blue, car, f, idx, x, y) \
   red    = ((USE_RIT(car->mask)) ? OR_THEM(f->iter[idx], car->mult[0]) : 0); 		\
   red   += ((USE_RRE(car->mask)) ? OR_THEM(f->slice[idx].real, car->mult[1]) : 0);\
   red   += ((USE_RIM(car->mask)) ? OR_THEM(f->slice[idx].imag, car->mult[2]) : 0); 	\
   red   %= EIGHT_BIT_COLOUR;						   		\
   green  = ((USE_GIT(car->mask)) ? OR_THEM(f->iter[idx], car->mult[1]) : 0); 		\
   green += ((USE_GRE(car->mask)) ? OR_THEM(f->slice[idx].real, car->mult[2]) : 0); 	\
   green += ((USE_GIM(car->mask)) ? OR_THEM(f->slice[idx].imag, car->mult[3]) : 0); 	\
   green %= EIGHT_BIT_COLOUR;						   		\
   blue   = ((USE_BIT(car->mask)) ? OR_THEM(f->iter[idx], car->mult[2]) : 0); 		\
   blue  += ((USE_BRE(car->mask)) ? OR_THEM(f->slice[idx].real, car->mult[3]) : 8); 	\
   blue  += ((USE_BIM(car->mask)) ? OR_THEM(f->slice[idx].imag, car->mult[4]) : 0); 	\
   blue  %= EIGHT_BIT_COLOUR;

/***
 * ... and at it...
 * 	let's just OR everything, shall we... (AND is too dark too often)
 */
#define apply_ca_rule2(red, green, blue, car, f, idx, x, y) \
   red    = ((USE_RIT(car->mask)) ? OR_THEM(f->iter[idx], car->mult[0]) : 0); 		\
   red   |= ((USE_RRE(car->mask)) ? OR_THEM(f->slice[idx].real, car->mult[1]) : 0);\
   red   |= ((USE_RIM(car->mask)) ? OR_THEM(f->slice[idx].imag, car->mult[2]) : 0); 	\
   red   %= EIGHT_BIT_COLOUR;						   		\
   green  = ((USE_GIT(car->mask)) ? OR_THEM(f->iter[idx], car->mult[1]) : 0); 		\
   green |= ((USE_GRE(car->mask)) ? OR_THEM(f->slice[idx].real, car->mult[2]) : 0); 	\
   green |= ((USE_GIM(car->mask)) ? OR_THEM(f->slice[idx].imag, car->mult[3]) : 0); 	\
   green %= EIGHT_BIT_COLOUR;						   		\
   blue   = ((USE_BIT(car->mask)) ? OR_THEM(f->iter[idx], car->mult[2]) : 0); 		\
   blue  |= ((USE_BRE(car->mask)) ? OR_THEM(f->slice[idx].real, car->mult[3]) : 8); 	\
   blue  |= ((USE_BIM(car->mask)) ? OR_THEM(f->slice[idx].imag, car->mult[4]) : 0); 	\
   blue  %= EIGHT_BIT_COLOUR;

/************************************************************************************
 * then I thought, wtfudge, I may as well try some colour tables... (I was avoiding them)
 */
#include "wallfart_rgb_ctables.h"

#define apply_ca_rule3(red, green, blue, car, f, idx, x, y) \
   red    = ((USE_RIT(car->mask)) ? (wallfartRedCt[f->iter[idx]] ) : 0);  	\
   red   |= ((USE_RRE(car->mask)) ? OR_THEM(f->slice[idx].real, car->mult[1]) : 0);	\
   red   |= ((USE_RIM(car->mask)) ? OR_THEM(f->slice[idx].imag, car->mult[2]) : 0); 	\
   red   %= EIGHT_BIT_COLOUR;						   		\
   green  = ((USE_GIT(car->mask)) ? (wallfartGreenCt[f->iter[idx]] ) : 0);	\
   green |= ((USE_GRE(car->mask)) ? OR_THEM(f->slice[idx].real, car->mult[2]) : 0); 	\
   green |= ((USE_GIM(car->mask)) ? OR_THEM(f->slice[idx].imag, car->mult[3]) : 0); 	\
   green %= EIGHT_BIT_COLOUR;						   		\
   blue   = ((USE_BIT(car->mask)) ? (wallfartBlueCt[f->iter[idx]] ) : 0); 	\
   blue  |= ((USE_BRE(car->mask)) ? OR_THEM(f->slice[idx].real, car->mult[3]) : 8); 	\
   blue  |= ((USE_BIM(car->mask)) ? OR_THEM(f->slice[idx].imag, car->mult[4]) : 0); 	\
   blue  %= EIGHT_BIT_COLOUR;

/** use a 15 colour colour table  **/
#define apply_ca_rule4(red, green, blue, car, f, idx, x, y) \
{int index = f->iter[idx]%15;\
   red   = (USE_RIT(car->mask)) ? wf15Red[index]   : car->mult[1];\
   green = (USE_GIT(car->mask)) ? wf15Green[index] : car->mult[3];\
   blue  = (USE_BIT(car->mask)) ? wf15Blue[index]  : car->mult[5];\
}

/* use a 16 colour colour table */
#define apply_ca_rule5(red, green, blue, car, f, idx, x, y) \
{int index = f->iter[idx]%16;\
   red   = wf16Red[index]   & car->mult[1] | car->mult[4];\
   green = wf16Green[index] & car->mult[3] | car->mult[2];\
   blue  = wf16Blue[index]  & car->mult[5] | car->mult[0];\
}

/* mixin' up some colour tables  */
#define apply_ca_rule6(red, green, blue, car, f, idx, x, y)\
if (USE_RIT(car->mask)) {\
red = abSolute((wbgyrRedCt[f->iter[idx]] | (roulletRed[f->iter[idx]])));\
} else {  red = car->mult[0];}\
if (USE_GIT(car->mask)) {\
green = abSolute(wbgyrGreenCt[f->iter[idx]] & roulletGreen[f->iter[idx]]);\
} else {  green = car->mult[1];}\
if (USE_BIT(car->mask)) {\
blue = abSolute(wbgyrBlueCt[f->iter[idx]] & (roulletBlue[f->iter[idx]]));\
} else { blue = car->mult[2];}

/* r7 maybe crash here if car->mult[0]== 0 ? */
#define ARVALUE(car) (unch)(((car->mult[0]/100)*2)+1)
#define apply_ca_rule7(red, green, blue, car, f, idx, x, y) \
if (f->iter[idx] % ARVALUE(car)) {apply_ca_rule0(red,green,blue,car,f,idx,x,y);\
} else {\
red = (wfRedCt[f->iter[idx]] | wfGreenCt[f->iter[idx]]);\
green = (car->mult[3] | wfGreenCt[f->iter[idx]]);\
blue = (wfBlueCt[f->iter[idx]] | wfRedCt[f->iter[idx]]);}

/* maybe */
#define mc_it(f,car) (unch)(f) * (car->mult[3]|car->mult[4]) % (EIGHT_BIT_COLOUR)
#define apply_ca_rule8(red, green, blue, car, f, idx, x, y)\
red = wallfartRedCt[f->iter[idx]] | mc_it(f->slice[idx].imag,car);\
green = wallfartGreenCt[f->iter[idx]] + (unch)car->mult[0];\
blue = wallfartBlueCt[f->iter[idx]] | mc_it(f->slice[idx].imag,car);

#define apply_ca_rule9(red, green, blue, car, f, idx, x, y)\
if (f->iter[idx] > 128) {\
apply_ca_rule8(red, green, blue, car, f, idx, x, y) ;\
} else if (f->iter[idx] > 64) { \
apply_ca_rule0(red, green, blue, car, f, idx, x, y) ;\
} else if (f->iter[idx] > 32) { \
apply_ca_rule6(red, green, blue, car, f, idx, x, y) ;\
} else if (f->iter[idx] > 16) { \
apply_ca_rule1(red, green, blue, car, f, idx, x, y) ;\
} else if (f->iter[idx] > 8) { \
apply_ca_rule0(red, green, blue, car, f, idx, x, y) ;\
} else if (f->iter[idx] > 4) { \
apply_ca_rule3(red, green, blue, car, f, idx, x, y) ;\
} else if (f->iter[idx] > 2) { \
apply_ca_rule2(red, green, blue, car, f, idx, x, y) ;\
}

#define apply_ca_rule10(red, green, blue, car, f, idx, x, y) \
{\
apply_ca_rule9(red,green,blue,car,f,idx,x,y);\
int index = f->iter[idx]%24;\
red   &= (USE_RIT(car->mask)) ? circular24Red[index]   : circular24Red[index]   | car->mult[0];\
green &= (USE_GIT(car->mask)) ? circular24Green[index] : circular24Green[index] | car->mult[0];\
blue  &= (USE_BIT(car->mask)) ? circular24Blue[index]  : circular24Blue[index]  | car->mult[0];\
}

/* I'm just changing things willy-nilly now; don't be looking for any logic:
 	I lost that facility of mind a very long time ago.. if I ever had it */
#define apply_ca_rule11(red, green, blue, car, f, idx, x, y)\
if (f->iter[idx] % 2)\
{\
red = ((unch)(idx *x *y*f->iter[idx]) % EIGHT_BIT_COLOUR);\
green = wallfartGreenCt[f->iter[idx]] | car->mult[0];\
blue = (unch)f->slice[idx].real | car->mult[1];\
} else if (f->iter[idx] % 3)\
{\
red = (unch)f->slice[idx].real | car->mult[2];\
green = ((unch)(idx *x *y*f->iter[idx]) % EIGHT_BIT_COLOUR);\
blue = wallfartBlueCt[f->iter[idx]] | car->mult[3];\
} else\
{\
red = wallfartRedCt[f->iter[idx]] | car->mult[3];\
green = (unch)f->slice[idx].real | car->mult[0];\
blue = ((unch)(idx *x *y*f->iter[idx]) % EIGHT_BIT_COLOUR);}

#define apply_ca_rule12(red, green, blue, car, f, idx, x, y)\
unch modulous = ((unch)((float)(car->mult[4])/100.0)+1);\
if ((unch)f->slice[idx].real % modulous) {\
red = 0; green = 0; blue=0;\
} else {\
red = (unch)(f->iter[idx] * car->mult[0]);\
green=car->mult[1]; blue=car->mult[2];}

#define apply_ca_rule13(red, green, blue, car, f, idx, x, y)\
red = (unch)roulletRed[f->iter[idx]] | car->mult[5];\
green = roulletGreen[f->iter[idx]] | car->mult[5];\
blue = roulletBlue[f->iter[idx]] | car->mult[5];

#define apply_ca_rule14(red, green, blue, car, f, idx, x, y)\
red = wbgyrRedCt[f->iter[idx]] + car->mult[3];\
green = wbgyrGreenCt[f->iter[idx]] + car->mult[3];\
blue = wbgyrBlueCt[f->iter[idx]] + car->mult[3];

#define apply_ca_rule15(red, green, blue, car, f, idx, x, y)\
red = wfRedCt[f->iter[idx]] * car->mult[4];\
green = wfGreenCt[f->iter[idx]] * car->mult[4];\
blue = wfBlueCt[f->iter[idx]] * car->mult[4];

#define apply_ca_rule16(red, green, blue, car, f, idx, x, y)\
if (f->iter[idx] < ((car->mult[0]/4+3)))\
{apply_ca_rule15(red,green,blue,car,f,idx,x,y);\
red |= car->mult[2];}\
else if (f->iter[idx] < ((car->mult[0]/4)+(car->mult[1]/5))+5)\
{apply_ca_rule14(red,green,blue,car,f,idx,x,y);\
green |= car->mult[3];}\
else \
{apply_ca_rule13(red,green,blue,car,f,idx,x,y);\
blue |= car->mult[4];\
}

#define OUTSIDE_RULES 17
extern int outside_rule_num;

#define APPLY_OUTSIDE_COLOUR()\
switch (outside_rule_num) {\
case 0:\
{apply_ca_rule0(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 1:\
{apply_ca_rule1(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 2:\
{apply_ca_rule2(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 3:\
{apply_ca_rule3(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 4:\
{apply_ca_rule4(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 5:\
{apply_ca_rule5(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 6:\
{apply_ca_rule6(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 7:\
{apply_ca_rule7(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 8:\
{apply_ca_rule8(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 9:\
{apply_ca_rule9(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 10:\
{apply_ca_rule10(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 11:\
{apply_ca_rule11(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 12:\
{apply_ca_rule12(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 13:\
{apply_ca_rule13(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 14:\
{apply_ca_rule14(red, green, blue, f->car, f, idx, x, y);};\
break;\
default:\
case 15:\
{apply_ca_rule15(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 16:\
{apply_ca_rule16(red, green, blue, f->car, f, idx, x, y);};\
break;\
{apply_ca_rule0(red, green, blue, f->car, f, idx, x, y);};\
} /* end switch(outside_rule_num) */

/****************************************************************/
// INSIDE COLOUR : when iterations == 0
// 	i divied the ca_rule into 6/3 for outside/inside variation
/*************************************************************** */

/* traditional black */
#define inside_ca_rule0(red, green, blue, car, f, idx, x, y) \
   red    = 0;	\
   green  = 0;	\
   blue   = 0;
/* a random colour */
#define inside_ca_rule1(red, green, blue, car, f, idx, x, y) \
   red    = car->mult[6];	\
   green  = car->mult[7];	\
   blue   = car->mult[8];
/* shades of grey */
#define inside_ca_rule2(red, green, blue, car, f, idx, x, y) \
   red 	  =car->mult[7];\
   green  =red;\
   blue   =green;
/* grey gradient dark->light */
#define inside_ca_rule3(red, green, blue, car, f, idx, x, y) \
   red    = ((EIGHT_BIT_COLOUR * idx) / (f->width * f->height));\
   green  = red; \
   blue   = green;
/* colour gradients */
#define inside_ca_rule4(red, green, blue, car, f, idx, x, y) \
if (car->mult[6] < 85)\
   {red = ((EIGHT_BIT_COLOUR * idx) / (f->width * f->height));\
	green = car->mult[7]; blue = car->mult[8];}\
else if (car->mult[6] < 170)\
   {green = ((EIGHT_BIT_COLOUR * idx) / (f->width * f->height));\
	red = car->mult[7]; blue = car->mult[8];}\
else\
   {blue = ((EIGHT_BIT_COLOUR * idx) / (f->width * f->height));\
	red = car->mult[7]; green = car->mult[8];}
/**/
/* 5,6,7,8 very similar, but different, so...  */
#define inside_ca_rule5(red, green, blue, car, f, idx, x, y) \
   red   = (car->mult[8] > 127) ? car->mult[8] :\
	(unch)(f->slice[idx].imag*(car->mult[6]*10))%255;\
   green = (car->mult[7] > 127) ? car->mult[7] :\
  	(unch)(f->slice[idx].imag*(car->mult[8]*10))%255;\
   blue  = (car->mult[6] > 127) ? car->mult[6] :\
	(unch)(f->slice[idx].imag*(car->mult[7]*10))%255;

#define inside_ca_rule6(red, green, blue, car, f, idx, x, y) \
   red   = (car->mult[8] > 127) ? car->mult[8] :\
	(unch)(f->slice[idx].real*(car->mult[6]|16))%255;\
   green = (car->mult[7] > 127) ? car->mult[7] :\
  	(unch)(f->slice[idx].real*(car->mult[8]|8))%255;\
   blue  = (car->mult[6] > 127) ? car->mult[6] :\
	(unch)(f->slice[idx].real*(car->mult[7]|4))%255;

#define inside_ca_rule7(red, green, blue, car, f, idx, x, y) \
inside_ca_rule6(red,green,blue,car,f,idx,x,y);\
   red   |= (unch)(f->slice[idx].imag*(car->mult[7]*10))%255;\
   green |= (unch)(f->slice[idx].imag*(car->mult[6]*20))%255;\
   blue  |= (unch)(f->slice[idx].imag*(car->mult[8]*30))%255;

#define mc_itI(f,car) ((unch)((f) * (car->mult[7] * car->mult[8])) % 255)
#define inside_ca_rule8(red, green, blue, car, f, idx, x, y) \
   red = (car->mult[6] > 127) ?\
    car->mult[6]/2 | mc_itI(f->slice[idx].real,car) :\
    car->mult[6] | mc_itI(f->slice[idx].imag,car) ;\
   green = (car->mult[7] > 127) ?\
    car->mult[7]/2 | mc_itI(f->slice[idx].real,car) :\
    car->mult[7] | mc_itI(f->slice[idx].imag,car) ;\
   blue  = (car->mult[8] > 127) ?\
    car->mult[8]/2 | mc_itI(f->slice[idx].real,car) :\
    car->mult[8] | mc_itI(f->slice[idx].imag,car);

#define c_it(f) (((unch)(f*100))%255)
#define varc_it(f,car)    (unch)(f*(car->mult[8]+car->mult[7]))%255
#define inside_ca_rule9(red, green, blue, car, f, idx, x, y)\
if (car->mult[6] > 127) {\
   red = wallfartRedCt[abSolute(c_it(f->slice[idx].imag))];\
} else {\
   red = wallfartRedCt[abSolute(c_it(f->slice[idx].real))];}\
if (car->mult[7] > 127) {\
   green = wallfartGreenCt[abSolute(c_it(f->slice[idx].imag))];\
} else {\
   green = wallfartGreenCt[abSolute(c_it(f->slice[idx].real))];}\
if (car->mult[8] > 127) {\
   blue = wallfartBlueCt[abSolute(c_it(f->slice[idx].imag))];\
} else {\
  blue = wallfartBlueCt[abSolute(varc_it(f->slice[idx].real,car))];}

/* every once in a while crashed on this one: not any more..  */
#define inside_ca_rule10(red, green, blue, car, f, idx, x, y) ;\
{\
int vari1 = ((car->mult[6]*8));\
int vari2 = (vari1 + (car->mult[7]*4));\
int vari3 = (vari2 + (car->mult[8]*2));\
   red   = (f->iter[abSolute(idx-vari1+y)] * car->mult[6]) %  255;\
   green = (f->iter[abSolute(idx-vari2+y)] * car->mult[7]) % 255;\
   blue  = (f->iter[abSolute(idx-vari3+y)] * car->mult[8]) % 255;\
}

#define inside_ca_rule11(red, green, blue, car, f, idx, x, y) \
int div = (int)((float)(f->height*f->width)/6.0);\
if (idx > div)\
{\
int back = (int)(((float)(f->height * f->width)/7.0)-1.0);\
   red = wallfartRedCt[(unch)f->iter[idx-back]];\
   red |= car->mult[8];\
   green = (unch)c_it(f->slice[idx-back].imag);\
   green |= car->mult[7];\
   blue= (unch)c_it(f->slice[idx-back].real);\
   blue |= car->mult[6];\
}

/**** FIXME stuff: */

#if 0 /* why did I leave this here ? */
#define inside_ca_rule10(red, green, blue, car, f, idx, x, y) ;\
{\
int vari1 = ((car->mult[6]*8));\
int vari2 = (vari1 + (car->mult[7]*4));\
int vari3 = (vari2 + (car->mult[8]*2));\
red = (f->iter[(idx-vari1+y)];\
green = (f->iter[(idx-vari2+y)];\
blue = (f->iter[idx-vari3+y];\
}
#endif

/* thin lines in Mandelbrot middle probably a crash now && then FIXME */
#define inside_ca_rule12(red, green, blue, car, f, idx, x, y) \
   red = ((idx < 5000) ? idx%EIGHT_BIT_COLOUR : (f->iter[idx-car->mult[6]*10+y])); \
   red += (f->iter[idx-car->mult[6]*10+y]); \
   green = ((idx < 6000) ? 0 : (f->iter[idx-car->mult[7]*10+y])); \
   green += (f->iter[idx-car->mult[7]*10+y]); \
   blue = ((idx < 7000) ? 0 : (f->iter[idx-7000+y]));\
   blue += (f->iter[idx-car->mult[8]*10+y]);

/* mostly red */
#define inside_ca_rule13(red, green, blue, car, f, idx, x, y) \
red=EIGHT_BIT_COLOUR;\
green=car->mult[8];\
blue=car->mult[7];
/* mostly green */
#define inside_ca_rule14(red, green, blue, car, f, idx, x, y) \
red=car->mult[8];\
green=EIGHT_BIT_COLOUR;\
blue=car->mult[7];
/* mostly blue */
#define inside_ca_rule15(red, green, blue, car, f, idx, x, y) \
red=car->mult[8];\
green=car->mult[7];\
blue=EIGHT_BIT_COLOUR;

#define inside_ca_rule16(red, green, blue, car, f, idx, x, y) \
{unch evaluator = ((unch)(f->slice[idx].real * car->mult[7]) % EIGHT_BIT_COLOUR);\
if (evaluator > 64)\
{\
inside_ca_rule5(red, green, blue, car, f, idx, x, y);\
} else if (evaluator > 128) {\
inside_ca_rule6(red, green, blue, car, f, idx, x, y);\
} else if (evaluator > 192) {\
inside_ca_rule7(red, green, blue, car, f, idx, x, y);\
} else {\
inside_ca_rule8(red, green, blue, car, f, idx, x, y);\
}}

/* it does makes a difference so  */
#define inside_ca_rule17(red, green, blue, car, f, idx, x, y) \
{unch evaluator = ((unch)(f->slice[idx].real * car->mult[7]) % EIGHT_BIT_COLOUR);\
if (evaluator < 64) {\
inside_ca_rule5(red, green, blue, car, f, idx, x, y);\
} else if (evaluator < 128) {\
inside_ca_rule6(red, green, blue, car, f, idx, x, y);\
} else if (evaluator < 192) {\
inside_ca_rule7(red, green, blue, car, f, idx, x, y);\
} else {\
inside_ca_rule8(red, green, blue, car, f, idx, x, y);\
}}

/* stripes of various densities && colours */
#define inside_ca_rule18(red, green, blue, car, f, idx, x, y) \
   red   = (car->mult[6] > 127) ? (x*y*idx)%car->mult[7] : car->mult[6];\
   green = (car->mult[7] > 127) ? (x*y*idx)%car->mult[8] : car->mult[7];\
   blue  = (car->mult[8] > 127) ? (x*y*idx)%car->mult[6] : car->mult[8];\

#define INSIDE_RULES 19
extern int inside_rule_num;

#define APPLY_INSIDE_COLOUR()\
switch (inside_rule_num) {\
case 0:\
{inside_ca_rule0(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 1:\
{inside_ca_rule1(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 2:\
{inside_ca_rule2(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 3:\
{inside_ca_rule3(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 4:\
{inside_ca_rule4(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 5:\
{inside_ca_rule5(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 6:\
{inside_ca_rule6(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 7:\
{inside_ca_rule7(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 8:\
{inside_ca_rule8(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 9:\
{inside_ca_rule9(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 10:\
{inside_ca_rule10(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 11:\
{inside_ca_rule11(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 12:\
{inside_ca_rule12(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 13:\
{inside_ca_rule13(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 14:\
{inside_ca_rule14(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 15:\
{inside_ca_rule15(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 16:\
{inside_ca_rule16(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 17:\
{inside_ca_rule17(red, green, blue, f->car, f, idx, x, y);};\
break;\
case 18:\
{inside_ca_rule18(red, green, blue, f->car, f, idx, x, y);};\
break;\
default:\
{inside_ca_rule1(red, green, blue, f->car, f, idx, x, y);};\
} /* end switch (inside_rule_num) */ 

#ifdef __cplusplus
}
#endif

#endif /* _WALLFART_COLOUR_H_ */
