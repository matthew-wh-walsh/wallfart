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
 */

/**
 *  This is WallFArt: A program that generates wallfarts. 
 *			(pseudo-randomly)
 *			  ((of-course))
 *
 *  "What is a 'wallfart'?", you ask:
 *
 *   1)	wallfart:  noun, a computer generated, Mandelbrot set
 *	  inspired, computer graphic suitable for application
 *	  as the background wallpaper in any computing desktop
 *	  environment.
 *
 *   2) wallfart:  not a verb; that would be disgusting.
 *
 */

#if defined(WIN32)
#include <windows.h>
BOOL WINAPI FreeConsole();
// for some reason, windows starts up a console whenever I don't want one
// Very frustrating. Using FreeConsole(), I attempt get rid of it 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "wallfart_image.h"
#include "wallfart_help_content.h"

/************************** compile time adjustments ******/
/**
 * BIGGER_BY value of 172 gives the program a 600 pixel height (Xorg)
 *   
 */
#if defined(WIN32)
/* a good size for my slow Win7 computer (no logic, an arbitrary choice) */
#define BIGGER_BY 400
//#define BIGGER_BY 234
#else
//#define BIGGER_BY 172 /* 606x600 */
//#define BIGGER_BY 240 /* 674x651 */
#define BIGGER_BY 340 /* 774x726 */
#endif
/**
 * setting ASPECT RATIO to 0 allows the program to calculate
 *  the aspect ratio at start up (using the default screen)
 */
#define ASPECT_RATIO 0  
//#define ASPECT_RATIO 0.75   /* force 4x3 aspect */
//#define ASPECT_RATIO 0.5625 /* force 16x9      */

#ifdef G_PLATFORM_WIN32
#define PROGRAM_NAME	(const gchar *)"WallFArt for Windows"
#else
#define PROGRAM_NAME	(const gchar *)"WallFArt"
#endif
/************************** end of compile time adjustments ******/
/****************************************************************************
 *  The WallFArt interface is arranged:  TOOLBAR
 *					 DISPLAY
 *					 STATUSBAR
 *					 CONTROLS
 *
 *	and this is the GTK+(v2.x.x) implementation thereof.
 *
 ****************************************************************************/
/**************************************************  BEGIN TOOLBAR SECTION **/
void oColour_cb(GtkWidget *b, gpointer fimage);
#define OUTSIDE_COLOUR_TIP_TEXT		"Change ''outside'' colour scheme."
#define OCOLOUR_TOOLBUTTON_LABEL	"Col_our1"
//
void iColour_cb(GtkWidget *b, gpointer fimage);
#define INSIDE_COLOUR_TIP_TEXT		"Change ''inside'' colour scheme."
#define INCOLOUR_TOOLBUTTON_LABEL	"Colo_ur2"
//
void shuffle_cb(GtkWidget *b, gpointer fimage);
//#define SHUFFLE_TOOLTIP_TEXT 		"Pseudo-randomly generate a new data set."
#define SHUFFLE_TOOLTIP_TEXT 		"Pseudo-randomly generate a new WDC (Wallfart Data Core ;)" 
#define SHUFFLE_TOOLBUTTON_LABEL	"S_huffle" 
//
void wallpaper_cb(GtkWidget *b, gpointer fimage);
#define WALLPAPER_TIP_TEXT		"Apply wallfart as desktop wallpaper (without saving)."
#define WALLPAPER_TOOLBUTTON_LABEL	"_Wallpaper"
//
void save_cb(GtkWidget *b, gpointer fimage);
#define SAVE_TOOL_TIP_TEXT		"Save the wallfart to disk."
#define SAVE_TOOLBUTTON_LABEL		"_Save"
//
void about_cb(GtkWidget *b, gpointer fimage);
#define ABOUT_TOOL_TIP_TEXT		"Help! What's this all about..."
#define ABOUT_TOOLBUTTON_LABEL		"_About"

#ifndef ABOUT_DIALOG_TITLE
#define ABOUT_DIALOG_TITLE ABOUT_TOOL_TIP_TEXT
#endif

