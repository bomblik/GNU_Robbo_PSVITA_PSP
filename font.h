#ifdef USE_PIXMAP_FONT

/*  GNU Robbo
 *  Copyright (C) 2002-2010 The GNU Robbo Team (see AUTHORS).
 *
 *  GNU Robbo is free software - you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  GNU Robbo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the impled warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU CC; see the file COPYING. If not, write to the
 *  Free Software Foundation, 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
 */

/* Defines */

/* Variables */
typedef SDL_Surface TTF_Font;

/* Function prototypes */
int TTF_Init(void);
void TTF_Quit(void);
SDL_Surface *TTF_OpenFont(const char *file, int ptsize);
void TTF_CloseFont(SDL_Surface *font);
SDL_Surface *TTF_RenderUTF8_Shaded(SDL_Surface *font, const char *text, SDL_Color fg, SDL_Color bg);

#endif


