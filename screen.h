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
/* Redraw bits */
#define REDRAW_INITIALISE 15
#define REDRAW_EVERYTHING 7
#define REDRAW_INTERMEDIATE 3
#define REDRAW_ANIMATED 1

/* Scoreline redraw bits */
#define SCORELINE_ICONS 1
#define SCORELINE_SCREWS 2
#define SCORELINE_KEYS 4
#define SCORELINE_BULLETS 8
#define SCORELINE_LEVEL 16
#define SCORELINE_PACK 32
#define SCORELINE_AUTHOR 64

/* Message box subfunctions */
#define MESSAGE_BOX_SUB_INITIALISE 1
#define MESSAGE_BOX_SUB_SHOW 2
#define MESSAGE_BOX_SUB_KILL 3

/* Game area fade subfunctions */
#define FADE_SUB_INITIALISE 3
#define FADE_SUB_SHOW 1
#define FADE_SUB_KILL FADE_SUB_INITIALISE



#define EFX_SHOOT 8

/* Variables */
SDL_Surface *screen;

struct {
	int redraw;			/* An ORed combination of REDRAW_* bits */
} intro_screen;

struct {
	int redraw;			/* An ORed combination of REDRAW_* bits */
} help_screen;

struct {
	int redraw;			/* An ORed combination of REDRAW_* bits */
} options_screen;

struct {
	int redraw;			/* An ORed combination of REDRAW_* bits */
} game_area;

struct {
	int xoffset;
	int yoffset;
	int redraw;			/* An ORed combination of SCORELINE_* bits */
} scoreline;

struct {
	int xoffset;
	int yoffset;
} authorline;

struct {
	int xres;
	int yres;
	int field_size;
	int fullscreen;		/* 0 or SDL_FULLSCREEN */
	int xshift;
	int yshift;
} video;

typedef struct msgbox {
	char name[256];			/* A unique name */
	char message[256];		/* A message on a single line */
	int timeout;			/* Units of main loop cycles */
	int dynamic;			/* TRUE to size the box around the text, FALSE to specify the size */
	int w, h;				/* The msgbox size if dynamic = FALSE */
	/* The properties below are set by the system when the msgbox is created */
	int x, y;
	int bartopleftx, bartoplefty;
	int barbottomrightx, barbottomrighty;
} MSG_Box;

/* Function prototypes */
int show_game_area(void);
int show_game_area_fade(int subfunction, int type);
SDL_Rect set_rect(int x, int y, int w, int h);
void clear_screen(void);
int set_video_mode(void);
void toggle_fullscreen(int *fullscreen);
void show_level_colour(int debug_colour_select_r, int debug_colour_select_g, int debug_colour_select_b, int debug_colour_select_component);
void inc_colour_component(int *colour_component);
void dec_colour_component(int *colour_component);
void show_introscreen(void);
void show_helpscreen(void);
void show_optionsscreen(void);
void show_message_box(int subfunction, MSG_Box *msg_box);
void show_endscreen(void);
void show_scoreline (void);