#define TOOLITEM_SIZE 3
GtkWidget *create_WallFArt_toolbar(WallfartImage *fi)
{
	GtkWidget *toolbar = gtk_toolbar_new();
	GtkToolItem *ti;
	gint toolnumber = 0;
//
// outside colour button:
	ti = gtk_tool_button_new_from_stock(GTK_STOCK_SELECT_COLOR);
	gtk_tool_item_set_tooltip_text(ti, OUTSIDE_COLOUR_TIP_TEXT);
	gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(ti), TRUE);
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(ti), OCOLOUR_TOOLBUTTON_LABEL);
	g_signal_connect(G_OBJECT(ti), "clicked", G_CALLBACK(oColour_cb), (gpointer)fi);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti, toolnumber++);
	gtk_widget_show(GTK_WIDGET(ti));
//
// inside colour button:
	ti = gtk_tool_button_new_from_stock(GTK_STOCK_COLOR_PICKER);
	gtk_tool_item_set_tooltip_text(ti, INSIDE_COLOUR_TIP_TEXT);
	gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(ti), TRUE);
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(ti), INCOLOUR_TOOLBUTTON_LABEL);
	g_signal_connect(G_OBJECT(ti), "clicked", 
			 G_CALLBACK(iColour_cb), (gpointer)fi);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti, toolnumber++);
	gtk_widget_show(GTK_WIDGET(ti));
//
// shuffle button
	ti = gtk_tool_button_new_from_stock(GTK_STOCK_EXECUTE);
	gtk_tool_item_set_tooltip_text(ti, SHUFFLE_TOOLTIP_TEXT);
	gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(ti), TRUE);
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(ti), SHUFFLE_TOOLBUTTON_LABEL);
	g_signal_connect(G_OBJECT(ti), "clicked", G_CALLBACK(shuffle_cb), (gpointer)fi);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti, toolnumber++);
	gtk_widget_show(GTK_WIDGET(ti));
//
// apply as wallpaper
	ti = gtk_tool_button_new_from_stock(GTK_STOCK_FULLSCREEN);
	gtk_tool_item_set_tooltip_text(ti, WALLPAPER_TIP_TEXT);
	gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(ti), TRUE);
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(ti), WALLPAPER_TOOLBUTTON_LABEL);
	g_signal_connect(G_OBJECT(ti), "clicked", 
			 G_CALLBACK(wallpaper_cb), (gpointer)fi);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti, toolnumber++);
	gtk_widget_show(GTK_WIDGET(ti));
//
// save
	ti = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	gtk_tool_item_set_tooltip_text(ti, SAVE_TOOL_TIP_TEXT);
	gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(ti), TRUE);
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(ti), SAVE_TOOLBUTTON_LABEL);
	g_signal_connect(G_OBJECT(ti), "clicked", G_CALLBACK(save_cb), (gpointer)fi);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti, toolnumber++);
	gtk_widget_show(GTK_WIDGET(ti));
//
// help/about ...	
	ti = gtk_tool_button_new_from_stock(GTK_STOCK_ABOUT);
	gtk_tool_item_set_tooltip_text(ti, ABOUT_TOOL_TIP_TEXT);
	gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(ti), TRUE);
	gtk_tool_button_set_label(GTK_TOOL_BUTTON(ti), ABOUT_TOOLBUTTON_LABEL);
	g_signal_connect(G_OBJECT(ti), "clicked", G_CALLBACK(about_cb), (gpointer)fi);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), ti, toolnumber++);
	gtk_widget_show(GTK_WIDGET(ti));

	gtk_toolbar_set_icon_size(GTK_TOOLBAR(toolbar), (GtkIconSize)TOOLITEM_SIZE);
	// style: 1 == icons; 2 == icons+txt; 3 == txt
	GtkToolbarStyle sty = (GtkToolbarStyle)2;
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), sty);
	gtk_widget_show_all(GTK_WIDGET(toolbar));

	return toolbar;
}

/**
 * the toolbar callbacks 
 */
