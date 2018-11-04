/** 
 *         WallFArt := 'Wallpaper Fractal Art'
 *	   ^^^^^^^^     ^^^^      ^       ^^^
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
 ** This file is part of WallFArt.
 *
 *   My first attempt at creating a GtkWidget. 
 *   Critique: 
 *     - I didn't have a clear idea about what I was trying accomplish
 *       when I started (generally, a poor design approach)
 *     - my goal was to be 'pleasantly surprised' by a computer every
 *	  once in a while rather than being 'miffed' by one almost all the time
 *	  so, in that sense, I accomplished my task even though I didn't know
 *	  what it was when I started -ain't it grand.
 */
#include <stdlib.h>
#include <gtk/gtk.h>

#include "wallfart_complex.h"
#include "wallfart_types.h"
#include "wallfart_data.h"

#include "wallfart_image.h"
#include "wallfart_image_marshaller.h"

#define VERBOSITY 0 
#define TYPETEST  0

#define STEP			 0.000001
#define MIN_R			-3.5 
#define MAX_R			 1.0
#define MIN_SHW		 	-1.3
#define MAX_SHW			 1.3
#define MIN_WIDTH		48
#define MAX_WIDTH		1024
#define MIN_HEIGHT		48
#define MAX_HEIGHT		768
#define N_CHANNELS		3

G_DEFINE_TYPE (WallfartImage, wallfart_image, GTK_TYPE_EVENT_BOX);

static gboolean wallfart_image_expose_event	(GtkWidget *w, GdkEventExpose *e);
static gboolean wallfart_image_motion_notify	(GtkWidget *w, GdkEventMotion *e);
static gboolean wallfart_image_button_press	(GtkWidget *w, GdkEventButton *e);
static gboolean wallfart_image_button_release	(GtkWidget *w, GdkEventButton *e);

static void	wallfart_image_ud_start_r	(GtkAdjustment *a, gpointer d);
static void	wallfart_image_ud_start_i	(GtkAdjustment *a, gpointer d);
static void	wallfart_image_ud_z_r		(GtkAdjustment *a, gpointer d);
static void	wallfart_image_ud_z_i		(GtkAdjustment *a, gpointer d);
static void 	wallfart_image_sync		(WallfartImage *fi);


static void	emit_status_signal(WallfartImage *fi, gdouble r, gdouble i, gdouble s);
enum 		{ STATUS_SIGNAL, LAST_SIGNAL };
static guint 	wallfart_image_signals[LAST_SIGNAL] = { 0 };

static void wallfart_image_class_init(WallfartImageClass *fic)
{
	GtkWidgetClass *wc 	 = GTK_WIDGET_CLASS(fic);

	wc->expose_event  	 = wallfart_image_expose_event;
	wc->motion_notify_event  = wallfart_image_motion_notify;
	wc->button_press_event	 = wallfart_image_button_press;
	wc->button_release_event = wallfart_image_button_release;

	GObjectClass *oc	 = G_OBJECT_CLASS(fic);
	wallfart_image_signals[STATUS_SIGNAL] = g_signal_new(
		"status-signal", G_OBJECT_CLASS_TYPE(fic),
		G_SIGNAL_RUN_LAST|G_SIGNAL_ACTION,
		G_STRUCT_OFFSET(WallfartImageClass, status),
		NULL, NULL,
		fic_VOID__DOUBLE_DOUBLE_DOUBLE,
		G_TYPE_NONE, 3, 
		G_TYPE_DOUBLE, G_TYPE_DOUBLE, G_TYPE_DOUBLE);
}

static void wallfart_image_init(WallfartImage *fi)
{
	fi->f 		= NULL;
	fi->pixbuf 	= NULL;
	fi->rgb_data 	= NULL;
	fi->rowstride 	= -1;
	fi->n_channels 	= -1;
	fi->width_adj 	= NULL;
	fi->height_adj 	= NULL;
	fi->side_adj	= NULL;
}

/*
 * TODO should move this && stop accessing struct directly, ie, change access like 
 *	fi->f->blabla = foo, to fi = fi_set_blabla(foo)  && fee = fi_get_blabla(fi);
 * 
 */
