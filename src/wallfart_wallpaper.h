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
 *	- mostly dialog texts for the apply as wallpaper functionality 
 *        (with clear references to Hamlet && Ricky Rickardo.. hmm, maybe
 *	   not so clear since I feel the need to point them out)
 *   
 */
#ifndef _WALLFART_WALLPAPER_H_
#define _WALLFART_WALLPAPER_H_

G_BEGIN_DECLS

#include <gtk/gtk.h>
#include "wallfart_image.h"

void wallfart_wallpaper_dialog(GtkWindow *topLevelWindow, WallfartImage *wfi);

#define WALLPAPER_DIALOG_TITLE "\" Do it ?   or  Don't do it ? \"  That is the question."

#define WFCAUTIONMSG " CAUTION! Please read this important message from your WallFArt program. "

#define WALLPAPER_APPLY_BUTTON " Do it! I laugh at caution, \"Ha Haa!\", LIFE IS TRANSITION..."
#define WALLPAPER_HELP_BUTTON  " Don't do it!  \"Lucy! We've got us some savin' to do!\"...  "

#define WALLPAPER_IS_TRANSIENT "\nWell hello there, wonderful!\n\nAs you well know, most, if not all, expressions of beauty are but transitory in nature. The ''applied wallfart'' is no exception. If you wish to keep this most beauteous of wallfarts for posterity, it needs to be saved properly: It needs to be given a name, and a home. The ''Save'' toolbar item is what should be used to accomplish that task. It is right beside the ''Wallpaper'' toolbar item you chose to get here.\n\nAlso, as your friendly neighborhood WallFArt program, it is of utmost concern to me that you not lose any of your precious data. I mean that sincerely. If you like your current background and don't know if you could get it back for yourself, overwriting it now may lose it for you forever: Gone! Just gone... like those socks that go missing in the dryer, they're gone.... you've got to let them go..... just let them go...... that's right, let all that anger go.......\nIt's poisoning you.\n\n\tOr, more simply put,\n\n\t\t- >>>>> WallFArt   HAS   NO   ''UNDO''   TOOL <<<<< - you have been warned -- WallFArt Out.\n\0~"

#define EOT "~" 

G_END_DECLS

#endif /* _WALLFART_WALLPAPER_H_ */