void shuffle_cb(GtkWidget *b, gpointer fimage)
{
	WallfartImage *fi = WALLFART_IMAGE(fimage);
	wallfart_image_shuffle(fi);
}

void oColour_cb(GtkWidget *b, gpointer fimage)
{
	WallfartImage *fi = WALLFART_IMAGE(fimage);
	wallfart_image_random_colour(fi);
}

void iColour_cb(GtkWidget *b, gpointer fimage)
{
	WallfartImage *fi = WALLFART_IMAGE(fimage);
	wallfart_image_random_colour_inside(fi);
}

#include "wallfart_wallpaper.h"
void wallpaper_cb(GtkWidget *b, gpointer fimage)
{
	GtkWidget *topLevelWidget = gtk_widget_get_toplevel(b);
	wallfart_wallpaper_dialog
		(GTK_WINDOW(topLevelWidget), WALLFART_IMAGE(fimage));
}

/***************************************************************************/

#define FILE_SAVE_DIALOG_TITLE	" Save to disk... "
#define PNG_SUFFIX 		".png"
static void save_file_to_disk(WallfartImage *fi, gchar *filename)
{
    gchar *save_name 	= NULL;
    GError *err 	= NULL;
   
    if (!g_str_has_suffix(filename, PNG_SUFFIX)) {
	    save_name = g_strconcat(filename, PNG_SUFFIX, NULL);
    } else {
	    save_name = g_strdup(filename); 
    }
    fi->pixbuf = gdk_pixbuf_new_from_data((const guchar *)fi->rgb_data,
		    GDK_COLORSPACE_RGB, FALSE, 8, 
		    fi->f->width, fi->f->height, fi->f->rowstride,
		    NULL, NULL);
/* ??? JPEG/PNG/BMP ??? */
    gdk_pixbuf_save(fi->pixbuf, (const char *)save_name, "png", &err, NULL);
//  gdk_pixbuf_save(fi->pixbuf, (const char *)save_name, "bmp", &err, NULL);
//  gdk_pixbuf_save(fi->pixbuf, (const char *)save_name, "jpeg", &err, "quality", "100", NULL);
    g_object_unref(fi->pixbuf);
    g_free(save_name); 
}

void save_cb(GtkWidget *b, gpointer fimage)
{
	WallfartImage *fi = WALLFART_IMAGE(fimage);
	GtkWidget *dlg = gtk_file_chooser_dialog_new(FILE_SAVE_DIALOG_TITLE,
		GTK_WINDOW(gtk_widget_get_toplevel(b)),
		GTK_FILE_CHOOSER_ACTION_SAVE, 
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE, GTK_RESPONSE_OK, NULL);

	gtk_dialog_set_default_response(GTK_DIALOG(dlg), GTK_RESPONSE_OK);
	gtk_file_chooser_set_do_overwrite_confirmation
		(GTK_FILE_CHOOSER(dlg), TRUE);

	if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_OK) {
		gchar *fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
		save_file_to_disk(fi, fname);
		g_free(fname);
	}
	gtk_widget_destroy(dlg);
}

/* NOTEtoSelf This is the only reference to config.h   */
//#include <config.h>
#if defined(PACKAGE_STRING)
#define HELP_C2_TITLE PACKAGE_STRING
#else
#define HELP_C2_TITLE "WallFArt v0.3.7: GPL 3 Free"
#endif

#define ENOUGH_DIALOG_OK_BUTTON "All right then, back to the program please..."
void enough_dialog_clicked(GtkWidget *topLevel, gpointer fimage)
{
	GtkWidget *hc2Dialog = gtk_dialog_new_with_buttons(HELP_C2_TITLE, 
						GTK_WINDOW(topLevel),
					GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
					ENOUGH_DIALOG_OK_BUTTON, GTK_RESPONSE_OK, NULL);	

	GtkWidget *content = help_content_table_2(topLevel);
	gtk_widget_show(content);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(hc2Dialog)->vbox), content, TRUE, TRUE, 0);
	gtk_window_set_resizable(GTK_WINDOW(GTK_DIALOG(hc2Dialog)), FALSE);

	gint response = gtk_dialog_run(GTK_DIALOG(hc2Dialog));

	switch (response) {
		case GTK_RESPONSE_OK:
		default:
			break;
	}
	gtk_widget_destroy(hc2Dialog);
}