static void wallfart_image_set_from_type
	(WallfartImage *fi, WallFArt_types t, gint w, gint h, gboolean randomly)
{
	fi->f = WallFArt_data_generate(t, w, h, NULL, randomly);
	if (!fi->f) {
		//g_print("AAAHH!!!!  Major faux-pos: Exit... \n");
		fprintf(stderr, "AAAHH!!!!  Major faux-pos: Exit... \n");
		gtk_main_quit();
	} else {
	/*** hmm, I don't seem to use these anymore */
		fi->side_adj = GTK_ADJUSTMENT(gtk_adjustment_new
			(fi->f->side,0.001,5.0,0.001, 0, 0));
		fi->width_adj = GTK_ADJUSTMENT(gtk_adjustment_new
			(fi->f->width, MIN_WIDTH, MAX_WIDTH, 1, 0, 0));
	/* who knows, I may revisit this stuff some day */

		fi->n_channels = fi->f->n_channels;
		fi->rowstride = fi->f->rowstride;
		fi->rgb_data = fi->f->rgb_data;
		fi->pixbuf = gdk_pixbuf_new_from_data(fi->rgb_data,
			     GDK_COLORSPACE_RGB, FALSE, 8,
			     fi->f->width, fi->f->height, fi->rowstride,
			     NULL, NULL);
	/* LEFT RIGHT (Z real) *************/
		fi->r_start_adj = GTK_ADJUSTMENT(gtk_adjustment_new
			(fi->f->startC.real, MIN_R, MAX_R, STEP, 0, 0));
		g_signal_connect(G_OBJECT(fi->r_start_adj), "value-changed", 
		 		 G_CALLBACK(wallfart_image_ud_start_r), (gpointer)fi);
	/* DOWN UP (Z imaginary) same values as L/R ********/ 
		fi->i_start_adj = GTK_ADJUSTMENT(gtk_adjustment_new
			(fi->f->startC.imag, MIN_R, MAX_R, STEP, 0, 0));
		g_signal_connect(G_OBJECT(fi->i_start_adj), "value-changed", 
				 G_CALLBACK(wallfart_image_ud_start_i), (gpointer)fi);

	/* .swoosh.. (C real) */
		fi->r_z_adj = GTK_ADJUSTMENT(gtk_adjustment_new
			(fi->f->Z.real, MIN_SHW, MAX_SHW, STEP, 0, 0));
		g_signal_connect(G_OBJECT(fi->r_z_adj), "value-changed", 
		 		 G_CALLBACK(wallfart_image_ud_z_r), (gpointer)fi);
	/* ..sweesh. (C imaginary)  */
		fi->i_z_adj = GTK_ADJUSTMENT(gtk_adjustment_new
			(fi->f->Z.imag, MIN_SHW, MAX_SHW, STEP, 0, 0));
		g_signal_connect(G_OBJECT(fi->i_z_adj), "value-changed", 
				 G_CALLBACK(wallfart_image_ud_z_i), (gpointer)fi);

 		gtk_widget_set_size_request
			(GTK_WIDGET(fi), fi->f->width, fi->f->height);
		gtk_widget_set_events(GTK_WIDGET(fi), GDK_POINTER_MOTION_MASK);
	}
}

GtkWidget *wallfart_image_new_from_type(WallFArt_types t, gint w, gint h)
{
	WallfartImage *fi;

	if (h < MIN_HEIGHT) h = MIN_HEIGHT;
	if (w < MIN_WIDTH) w  = MIN_WIDTH;
	fi = g_object_new(wallfart_image_get_type(), NULL);	
	wallfart_image_set_from_type(fi, t, w, h, FALSE);

	return GTK_WIDGET(fi);
}

GtkWidget *wallfart_image_new(gint w, gint h)
{
	WallfartImage *fi;

	if (w < MIN_WIDTH || h < MIN_HEIGHT)
		{ w = MIN_WIDTH; h = MIN_HEIGHT; }
	fi = g_object_new(wallfart_image_get_type(), NULL);	
	 wallfart_image_set_from_type(fi, RANDOMTYPE, w, h, FALSE); 

	return GTK_WIDGET(fi);
}

static void emit_status_signal(WallfartImage *fi, gdouble r, gdouble i, gdouble s)
{
	g_signal_emit(fi, wallfart_image_signals[STATUS_SIGNAL], 0, r, i, s);
}

