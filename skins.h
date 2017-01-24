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
#define MAX_SKINS 10
#define DEFAULT_SKIN "tronic"

#define FONTSIZE16 10
#define FONTSIZE32 20

/* Default colours used throughout */
#define BACKGROUND_COLOUR 0x000000
#define VERSION_TEXT_COLOUR 0xFF8000
#define GENERAL_TEXT_COLOUR 0xFFFFFF
#define MENU_TEXT_COLOUR 0xFFFFFF
#define MENU_SELECTED_TEXT_COLOUR 0x639ACE
#define MENU_SELECTED_LIMIT_TEXT_COLOUR 0xFF0000
#define MENU_GREYED_TEXT_COLOUR 0x909090
#define DEFAULT_CONTROLS_TEXT_COLOUR 0xFF0000
#define CREDITS_TEXT_COLOUR 0xFF8000
#define HELP_TILE_COLOUR 0x709070
#define FADE_COLOUR 0x000000
#define AUTHOR_TEXT_COLOUR 0xFFFFFF
#define DESIGNER_TEXT_COLOUR 0xFFFF00
#define DESIGNER_GAMEOBJ_TILE_COLOUR 0x0000F0
#define DESIGNER_OBJS_TILE_COLOUR 0x6F006F
#define DESIGNER_OBJV_TILE_COLOUR 0x202020

/* Variables */
SDL_Surface *wm_icon;
SDL_Surface *icons;
#ifdef LIGHTNINGENABLED
SDL_Surface *efx;
#endif
SDL_Surface *ciphers;
SDL_Surface *alpha;
SDL_Surface *bgrnd;
TTF_Font *font;

SDL_Rect robbo_images_srcrect[8];
SDL_Rect score_images_srcrect[10];
SDL_Rect score_screw_srcrect;
SDL_Rect score_key_srcrect;
SDL_Rect score_bullet_srcrect;
SDL_Rect score_level_srcrect;

struct skin
{
  char foldername[100];		/* e.g. tronic */
  char name[60];		/* e.g. Tronic */
  char author[60];
  char about[3][60];		/* Up to 3 lines of text */
  char soundskin[60];		/* file name of the sound skin */
  Uint32 background_colour;
  Uint32 version_text_colour;
  Uint32 general_text_colour;
  Uint32 menu_text_colour;
  Uint32 menu_selected_text_colour;
  Uint32 menu_selected_limit_text_colour;
  Uint32 menu_greyed_text_colour;
  Uint32 default_controls_text_colour;
  Uint32 credits_text_colour;
  Uint32 help_tile_colour;
  Uint32 fade_colour;
  Uint32 author_text_colour;
  Uint32 designer_text_colour;
  Uint32 designer_GAMEOBJ_tile_colour;
  Uint32 designer_OBJS_tile_colour;
  Uint32 designer_OBJV_tile_colour;
};
struct skin skins[MAX_SKINS];

int skin_count;			/* How many skins are in the list */
int selected_skin;		/* Which skin in the list is currently selected */
int temp_selected_skin;

/* Function prototypes */
int find_all_skins (void);
void read_skinrc_files (void);
void sort_skins (void);
int read_skin_level_colour_override (void);
void load_selected_skin (void);
void set_wm_icon (void);
SDL_Surface *create_skin_preview (void);
void load_TTF_font (void);
