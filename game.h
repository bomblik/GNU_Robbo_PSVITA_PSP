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


//defines for Windows
//#define PLATFORM_WIN32
//#define PLATFORM_PC

#ifdef PLATFORM_WIN32
  #define PACKAGE_DATA_DIR "./data"
  #define VERSION "0.66"
  #define __func__ ""  /* unfortunatelly under vs6 is nothing like this macro ;( */
#endif

//#define HAVE_DESIGNER


#include <SDL/SDL.h>
#include <SDL/SDL_endian.h>

#ifndef PLATFORM_ZAURUS
  #include <SDL/SDL_image.h>
#endif

#ifdef USE_PIXMAP_FONT
  #include "font.h"
#else
  #include <SDL/SDL_ttf.h>
#endif

#include <SDL/SDL_mixer.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>		/* For ceil() */
#include <time.h>		/* For random data */
#include <sys/stat.h>		/* For time_t struct */

#ifndef PLATFORM_WIN32
  #include <unistd.h>		/* For getpid() */
  #include <dirent.h>		/* For dir functions */
#else
  #include "win32/dirent.h"		/* For dir functions windows has no such library */
#endif

#ifdef _SMOOTH_SCRL_ 
#define SCRL_MULT1 2
#define SCRL_MULT2 8
#endif


#include "ROB_engine.h"
#include "screen.h"
#include "board.h"
#include "controls.h"
#include "levels.h"
#include "skins.h"
#include "rcfile.h"
#include "sound.h"
#include "locales.h"
#include "pointer_controls.h"
#include "konstruktor.h"

/* Defines */
/*
#define DEBUG_RECORD_DEMO
#define DEBUG_COLOUR_SELECT
*/


#define TRUE 1
#define FALSE 0

#define UNDEFINED -1

#define SINGLE_SPACE " "

#define DATA_UNREAD 0
#define DATA_READING 1
#define DATA_READ 2
#define DATA_INCOMPLETE 3


#ifdef _SMOOTH_SCRL_ 
#define GAME_CYCLE_LIMIT_MAX (30*SCRL_MULT1)
#define GAME_CYCLE_LIMIT_MIN (20*SCRL_MULT1)
#define DEFAULT_GAME_CYCLE_LIMIT (20*SCRL_MULT1)
#else
#define GAME_CYCLE_LIMIT_MAX 33
#define GAME_CYCLE_LIMIT_MIN 20
#define DEFAULT_GAME_CYCLE_LIMIT 25
#endif

/* Game modes */
#define INTRO_SCREEN 0
#define GAME_ON 1
#define END_SCREEN 2
#define HELP_SCREEN 3
#define OPTIONS_SCREEN 4
#define DESIGNER_ON 5

/* Hotspot ids */
#define HOTSPOTS_ID 100		/* Requires at most OPTIONS_COUNT * 3 unique ids */

/* Help and Options page counts */
#define HELP_SCREEN_PAGES 4
#define OPTIONS_SCREEN_PAGES 8

/* Option page locations */
#define OPTIONS_MENUPOS_LEFT 9
#define OPTIONS_MENUPOS_SAVE 10
#define OPTIONS_MENUPOS_EXIT 11
#define OPTIONS_MENUPOS_RIGHT 12

/* Option count */
#define OPTIONS_COUNT 104

/* Option ids */
#define OPTIONS_LANGUAGE (0 * 13 + 0)
#define OPTIONS_SOUND (0 * 13 + 3)
#define OPTIONS_SFX_VOL (0 * 13 + 4)
#define OPTIONS_SAVE_FREQUENCY (0 * 13 + 6)
#define OPTIONS_GAME_SPEED (0 * 13 + 7)

#define OPTIONS_SKIN (1 * 13 + 0)

#define OPTIONS_DEFAULT_JOYSTICK (2 * 13 + 0)
#define OPTIONS_JOYSTICK_AXES_DEAD_ZONE (2 * 13 + 1)
#define OPTIONS_SYSTEM_POINTER (2 * 13 + 2)
#define OPTIONS_POINTER_CONTROLS_PAD_TYPE (2 * 13 + 3)
#define OPTIONS_KEY_REPEAT_DELAY (2 * 13 + 5)
#define OPTIONS_KEY_REPEAT_INTERVAL (2 * 13 + 6)
#define OPTIONS_RESTORE_DEFAULT_CONTROLS (2 * 13 + 8)

#define OPTIONS_RECONFIGURATION_HELP (3 * 13 + 0)
#define OPTIONS_ACTION_UP (4 * 13 + 0)
#define OPTIONS_ACTION_TOGGLE_FULLSCREEN (5 * 13 + 8)
#define OPTIONS_ACTION_TOGGLE_DESIGNER (6 * 13 + 8)
#define OPTIONS_ACTION_MODIFIER1 (7 * 13 + 2)
#define OPTIONS_ACTION_MODIFIER4 (7 * 13 + 5)
#define OPTIONS_ACTION_SCROLL_UP (7 * 13 + 6)
#define OPTIONS_ACTION_SCROLL_DOWN (7 * 13 + 7)
#define OPTIONS_ACTION_PRIMARY_CLICK (7 * 13 + 8)

/* Intro screen menu item count */
#define INTRO_SCREEN_MENU_ITEM_COUNT 7

/* Intro screen menu item ids */
#define INTRO_SCREEN_MENU_ITEM_START 0
#define INTRO_SCREEN_MENU_ITEM_LEVEL 1
#define INTRO_SCREEN_MENU_ITEM_PACK 2
#define INTRO_SCREEN_MENU_ITEM_HELP 3
#define INTRO_SCREEN_MENU_ITEM_OPTIONS 4
#define INTRO_SCREEN_MENU_ITEM_DESIGNER 5
#define INTRO_SCREEN_MENU_ITEM_EXIT 6

/* demo_mode subfunctions */
#define DEMO_MODE_INITIALISE 0
#define DEMO_MODE_TIMEOUT_INITIALISE 1
#define DEMO_MODE_TIMEOUT_DECREMENT 2
#define DEMO_MODE_ACTIVATE 3
#define DEMO_MODE_IS_ACTIVE 4
#define DEMO_MODE_DEACTIVATE 5
#define DEMO_MODE_RECORD_INITIALISE 6
#define DEMO_MODE_RECORD 7
#define DEMO_MODE_PLAYBACK 8
#define DEMO_MODE_DUMP 9

/* Variables */
#ifdef DEBUG_COLOUR_SELECT
int debug_colour_select_r;
int debug_colour_select_g;
int debug_colour_select_b;
int debug_colour_select_component;
#endif
int sound;
int game_mode;
int game_cycle_delay;		/* Defaults to 10ms */
int game_cycle_limit;		/* 20, 25, 33 Hz */
int temp_game_cycle_limit;	/* 20, 25, 33 Hz */
int cycle_count;		/* A running cycle count used for time stamping objects */
int temp_game_sound;
int temp_sfx_vol;
int introscreenselecteditem;
int helppage;
int helppageselecteditem;
int options[104];
int optionspage;
int optionspageselecteditem[OPTIONS_SCREEN_PAGES];

/* Function prototypes */
int my_rand ();
void my_srand (unsigned int seed);
int demo_mode (int demo_mode_state, int actionid);
void manage_game_on_input (int actionid);
/* Event processors */
void intro_screen_event_processor (ROB_Event * rob_event);
void help_screen_event_processor (ROB_Event * rob_event);
void options_screen_event_processor (ROB_Event * rob_event);