static void wallfart_image_ud_start_r(GtkAdjustment *a, gpointer d)
{
	g_return_if_fail(a != NULL);
	g_return_if_fail(GTK_IS_ADJUSTMENT(a));

	WallfartImage *fi = WALLFART_IMAGE(d);
	g_return_if_fail(IS_WALLFART_IMAGE(fi));

	if (gtk_adjustment_get_value(a) != fi->f->startC.real)
	{
	 complex_init(fi->f->startC, gtk_adjustment_get_value(a), fi->f->startC.imag);
	 fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
			fi->f->height, fi->f->startC, fi->f->Z, 
			fi->f->modifier, fi->f->side, fi->f->car);
	 wallfart_image_touch(fi);
	}
}

static void wallfart_image_ud_start_i(GtkAdjustment *a, gpointer d)
{
	g_return_if_fail(a != NULL);
	g_return_if_fail(GTK_IS_ADJUSTMENT(a));
	WallfartImage *fi = WALLFART_IMAGE(d);
	g_return_if_fail(IS_WALLFART_IMAGE(fi));

	if (gtk_adjustment_get_value(a) != fi->f->startC.imag)
	{
	 complex_init(fi->f->startC, fi->f->startC.real, gtk_adjustment_get_value(a));
	 fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
			fi->f->height, fi->f->startC, fi->f->Z, 
			fi->f->modifier, fi->f->side, fi->f->car);
	 wallfart_image_touch(fi);
	}
}

static void wallfart_image_ud_z_r(GtkAdjustment *a, gpointer d)
{
	g_return_if_fail(a != NULL);
	g_return_if_fail(GTK_IS_ADJUSTMENT(a));
	WallfartImage *fi = WALLFART_IMAGE(d);
	g_return_if_fail(IS_WALLFART_IMAGE(fi));

	if (gtk_adjustment_get_value(a) != fi->f->Z.real)
	{
		complex_init(fi->f->Z, gtk_adjustment_get_value(a), fi->f->Z.imag);
		fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
			fi->f->height, fi->f->startC, fi->f->Z, 
			fi->f->modifier, fi->f->side, fi->f->car);
		wallfart_image_touch(fi);
	}
}

static void wallfart_image_ud_z_i(GtkAdjustment *a, gpointer d)
{
	g_return_if_fail(a != NULL);
	g_return_if_fail(GTK_IS_ADJUSTMENT(a));
	WallfartImage *fi = WALLFART_IMAGE(d);
	g_return_if_fail(IS_WALLFART_IMAGE(fi));


	if (gtk_adjustment_get_value(a) != fi->f->Z.imag)
	{
		complex_init(fi->f->Z, fi->f->Z.real, gtk_adjustment_get_value(a));
		fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
			fi->f->height, fi->f->startC, fi->f->Z, 
			fi->f->modifier, fi->f->side, fi->f->car);
		wallfart_image_touch(fi);
	}
}

void wallfart_image_touch(WallfartImage *fi)
{
	GtkWidget *w = GTK_WIDGET(fi); 
	if (!w->window) return;

	GdkRegion *region = gdk_drawable_get_clip_region(w->window);
	gdk_window_invalidate_region(w->window, region, TRUE);
	gdk_window_process_updates(w->window, TRUE);
	gdk_region_destroy(region);
}

/**
 *  
 */
