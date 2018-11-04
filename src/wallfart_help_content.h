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
 *  WallFArt help: 
 *	First level help:  the manditory help haiku; 
 *	Second level help: an xpm graphic (that's right, I said graphic)
 */
#ifndef _WALLFART_HELP_CONTENT_
#define _WALLFART_HELP_CONTENT_

#include <gtk/gtk.h>
#include "wallfart_image.h"

GtkWidget *help_content_table_1(WallfartImage *fi);
GtkWidget *help_content_table_2(GtkWidget *topLevelWidget);
GtkWidget *image_widget_from_xpm_data(char *theXpm[], GtkWidget *window);

#endif