// #include "wallfart_help_content.h" /* for convert xpm to gtkImage */
#define THE_HELP_BUTTON "Enough of this haiku madness! Help me..."
#define THE_OK_BUTTON	"Help!?!? I don' need no stinking help..."
void about_cb(GtkWidget *b, gpointer fimage)
{
	GError *e = NULL;
	GtkWidget *topLevelWidget = gtk_widget_get_toplevel(b);

	GtkWidget *dialog = gtk_dialog_new_with_buttons(ABOUT_DIALOG_TITLE, 
		GTK_WINDOW(topLevelWidget),
		GTK_DIALOG_MODAL|GTK_DIALOG_DESTROY_WITH_PARENT,
		THE_HELP_BUTTON, GTK_RESPONSE_HELP,
		THE_OK_BUTTON, GTK_RESPONSE_OK,
		NULL);

	GtkWidget *content = help_content_table_1(WALLFART_IMAGE(fimage));
	gtk_widget_show(content);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), content, TRUE, TRUE, 0);
	gtk_window_set_resizable(GTK_WINDOW(GTK_DIALOG(dialog)), FALSE);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), (gint)GTK_RESPONSE_OK);	
	gint response = gtk_dialog_run(GTK_DIALOG(dialog));
	switch(response) {
		case GTK_RESPONSE_HELP:
			enough_dialog_clicked(topLevelWidget, (gpointer)fimage);
			break;
		case GTK_RESPONSE_OK:
		default:
			break;
	}
	gtk_widget_destroy(dialog);
}

/**************************************************** END TOOLBAR SECTION ***/
/****************************************************************************/

/**************************************************** BEGIN DISPLAY AREA ****/

#define wfFabs(f) f < 0.0 ? f * -1.0 : f
void stat_cb(GtkWidget *wfimage, gdouble r, gdouble i, gdouble s, gpointer sbar)
{
	GtkStatusbar *sb = GTK_STATUSBAR(sbar);
	WallfartImage *wfi = WALLFART_IMAGE(wfimage);

	gchar *A = g_strdup_printf("{(%0.15f ", r);
	gchar *B = (i < 0.0) ? "-" : "+";
	gchar *C = g_strdup_printf(" %0.15f", wfFabs(i));
	gchar *D = " i)}";
	/* TODO CLEAN ME PLEASE */
#if defined(WIN32) 
	gchar *E = g_strdup_printf("  ::  {%0.16f (gap)}", wfi->f->gap);
#else
	gchar *E = g_strdup_printf("      ::      {%0.16f (gap)}", wfi->f->gap);
#endif
	gchar *msg = g_strconcat(A,B,C,D,E, NULL); 
	gtk_statusbar_pop(sb, 0);
	gtk_statusbar_push(sb, 0, msg);

	g_free(A);
	g_free(C);
	g_free(E);
}

#define STATUS_TOOLTIP_TEXT "Mouse positon and the \"gap\"\n\t(distance between points in the set)"
GtkWidget *create_WallFArt_status_bar(WallfartImage *fi)
{
	GtkWidget *status_bar = gtk_statusbar_new();
	gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(status_bar), FALSE);

	g_object_set(status_bar, "has-tooltip", TRUE, NULL);
	gtk_widget_set_tooltip_text(status_bar, STATUS_TOOLTIP_TEXT);
	g_signal_connect(G_OBJECT(fi), "status-signal", 
		G_CALLBACK(stat_cb), (gpointer)status_bar);
	return status_bar;
}

#define IBOX_TOOLTIP_TEXT "Left click: Zoom in a little;\n\tRight click: Zoom out a little less."
GtkWidget *create_WallFArt_image_box(WallfartImage *fi)
{
	GtkWidget *vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(fi), TRUE, TRUE, 0);

	g_object_set(fi, "has-tooltip", TRUE, NULL);
	gtk_widget_set_tooltip_text(GTK_WIDGET(fi), IBOX_TOOLTIP_TEXT); 

	GtkWidget *WallFArt_status = create_WallFArt_status_bar(WALLFART_IMAGE(fi));
	gtk_box_pack_start(GTK_BOX(vbox), WallFArt_status, TRUE, TRUE, 0);
	gtk_widget_show(WallFArt_status);

	gtk_widget_show(GTK_WIDGET(vbox));
	return vbox;
}