static void wallfart_image_sync(WallfartImage *fi)
{
	g_return_if_fail(IS_WALLFART_IMAGE(fi));
	g_return_if_fail(fi != NULL);

#if VERBOSITY
	g_print(" IMAGESYNC: update... \n");
	g_print(" **** gap = %0.15f; side= %0.15f;  \n", fi->f->gap, fi->f->side);
#endif
   /* right/left */
	gtk_adjustment_set_value(fi->r_start_adj, fi->f->startC.real);
   /* up down */
	gtk_adjustment_set_value(fi->i_start_adj, fi->f->startC.imag);
   /* *** swoosh */
	gtk_adjustment_set_value(fi->r_z_adj, fi->f->Z.real);
   /* *** swishh */
	gtk_adjustment_set_value(fi->i_z_adj, fi->f->Z.imag);

   /* change adjustment ranges according to zoom */
	if (fi->f->side < 1.0) {
		gtk_adjustment_set_lower(fi->r_start_adj,
				(fi->f->startC.real - fi->f->side));
		gtk_adjustment_set_upper(fi->r_start_adj, 
				(fi->f->startC.real + fi->f->side));
		gtk_adjustment_set_lower(fi->i_start_adj, 
				(fi->f->startC.imag - fi->f->side)); 
		gtk_adjustment_set_upper(fi->i_start_adj, 
				(fi->f->startC.imag + fi->f->side));

#ifdef DOITFORSWOSHSWESH
		gtk_adjustment_set_lower(fi->r_z_adj, 
				fi->f->Z.real - (fi->f->side * 2.0));
					/* don't remember why * 2.0 */
		gtk_adjustment_set_upper(fi->r_z_adj, 
				fi->f->Z.real + (fi->f->side * 2.0));
		gtk_adjustment_set_lower(fi->i_z_adj, 
				fi->f->Z.imag - (fi->f->side * 2.0));
		gtk_adjustment_set_upper(fi->i_z_adj, 
				fi->f->Z.imag + (fi->f->side * 2.0));
#endif
	} else { 
		gtk_adjustment_set_lower(fi->r_start_adj, MIN_R);
		gtk_adjustment_set_upper(fi->r_start_adj, MAX_R);
		gtk_adjustment_set_lower(fi->i_start_adj, MIN_R);
		gtk_adjustment_set_upper(fi->i_start_adj, MAX_R);

#ifdef DOITFORSWOSHSWESH
		gtk_adjustment_set_lower(fi->r_z_adj, MIN_SHW);
		gtk_adjustment_set_upper(fi->r_z_adj, MAX_SHW);
		gtk_adjustment_set_lower(fi->i_z_adj, MIN_SHW);
		gtk_adjustment_set_upper(fi->i_z_adj, MAX_SHW);
#endif
	}
	fi->rowstride = fi->f->rowstride;
	fi->rgb_data  = fi->f->rgb_data;
}

static gboolean wallfart_image_expose_event(GtkWidget *w, GdkEventExpose *e)
{
	g_return_val_if_fail(w != NULL, FALSE);
	g_return_val_if_fail(IS_WALLFART_IMAGE(w), FALSE);
	g_return_val_if_fail(e != NULL, FALSE);

	gdk_draw_rgb_image(w->window, w->style->black_gc, 0, 0,
			   WALLFART_IMAGE(w)->f->width,
			   WALLFART_IMAGE(w)->f->height,
			   GDK_RGB_DITHER_NONE,
			   WALLFART_IMAGE(w)->rgb_data,
			   WALLFART_IMAGE(w)->rowstride);
	return TRUE;
}

static gboolean wallfart_image_motion_notify(GtkWidget *w, GdkEventMotion *e)
{
	g_return_val_if_fail(w != NULL, FALSE);	
	WallfartImage *fi = WALLFART_IMAGE(w);
	g_return_val_if_fail(IS_WALLFART_IMAGE(fi), FALSE);

	if ((e->x < fi->f->width) && (e->y < fi->f->height)) {
		gdouble p_real_p = e->x * fi->f->gap + fi->f->startC.real;
		gdouble p_imag_p = e->y * fi->f->gap + fi->f->startC.imag;
		emit_status_signal(fi, p_real_p, p_imag_p, fi->f->side);
	}
	return TRUE;
}

static void busy_cursor(WallfartImage *w)
{
	GdkCursor *c = gdk_cursor_new(GDK_CLOCK);
	GdkWindow *cWin = (gtk_widget_get_toplevel(GTK_WIDGET(w)))->window;
	gdk_window_set_cursor(cWin, c);
	gdk_flush();
	gdk_cursor_unref(c);
}

static void normal_cursor(WallfartImage *w)
{
	GdkCursor *c = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	GdkWindow *cursorWin = (gtk_widget_get_toplevel(GTK_WIDGET(w)))->window;
	gdk_window_set_cursor(cursorWin, c);
	gdk_flush();
	gdk_cursor_unref(c);
}

#define BUTTON1		1 /* left   */
#define BUTTON2		3 /* right  */
#define BUTTON3		2 /* middle */
#define CENTRE_BUTTON	2
#define NOKEY		0
#define SHIFT		1 /* going to add some functionality here    */
#define CAPSLOCK	2 /* just for my own amusment (MWHW:03_2018) */
#define CTRL		4
#define CTRLSHIFT	5
#define CTRL_ALT	12
#define ZOOM_IN 	1.5
#define ZOOM_OUT 	0.8
/**
 *   I really wanted to clean this up but.. I'm old and feeble minded..
 */
