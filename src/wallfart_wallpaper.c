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
 *   Windows changes the desktop very differently than Linux/Xorg
 *    the wallfart_wallpaper files are where it is implemented. 
 *   (this code creates a 774x330 pixel confirmation dialog (Linux/Xorg))
 */
#include <gtk/gtk.h>
#include <string.h>

#if defined(WIN32)
#include <windows.h>
#endif

#include "wallfart_image.h"
#include "wallfart_wallpaper.h"

/**
 *  the "Are You Sure?" confirmation dialog.
 */ 
void wallfart_wallpaper_dialog(GtkWindow *topLevelWindow, WallfartImage *wfi)
{
	extern void wallfart_apply_wallpaper();
	GError *e = NULL;
	GtkWidget *content;

	GtkWidget *theDialog = gtk_dialog_new_with_buttons
					(WALLPAPER_DIALOG_TITLE,
					GTK_WINDOW(topLevelWindow),
					GTK_DIALOG_MODAL|
					GTK_DIALOG_DESTROY_WITH_PARENT,
					WALLPAPER_APPLY_BUTTON, GTK_RESPONSE_HELP, 
					WALLPAPER_HELP_BUTTON, GTK_RESPONSE_OK, 
					NULL);	

   /* A little warning emphasis and a personal plea, ala Pango markup */
	const gchar *cautionMsg = WFCAUTIONMSG;
	gchar *fmt = "<span weight=\"bold\" rise=\"200\" size=\"large\">%s</span>";
	gchar *printStr = g_markup_printf_escaped(fmt, cautionMsg);
	GtkWidget *aLabel = gtk_label_new("");
	gtk_label_set_use_markup(GTK_LABEL(aLabel), TRUE);
	gtk_label_set_markup(GTK_LABEL(aLabel), printStr);
	gtk_widget_show(aLabel);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(theDialog)->vbox), aLabel, TRUE, TRUE, 0);
	g_free(printStr);

   /* Now my lovely, if a tad lengthy, tome about beauty, loss, life and pain */ 
	const gchar *theText = WALLPAPER_IS_TRANSIENT;
	gchar *eot = g_strrstr(WALLPAPER_IS_TRANSIENT, EOT); 
	//gint textLen = eot - theText;
	gint textLen = strlen(theText);
	GtkTextBuffer *txtBuffer = gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(txtBuffer, theText, textLen);
	GtkWidget *txtViewer = gtk_text_view_new_with_buffer(txtBuffer);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(txtViewer), 10);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(txtViewer), 10);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(txtViewer), FALSE);
	//NOgtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(txtViewer), GTK_WRAP_NONE);
/*** what is the difference ? */
	//OKgtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(txtViewer), GTK_WRAP_WORD);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(txtViewer), GTK_WRAP_WORD_CHAR);
/*** ??? */
	//OKbutNeedsNLgtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(txtViewer), GTK_WRAP_CHAR);
	gtk_widget_show(txtViewer);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG
				(theDialog)->vbox), txtViewer, TRUE, TRUE, 0);

    /* Finally, let's run that dialog. */
	gtk_dialog_set_default_response(GTK_DIALOG(theDialog), (gint)GTK_RESPONSE_OK);
	gtk_window_set_default_size(GTK_WINDOW(GTK_DIALOG(theDialog)), 800, 600);
	gtk_window_set_resizable(GTK_WINDOW(GTK_DIALOG(theDialog)), FALSE);
	gint response = gtk_dialog_run(GTK_DIALOG(theDialog));
	switch (response) {
		case GTK_RESPONSE_HELP:
		wallfart_apply_wallpaper(topLevelWindow, wfi);
			break;
		case GTK_RESPONSE_OK:
		default:
			break;
	}
	gtk_widget_destroy(theDialog);
}

#if defined(WIN32)
/**
 *   Changing these registry keys changes desktop wallpaper for Win*
 *	HKCU\Control Panel\Wallpaper	
 *	HKCU\Control Panel\WallpaperStyle
 *	HKCU\Control Panel\TileWallpaper
 */