/****************************************************** END DISPLAY AREA ****/
/****************************************************************************/

/**************************************************** BEGIN CONTROL AREA ****/

#define RL_TOOLTIP_TEXT 		"Right/Left"
#define UD_TOOLTIP_TEXT			"Down/Up"
#define SW1_TOOLTIP_TEXT		".swoosh.."
#define SW2_TOOLTIP_TEXT		"..sweesh." 
GtkWidget *create_WallFArt_controls(WallfartImage *fi)
{
	GtkWidget *vbox  = gtk_vbox_new(TRUE,0);
		GtkWidget *hbox1  = gtk_hbox_new(TRUE, 0);
			GtkWidget *scale = gtk_hscale_new(fi->r_start_adj);
			gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);
			gtk_scale_set_digits(GTK_SCALE(scale), 12);
			gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
					// DELAYED || CONTINUOUS || DISCONTINUOUS 
			gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_DELAYED); 
     /* on a multi-core ++ GPU machine GTK_UPDATE_CONTINUOUS would work well. */
     /* but, I don't have a multi-core ++ GPU machine */
			gtk_widget_set_tooltip_text(scale, RL_TOOLTIP_TEXT); 
			gtk_box_pack_start(GTK_BOX(hbox1), scale, TRUE, TRUE, 2);
			gtk_widget_show(scale);

		GtkWidget *hbox2  = gtk_hbox_new(TRUE, 0);
			scale = gtk_hscale_new(fi->i_start_adj);
			gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);
			gtk_scale_set_digits(GTK_SCALE(scale), 12);
			gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
			gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_DELAYED); 
			gtk_widget_set_tooltip_text(scale, UD_TOOLTIP_TEXT); 
			gtk_box_pack_start(GTK_BOX(hbox2), scale, TRUE, TRUE, 2);
			gtk_widget_show(scale);

		GtkWidget *hbox3 = gtk_hbox_new(TRUE, 0);
			scale = gtk_hscale_new(fi->r_z_adj);
			gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);
			gtk_scale_set_digits(GTK_SCALE(scale), 12);
			gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
			gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_DELAYED); 
			gtk_widget_set_tooltip_text(scale, SW1_TOOLTIP_TEXT); 
			gtk_box_pack_start(GTK_BOX(hbox3), scale, TRUE, TRUE, 2);
			gtk_widget_show(scale);

		GtkWidget *hbox4 = gtk_hbox_new(TRUE, 0);
			scale = gtk_hscale_new(fi->i_z_adj);
			gtk_scale_set_draw_value(GTK_SCALE(scale), TRUE);
			gtk_scale_set_digits(GTK_SCALE(scale), 12);
			gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
			gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_DELAYED); 
			gtk_widget_set_tooltip_text(scale, SW2_TOOLTIP_TEXT); 
			gtk_box_pack_start(GTK_BOX(hbox4), scale, TRUE, TRUE, 2);
			gtk_widget_show(scale);

			gtk_box_pack_start(GTK_BOX(vbox), hbox1, FALSE, FALSE, 0);
		      gtk_box_pack_start(GTK_BOX(vbox), hbox2, FALSE, FALSE, 0);
		    gtk_box_pack_start(GTK_BOX(vbox), hbox3, FALSE, FALSE, 0);
	   	  gtk_box_pack_start(GTK_BOX(vbox), hbox4, FALSE, FALSE, 0);

		gtk_widget_show(hbox4);
	      gtk_widget_show(hbox3);
	    gtk_widget_show(hbox2);
	  gtk_widget_show(hbox1);

	return vbox;
}
/******************************************************** END CONTROL AREA ***/