static gboolean wallfart_image_button_press(GtkWidget *w, GdkEventButton *e)
{
    extern void psycoShuffle(WallfartImage *fi);
    g_return_val_if_fail(w != NULL, FALSE);
    g_return_val_if_fail(IS_WALLFART_IMAGE(w), FALSE);
    g_return_val_if_fail(e != NULL, FALSE);
    WallfartImage *fi = WALLFART_IMAGE(w);

    /* check if mouse is some where over display area */
    if ((e->x < fi->f->width) && (e->y < fi->f->height)) {
	gdouble middle_x = (gdouble)fi->f->width/2.0;
	gdouble middle_y = (gdouble)fi->f->height/2.0;
	gdouble p_real_p = e->x * fi->f->gap + fi->f->startC.real;
	gdouble p_imag_p = e->y * fi->f->gap + fi->f->startC.imag;

	busy_cursor(fi);

	/* LEFT MOUSE BUTTON (e->button == 1) : 0 == mouse only; 1 == mouse + shift;
	 *	2 == mouse + capsLock; 4 == mouse + Ctrl; 12 == Ctrl-Alt-Mouse
	 */
	if (e->button == BUTTON1) { 
		if (e->state == NOKEY) {
			gdouble new_side = fi->f->side/(gdouble)ZOOM_IN;
			gdouble new_gap  = new_side/(gdouble)fi->f->width;
			complex_init(fi->f->startC, p_real_p - new_gap * middle_x,
					p_imag_p - new_gap * middle_y);
			fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
				fi->f->height, fi->f->startC, fi->f->Z, 
				fi->f->modifier, new_side, fi->f->car);
		} else if (e->state == SHIFT) {
			gdouble new_side = fi->f->side/(gdouble)(ZOOM_IN*2);
			gdouble new_gap  = new_side/(gdouble)fi->f->width;
			complex_init(fi->f->startC, p_real_p - new_gap * middle_x,
					p_imag_p - new_gap * middle_y);
			fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
				fi->f->height, fi->f->startC, fi->f->Z, 
				fi->f->modifier, new_side, fi->f->car);
		} else if (e->state == CTRL_ALT) {

//psycoShuffle(fi);
WallFArt_dump(fi->f);
//			psycoShuffle(fi);
		} else {  /* any other e->state */
			; /* do nothing */
		}
	} else if (e->button == BUTTON2) {
		if (e->state == NOKEY) {
			gdouble new_side = fi->f->side/(gdouble)(ZOOM_IN*0.5);
			gdouble new_gap  = new_side/(gdouble)fi->f->width;
			complex_init(fi->f->startC, p_real_p - new_gap * middle_x,
					p_imag_p - new_gap * middle_y);
			fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
				fi->f->height, fi->f->startC, fi->f->Z, 
				fi->f->modifier, new_side, fi->f->car);
		} else if (e->state == SHIFT) {
			gdouble new_side = fi->f->side/(gdouble)(((ZOOM_IN*0.5)/2.0));
			gdouble new_gap  = new_side/(gdouble)fi->f->width;
			complex_init(fi->f->startC, p_real_p - new_gap * middle_x,
					p_imag_p - new_gap * middle_y);
			fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
				fi->f->height, fi->f->startC, fi->f->Z, 
				fi->f->modifier, new_side, fi->f->car);
		} else {
		    ;	//g_print("-right mouse and something other than shift \n");
		}
		
	} else if (e->button == CENTRE_BUTTON) { 
		if (e->state == CTRL_ALT) {
	 	      ;	  //g_print("  center button %d pressed in state %d\n", e->button, e->state);
		}
	}
	emit_status_signal(fi, fi->f->startC.real, fi->f->startC.imag, fi->f->side);
	wallfart_image_sync(fi);
	wallfart_image_touch(fi);
	normal_cursor(fi);
    }
   return TRUE;
}

/**
 */
