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
 *  creates a (help window @ 704x741 pixels (linux/Xorg))
 */
#include <gtk/gtk.h>
#include "wallfart_image.h"

/**
 *  I needed to have me some fun with the help so, I made it a graphic.
 *    (Introduced by a haiku)
 **/
GtkWidget *image_widget_from_xpm_data(char *theXpm[], GtkWidget *window)
{
	GtkWidget *pixmap;
	GdkPixmap *gdk_pixmap;
	GdkBitmap *mask;
	GtkStyle *style;
	GdkGC *gc;
    
	style = gtk_widget_get_default_style();
	gc = style->black_gc;
	gdk_pixmap = gdk_pixmap_create_from_xpm_d 
		(window->window, 
		 &mask, &style->bg[GTK_STATE_NORMAL], 
		 theXpm);

	pixmap = gtk_image_new_from_pixmap (gdk_pixmap, mask);
	gtk_widget_show (pixmap);

	return pixmap;
}

GtkWidget *help_content_table_2(GtkWidget *topLevelWidget)
{
	GtkWindow *topLevelWindow  = GTK_WINDOW(topLevelWidget);

/* XPM: static char *wallfart_help_graphic[] */
#include "wallfart_help_graphic.xpm"

	GtkWidget *content_2_Image = image_widget_from_xpm_data
				(wallfart_help_graphic, topLevelWidget);
	gtk_widget_show(content_2_Image);
	return content_2_Image;
}

/**
 * the initial help/about dialog Widget content; I used some pango markup
 *	to make it a little pretty... 
 */
GtkWidget *help_content_table_1(WallfartImage *fi)
{
	GtkWidget *l;
	GtkWidget *vbox = gtk_vbox_new(TRUE, 0);

	char *pFmtStr = "<span size=\"large\"><span weight=\"bold\">%s</span><span weight=\"bold\" size=\"large\">%s</span><span weight=\"normal\">%s</span><span weight=\"bold\" size=\"x-large\">%s</span><span weight=\"normal\">%s</span><span weight=\"bold\" size=\"large\">%s</span><span weight=\"bold\">%s</span></span>";
	const char *str1 = "~ WallFArt: ";
	const char *str2 = "Wall";
	const char *str3 = "paper ";
	const char *str4 = "F";
	const char *str5 = "ractal ";
	const char *str6 = "Art";
	const char *str7 = " ~";

	char *oStr = g_markup_printf_escaped
		(pFmtStr, str1, str2, str3, str4, str5, str6, str7);
	l = gtk_label_new("");
	gtk_label_set_use_markup(GTK_LABEL(l), TRUE);	
	gtk_label_set_markup(GTK_LABEL(l), oStr);
	gtk_box_pack_start(GTK_BOX(vbox), l, FALSE, FALSE, 0);
	gtk_widget_show(l);
	g_free(oStr);

	const char *msgLine1  = " ~ it's ''pseudo-random'' baby... ~ ";
	const char *pangoFmt1 = "<span style=\"oblique\" size=\"small\">%s</span>"; 
	char *theWhole1 = g_markup_printf_escaped(pangoFmt1,msgLine1);
	l = gtk_label_new("");
	gtk_label_set_use_markup(GTK_LABEL(l), TRUE);	
	gtk_label_set_markup(GTK_LABEL(l), theWhole1);
	gtk_box_pack_start(GTK_BOX(vbox), l, FALSE, FALSE, 0);
	gtk_widget_show(l);
	g_free(theWhole1);

	l = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(vbox), l, FALSE, FALSE, 0);
	gtk_widget_show(l);

#define WRITE_A_LINE(line)\
 l=gtk_label_new(line);\
 gtk_box_pack_start(GTK_BOX(vbox), l, FALSE, FALSE, 0);\
 gtk_widget_show(l);

     /* and finally, the requisite 'help haiku' */

	WRITE_A_LINE("A hovering mouse");
	WRITE_A_LINE("doth surely reveal but .swoosh..");
	WRITE_A_LINE("and ..sweesh. may conceal");
	WRITE_A_LINE("~");

	return vbox;
}