/**
 * return the screen aspect ratio and determine screen wid1th and height
 *  unless ASPECT_RATIO has been specified already then just get width/height
 */
gdouble get_screen_size_and_aspect(gint *w, gint *h)
{
	gint screen_width  = gdk_screen_get_width(gdk_screen_get_default());
	gint screen_height = gdk_screen_get_height(gdk_screen_get_default());
	gdouble aspect_ratio = ((gdouble)screen_height/(gdouble)screen_width);
	*w = screen_width; *h = (gint)((gdouble)screen_width * aspect_ratio);
	if (ASPECT_RATIO > 0) {
		return ASPECT_RATIO;
	} else {
		return aspect_ratio;
	}
}	

/******************************************************************** MAIN **/
//int WINAPI winMain(void)
int main(int argc, char **argv)
{
   /* initialize */
	gtk_init(&argc, &argv);
	g_set_prgname(PROGRAM_NAME);

#define QUIET_MODE_WINDOWS 1
#if defined(WIN32) /* stop Windows from starting a console everytime */
#if QUIET_MODE_WINDOWS /* unless I want it to */
	FreeConsole();
#endif
#endif

   /* main program vbox to be packed: toobar->display/status->controls */
	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

   /* determine program size */
	gint scrWidth, scrHeight;
	gint WallFArt_wide, WallFArt_high;
	gdouble scrAspect = get_screen_size_and_aspect(&scrWidth, &scrHeight);
#define WFI_WIDE 430 /* 6 toolbar buttons wide (size 3 toolbuttons) (Xorg) */
	if (scrAspect > 0.7) { /* ~4/3 */ 
		WallFArt_wide = WFI_WIDE + BIGGER_BY;
		WallFArt_high = ((int)(float)WallFArt_wide * scrAspect);
	} else { /* ~16x9 */
		WallFArt_wide = WFI_WIDE + (BIGGER_BY*2); /* arbitrary 2 */
		WallFArt_high = ((int)(float)WallFArt_wide * scrAspect);
	}

	GtkWidget *wfi; /* wfi == W all F art I mage */
#define DO_IT_RANDOMLY 1
	if (DO_IT_RANDOMLY) {
	    srand((unsigned int)time(0));
	    wfi = wallfart_image_new_from_type
			(RANDOMTYPE, WallFArt_wide, WallFArt_high);
	} else { 
	/* TEST, MANDELBROT, MANDELCUBE, MANDELQUAD, MANDELQUINT, MANDELHEXNOT, STARFISH3, STARFISH, STARSHINE, SIMPLEC, FUZZY, FUZZY2, MONET, TRIANGLE, STRETCH, REVERSE, BUG, FACE, ZSQ_HALFC_Z */
	    wfi = wallfart_image_new_from_type
			(TEST, WallFArt_wide, WallFArt_high);
	}

   /* create the program toolbar */
	GtkWidget *toolbar = create_WallFArt_toolbar(WALLFART_IMAGE(wfi));
	gtk_box_pack_start(GTK_BOX(vbox), toolbar, TRUE,   TRUE,     0);
	gtk_widget_show(toolbar);

   /* create the display/status area */
	GtkWidget *image_box = create_WallFArt_image_box(WALLFART_IMAGE(wfi));
	gtk_box_pack_start(GTK_BOX(vbox), image_box, TRUE, TRUE, 0);
	gtk_widget_show(image_box);

   /* create the controls at the bottom */
	GtkWidget *ctrl = create_WallFArt_controls(WALLFART_IMAGE(wfi));
	gtk_box_pack_start(GTK_BOX(vbox), ctrl, TRUE, TRUE, 0);
	gtk_widget_show(ctrl);

   /* OK, let's finish things up, shall we  */
	GtkWidget *w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(w), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_container_add(GTK_CONTAINER(w), vbox);
	gtk_container_set_border_width(GTK_CONTAINER(w), 2);
	gtk_window_set_resizable(GTK_WINDOW(w), FALSE);
	gtk_widget_show(wfi);
	gtk_widget_show(vbox);
	gtk_widget_show(w);

	gtk_main();
	return 0;
}