static gboolean wallfart_image_button_release(GtkWidget *w, GdkEventButton *e)
{
	g_return_val_if_fail(w != NULL, FALSE);
	g_return_val_if_fail(e != NULL, FALSE);
	WallfartImage *fi = WALLFART_IMAGE(w);
	g_return_val_if_fail(IS_WALLFART_IMAGE(fi), FALSE);

	emit_status_signal(fi, fi->f->startC.real, fi->f->startC.imag, fi->f->side);
	return TRUE;
}

void wallfart_image_random_colour(WallfartImage *fi)
{
	g_return_if_fail(fi != NULL);
	g_return_if_fail(IS_WALLFART_IMAGE(fi));

	fi->f = WallFArt_repaint(fi->f);
	wallfart_image_touch(fi);
}

void wallfart_image_random_colour_inside(WallfartImage *fi)
{
	g_return_if_fail(fi != NULL);
	g_return_if_fail(IS_WALLFART_IMAGE(fi));
	fi->f = WallFArt_repaint_inside(fi->f);
	wallfart_image_touch(fi);
}

void wallfart_image_set_size(WallfartImage *fi, gint w, gint h)
{
	g_return_if_fail(fi != NULL);
	g_return_if_fail(IS_WALLFART_IMAGE(fi));
	if (w < MIN_WIDTH) w  = MIN_WIDTH;
	if (h < MIN_HEIGHT) h = MIN_HEIGHT;

 	fi->f = WallFArt_resize(fi->f, w, h);
	wallfart_image_sync(fi);
	wallfart_image_touch(fi);
}

void size_scale_cb(GtkAdjustment *adj, gpointer d)
{
	gint width, height;
	width  = (gint)gtk_adjustment_get_value(adj);
	//height = width * 0.75;
	height = width * 0.8;

	wallfart_image_set_size(WALLFART_IMAGE(d), width, height);
}

/**
 * zoom
 */
GtkWidget *wallfart_image_get_size_scale(WallfartImage *fi, GtkOrientation orientation)
{
	g_return_val_if_fail(fi != NULL, NULL);
	g_return_val_if_fail(IS_WALLFART_IMAGE(fi), NULL);

	g_signal_connect(G_OBJECT(fi->width_adj), "value-changed", 
			 G_CALLBACK(size_scale_cb), (gpointer)fi);

	GtkWidget *scale, *box;
	if (orientation == GTK_ORIENTATION_VERTICAL) {
		scale = gtk_vscale_new(GTK_ADJUSTMENT(fi->width_adj));
		box   = gtk_vbox_new(FALSE, 1);
	} else {
		scale = gtk_hscale_new(GTK_ADJUSTMENT(fi->width_adj));
		box   = gtk_hbox_new(FALSE, 1);
	}

	gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);
	gtk_scale_set_digits(GTK_SCALE(scale), 16);
	/* gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_DISCONTINUOUS); */
	gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_DELAYED); 

	GtkWidget *label = gtk_label_new("bla bla ");
	gtk_box_pack_start(GTK_BOX(box), scale, TRUE, TRUE, 1);
	gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 1);
	gtk_widget_show(scale);
	gtk_widget_show(label);
	gtk_widget_show(box);
	//gtk_widget_set_size_request(label, 40, 30);
	//gtk_widget_set_size_request(label, LABEL_SIZE);

	return box;
}

void side_scale_cb(GtkAdjustment *adj, gpointer d)
{
	WallfartImage *fi = WALLFART_IMAGE(d);
	gdouble new_side = gtk_adjustment_get_value(adj);

	if (fi->f->side != new_side) {
		fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
			fi->f->height, fi->f->startC, fi->f->Z, 
			fi->f->modifier, new_side, fi->f->car);
		wallfart_image_sync(fi);
		wallfart_image_touch(fi);
	}
}

/* ? */
GtkWidget *wallfart_image_get_side_scale(WallfartImage *fi, GtkOrientation orientation)
{
	g_return_val_if_fail(fi != NULL, NULL);
	g_return_val_if_fail(IS_WALLFART_IMAGE(fi), NULL);

	g_signal_connect(G_OBJECT(fi->side_adj), "value-changed", 
			 G_CALLBACK(side_scale_cb), (gpointer)fi);
	GtkWidget *scale, *box;
	if (orientation == GTK_ORIENTATION_VERTICAL) {
		scale = gtk_vscale_new(GTK_ADJUSTMENT(fi->side_adj));
		box   = gtk_vbox_new(FALSE, 0);
	} else {
		scale = gtk_hscale_new(GTK_ADJUSTMENT(fi->side_adj));
		box   = gtk_hbox_new(FALSE, 0);
	}
	gtk_box_pack_start(GTK_BOX(box), scale, TRUE, TRUE, 1);
	gtk_widget_show(scale);

	return box;
}


