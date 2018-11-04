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
 * My attempt to learn to use gtk/gdk/glib etc and make a Widget 
 * 	I could call my very own... 	
 */	
#ifndef _WALLFART_IMAGE_
#define _WALLFART_IMAGE_

#include <gtk/gtk.h>
#include "wallfart_complex.h"
#include "wallfart_types.h"
#include "wallfart_data.h"

G_BEGIN_DECLS

#define WALLFART_IMAGE_TYPE							  \
	(wallfart_image_get_type())
#define WALLFART_IMAGE(obj)							  \
	(G_TYPE_CHECK_INSTANCE_CAST((obj), WALLFART_IMAGE_TYPE, WallfartImage))
#define WALLFART_IMAGE_CLASS(klass)						  \
	(G_TYPE_CHECK_CLASS_CAST((klass), WALLFART_IMAGE_TYPE, WallfartImageClass))
#define IS_WALLFART_IMAGE(obj)							  \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj), WALLFART_IMAGE_TYPE))
#define IS_WALLFART_IMAGE_CLASS(klass)						  \
	(G_TYPE_CHECK_CLASS_TYPE((klass), WALLFART_IMAGE_TYPE))
#define WALLFART_IMAGE_GET_CLASS(obj)						  \
	(G_TYPE_INSTANCE_GET_CLASS((obj), WALLFART_IMAGE_TYPE, WallfartImageClass))

typedef struct _WallfartImage      WallfartImage;
typedef struct _WallfartImageClass WallfartImageClass;

struct _WallfartImage {

	GtkEventBox 	parent;

	WallFArt	*f;
	GdkPixbuf 	*pixbuf;
	guchar    	*rgb_data;
	gint	   	 rowstride;
	gint	   	 n_channels;
	GtkAdjustment	*width_adj;  
	GtkAdjustment	*height_adj;
	GtkAdjustment	*side_adj;
	GtkAdjustment	*r_start_adj; 
	GtkAdjustment	*i_start_adj;
	GtkAdjustment	*r_z_adj;
	GtkAdjustment	*i_z_adj;
};

struct _WallfartImageClass {

	GtkEventBoxClass 	parent_class;
	void (*status)		(WallfartImage *fi, gdouble r, gdouble i, gdouble s);
};

GType      wallfart_image_get_type	(void) G_GNUC_CONST; 

GtkWidget *wallfart_image_new_from_type	(WallFArt_types t, gint w, gint h); 
GtkWidget *wallfart_image_new		(gint w, gint h); 

void	   wallfart_image_touch		(WallfartImage *fi);
void 	   wallfart_image_random_colour	(WallfartImage *fi);
void 	   wallfart_image_random_colour_inside(WallfartImage *fi);
void	   wallfart_image_shuffle	(WallfartImage *fi);
void 	   wallfart_image_set_size	(WallfartImage *fi, gint w, gint h);
void 	   wallfart_image_reset_image	(WallfartImage *fi);

G_END_DECLS

#endif /* _WALLFART_IMAGE_H_ */