void wallfart_apply_wallpaper(GtkWindow *w, WallfartImage *fi)
{
	extern gchar *create_temporary_wallfart_image();

    /* the part of the registry to be modified; 
				TODO what if it doesn't exist? */
	const char *hkeyBase = "Control Panel\\Desktop";
	HKEY anHKEY;
	LONG openIt = RegOpenKeyEx(HKEY_CURRENT_USER, 
			hkeyBase, 
			0, KEY_ALL_ACCESS, &anHKEY);

	if (openIt == ERROR_SUCCESS) {
		openIt = RegSetValueEx
			(anHKEY, "WallpaperStyle", 0, REG_SZ, "2", 2); 
	} // TODO  what if != ERROR_SUCCESS ?? 

	if (openIt == ERROR_SUCCESS) {
		openIt = RegSetValueEx
			(anHKEY, "TileWallpaper", 0, REG_SZ, "0", 2);
	} // TODO  again
	RegCloseKey(anHKEY);

	char *pic = (char *)create_temporary_wallfart_image(fi);
	BOOL yn = SystemParametersInfo(SPI_SETDESKWALLPAPER, 
			0, pic, SPIF_UPDATEINIFILE|SPIF_SENDCHANGE);	
	g_free(pic);
}

/**
 *  - determine the users' 'official' temporary directory 
 *  - create a bmp of the wallfart there 
 *  - it seems every user on a Windows machine has their own temporary 
 *      directory so I don't need a unique tmpfile name ala mktemp()
 *	which works out nicely because I've forgotten how it's done...
 */
#define WALLFART_TMP_FILENAME "/00wallfarttmp.bmp"
gchar *create_temporary_wallfart_image(WallfartImage *fi)
{

	GError *err = NULL;
	gchar *fullPathName = NULL;
	const gchar *userTempDir = NULL;  /* = g_get_tmp_dir(); */

	if (userTempDir = (gchar *)g_get_tmp_dir()) {
		fullPathName = g_strdup(userTempDir);
		fullPathName = g_strconcat(fullPathName, 
				WALLFART_TMP_FILENAME, NULL);
   /* TODO could just generate a full screen sized wallpaper 
				image rather than scaling */
   /* now generate the pixbuf */ 
		fi->pixbuf = gdk_pixbuf_new_from_data
				((const guchar *)fi->rgb_data,
				GDK_COLORSPACE_RGB, FALSE, 8, 
				fi->f->width, fi->f->height, 
				fi->f->rowstride, NULL, NULL);
   /* and write the pixbuf to a bmp */
		gdk_pixbuf_save(fi->pixbuf, 
			(const char *)fullPathName, "bmp", NULL, NULL);
	//	g_free(userTempDir); /* does this need to be freed? 	*/
	/* Nope: In fact, that g_free() breaks things. Not sure why. */
		g_object_unref(fi->pixbuf);

	} else {
		return (gchar *)NULL;	
	}
   /* replace MinGW '/a/b/c/dir/..' unixish forward slashes with 
				'a\b\c\\dir\..' windowsish back slashes */
	gint sl = 0;
	do {
		if (fullPathName[sl] == '/') fullPathName[sl] = '\\';
	} while(fullPathName[sl++]);

   return fullPathName;
}

#else  /* this is not Windows */

/**
 *  The GDK/Xorg/Linux 'apply as wallpaper' specific code.
 */
void wallfart_apply_wallpaper(GtkWidget *b, gpointer wfi)
{
	WallfartImage *fi = WALLFART_IMAGE(wfi);

	GdkScreen *default_screen = gdk_screen_get_default();
	gint s_width  = gdk_screen_get_width(default_screen);
	gint s_height = gdk_screen_get_height(default_screen);
	GdkWindow *root_window = gdk_screen_get_root_window(default_screen);

	GdkPixmap *pixmap = NULL;
	GdkBitmap *bitmap = NULL;
	GdkColormap *cmap = gdk_rgb_get_colormap();

	fi->pixbuf = gdk_pixbuf_new_from_data((const guchar *)fi->rgb_data,
			GDK_COLORSPACE_RGB, FALSE, 8, 
			fi->f->width, fi->f->height, fi->f->rowstride,
			NULL, NULL);
	/* TODO no need to scale with the hardware these days */
  	/* Just make a screen sized WDC */ 
	GdkPixbuf *screen_pixbuf = gdk_pixbuf_scale_simple(fi->pixbuf, 
			s_width, s_height, GDK_INTERP_HYPER);
			//s_width, s_height, GDK_INTERP_HYPER);
			//s_width, s_height, GDK_INTERP_NEAREST);
			//s_width, s_height, GDK_INTERP_BILINEAR);
			/* INTERP_NEARST FASTEST but lowest qual */
		/*GDK_INTERP_NEAREST||TILES||BILINEAR||HYPER     */

	gdk_pixbuf_render_pixmap_and_mask_for_colormap(screen_pixbuf, 
			cmap, &pixmap, &bitmap, 255);
	gdk_window_set_back_pixmap(root_window, pixmap, FALSE);
	gdk_window_clear(root_window);

	g_object_unref(screen_pixbuf);
	g_object_unref(fi->pixbuf);
	g_object_unref(pixmap);
}
#endif  /* defined(WIN32) */