void wallfart_image_reset_image(WallfartImage *fi)
{
	g_return_if_fail(fi != NULL);
	g_return_if_fail(IS_WALLFART_IMAGE(fi));

	WallFArt_complex start, z;
	gdouble side;

	WallFArt_data_get_vars(fi->f->type, &start, &z, &side, FALSE);
	busy_cursor(fi);
	fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
		fi->f->height, start, z, 
		fi->f->modifier, side, fi->f->car);
	normal_cursor(fi);
	wallfart_image_sync(fi);
	wallfart_image_touch(fi);
}

void wallfart_image_super_shuffle(WallfartImage *fi)
{
	g_return_if_fail(fi != NULL);
	g_return_if_fail(IS_WALLFART_IMAGE(fi));

	WallFArt_complex start, z;
	gdouble side;

	fi->f->type = PICK_A_RANDOM_TYPE(); 
	WallFArt_data_get_vars(fi->f->type, &start, &z, &side, FALSE);
	busy_cursor(fi);
	fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
		fi->f->height, start, z, 
		fi->f->modifier, side, fi->f->car);
	normal_cursor(fi);

	wallfart_image_random_colour_inside(fi);
	wallfart_image_random_colour(fi);
	wallfart_image_sync(fi);
	wallfart_image_touch(fi);
}

void wallfart_image_shuffle(WallfartImage *fi)
{
	g_return_if_fail(fi != NULL);
	g_return_if_fail(IS_WALLFART_IMAGE(fi));

	WallFArt_complex start, z;
	gdouble side;
	static int full_count = 0;
	int totally_random;

	fi->f->type = PICK_A_RANDOM_TYPE(); 

	if (fi->f->type == MANDELBROT) {
	/* as an homage to B. Mandelbrot, always realize a complete 
	    Mandelbrot set whenever it comes up as an option: rip
	      'blessed are they that mourn'... really?  */
		totally_random = FALSE;
	} else {
	/* I can never get things to feel random enough... frustrating */
		int next_divisor = ((int)((10.0) * (float)rand()/(float)RAND_MAX)+1);
		totally_random = ++full_count % (RANDOMTYPE/next_divisor);
	}
#if 0 
/* to test SHUFFling && types */
fi->f->type = TEST;
totally_random = 0;
#endif
	if (totally_random) {
//g_print(" ============= TOTALLY RANDOM ==\n");
		WallFArt_data_get_vars(fi->f->type, &start, &z, &side, TRUE);
	} else {
//g_print(" ************* NOT RANDOM == ***********************\n");
		WallFArt_data_get_vars(fi->f->type, &start, &z, &side, FALSE);
	}

	busy_cursor(fi);
	fi->f = WallFArt_new(fi->f, fi->f->type, fi->f->width,
		fi->f->height, start, z, 
		fi->f->modifier, side, fi->f->car);
	normal_cursor(fi);

	wallfart_image_sync(fi);
	wallfart_image_touch(fi);
}

/* ...what to do? what to do?? */
void psycoShuffle(WallfartImage *fi)
{
	
	for (int x=0; x<32; x++) {
		wallfart_image_random_colour(fi);
		wallfart_image_random_colour_inside(fi);
	}
	for (int x=0; x<16; x++) {
		wallfart_image_random_colour(fi);
	}
	for (int x=0; x<8; x++) {
		wallfart_image_random_colour_inside(fi);
	}
	//WALLFART_image_shuffle(fi);
	for (int x=0; x<8; x++) {
		wallfart_image_random_colour_inside(fi);
	}
	for (int x=0; x<16; x++) {
		wallfart_image_random_colour(fi);
	}
	for (int x=0; x<32; x++) {
		wallfart_image_random_colour(fi);
		wallfart_image_random_colour_inside(fi);
	}
	// FIXME sleep(1);
		wallfart_image_random_colour(fi);
	// FIXME sleep(1);
		wallfart_image_random_colour_inside(fi);
}
