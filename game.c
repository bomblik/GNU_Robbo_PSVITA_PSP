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

#include <SDL/SDL_main.h>
#include "game.h"

#ifdef PLATFORM_PSP
#include <pspkernel.h>
#include <psppower.h>

PSP_HEAP_SIZE_KB (8 * 1024);
PSP_MAIN_THREAD_ATTR (0);
PSP_MAIN_THREAD_STACK_SIZE_KB (2 * 1024);
#endif

/* Defines */
/*
#define DEBUG_MAIN
#define DEBUG_DEMO_MODE
#define DEBUG_DUMP_VM_USAGE
*/

#define MESSAGE_BOX_GETKEY_ID "getkey"
#define MESSAGE_BOX_GETKEY_ERROR_ID "getkeyerror"
#define MESSAGE_BOX_SAVING_ID "saving"
#define MESSAGE_BOX_RESTORE_ID "restore"

#define DELAY_MESSAGE_BOX_GETKEY (5 * game_cycle_limit)	/* ms */
#define DELAY_MESSAGE_BOX_GETKEY_ERROR (1.5 * game_cycle_limit)	/* ms */
#define DELAY_MESSAGE_BOX_SAVING (1.5 * game_cycle_limit)	/* ms */
#define DELAY_MESSAGE_BOX_RESTORE (1.5 * game_cycle_limit)	/* ms */

#define DELAY_DEMO (8 * game_cycle_limit)	/* ms */

#define DEMO_MODE_LENGTH 128
#define DEMO_MODE_DEMO_COUNT 13	/* Equal to number of demos + 1 */
#define DEMO_MODE_RECORD_SLOT DEMO_MODE_DEMO_COUNT - 1

/* Variables */
SDL_TimerID game_timer_id;	/* For timing */
int game_next_cycle;	/* A flag which will be set to TRUE by the timer */
int quit_game = 0;		/* Exits the main loop when TRUE */
unsigned int next_rand;		/* seed for random function */
int calling_game_mode;		/* So we don't lose the calling state */
int getkey_timeout = 0;		/* For [re]assigning controls */
int getkey_count = 0;		/* For [re]assigning controls */
int getkey_state = SDL_RELEASED;	/* For [re]assigning controls */
int gua_device;			/* Used when getting actions */
int gua_id;			/* Used when getting actions */
int gua_state;			/* Used when getting actions */

/* Function prototypes */
Uint32 game_timer (Uint32 interval, void *param);
void clean_up_before_exit (void);
void manage_intro_screen_select (int menuitemid);
void manage_intro_screen_decrement (int menuitemid);
void manage_intro_screen_increment (int menuitemid);
void manage_help_select (int selecteditem);
void manage_options_select (int optionid);
void manage_options_first (int optionid);
void manage_options_last (int optionid);
void manage_options_decrement (int optionid);
void manage_options_increment (int optionid);


#ifdef PLATFORM_PSP
static int setup_callbacks ();
#endif

/***************************************************/
/* Random number choosing **************************/
/***************************************************/

int
my_rand (void)
{
  next_rand = next_rand * 103515245 + 12345;
  return (next_rand >> 7);
}

/***************************************************/
/*** Random generator init :) **********************/
/***************************************************/

void
my_srand (unsigned int seed)
{
  next_rand += seed;
}

/***************************************************************************
 * Main                                                                    *
 ***************************************************************************/
/* Main loop */

#ifdef __cplusplus
extern "C"
#endif
  int
main (int argc, char *argv[])
{
  int count, result, actionid;

#ifdef DEBUG_SOUND2
	int scnt;
#endif  
#ifdef DEBUG_DUMP_VM_USAGE
  char vmusage[80];
#endif
  MSG_Box temp_msg_box;
  time_t t;

#ifdef PLATFORM_PSVITA
  SDL_Surface *image;
  SDL_RWops *rwop;
  SDL_Rect offset;
#endif

#ifdef DEBUG_MAIN
  printf ("*** Start %s ***\n", __func__);
  printf ("size of int: %i\n", sizeof (int));
  printf ("SDL_PRESSED=%i\n", SDL_PRESSED);
  printf ("SDL_RELEASED=%i\n", SDL_RELEASED);
  printf ("*** Stop %s ***\n", __func__);
#endif

  wm_icon = NULL;	/* This is no longer loaded with the skin */
  /* It is important that these surfaces are initialised to NULL here
   * because load_selected_skin will free them before loading another
   * skin if they are not NULL, and clean_up_before_exit will free them
   * if they are not NULL before returning to the OS */
  icons = NULL;
  ciphers = NULL;
  alpha = NULL;
  bgrnd = NULL;
  k_icons = NULL;
  font = NULL;

#ifdef PLATFORM_PSP
  /* Set up PSP exit and power callbacks */
  setup_callbacks ();
#endif

#ifdef PLATFORM_PSVITA
  #ifdef PSVITA_DEBUG
    psp2shell_init(3333, 0);
    sceKernelDelayThread(2*1000000);
  #endif
#endif

  /* Set some defaults */
  time (&t);
  my_srand (t);			/* random generator init */
  /* Initialise game properties */
  game_mode = INTRO_SCREEN;
  game_cycle_delay = 10;
  game_cycle_limit = DEFAULT_GAME_CYCLE_LIMIT;
  game_next_cycle = TRUE;
  game_timer_id = NULL;
  cycle_count = 0;
  restart_timeout = -1;
  
  /* Initialise menu properties */
  introscreenselecteditem = 0;
  helppage = 0;
  helppageselecteditem = 1;
  optionspage = 0;
  for (count = 0; count < OPTIONS_SCREEN_PAGES; count++)
      optionspageselecteditem[count] = 0;
  /* Initialise control properties */
  key_repeat_delay = DEFAULT_KEY_REPEAT_DELAY;
  key_repeat_interval = DEFAULT_KEY_REPEAT_INTERVAL;
  default_joystick = UNDEFINED;
  strcpy (default_joystick_name, "-1");
  joystick_dead_zone = JOYSTICK_DEAD_ZONE;
  pointer_controls.state = POINTER_CONTROLS_MODE_OFF;
  pointer_controls.shoot_state = POINTER_CONTROLS_SHOOT_OFF;
  pointer_controls.pad_type = POINTER_CONTROLS_PAD_TYPE_VIEWPORT;	/* POINTER_CONTROLS_PAD_TYPE_SCREEN; */
#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC) || defined(PLATFORM_CAANOO)
  gnurobbo_op_env.systempointer = TRUE;
#elif defined(PLATFORM_GP2X)
  gnurobbo_op_env.systempointer = FALSE;
#elif defined(PLATFORM_ZAURUS)
  gnurobbo_op_env.systempointer = FALSE;
#elif defined(PLATFORM_FREMANTLE)
  gnurobbo_op_env.systempointer = FALSE;
#elif defined(PLATFORM_PSP)
  gnurobbo_op_env.systempointer = FALSE;
#elif defined(PLATFORM_PSVITA)
  gnurobbo_op_env.systempointer = FALSE;
#endif
  gnurobbo_op_env.pointer = FALSE;

#if defined(SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS)
  gnurobbo_op_env.systempointer = FALSE;
  gnurobbo_op_env.pointer_mode = ROB_POINTER_MODE_OFF;
#else
  gnurobbo_op_env.pointer_mode = ROB_POINTER_MODE_PHYSICAL;
#endif

  gnurobbo_op_env.pointer_move_unit_low = UNDEFINED;
  gnurobbo_op_env.pointer_move_unit_high = UNDEFINED;
  gnurobbo_op_env.pointer_move_unit_threshold = UNDEFINED;
  /* Initialise game mechanics properties */
  game_mechanics.sensible_bears = MECHANIC_SENSIBLE_BEARS;
  game_mechanics.sensible_questionmarks = MECHANIC_SENSIBLE_QUESTIONMARKS;
  game_mechanics.sensible_solid_lasers = MECHANIC_SENSIBLE_SOLID_LASERS;
  /* Initialise video properties */
  video.xres = video.yres = video.field_size = video.fullscreen = UNDEFINED;
  viewport.max_w = viewport.max_h = viewport.maximise = UNDEFINED;
  intro_screen.redraw |= REDRAW_INITIALISE;
  /* Initialise sound properties */
  sound=1;
#ifdef HAVE_MUSIC
  volume = SND_MUS_DEFAULT_VOLUME;
  sfx_vol = SND_SFX_DEFAULT_VOLUME;
#else
  volume = SND_SFX_DEFAULT_VOLUME;
  sfx_vol = SND_SFX_DEFAULT_VOLUME;
#endif
  
  
  
  /* Initialise miscellaneous properties */
  rcfile.save_frequency = RCFILE_SAVE_ON_EXIT;
	K_exit=FALSE;
  /* Set-up the default user controls */
  set_default_user_controls (user_controls);

#ifndef PLATFORM_PSVITA
  /* Process any command line arguments. These will override any found in any resource file. */
  if (argc > 1)
    {
      for (count = 1; count < argc; count++)
	{
	  if (!strcmp (argv[count], "--help"))
	    {
						/*  1234567890123456789012345678901234567890 <- Formatting for small terminal. */
	      fprintf (stdout, "GNU Robbo version %s\n", VERSION);
	      fprintf (stdout, "Usage: gnurobbo [options]\n");
	      fprintf (stdout, "\nOptions:\n");
	      fprintf (stdout, "  -XRESxYRES  e.g. -800x480\n");
	      fprintf (stdout, "  -ts n       Tile size 16 or 32\n");
	      fprintf (stdout, "  -vpx        Maximise the viewport\n");
	      fprintf (stdout, "  -f          Run the game fullscreen\n");
	      fprintf (stdout, "\nReport bugs to :-\n");
	      fprintf (stdout, "http://sourceforge.net/projects/gnurobbo\n");
	      return 1;
	    }
	  else if (!strcmp (argv[count], "-f"))
	    {
	      video.fullscreen = SDL_FULLSCREEN;
	    }
	  else if (!strcmp (argv[count], "-vpx"))
	    {
	      viewport.maximise = TRUE;
	    }
	  else if (!strcmp (argv[count], "-ts"))
	    {
	      if (count < argc - 1 &&
		  sscanf (argv[count + 1], "%i", &video.field_size) == 1 &&
		  (video.field_size == 16 || video.field_size == 32))
		{
		  count++;
		}
	      else
		{
		  fprintf (stdout,
			   "Invalid tile size: Valid sizes are 16 and 32.\n");
		  return 1;
		}
	    }
	  else if (sscanf (argv[count], "-%ix%i", &video.xres, &video.yres) ==
		   2)
	    {
	      if (video.xres < 240 || video.yres < 240)
		{
		  fprintf (stdout,
			   "Invalid resolution: A minimum of 240x240 is required.\n");
		  return 1;
		}
	    }
	  else
	    {
	      fprintf (stdout, "Unrecognised option %s.\n", argv[count]);
	      return 1;
	    }
	}
      /* Now we can check that the field_size is valid for the requested resolution if any */
      if (video.field_size == 32 &&
	  ((video.xres != UNDEFINED && video.xres < 480)
	   || (video.yres != UNDEFINED && video.yres < 480)))
	{
	  fprintf (stdout,
		   "Invalid tile size: A size of 32 requires a minimum resolution of 480x480.\n");
	  return 1;
	}
    }
#else
  video.fullscreen = TRUE;
  video.xres = SCREEN_WIDTH;
  video.yres = SCREEN_HEIGHT;
  video.field_size = 32;
  viewport.maximise = TRUE;

  sceIoMkdir(CONFIG_DATA_DIR, 0777);
  sceIoMkdir(CONFIG_DATA_DIR "/" LEVELS_DIR, 0777);
#endif

#ifdef DEBUG_MAIN
  printf ("*** Start %s ***\n", __func__);
  printf ("video.fullscreen=%i\n", video.fullscreen);
  printf ("video.xres=%i\n", video.xres);
  printf ("video.yres=%i\n", video.yres);
  printf ("video.field_size=%i\n", video.field_size);
  printf ("viewport.maximise=%i\n", viewport.maximise);
  printf ("*** Stop %s ***\n", __func__);
#endif

  printf ("PACKAGE_DATA_DIR is %s\n", PACKAGE_DATA_DIR);

  /* Create the default user level pack if it is not found to exist and if the
   * target platform requires it also create the local data folder structure */
  create_userpack();

  /* Initialise the level pack list and currently selected pack and level */
  found_pack_count = level_pack_count = 1;
  selected_pack = 0;
  strcpy (level_packs[selected_pack].filename, PACKAGE_DATA_DIR "/" LEVELS_DIR "/" DEFAULT_LEVEL_PACK);
  strcpy (level_packs[selected_pack].name, "-1");	/* We don't know yet */
  level_packs[selected_pack].last_level = DEFAULT_LEVEL_START;	/* We don't know yet */
  level_packs[selected_pack].level_reached = DEFAULT_LEVEL_START;	/* The player hasn't reached any further than 1 yet */
  level_packs[selected_pack].level_selected = DEFAULT_LEVEL_START;	/* Level 1 is selected for play */
  level_packs[selected_pack].selected = TRUE;	/* Original pack is selected for play */

  /* Find all level packs */
  if (find_all_dat_files ())
    return 1;

  /* Read values from the level packs */
  read_level_packs ();

  /* Initialise the skin list and currently selected skin */
  skin_count = 1;
  selected_skin = 0;
  strcpy (skins[selected_skin].foldername, DEFAULT_SKIN);

  /* Find all skins */
  if (find_all_skins ())
    return 1;

  /* Read values from the skinrc files */
  read_skinrc_files ();

	/* Initialise the locale list and currently selected locale */
	locale_count = 1;
	selected_locale = 0;
	strcpy (locales[selected_locale].foldername, DEFAULT_LOCALE);

	/* Find all locales */
	if (find_all_locales ())
	return 1;

	/* Read values from the localerc files */
	read_localerc_files ();

	/* If possible, change the default locale to something
	 * that is equivalent to the LANG environment variable */
	set_locale_to_LANG();

	/* Set-up the resource file path for the required platform and read it now */
	#if defined(PLATFORM_WIN32)
		strcpy (path_resource_file, getenv("TEMP"));
	#elif defined(PLATFORM_PC)
		strcpy (path_resource_file, getenv ("HOME"));
		strcat (path_resource_file, "/");
	#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
		strcpy (path_resource_file, "./");
	#elif defined(PLATFORM_ZAURUS)
		strcpy (path_resource_file, getenv ("HOME"));
		strcat (path_resource_file, "/");
	#elif defined(PLATFORM_FREMANTLE)
		strcpy (path_resource_file, getenv ("HOME"));
		strcat (path_resource_file, "/MyDocs/.gnurobbo/");
	#elif defined(PLATFORM_PSP)
		strcpy (path_resource_file, "./");
	#elif defined(PLATFORM_PSVITA)
		strcpy (path_resource_file, CONFIG_DATA_DIR "/" );
	#endif
	strcat (path_resource_file, RESOURCE_FILE);
	printf ("RESOURCE_FILE is %s\n", path_resource_file);
	result = read_resource_file (path_resource_file);
	if (result == 1)
	{
		/* rc file was not found so write new default rc file */
		save_resource_file (path_resource_file, TRUE);
	}
	else if (result == 2)
	{
		/* rc file data is corrupt */
		return 1;
	}

#ifdef DEBUG_MAIN
  printf ("*** Start %s ***\n", __func__);
  printf ("skin_count=%i\n", skin_count);
  printf ("selected_skin=%i\n", selected_skin);
  for (count = 0; count < skin_count; count++)
    {
      printf ("skins[%i].foldername=%s\n", count, skins[count].foldername);
      printf ("skins[%i].name=%s\n", count, skins[count].name);
      printf ("skins[%i].author=%s\n", count, skins[count].author);
      printf ("skins[%i].about=%s\n", count, skins[count].about);
      printf ("skins[%i].background_colour=0x%06X\n", count,
	      skins[count].background_colour);
      printf ("skins[%i].version_text_colour=0x%06X\n", count,
	      skins[count].version_text_colour);
      printf ("skins[%i].general_text_colour=0x%06X\n", count,
	      skins[count].general_text_colour);
      printf ("skins[%i].menu_text_colour=0x%06X\n", count,
	      skins[count].menu_text_colour);
      printf ("skins[%i].menu_selected_text_colour=0x%06X\n", count,
	      skins[count].menu_selected_text_colour);
      printf ("skins[%i].menu_selected_limit_text_colour=0x%06X\n", count,
	      skins[count].menu_selected_limit_text_colour);
      printf ("skins[%i].menu_greyed_text_colour=0x%06X\n", count,
	      skins[count].menu_greyed_text_colour);
      printf ("skins[%i].default_controls_text_colour=0x%06X\n", count,
	      skins[count].default_controls_text_colour);
      printf ("skins[%i].credits_text_colour=0x%06X\n", count,
	      skins[count].credits_text_colour);
      printf ("skins[%i].help_tile_colour=0x%06X\n", count,
	      skins[count].help_tile_colour);
      printf ("skins[%i].fade_colour=0x%06X\n", count,
	      skins[count].fade_colour);
      printf ("skins[%i].author_text_colour=0x%06X\n", count,
	      skins[count].author_text_colour);
    }
  printf ("*** Stop %s ***\n", __func__);
  printf ("*** Start %s ***\n", __func__);
  printf ("found_pack_count=%i\n", found_pack_count);
  printf ("level_pack_count=%i\n", level_pack_count);
  printf ("selected_pack=%i\n", selected_pack);
  for (count = 0; count < level_pack_count; count++)
    {
      printf ("level_packs[%i].filename=%s\n", count,
	      level_packs[count].filename);
      printf ("level_packs[%i].name=%s\n", count, level_packs[count].name);
      printf ("level_packs[%i].last_level=%i\n", count,
	      level_packs[count].last_level);
      printf ("level_packs[%i].level_reached=%i\n", count,
	      level_packs[count].level_reached);
      printf ("level_packs[%i].level_selected=%i\n", count,
	      level_packs[count].level_selected);
      printf ("level_packs[%i].selected=%i\n", count,
	      level_packs[count].selected);
    }
  printf ("*** Stop %s ***\n", __func__);
#endif

  /* Sort level packs here */
  sort_level_packs ();

  /* Sort skins here */
  sort_skins ();

  /* Sort locales here */
  sort_locales ();

  /* Initialise SDL */
#ifndef PLATFORM_PSVITA
  if (SDL_Init (SDL_INIT_EVERYTHING))
#else
  if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK))
#endif
    {
      fprintf (stdout, "Cannot initialise SDL: %s", SDL_GetError ());
      return 1;
    }

  atexit (clean_up_before_exit);

  /* Initialise the joystick list and attempt to open one */
  if ((joystick_count = get_joystick_list (joystick_list, TRUE)))
    {
      /* Set default joystick */
      if ((default_joystick =
	   initialise_joystick (default_joystick, default_joystick_name,
				TRUE)) == UNDEFINED)
	default_joystick =
	  initialise_joystick (UNDEFINED, default_joystick_name, TRUE);
    }

#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
  /* Load the window manager icon and set caption */
  set_wm_icon();
  SDL_WM_SetCaption ("GNU Robbo", "gnurobbo");
#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
#elif defined(PLATFORM_ZAURUS)
#elif defined(PLATFORM_PSP)
#elif defined(PLATFORM_PSVITA)
#endif

  /* Set the video mode */
  if (set_video_mode ())
    {
      fprintf (stdout, "Cannot initialise screen: %s\n", SDL_GetError ());
      exit (1);
    }

#if defined(SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS)
  SDL_ShowCursor(SDL_DISABLE);
#endif

#if defined(PLATFORM_PSVITA) && defined(PLATFORM_PSVITA_GEKIHEN_CONTEST_SPLASH_SCREEN)
  rwop=SDL_RWFromFile(PACKAGE_DATA_DIR "/gekihen-splash.png", "rb");
  image=IMG_LoadPNG_RW(rwop);
  if(!image)
  {
    printf("Error loading gekihen splash screen\n");
  }

  offset.x = 0;
  offset.y = 0;

  SDL_BlitSurface( image, NULL, screen, &offset );
  SDL_FreeSurface (image);
  SDL_Flip (screen);
#endif

  /* Initiate a 10ms timer */
  game_timer_id = SDL_AddTimer (10, game_timer, NULL);

  /* Font initialisation */
  if (TTF_Init ())
    {
      fprintf (stdout, "Cannot initialise SDL_ttf module\n");
      exit (1);
    }

  /* Load selected skin */
  load_selected_skin ();
  /* this initializes audio and the beginning of the game and loads sound skin */
  audio_init();
  load_soundskin (skins[selected_skin].soundskin);
  make_playlist ();
  /* Load selected locale */
  load_selected_locale ();

  /* Disable key repeat (key repeating is handled by get_user_action() ) */
  set_key_repeat (0, SDL_DEFAULT_REPEAT_INTERVAL);

  /* Initialise the GNU Robbo Rectangular Object Engine */
  if (gnurobbo_op_env.pointer_move_unit_low == UNDEFINED)
    gnurobbo_op_env.pointer_move_unit_low = 4 * video.field_size / 16;
  if (gnurobbo_op_env.pointer_move_unit_high == UNDEFINED)
    gnurobbo_op_env.pointer_move_unit_high = 8 * video.field_size / 16;
  if (gnurobbo_op_env.pointer_move_unit_threshold == UNDEFINED)
    gnurobbo_op_env.pointer_move_unit_threshold = 6 * video.field_size / 16;
  gnurobbo_op_env.event_processor = &intro_screen_event_processor;
  ROB_Init (&gnurobbo_op_env);

  demo_mode (DEMO_MODE_INITIALISE, 0);

#ifdef DEBUG_DUMP_VM_USAGE
  printf ("*** Start %s ***\n", __func__);
  sprintf (vmusage, "cat /proc/%u/status | grep Vm", getpid ());
  system (vmusage);
  printf ("*** Stop %s ***\n", __func__);
#endif

  while (!quit_game)
    {
		/***************************************************************************
		 * DESIGNER_ON                                                             *
		 ***************************************************************************/

      if (game_mode == DESIGNER_ON)
	{
	  quit_game |= get_user_action (&actionid, TRUE, &gua_device, &gua_id,
					&gua_state);
	   if (actionid == ACTION_EXIT)
	    {
			if(K_exit==TRUE) {
			    konstruktor_end ();
				#if defined(SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS)
				ROB_SetOpEnvPointer(FALSE, ROB_POINTER_MODE_OFF);
				#endif
				game_mode = GAME_ON;
				K_exit=FALSE;
			} 
			else
			{
				K_exit=TRUE;
				sprintf(infostring, "Exit designer - select again to exit");
				inforedraw = 1;
			}
	    } else {
			
			if(actionid!=-1) {
				if(K_exit==TRUE) {
					inforedraw=1;
					sprintf(infostring,"  ");
				}
				K_exit=FALSE;
			}
		}
	  switch (actionid)
	    {
	    case ACTION_TOGGLE_FULLSCREEN:
	      toggle_fullscreen (&video.fullscreen);
	      break;
	    case ACTION_PREVIOUS_LEVEL:
	      if (level_packs[selected_pack].level_selected > 1)
		{
		  level_packs[selected_pack].level_selected--;
		  /* Cancel any active or pending fades */
		  restart_timeout = -1;
		  show_game_area_fade (FADE_SUB_KILL, 0);
		  #if defined(SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS)
		  ROB_SetOpEnvPointer(TRUE, ROB_POINTER_MODE_SIMULATED);
		  #endif
		  konstruktor_init ();
		}
	      break;
	    case ACTION_NEXT_LEVEL:
	      if (level_packs[selected_pack].level_selected <
		  level_packs[selected_pack].last_level)
		{
		  level_packs[selected_pack].level_selected++;
		  /* Cancel any active or pending fades */
		  restart_timeout = -1;
		  show_game_area_fade (FADE_SUB_KILL, 0);
		  #if defined(SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS)
		  ROB_SetOpEnvPointer(TRUE, ROB_POINTER_MODE_SIMULATED);
		  #endif
		  konstruktor_init ();
		}
	      break;
	    case ACTION_PREVIOUS_PACK:
	      if (selected_pack > 0)
		{
		  level_packs[selected_pack].selected = FALSE;
		  selected_pack--;
		  level_packs[selected_pack].selected = TRUE;
		  /* Cancel any active or pending fades */
		  restart_timeout = -1;
		  show_game_area_fade (FADE_SUB_KILL, 0);
		  #if defined(SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS)
		  ROB_SetOpEnvPointer(TRUE, ROB_POINTER_MODE_SIMULATED);
		  #endif
		  konstruktor_init ();
		}
	      break;
	    case ACTION_NEXT_PACK:
	      if (selected_pack < found_pack_count - 1)
		{
		  level_packs[selected_pack].selected = FALSE;
		  selected_pack++;
		  level_packs[selected_pack].selected = TRUE;
		  /* Cancel any active or pending fades */
		  restart_timeout = -1;
		  show_game_area_fade (FADE_SUB_KILL, 0);
		  #if defined(SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS)
		  ROB_SetOpEnvPointer(TRUE, ROB_POINTER_MODE_SIMULATED);
		  #endif
		  konstruktor_init ();
		}
	      break;
	    case ACTION_PRIMARY_CLICK:
	      konstruktor_click ();
	      break;
	    case ACTION_SCROLL_UP:
	      #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
	      for (count = 0; count < 4; count++)
	      #endif
	      konstruktor_scroll (0);
	      break;
	    case ACTION_UP:
	      #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
	      konstruktor_scroll (0);
	      #endif
	      break;
	    case ACTION_SCROLL_DOWN:
	      #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
	      for (count = 0; count < 4; count++)
	      #endif
	      konstruktor_scroll (2);
	      break;
	    case ACTION_DOWN:
	      #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
	      konstruktor_scroll (2);
	      #endif
	      break;
	    case ACTION_LEFT:
	      #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
	      konstruktor_scroll (1);
	      #endif
	      break;
	    case ACTION_RIGHT:
	      #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
	      konstruktor_scroll (3);
	      #endif
	      break;
#ifdef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
        case ACTION_SCROLL_LEFT:
          konstruktor_scroll (1);
          break;
        case ACTION_SCROLL_RIGHT:
          konstruktor_scroll (3);
          break;
#endif
/*	    case ACTION_EXIT:
	      konstruktor_end ();
	      game_mode = GAME_ON;
	      break;
*/

		  case ACTION_VOLUP:
#ifdef DEBUG_COLOUR_SELECT
	      if (debug_colour_select_component == 0)
		{
		  inc_colour_component (&debug_colour_select_r);
		}
	      else if (debug_colour_select_component == 1)
		{
		  inc_colour_component (&debug_colour_select_g);
		}
	      else if (debug_colour_select_component == 2)
		{
		  inc_colour_component (&debug_colour_select_b);
		}
	      level.colour_override =
		debug_colour_select_r << 16 | debug_colour_select_g << 8 |
		debug_colour_select_b;
	      game_area.redraw |= REDRAW_EVERYTHING;
#else
	      volume_up ();
#endif
	      break;
	    case ACTION_VOLDOWN:
#ifdef DEBUG_COLOUR_SELECT
	      if (debug_colour_select_component == 0)
		{
		  dec_colour_component (&debug_colour_select_r);
		}
	      else if (debug_colour_select_component == 1)
		{
		  dec_colour_component (&debug_colour_select_g);
		}
	      else if (debug_colour_select_component == 2)
		{
		  dec_colour_component (&debug_colour_select_b);
		}
	      level.colour_override =
		debug_colour_select_r << 16 | debug_colour_select_g << 8 |
		debug_colour_select_b;
	      game_area.redraw |= REDRAW_EVERYTHING;
#else
	      volume_down ();
#endif
	      break;
	    }

	 
	}
      else if (game_mode == GAME_ON)
	{
		/***************************************************************************
		 * GAME ON                                                                 *
		 ***************************************************************************/
	  /* Get none or one user action and quit on SDL_QUIT */
	  quit_game |=
	    get_user_action (&actionid, TRUE, &gua_device, &gua_id,
			     &gua_state);

#ifdef DEBUG_RECORD_DEMO
	  if (actionid != UNDEFINED && !demo_mode (DEMO_MODE_IS_ACTIVE, 0)
	      && robbo.moved == 0)
	    demo_mode (DEMO_MODE_RECORD, actionid);
#endif

	  /* The restart timeout counts down to a forced restart such as following a BIG_BOOM */
	  if (restart_timeout)
	    {
	      restart_timeout--;
	    }
	  else if (restart_timeout == 0)
	    {
	      restart_timeout--;
	      actionid = ACTION_RESTART;
	    }

	  /* If demo mode is active then substitute the actionid with one from the demo */
	  if (demo_mode (DEMO_MODE_IS_ACTIVE, 0))
	    {
	      if (actionid == UNDEFINED)
		{
		  if (robbo.moved == 0)
		    actionid = demo_mode (DEMO_MODE_PLAYBACK, 0);
		}
	      else
		{
		  actionid = ACTION_EXIT;	/* Quit demo mode on any action */
		}
	    }

	  switch (actionid)
	    {
	    case ACTION_UP:
	    case ACTION_DOWN:
	    case ACTION_LEFT:
	    case ACTION_RIGHT:
	    case ACTION_RESTART:
	    case ACTION_SHOOT_UP:
	    case ACTION_SHOOT_DOWN:
	    case ACTION_SHOOT_LEFT:
	    case ACTION_SHOOT_RIGHT:
	    case ACTION_SELECT:
	    case ACTION_EXIT:
	    case ACTION_HELP:
	    case ACTION_OPTIONS:
#ifdef HAVE_DESIGNER
	    case ACTION_TOGGLE_DESIGNER:
#endif
	      manage_game_on_input (actionid);
	      break;
	    case ACTION_TOGGLE_FULLSCREEN:
	      toggle_fullscreen (&video.fullscreen);
	      break;
	    case ACTION_PREVIOUS_LEVEL:
	      if (level_packs[selected_pack].level_selected > 1)
		{
		  level_packs[selected_pack].level_selected--;
		  /* Cancel any active or pending fades */
		  restart_timeout = -1;
		  show_game_area_fade (FADE_SUB_KILL, 0);
		  if (level_init ())
		    {
		      game_mode = INTRO_SCREEN;
		      intro_screen.redraw |= REDRAW_INITIALISE;
		    }
		  else
		    {
		      game_area.redraw |= REDRAW_EVERYTHING;
		    }
		}
	      break;
	    case ACTION_NEXT_LEVEL:
	      if (level_packs[selected_pack].level_selected <
		  level_packs[selected_pack].level_reached)
		{
		  level_packs[selected_pack].level_selected++;
		  /* Cancel any active or pending fades */
		  restart_timeout = -1;
		  show_game_area_fade (FADE_SUB_KILL, 0);
		  if (level_init ())
		    {
		      game_mode = INTRO_SCREEN;
		      intro_screen.redraw |= REDRAW_INITIALISE;
		    }
		  else
		    {
		      game_area.redraw |= REDRAW_EVERYTHING;
		    }
		}
	      break;
	    case ACTION_PREVIOUS_PACK:
	      if (selected_pack > 0)
		{
		  level_packs[selected_pack].selected = FALSE;
		  selected_pack--;
		  level_packs[selected_pack].selected = TRUE;
		  /* Cancel any active or pending fades */
		  restart_timeout = -1;
		  show_game_area_fade (FADE_SUB_KILL, 0);
		  if (level_init ())
		    {
		      game_mode = INTRO_SCREEN;
		      intro_screen.redraw |= REDRAW_INITIALISE;
		    }
		  else
		    {
		      game_area.redraw |= REDRAW_EVERYTHING;
		    }
		}
	      break;
	    case ACTION_NEXT_PACK:
	      if (selected_pack < found_pack_count - 1)
		{
		  level_packs[selected_pack].selected = FALSE;
		  selected_pack++;
		  level_packs[selected_pack].selected = TRUE;
		  /* Cancel any active or pending fades */
		  restart_timeout = -1;
		  show_game_area_fade (FADE_SUB_KILL, 0);
		  if (level_init ())
		    {
		      game_mode = INTRO_SCREEN;
		      intro_screen.redraw |= REDRAW_INITIALISE;
		    }
		  else
		    {
		      game_area.redraw |= REDRAW_EVERYTHING;
		    }
		}
	      break;
	    case ACTION_HOME:
	    case ACTION_END:
	      break;
	    case ACTION_PAGEUP:
#ifdef DEBUG_COLOUR_SELECT
	      debug_colour_select_component--;	/* R = 0, G = 1, B = 2 */
	      if (debug_colour_select_component < 0)
		debug_colour_select_component = 2;
#endif
	      break;
	    case ACTION_PAGEDOWN:
#ifdef DEBUG_COLOUR_SELECT
	      debug_colour_select_component++;	/* R = 0, G = 1, B = 2 */
	      if (debug_colour_select_component > 2)
		debug_colour_select_component = 0;
#endif
	      break;
	    case ACTION_VOLUP:
#ifdef DEBUG_COLOUR_SELECT
	      if (debug_colour_select_component == 0)
		{
		  inc_colour_component (&debug_colour_select_r);
		}
	      else if (debug_colour_select_component == 1)
		{
		  inc_colour_component (&debug_colour_select_g);
		}
	      else if (debug_colour_select_component == 2)
		{
		  inc_colour_component (&debug_colour_select_b);
		}
	      level.colour_override =
		debug_colour_select_r << 16 | debug_colour_select_g << 8 |
		debug_colour_select_b;
	      game_area.redraw |= REDRAW_EVERYTHING;
#else
	      volume_up ();
#endif
	      break;
	    case ACTION_VOLDOWN:
#ifdef DEBUG_COLOUR_SELECT
	      if (debug_colour_select_component == 0)
		{
		  dec_colour_component (&debug_colour_select_r);
		}
	      else if (debug_colour_select_component == 1)
		{
		  dec_colour_component (&debug_colour_select_g);
		}
	      else if (debug_colour_select_component == 2)
		{
		  dec_colour_component (&debug_colour_select_b);
		}
	      level.colour_override =
		debug_colour_select_r << 16 | debug_colour_select_g << 8 |
		debug_colour_select_b;
	      game_area.redraw |= REDRAW_EVERYTHING;
#else
	      volume_down ();
#endif
	      break;
	    case ACTION_MODIFIER1:
	    case ACTION_MODIFIER2:
	    case ACTION_MODIFIER3:
	    case ACTION_MODIFIER4:
	      break;
	    default:
	      break;
	    }
	}
      else if (game_mode == INTRO_SCREEN)
	{
		/***************************************************************************
		 * INTRO SCREEN                                                            *
		 ***************************************************************************/
	  /* Get none or one user action and quit on SDL_QUIT */
	  quit_game |=
	    get_user_action (&actionid, TRUE, &gua_device, &gua_id,
			     &gua_state);

	  /* Reset the demo mode timeout if a key was pressed */
	  if (actionid != UNDEFINED)
	    demo_mode (DEMO_MODE_TIMEOUT_INITIALISE, 0);

	  /* Decrement the demo mode timeout which will automatically force
	     demo mode after a predetermined period of time */
	  demo_mode (DEMO_MODE_TIMEOUT_DECREMENT, 0);

	  switch (actionid)
	    {
	    case ACTION_UP:
	      introscreenselecteditem--;
/*#ifndef HAVE_DESIGNER*/
	      if (introscreenselecteditem == INTRO_SCREEN_MENU_ITEM_DESIGNER)
	        introscreenselecteditem--;
/*#endif*/
	      if (introscreenselecteditem < 0)
		introscreenselecteditem = INTRO_SCREEN_MENU_ITEM_COUNT - 1;
	      intro_screen.redraw |= REDRAW_INTERMEDIATE;
	      break;
	    case ACTION_DOWN:
	      introscreenselecteditem++;
/*#ifndef HAVE_DESIGNER*/
	      if (introscreenselecteditem == INTRO_SCREEN_MENU_ITEM_DESIGNER)
	        introscreenselecteditem++;
/*#endif*/
	      if (introscreenselecteditem >= INTRO_SCREEN_MENU_ITEM_COUNT)
		introscreenselecteditem = 0;
	      intro_screen.redraw |= REDRAW_INTERMEDIATE;
	      break;
	    case ACTION_LEFT:
	      if (introscreenselecteditem == INTRO_SCREEN_MENU_ITEM_LEVEL ||
		  introscreenselecteditem == INTRO_SCREEN_MENU_ITEM_PACK)
		{
		  manage_intro_screen_decrement (introscreenselecteditem);
		}
	      break;
	    case ACTION_RIGHT:
	      if (introscreenselecteditem == INTRO_SCREEN_MENU_ITEM_LEVEL ||
		  introscreenselecteditem == INTRO_SCREEN_MENU_ITEM_PACK)
		{
		  manage_intro_screen_increment (introscreenselecteditem);
		}
	      break;
	    case ACTION_RESTART:
	      break;
	    case ACTION_SHOOT_UP:
	    case ACTION_SHOOT_DOWN:
	    case ACTION_SHOOT_LEFT:
	    case ACTION_SHOOT_RIGHT:
	      break;
	    case ACTION_SELECT:
	      manage_intro_screen_select (introscreenselecteditem);
	      break;
	    case ACTION_EXIT:
	      introscreenselecteditem = INTRO_SCREEN_MENU_ITEM_EXIT;
	      intro_screen.redraw |= REDRAW_INTERMEDIATE;
	      break;
	    case ACTION_HELP:
	      manage_intro_screen_select (INTRO_SCREEN_MENU_ITEM_HELP);
	      break;
	    case ACTION_OPTIONS:
	      manage_intro_screen_select (INTRO_SCREEN_MENU_ITEM_OPTIONS);
	      break;
	    case ACTION_TOGGLE_FULLSCREEN:
	      toggle_fullscreen (&video.fullscreen);
	      break;
	    case ACTION_PREVIOUS_LEVEL:
	      manage_intro_screen_decrement (INTRO_SCREEN_MENU_ITEM_LEVEL);
	      break;
	    case ACTION_NEXT_LEVEL:
	      manage_intro_screen_increment (INTRO_SCREEN_MENU_ITEM_LEVEL);
	      break;
	    case ACTION_PREVIOUS_PACK:
	      manage_intro_screen_decrement (INTRO_SCREEN_MENU_ITEM_PACK);
	      break;
	    case ACTION_NEXT_PACK:
	      manage_intro_screen_increment (INTRO_SCREEN_MENU_ITEM_PACK);
	      break;
	    case ACTION_HOME:
	      if (introscreenselecteditem == INTRO_SCREEN_MENU_ITEM_LEVEL)
		{
		  /* First level */
		  level_packs[selected_pack].level_selected = 1;
		  intro_screen.redraw |= REDRAW_INTERMEDIATE;
		}
	      else if (introscreenselecteditem == INTRO_SCREEN_MENU_ITEM_PACK)
		{
		  /* First pack */
		  level_packs[selected_pack].selected = FALSE;
		  selected_pack = 0;
		  level_packs[selected_pack].selected = TRUE;
		  intro_screen.redraw |= REDRAW_INTERMEDIATE;
		}
	      break;
	    case ACTION_END:
	      if (introscreenselecteditem == INTRO_SCREEN_MENU_ITEM_LEVEL)
		{
		  /* Last level */
		  level_packs[selected_pack].level_selected =
		    level_packs[selected_pack].level_reached;
		  intro_screen.redraw |= REDRAW_INTERMEDIATE;
		}
	      else if (introscreenselecteditem == INTRO_SCREEN_MENU_ITEM_PACK)
		{
		  /* Last pack */
		  level_packs[selected_pack].selected = FALSE;
		  selected_pack = found_pack_count - 1;
		  level_packs[selected_pack].selected = TRUE;
		  intro_screen.redraw |= REDRAW_INTERMEDIATE;
		}
	      break;
	    case ACTION_PAGEUP:
	    case ACTION_PAGEDOWN:
	      break;
	    case ACTION_TOGGLE_DESIGNER:
	      manage_intro_screen_select (INTRO_SCREEN_MENU_ITEM_DESIGNER);
	      break;
	    case ACTION_VOLUP:
	      volume_up ();
	      break;
	    case ACTION_VOLDOWN:
	      volume_down ();
	      break;
	    case ACTION_MODIFIER1:
	    case ACTION_MODIFIER2:
	    case ACTION_MODIFIER3:
	    case ACTION_MODIFIER4:
	      break;
	    default:
	      break;
	    }
	}
      else if (game_mode == END_SCREEN)
	{
		/***************************************************************************
		 * END SCREEN                                                              *
		 ***************************************************************************/
	  /* Get none or one user action and quit on SDL_QUIT */
	  quit_game |=
	    get_user_action (&actionid, TRUE, &gua_device, &gua_id,
			     &gua_state);
	  switch (actionid)
	    {
	    case ACTION_UP:
	    case ACTION_DOWN:
	    case ACTION_LEFT:
	    case ACTION_RIGHT:
	    case ACTION_RESTART:
	    case ACTION_SHOOT_UP:
	    case ACTION_SHOOT_DOWN:
	    case ACTION_SHOOT_LEFT:
	    case ACTION_SHOOT_RIGHT:
	    case ACTION_SELECT:
	      break;
	    case ACTION_EXIT:
	      game_mode = INTRO_SCREEN;
	      intro_screen.redraw |= REDRAW_INITIALISE;
	      break;
	    case ACTION_HELP:
	    case ACTION_OPTIONS:
	      break;
	    case ACTION_TOGGLE_FULLSCREEN:
	      toggle_fullscreen (&video.fullscreen);
	      break;
	    case ACTION_PREVIOUS_LEVEL:
	    case ACTION_NEXT_LEVEL:
	    case ACTION_PREVIOUS_PACK:
	    case ACTION_NEXT_PACK:
	    case ACTION_HOME:
	    case ACTION_END:
	    case ACTION_PAGEUP:
	    case ACTION_PAGEDOWN:
	    case ACTION_TOGGLE_DESIGNER:
	      break;
	    case ACTION_VOLUP:
	      volume_up ();
	      break;
	    case ACTION_VOLDOWN:
	      volume_down ();
	      break;
	    case ACTION_MODIFIER1:
	    case ACTION_MODIFIER2:
	    case ACTION_MODIFIER3:
	    case ACTION_MODIFIER4:
	      break;
	    default:
	      break;
	    }
	}
      else if (game_mode == HELP_SCREEN)
	{
		/***************************************************************************
		 * HELP SCREEN                                                             *
		 ***************************************************************************/
	  /* Get none or one user action and quit on SDL_QUIT */
	  quit_game |=
	    get_user_action (&actionid, TRUE, &gua_device, &gua_id,
			     &gua_state);
	  switch (actionid)
	    {
	    case ACTION_UP:
	    case ACTION_DOWN:
	      break;
	    case ACTION_LEFT:
	      helppageselecteditem--;
	      if (helppage > 0 && helppageselecteditem < 0)
		{
		  helppage--;
		  helppageselecteditem = 2;
		  help_screen.redraw |= REDRAW_EVERYTHING;
		}
	      else
		{
		  if (helppage == 0 && helppageselecteditem < 1)
		    helppageselecteditem = 1;
		  help_screen.redraw |= REDRAW_INTERMEDIATE;
		}
	      break;
	    case ACTION_RIGHT:
	      helppageselecteditem++;
	      if (helppage < HELP_SCREEN_PAGES - 1
		  && helppageselecteditem > 2)
		{
		  helppage++;
		  helppageselecteditem = 0;
		  help_screen.redraw |= REDRAW_EVERYTHING;
		}
	      else
		{
		  if (helppage == HELP_SCREEN_PAGES - 1
		      && helppageselecteditem > 1)
		    helppageselecteditem = 1;
		  help_screen.redraw |= REDRAW_INTERMEDIATE;
		}
	      break;
	    case ACTION_RESTART:
	    case ACTION_SHOOT_UP:
	    case ACTION_SHOOT_DOWN:
	    case ACTION_SHOOT_LEFT:
	    case ACTION_SHOOT_RIGHT:
	      break;
	    case ACTION_SELECT:
	      manage_help_select (helppageselecteditem);
	      break;
	    case ACTION_EXIT:
	      manage_help_select (1);
	      break;
	    case ACTION_HELP:
	    case ACTION_OPTIONS:
	      break;
	    case ACTION_TOGGLE_FULLSCREEN:
	      toggle_fullscreen (&video.fullscreen);
	      break;
	    case ACTION_PREVIOUS_LEVEL:
	    case ACTION_NEXT_LEVEL:
	    case ACTION_PREVIOUS_PACK:
	    case ACTION_NEXT_PACK:
	      break;
	    case ACTION_HOME:
	      /* First page */
	      if (helppage != 0)
		{
		  helppage = 0;
		  helppageselecteditem = 1;
		  help_screen.redraw |= REDRAW_EVERYTHING;
		}
	      break;
	    case ACTION_END:
	      /* Last page */
	      if (helppage != HELP_SCREEN_PAGES - 1)
		{
		  helppage = HELP_SCREEN_PAGES - 1;
		  helppageselecteditem = 1;
		  help_screen.redraw |= REDRAW_EVERYTHING;
		}
	      break;
	    case ACTION_PAGEUP:
	      /* Previous page */
	      manage_help_select (0);
	      break;
	    case ACTION_PAGEDOWN:
	      /* Next page */
	      manage_help_select (2);
	      break;
	    case ACTION_TOGGLE_DESIGNER:
	      break;
	    case ACTION_VOLUP:
	      volume_up ();
	      break;
	    case ACTION_VOLDOWN:
	      volume_down ();
	      break;
	    case ACTION_MODIFIER1:
	    case ACTION_MODIFIER2:
	    case ACTION_MODIFIER3:
	    case ACTION_MODIFIER4:
	      break;
	    default:
	      break;
	    }
	}
      else if (game_mode == OPTIONS_SCREEN)
	{
		/***************************************************************************
		 * OPTIONS SCREEN                                                          *
		 ***************************************************************************/
	  /* All the screens are laid out like this no matter what's displayed :-
	     --------------------
	     | 0                | options[OPTIONS_COUNT] is an array of all the options spread across all
	     | .                | the pages. Each element is either TRUE if there is a selectable option
	     | .                | present or FALSE if it is non-selectable i.e. a piece of text or something
	     | 8                | that occupies that space. Each option has a unique id which is an index into
	     |                  | this array and is used to identify the option when the user presses a control
	     | 9   10    11  12 | that may modify it. This array is also used in screen.c to assist in building
	     | <  Save  Exit  > | the pages. Additionally the selected menu item is recorded for each page in
	     -------------------- optionspageselecteditem[] with the current page recorded in optionspage.
	   */

	  /* Are we waiting for a key/button press to assign to a control? */
	  if (getkey_timeout)
	    {
	      getkey_timeout--;
	      if (getkey_timeout == 0)
		{
		  strcpy (temp_msg_box.name, MESSAGE_BOX_GETKEY_ERROR_ID);
		  strcpy (temp_msg_box.message, txt_No_input_was_detected);
		  temp_msg_box.timeout = DELAY_MESSAGE_BOX_GETKEY_ERROR;
		  temp_msg_box.dynamic = TRUE;
		  temp_msg_box.w = temp_msg_box.h = 0;
		  show_message_box (MESSAGE_BOX_SUB_INITIALISE,
				    &temp_msg_box);
		}
	      else
		{
		  /* Get none or one user action and quit on SDL_QUIT */
		  quit_game |=
		    get_user_action (&actionid, FALSE, &gua_device, &gua_id,
				     &gua_state);
		  /* Record the pressed/released state changes and look for a release. This is how it works :-
		     If the user pressed the confirm control to initialise reassignment then the initial state
		     is pressed and so to register a new key we will be looking for a release as the 3rd state
		     change.
		     If the user pressed and released the primary click control to initialise reassignment then
		     the initial state is released and so to register a new key we will be looking for a release
		     as the 2nd state change.
		     The difference is because click activates on release */
		  if ((gua_state == SDL_PRESSED || gua_state == SDL_RELEASED)
		      && getkey_state != gua_state)
		    {
		      getkey_state = gua_state;
		      getkey_count++;	/* Count the state changes */
		      if (getkey_state == SDL_RELEASED && getkey_count >= 2)
			{
			  show_message_box (MESSAGE_BOX_SUB_KILL, NULL);	/* Kill the message box */
			  getkey_timeout = 0;	/* Kill the timeout */
			  /* Store the new control */
				temp_user_controls[optionspageselecteditem
					[optionspage] + (optionspage -
					OPTIONS_ACTION_UP / 13) * 9].device = gua_device;
				temp_user_controls[optionspageselecteditem
					[optionspage] + (optionspage -
					OPTIONS_ACTION_UP / 13) * 9].id = gua_id;
				options_screen.redraw |= REDRAW_INTERMEDIATE;
			}
		    }
		}
	      /* No, so resume normal action servicing */
	    }
	  else
	    {
	      /* Get none or one user action and quit on SDL_QUIT */
	      quit_game |=
		get_user_action (&actionid, TRUE, &gua_device, &gua_id,
				 &gua_state);
	      switch (actionid)
		{
		case ACTION_UP:
		  do
		    {
		      if (optionspageselecteditem[optionspage] > 8)
			optionspageselecteditem[optionspage] =
			  OPTIONS_MENUPOS_LEFT;
		      optionspageselecteditem[optionspage]--;
		      if (optionspageselecteditem[optionspage] < 0)
			optionspageselecteditem[optionspage] =
			  OPTIONS_MENUPOS_SAVE;
		    }
		  while (!options
			 [optionspage * 13 +
			  optionspageselecteditem[optionspage]]);
		  options_screen.redraw |= REDRAW_INTERMEDIATE;
		  break;
		case ACTION_DOWN:
		  do
		    {
		      if (optionspageselecteditem[optionspage] > 8)
			optionspageselecteditem[optionspage] = -1;
		      optionspageselecteditem[optionspage]++;
		      if (optionspageselecteditem[optionspage] > 8)
			optionspageselecteditem[optionspage] =
			  OPTIONS_MENUPOS_SAVE;
		    }
		  while (!options
			 [optionspage * 13 +
			  optionspageselecteditem[optionspage]]);
		  options_screen.redraw |= REDRAW_INTERMEDIATE;
		  break;
		case ACTION_LEFT:
		  if (optionspageselecteditem[optionspage] >
		      OPTIONS_MENUPOS_LEFT)
		    {
		      if (options
			  [optionspage * 13 +
			   optionspageselecteditem[optionspage] - 1])
			optionspageselecteditem[optionspage]--;
		      options_screen.redraw |= REDRAW_INTERMEDIATE;
		    }
		  else if (optionspageselecteditem[optionspage] ==
			   OPTIONS_MENUPOS_LEFT)
		    {
		      /* Previous page */
		      if (optionspage > 0)
			{
			  optionspageselecteditem[--optionspage] =
			    OPTIONS_MENUPOS_RIGHT;
			  options_screen.redraw |= REDRAW_EVERYTHING;
			}
		    }
		  else if (optionspageselecteditem[optionspage] <
			   OPTIONS_MENUPOS_LEFT)
		    {
		      manage_options_decrement (optionspage * 13 +
						optionspageselecteditem
						[optionspage]);
		    }
		  break;
		case ACTION_RIGHT:
		  if (optionspageselecteditem[optionspage] >=
		      OPTIONS_MENUPOS_LEFT
		      && optionspageselecteditem[optionspage] <
		      OPTIONS_MENUPOS_RIGHT)
		    {
		      if (options
			  [optionspage * 13 +
			   optionspageselecteditem[optionspage] + 1])
			optionspageselecteditem[optionspage]++;
		      options_screen.redraw |= REDRAW_INTERMEDIATE;
		    }
		  else if (optionspageselecteditem[optionspage] ==
			   OPTIONS_MENUPOS_RIGHT)
		    {
		      /* Next page */
		      if (optionspage < OPTIONS_SCREEN_PAGES - 1)
			{
			  optionspageselecteditem[++optionspage] =
			    OPTIONS_MENUPOS_LEFT;
			  options_screen.redraw |= REDRAW_EVERYTHING;
			}
		    }
		  else if (optionspageselecteditem[optionspage] <
			   OPTIONS_MENUPOS_LEFT)
		    {
		      manage_options_increment (optionspage * 13 +
						optionspageselecteditem
						[optionspage]);
		    }
		  break;
		case ACTION_RESTART:
		  if (optionspageselecteditem[optionspage] <
		      OPTIONS_MENUPOS_LEFT)
		    {
		      /* Reset a control */
		      if (optionspage * 13 +
			  optionspageselecteditem[optionspage] >=
			  OPTIONS_ACTION_UP
			  && optionspage * 13 +
			  optionspageselecteditem[optionspage] <=
			  OPTIONS_ACTION_PRIMARY_CLICK)
			{
			  temp_user_controls[optionspage * 13 +
					     optionspageselecteditem
					     [optionspage] -
					     OPTIONS_ACTION_UP -
					     (optionspage - 4) * 4].device =
			    UNDEFINED;
			  temp_user_controls[optionspage * 13 +
					     optionspageselecteditem
					     [optionspage] -
					     OPTIONS_ACTION_UP -
					     (optionspage - 4) * 4].id =
			    UNDEFINED;
			  temp_user_controls[optionspage * 13 +
					     optionspageselecteditem
					     [optionspage] -
					     OPTIONS_ACTION_UP -
					     (optionspage - 4) * 4].mod =
			    UNDEFINED;
			}
		      options_screen.redraw |= REDRAW_INTERMEDIATE;
		    }
		  break;
		case ACTION_SHOOT_UP:
		case ACTION_SHOOT_DOWN:
		case ACTION_SHOOT_LEFT:
		case ACTION_SHOOT_RIGHT:
		  break;
		case ACTION_SELECT:
		  /* Unlike the others, this will also need to be entirely manipulatable via the pointer
		     and so it is additionally sent OPTIONS_MENUPOS_LEFT to OPTIONS_MENUPOS_RIGHT */
		  manage_options_select (optionspage * 13 +
					 optionspageselecteditem
					 [optionspage]);
		  break;
		case ACTION_EXIT:
		  manage_options_select (OPTIONS_MENUPOS_EXIT);	/* Any enabled exit will suffice */
		  break;
		case ACTION_HELP:
		case ACTION_OPTIONS:
		  break;
		case ACTION_TOGGLE_FULLSCREEN:
		  toggle_fullscreen (&video.fullscreen);
		  break;
		case ACTION_PREVIOUS_LEVEL:
		case ACTION_NEXT_LEVEL:
		case ACTION_PREVIOUS_PACK:
		case ACTION_NEXT_PACK:
		  break;
		case ACTION_HOME:
		  if (optionspageselecteditem[optionspage] >=
		      OPTIONS_MENUPOS_LEFT
		      && optionspageselecteditem[optionspage] <=
		      OPTIONS_MENUPOS_RIGHT)
		    {
		      optionspage = 0;
		      optionspageselecteditem[optionspage] =
			OPTIONS_MENUPOS_SAVE;
		      options_screen.redraw |= REDRAW_EVERYTHING;
		    }
		  else if (optionspageselecteditem[optionspage] <
			   OPTIONS_MENUPOS_LEFT)
		    {
		      manage_options_first (optionspage * 13 +
					    optionspageselecteditem
					    [optionspage]);
		    }
		  break;
		case ACTION_END:
		  if (optionspageselecteditem[optionspage] >=
		      OPTIONS_MENUPOS_LEFT
		      && optionspageselecteditem[optionspage] <=
		      OPTIONS_MENUPOS_RIGHT)
		    {
		      optionspage = OPTIONS_SCREEN_PAGES - 1;
		      optionspageselecteditem[optionspage] =
			OPTIONS_MENUPOS_EXIT;
		      options_screen.redraw |= REDRAW_EVERYTHING;
		    }
		  else if (optionspageselecteditem[optionspage] <
			   OPTIONS_MENUPOS_LEFT)
		    {
		      manage_options_last (optionspage * 13 +
					   optionspageselecteditem
					   [optionspage]);
		    }
		  break;
		case ACTION_PAGEUP:
		  /* Previous page */
		  manage_options_select (13 + OPTIONS_MENUPOS_LEFT);	/* Use <back on page1 as page0's is disabled */
		  break;
		case ACTION_PAGEDOWN:
		  /* Next page */
		  manage_options_select (OPTIONS_MENUPOS_RIGHT);	/* Any enabled next> will suffice */
		  break;
		case ACTION_TOGGLE_DESIGNER:
		  break;
		case ACTION_VOLUP:
		  volume_up ();
		  break;
		case ACTION_VOLDOWN:
		  volume_down ();
		  break;
		case ACTION_MODIFIER1:
		case ACTION_MODIFIER2:
		case ACTION_MODIFIER3:
		case ACTION_MODIFIER4:
		  break;
		default:
		  break;
		}
	    }
	}

      /* [Re]draw the screen and update the game objects if GAME_ON */
      if (game_mode == GAME_ON)
	{
	  /* Update all the game objects */
	  update_game ();
	  /* [Re]draw the screen */
	  game_area.redraw |= REDRAW_ANIMATED;
	  show_game_area ();	/* This happens here and nowhere else */
	  /* If the fade is active then show it */
	  show_game_area_fade (FADE_SUB_SHOW, 0);

#ifdef DEBUG_COLOUR_SELECT
	  show_level_colour (debug_colour_select_r, debug_colour_select_g,
			     debug_colour_select_b,
			     debug_colour_select_component);
#endif
	}
	 else if (game_mode == DESIGNER_ON)
	{
	 /* animate konstruktor stuff */
	 konstruktor_animate ();
	 #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
	 game_area.redraw |= REDRAW_ANIMATED;
	 #else
	 game_area.redraw |= REDRAW_EVERYTHING;
	 #endif
	 konstruktor_show_game_area ();

	}	
      else if (game_mode == INTRO_SCREEN)
	{
	  /* [Re]draw the screen */
	  intro_screen.redraw |= REDRAW_ANIMATED;
	  show_introscreen ();	/* This happens here and nowhere else */
	}
      else if (game_mode == END_SCREEN)
	{
	  /* [Re]draw the screen */
	  show_endscreen ();	/* This happens here and nowhere else */
	}
      else if (game_mode == HELP_SCREEN)
	{
	  /* [Re]draw the screen */
	  help_screen.redraw |= REDRAW_ANIMATED;
	  show_helpscreen ();	/* This happens here and nowhere else */
	}
      else if (game_mode == OPTIONS_SCREEN)
	{
	  /* [Re]draw the screen */
	  options_screen.redraw |= REDRAW_ANIMATED;
	  show_optionsscreen ();	/* This happens here and nowhere else */
	}

     /* Render any ROB engine objects */
      ROB_RenderObjects ();

     /* If there's a message box active then show it */
      show_message_box (MESSAGE_BOX_SUB_SHOW, NULL);

      /* Update the screen */
      SDL_Flip (screen);

      /* I think I should explain how the timing works as people may think I'm simply calling SDL_Delay(10)
         every cycle and then continuing which would be OKish if the workload was always the same.
         There is an SDL timer that sets the game_next_cycle flag every 20Hz (slow), 25Hz (normal) or 33Hz (fast).
         This game is far from being CPU intensive especially since only small portions of the screen are being
         updated when needed, so 90%+ of the time is spent in the while loop waiting for the game_next_cycle
         flag to be set by the SDL timer. The SDL_Delay(game_cycle_delay) within the while loop is simply passing
         control to the OS to service other tasks. If there was no delay here then this game would be a CPU hog.
         I should mention that on my computer I can do SDL_Delay(0) but other computers will need a minimum of
         10ms, therefore game_cycle_delay is set to 10ms by default but can be set to something else by modifying
         [game_cycle_delay] in the .gnurobborc file in your home folder. I expect this will never be modified */

      /* Is there a cycle limit in place? */
      if (game_cycle_limit)
	{ 
	  /* Wait until the timer sets the flag before proceeding */

	  while (!game_next_cycle) 
	      SDL_Delay (game_cycle_delay);	/* Robbo spends 90%+ of his time in this loop :) */

	  game_next_cycle = FALSE;

	}

      cycle_count++;

      if (game_mode != INTRO_SCREEN)
	demo_mode (DEMO_MODE_TIMEOUT_INITIALISE, 0);	/* Easier to do it once here */

    }

#ifdef DEBUG_RECORD_DEMO
  demo_mode (DEMO_MODE_DUMP, 0);
#endif

  save_resource_file (path_resource_file, TRUE);

  /* atexit() will call clean_up_before_exit() on return */
  return 0;
}

/***************************************************************************
 * Clean Up Before Exit                                                    *
 ***************************************************************************/
/* This is registered with atexit() once SDL is initialised */

void
clean_up_before_exit (void)
{

  if (game_timer_id) SDL_RemoveTimer (game_timer_id);

  /* WARNING: attempting to close joystick 0 on the GP2X using the GPH
   * SDK causes a seg fault and it may do the same on similar devices */
#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
  if (joystick)
    SDL_JoystickClose (joystick);
#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
  if ((joystick) && (SDL_JoystickIndex (joystick)) > 0)
    SDL_JoystickClose (joystick);
#elif defined(PLATFORM_ZAURUS)
  if (joystick)
    SDL_JoystickClose (joystick);
#elif defined(PLATFORM_FREMANTLE)
  if (joystick)
    SDL_JoystickClose (joystick);
#elif defined(PLATFORM_PSP)
  if (joystick)
    SDL_JoystickClose (joystick);
#elif defined(PLATFORM_PSVITA)
  if (joystick)
    SDL_JoystickClose (joystick);
#endif

  /* Shutdown the GNU Robbo Rectangular Object Engine */
  if (ROB_Quit ())
    printf ("%s: %s\n", __func__, ROB_GetError ());

  /* Free SDL surfaces if they are not NULL */
  if (wm_icon)
    SDL_FreeSurface (wm_icon);
  if (icons)
    SDL_FreeSurface (icons);
  if (ciphers)
    SDL_FreeSurface (ciphers);
  if (alpha)
    SDL_FreeSurface (alpha);
  if (bgrnd)
    SDL_FreeSurface (bgrnd);
  if (k_icons)
    SDL_FreeSurface (k_icons);

  /* Free the font if not NULL */
  if (font)
    TTF_CloseFont (font);

  /* kill audio content if it is necessary */
  audio_destroy ();

  TTF_Quit ();
  SDL_Quit ();

  #if defined(PLATFORM_PSVITA)
  sceKernelExitProcess(0);
  #endif
}

/***************************************************************************
 * Game Timer                                                              *
 ***************************************************************************/

Uint32
game_timer (Uint32 interval, void *param)
{
  static int intervals = 0;

  /* Count the units of 10ms and set a flag which enables the main loop to proceed */
  if (game_cycle_limit)
    {
      intervals++;
      if (intervals >= 100 / game_cycle_limit)
	{
	  game_next_cycle = TRUE;
	  intervals = 0;
	}
    }
  else
    {
      intervals = 0;
    }

  return interval;
}

/***************************************************************************
 * Demo Mode                                                               *
 ***************************************************************************/
/* Because of the randomness of certain objects it is advisable to record
   demos of Robbo interacting with simple things such as boxes, doors and
   teleports. Interacting with monsters and guns early on could end the demo
   prematurely so do it towards the end where unpredictability is not an issue.
   
   On entry: subfunction = DEMO_MODE_INITIALISE to initialise at program start
			 subfunction = DEMO_MODE_TIMEOUT_INITIALISE to reset the timeout
			 subfunction = DEMO_MODE_TIMEOUT_DECREMENT to decrement the timeout
			 subfunction = DEMO_MODE_ACTIVATE to activate demo mode
			 subfunction = DEMO_MODE_IS_ACTIVE to report if currently active 
			 subfunction = DEMO_MODE_DEACTIVATE to deactivate demo mode if active
			 subfunction = DEMO_MODE_RECORD_INITIALISE to initialise prior to recording
			 subfunction = DEMO_MODE_RECORD to record a demo with actionid = actionid
			 subfunction = DEMO_MODE_PLAYBACK to playback a demo
			 subfunction = DEMO_MODE_DUMP to dump a recorded demo to the console on program exit
   On exit:	 returns an actionid for DEMO_MODE_PLAYBACK
			 returns TRUE or FALSE for DEMO_MODE_IS_ACTIVATED
			 returns TRUE or FALSE for DEMO_MODE_TIMEOUT_DECREMENT indicating if
				demo mode was automatically activated or not
			 else UNDEFINED (-1)
*/

int demo_mode(int subfunction, int actionid) {
	static int demo_mode_active;								/* It's ON */
	static int demo_mode_timeout;								/* Time to wait before activating demo mode */
	static int demo_mode_frame;									/* Input frame pointer */
	static int *demo_mode_demo_data[DEMO_MODE_DEMO_COUNT];		/* Pointers to playback input data + 1 for the record slot */
	static int demo_mode_demos[DEMO_MODE_DEMO_COUNT - 1][2];	/* Available demos [pack][level] */
	static int demo_mode_demos_index;							/* Index into demo_mode_demos */
	static int demo_mode_original_selected_pack;				/* The current user selected pack which the demo will replace */
	static int demo_mode_original_level_selected;				/* The current user level selected which the demo will replace */

	/* Playback input data: record at any speed, it doesn't make any difference */
	static int demo_mode_demo_data_Forever_level4[] = { ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_DOWN, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_EXIT };
	static int demo_mode_demo_data_K_Robbo_level17[] = { ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_UP, ACTION_MODIFIER2, ACTION_SHOOT_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_MODIFIER2, ACTION_SHOOT_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_EXIT };
	static int demo_mode_demo_data_R653924_level2[] = { ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_MODIFIER2, ACTION_SHOOT_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_EXIT };
	static int demo_mode_demo_data_Robbo2009_level9[] = { ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_RIGHT, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_RIGHT, ACTION_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_LEFT, ACTION_DOWN, ACTION_LEFT, ACTION_DOWN, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_EXIT };
	static int demo_mode_demo_data_RobboXI_level3[] = { ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_DOWN, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_MODIFIER2, ACTION_SHOOT_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_MODIFIER2, ACTION_SHOOT_RIGHT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_EXIT };
	static int demo_mode_demo_data_Robbo98_level10[] = { ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_MODIFIER2, ACTION_SHOOT_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_MODIFIER2, ACTION_SHOOT_DOWN, ACTION_UP, ACTION_MODIFIER2, ACTION_SHOOT_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_MODIFIER2, ACTION_SHOOT_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_RIGHT, ACTION_EXIT };
	static int demo_mode_demo_data_Robbo_B_level4[] = { ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_DOWN, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_RIGHT, ACTION_SHOOT_UP, ACTION_MODIFIER2, ACTION_RIGHT, ACTION_RIGHT, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_EXIT };
	static int demo_mode_demo_data_RobboBoss_level12[] = { ACTION_DOWN, ACTION_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_DOWN, ACTION_MODIFIER2, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_MODIFIER2, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_MODIFIER2, ACTION_SHOOT_LEFT, ACTION_DOWN, ACTION_MODIFIER2, ACTION_SHOOT_LEFT, ACTION_UP, ACTION_MODIFIER2, ACTION_SHOOT_LEFT, ACTION_DOWN, ACTION_MODIFIER2, ACTION_SHOOT_LEFT, ACTION_LEFT, ACTION_UP, ACTION_MODIFIER2, ACTION_SHOOT_LEFT, ACTION_DOWN, ACTION_MODIFIER2, ACTION_SHOOT_LEFT, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_EXIT };
	static int demo_mode_demo_data_RobboII_level8[] = { ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_UP, ACTION_DOWN, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_EXIT };
	static int demo_mode_demo_data_RobboIV_level7[] = { ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_DOWN, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_UP, ACTION_MODIFIER2, ACTION_SHOOT_UP, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_UP, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_EXIT };
	static int demo_mode_demo_data_RobboV_level6[] = { ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_RIGHT, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_UP, ACTION_LEFT, ACTION_UP, ACTION_RIGHT, ACTION_UP, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_EXIT };
	static int demo_mode_demo_data_RobboX_level2[] = { ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_UP, ACTION_RIGHT, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_RIGHT, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_UP, ACTION_RIGHT, ACTION_LEFT, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_UP, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_UP, ACTION_MODIFIER2, ACTION_SHOOT_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_UP, ACTION_UP, ACTION_UP, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_DOWN, ACTION_DOWN, ACTION_RIGHT, ACTION_RIGHT, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_DOWN, ACTION_LEFT, ACTION_MODIFIER2, ACTION_SHOOT_DOWN, ACTION_LEFT, ACTION_LEFT, ACTION_RIGHT, ACTION_RIGHT, ACTION_RIGHT, ACTION_LEFT, ACTION_DOWN, ACTION_EXIT };

	static int demo_mode_demo_data_recorded[DEMO_MODE_LENGTH];
	int returnval = UNDEFINED, count;

	if (subfunction == DEMO_MODE_INITIALISE) {
		demo_mode_active = FALSE;
		demo_mode_timeout = DELAY_DEMO;
		demo_mode_frame = 0;
		demo_mode_demos_index = -1;	/* Will be incremented first before use */
		/* Initialise the demo data */
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "Forever")) break;
		demo_mode_demos[0][0] = count; demo_mode_demos[0][1] = 4;
		demo_mode_demo_data[0] = demo_mode_demo_data_Forever_level4;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "K-Robbo")) break;
		demo_mode_demos[1][0] = count; demo_mode_demos[1][1] = 17;
		demo_mode_demo_data[1] = demo_mode_demo_data_K_Robbo_level17;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "R653924")) break;
		demo_mode_demos[2][0] = count; demo_mode_demos[2][1] = 2;
		demo_mode_demo_data[2] = demo_mode_demo_data_R653924_level2;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "Robbo2009")) break;
		demo_mode_demos[3][0] = count; demo_mode_demos[3][1] = 9;
		demo_mode_demo_data[3] = demo_mode_demo_data_Robbo2009_level9;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "RobboXI")) break;
		demo_mode_demos[4][0] = count; demo_mode_demos[4][1] = 3;
		demo_mode_demo_data[4] = demo_mode_demo_data_RobboXI_level3;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "Robbo98")) break;
		demo_mode_demos[5][0] = count; demo_mode_demos[5][1] = 10;
		demo_mode_demo_data[5] = demo_mode_demo_data_Robbo98_level10;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "Robbo-B")) break;
		demo_mode_demos[6][0] = count; demo_mode_demos[6][1] = 4;
		demo_mode_demo_data[6] = demo_mode_demo_data_Robbo_B_level4;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "RobboBoss")) break;
		demo_mode_demos[7][0] = count; demo_mode_demos[7][1] = 12;
		demo_mode_demo_data[7] = demo_mode_demo_data_RobboBoss_level12;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "RobboII")) break;
		demo_mode_demos[8][0] = count; demo_mode_demos[8][1] = 8;
		demo_mode_demo_data[8] = demo_mode_demo_data_RobboII_level8;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "RobboIV")) break;
		demo_mode_demos[9][0] = count; demo_mode_demos[9][1] = 7;
		demo_mode_demo_data[9] = demo_mode_demo_data_RobboIV_level7;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "RobboV")) break;
		demo_mode_demos[10][0] = count; demo_mode_demos[10][1] = 6;
		demo_mode_demo_data[10] = demo_mode_demo_data_RobboV_level6;
		for (count = 0; count < level_pack_count - 1; count++) if (!strcmp(level_packs[count].name, "RobboX")) break;
		demo_mode_demos[11][0] = count; demo_mode_demos[11][1] = 2;
		demo_mode_demo_data[11] = demo_mode_demo_data_RobboX_level2;

		/* The record slot is the last pointer */
		demo_mode_demo_data[DEMO_MODE_RECORD_SLOT] = demo_mode_demo_data_recorded;
		
	} else if (subfunction == DEMO_MODE_TIMEOUT_INITIALISE) {
		demo_mode_timeout = DELAY_DEMO;

	} else if (subfunction == DEMO_MODE_TIMEOUT_DECREMENT) {
		if (!--demo_mode_timeout) demo_mode(DEMO_MODE_ACTIVATE, 0);
		returnval = demo_mode_active;

	} else if (subfunction == DEMO_MODE_ACTIVATE) {
		demo_mode_active = TRUE;
		demo_mode_frame = 0;
		/* * Store the currently selected pack
		   * Select the demo pack
		   * Store level_selected for the demo pack
		   * Select the demo level */
		demo_mode_original_selected_pack = selected_pack;
		if (++demo_mode_demos_index >= DEMO_MODE_DEMO_COUNT - 1) demo_mode_demos_index = 0;
		selected_pack = demo_mode_demos[demo_mode_demos_index][0];
		demo_mode_original_level_selected = level_packs[selected_pack].level_selected;
		level_packs[selected_pack].level_selected = demo_mode_demos[demo_mode_demos_index][1];
		#ifdef DEBUG_DEMO_MODE
			printf("*** Start %s ***\n", __func__);
			printf("Playing demo: pack %i level %i\n", demo_mode_demos[demo_mode_demos_index][0], demo_mode_demos[demo_mode_demos_index][1]);
			printf("*** Stop %s ***\n", __func__);
		#endif
		if (level_init()) {
			demo_mode(DEMO_MODE_DEACTIVATE, 0);			/* Restore user selected pack and level */
			demo_mode(DEMO_MODE_TIMEOUT_INITIALISE, 0);	/* Restart timer */
			game_mode = INTRO_SCREEN;
			intro_screen.redraw |= REDRAW_INITIALISE;
		} else {
			/* Game on */
			game_mode = GAME_ON;
			game_area.redraw |= REDRAW_INITIALISE;
			/* Initialise the fade */
			show_game_area_fade(FADE_SUB_INITIALISE, 16);
		}

	} else if (subfunction == DEMO_MODE_IS_ACTIVE) {
		returnval = demo_mode_active;

	} else if (subfunction == DEMO_MODE_DEACTIVATE) {
		if (demo_mode_active) {
			demo_mode_active = FALSE;
			/* * Restore level_selected for the demo pack
			   * Restore the selected pack */
			level_packs[selected_pack].level_selected = demo_mode_original_level_selected;
			selected_pack = demo_mode_original_selected_pack;
		}

	} else if (subfunction == DEMO_MODE_RECORD_INITIALISE) {
		for (count = 0; count < DEMO_MODE_LENGTH; count++) *(demo_mode_demo_data[DEMO_MODE_RECORD_SLOT] + count) = UNDEFINED;
		demo_mode_frame = 0;

	} else if (subfunction == DEMO_MODE_RECORD) {
		if (demo_mode_frame >= DEMO_MODE_LENGTH) {
			printf("Demo recording reached end of buffer\n");
			*(demo_mode_demo_data[DEMO_MODE_RECORD_SLOT] + DEMO_MODE_LENGTH - 1) = ACTION_EXIT;
		} else {
			*(demo_mode_demo_data[DEMO_MODE_RECORD_SLOT] + demo_mode_frame++) = actionid;
		}

	} else if (subfunction == DEMO_MODE_PLAYBACK) {
		returnval = *(demo_mode_demo_data[demo_mode_demos_index] + demo_mode_frame++);

	} else if (subfunction == DEMO_MODE_DUMP) {
		printf("\nstatic int demo_mode_demo_data_%s_level%i[] = { ", level_packs[selected_pack].name, level_packs[selected_pack].level_selected);
		for (count = 0; count < DEMO_MODE_LENGTH; count++) {
			if (*(demo_mode_demo_data[DEMO_MODE_RECORD_SLOT] + count) != UNDEFINED) printf("%i, ", *(demo_mode_demo_data[DEMO_MODE_RECORD_SLOT] + count));
		}
		printf("%c%c };\n\n", 8, 8);
	}

	return returnval;
}

/***************************************************************************
 * Manage Game On Input                                                    *
 ***************************************************************************/
/* This is required to activate things with a pointer and since there is no
   in-game menu equivalent we can reference everything by actionid as normal */

void
manage_game_on_input (int actionid)
{

  switch (actionid)
    {
    case ACTION_UP:
      /* Quit scrolling when Robbo is not teleporting and not in demo mode */
      if (!demo_mode (DEMO_MODE_IS_ACTIVE, 0) && !robbo.teleporting
	  && viewport.cycles_to_dest > 1)
	viewport.cycles_to_dest = 1;
      /* Attempt to move Robbo to his intended destination */
      if (robbo.alive)
	move_robbo (0, -1);
      break;
    case ACTION_DOWN:
      /* Quit scrolling when Robbo is not teleporting and not in demo mode */
      if (!demo_mode (DEMO_MODE_IS_ACTIVE, 0) && !robbo.teleporting
	  && viewport.cycles_to_dest > 1)
	viewport.cycles_to_dest = 1;
      /* Attempt to move Robbo to his intended destination */
      if (robbo.alive)
	move_robbo (0, 1);
      break;
    case ACTION_LEFT:
      /* Quit scrolling when Robbo is not teleporting and not in demo mode */
      if (!demo_mode (DEMO_MODE_IS_ACTIVE, 0) && !robbo.teleporting
	  && viewport.cycles_to_dest > 1)
	viewport.cycles_to_dest = 1;
      /* Attempt to move Robbo to his intended destination */
      if (robbo.alive)
	move_robbo (-1, 0);
      break;
    case ACTION_RIGHT:
      /* Quit scrolling when Robbo is not teleporting and not in demo mode */
      if (!demo_mode (DEMO_MODE_IS_ACTIVE, 0) && !robbo.teleporting
	  && viewport.cycles_to_dest > 1)
	viewport.cycles_to_dest = 1;
      /* Attempt to move Robbo to his intended destination */
      if (robbo.alive)
	move_robbo (1, 0);
      break;
    case ACTION_RESTART:
      restart_timeout = -1;
      if (level_init ())
	{
	  game_mode = INTRO_SCREEN;
	  music_stop ();
	  intro_screen.redraw |= REDRAW_INITIALISE;
	}
      else
	{
	  game_area.redraw |= REDRAW_EVERYTHING;
	  /* Initialise the fade */
	  show_game_area_fade (FADE_SUB_INITIALISE, 17);
	}
      break;
    case ACTION_SHOOT_UP:
      if (robbo.alive)
	shoot_robbo (0, -1);
      break;
    case ACTION_SHOOT_DOWN:
      if (robbo.alive)
	shoot_robbo (0, 1);
      break;
    case ACTION_SHOOT_LEFT:
      if (robbo.alive)
	shoot_robbo (-1, 0);
      break;
    case ACTION_SHOOT_RIGHT:
      if (robbo.alive)
	shoot_robbo (1, 0);
      break;
    case ACTION_SELECT:
      break;
    case ACTION_EXIT:
      /* If demo mode is active then deactivate it */
      demo_mode (DEMO_MODE_DEACTIVATE, 0);
      /* Cancel any active or pending fades */
      restart_timeout = -1;
      show_game_area_fade (FADE_SUB_KILL, 0);
      game_mode = INTRO_SCREEN;
      music_stop ();
      intro_screen.redraw |= REDRAW_INITIALISE;
      break;
    case ACTION_HELP:
      /* Cancel any active fade */
      show_game_area_fade (FADE_SUB_KILL, 0);
      calling_game_mode = game_mode;
      game_mode = HELP_SCREEN;
      help_screen.redraw |= REDRAW_INITIALISE;
      break;
    case ACTION_OPTIONS:
      /* Cancel any active fade */
      show_game_area_fade (FADE_SUB_KILL, 0);
      calling_game_mode = game_mode;
      game_mode = OPTIONS_SCREEN;
      options_screen.redraw |= REDRAW_INITIALISE;
      break;
    case ACTION_TOGGLE_DESIGNER:
      /* ok, we will have to initialize designer mode */
      game_mode = DESIGNER_ON;
      #if defined(SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS)
      ROB_SetOpEnvPointer(TRUE, ROB_POINTER_MODE_SIMULATED);
      #endif
      konstruktor_init ();
      break;
    }
}

/***************************************************************************
 * Manage Intro Screen Select                                              *
 ***************************************************************************/

void
manage_intro_screen_select (int menuitemid)
{

  if (menuitemid == INTRO_SCREEN_MENU_ITEM_START ||
      menuitemid == INTRO_SCREEN_MENU_ITEM_LEVEL ||
      menuitemid == INTRO_SCREEN_MENU_ITEM_PACK)
    {
      /* Game on */
      game_mode = GAME_ON;
      if (level_init ())
	{
	  game_mode = INTRO_SCREEN;
	  music_stop ();
	  intro_screen.redraw |= REDRAW_INITIALISE;
	}
      else
	{
	  game_area.redraw |= REDRAW_INITIALISE;
	  /* Initialise the fade */
	  show_game_area_fade (FADE_SUB_INITIALISE, 16);
	}
    }
  else if (menuitemid == INTRO_SCREEN_MENU_ITEM_HELP)
    {
      calling_game_mode = game_mode;
      game_mode = HELP_SCREEN;
      help_screen.redraw |= REDRAW_INITIALISE;
    }
  else if (menuitemid == INTRO_SCREEN_MENU_ITEM_OPTIONS)
    {
      calling_game_mode = game_mode;
      game_mode = OPTIONS_SCREEN;
      options_screen.redraw |= REDRAW_INITIALISE;
    }
  else if (menuitemid == INTRO_SCREEN_MENU_ITEM_DESIGNER)
    {
      /* It won't work from here */
    }
  else if (menuitemid == INTRO_SCREEN_MENU_ITEM_EXIT)
    {
      quit_game |= 1;
    }
}

/***************************************************************************
 * Manage Intro Screen Decrement                                           *
 ***************************************************************************/

void
manage_intro_screen_decrement (int menuitemid)
{

  if (menuitemid == INTRO_SCREEN_MENU_ITEM_LEVEL)
    {
      if (level_packs[selected_pack].level_selected > 1)
	level_packs[selected_pack].level_selected--;
    }
  else if (menuitemid == INTRO_SCREEN_MENU_ITEM_PACK)
    {
      if (selected_pack > 0)
	{
	  level_packs[selected_pack].selected = FALSE;
	  level_packs[--selected_pack].selected = TRUE;
	}
    }
  intro_screen.redraw |= REDRAW_INTERMEDIATE;
}

/***************************************************************************
 * Manage Intro Screen Increment                                           *
 ***************************************************************************/

void
manage_intro_screen_increment (int menuitemid)
{

  if (menuitemid == INTRO_SCREEN_MENU_ITEM_LEVEL)
    {
      if (level_packs[selected_pack].level_selected <
	  level_packs[selected_pack].level_reached)
	level_packs[selected_pack].level_selected++;
    }
  else if (menuitemid == INTRO_SCREEN_MENU_ITEM_PACK)
    {
      if (selected_pack < found_pack_count - 1)
	{
	  level_packs[selected_pack].selected = FALSE;
	  level_packs[++selected_pack].selected = TRUE;
	}
    }
  intro_screen.redraw |= REDRAW_INTERMEDIATE;
}

/***************************************************************************
 * Manage Help Select                                                      *
 ***************************************************************************/

void
manage_help_select (int selecteditem)
{

  if (selecteditem == 0)
    {
      /* Previous page */
      if (helppage > 0)
	{
	  helppage--;
	  helppageselecteditem = 1;
	  help_screen.redraw |= REDRAW_EVERYTHING;
	}
    }
  else if (selecteditem == 1)
    {
      /* Return to calling state */
      game_mode = calling_game_mode;
      if (game_mode == INTRO_SCREEN)
	{
	  intro_screen.redraw |= REDRAW_INITIALISE;
	}
      else if (game_mode == GAME_ON)
	{
	  game_area.redraw |= REDRAW_INITIALISE;
	}
    }
  else if (selecteditem == 2)
    {
      /* Next page */
      if (helppage < HELP_SCREEN_PAGES - 1)
	{
	  helppage++;
	  helppageselecteditem = 1;
	  help_screen.redraw |= REDRAW_EVERYTHING;
	}
    }
}

/***************************************************************************
 * Manage Options Select                                                   *
 ***************************************************************************/
/* This function makes it easy to manage the modification of dozens of options
   across dozens of pages either using an up/down menu item selector or by
   targeting the items directly with a pointer */

void
manage_options_select (int optionid)
{
  char temp_default_joystick_name[MAX_JOYSTICK_NAME_LENGTH];
  MSG_Box temp_msg_box;
  int count, result;

  if (options[optionid])
    {
      if (optionid % 13 == OPTIONS_MENUPOS_LEFT)
	{
	  /* Previous page */
	  if (optionspage > 0)
	    {
	      optionspage--;
	      options_screen.redraw |= REDRAW_EVERYTHING;
	    }
	}
      else if (optionid % 13 == OPTIONS_MENUPOS_SAVE)
	{
	  /* Save options */
	  game_cycle_limit = temp_game_cycle_limit;
	  if (joystick_count > 0)
	    {
	      if (default_joystick != temp_default_joystick)
		{
		  strcpy (temp_default_joystick_name, "-1");
		  if ((result =
		       initialise_joystick (temp_default_joystick,
					    temp_default_joystick_name,
					    TRUE)) != UNDEFINED)
		    {
		      default_joystick = result;
		      get_joystick_name (default_joystick,
					 default_joystick_name);
		    }
		}
	      joystick_dead_zone = temp_joystick_dead_zone;
	    }
	  key_repeat_delay = temp_key_repeat_delay;
	  key_repeat_interval = temp_key_repeat_interval;
	  if (selected_locale != temp_selected_locale)
	    {
	      selected_locale = temp_selected_locale;
	      load_selected_locale ();
	    }
	  rcfile.save_frequency = temp_rcfile_save_frequency;
	  for (count = 0; count < USER_CONTROLS; count++)
	    {
	      user_controls[count].device = temp_user_controls[count].device;
	      user_controls[count].id = temp_user_controls[count].id;
	      user_controls[count].mod = temp_user_controls[count].mod;
	      user_controls[count].state = SDL_RELEASED;	/* Stops controls becoming stuck in a pressed state */
	      user_controls[count].cyclesactive = 0;	/* Stops controls becoming stuck in a pressed state */
	      user_controls[count].delay = KEY_REPEAT_DELAY;
	      user_controls[count].interval = KEY_REPEAT_INTERVAL;
	    }
	  if (gnurobbo_op_env.systempointer !=
	      temp_gnurobbo_op_env.systempointer)
	    {
	      gnurobbo_op_env.systempointer =
		temp_gnurobbo_op_env.systempointer;
	      ROB_SetOpEnvSystemPointer (gnurobbo_op_env.systempointer);
	    }
	  pointer_controls.pad_type = temp_pointer_controls.pad_type;
	  if (selected_skin != temp_selected_skin)
	    {
	      selected_skin = temp_selected_skin;
	      load_selected_skin ();
	      level.colour_override = UNDEFINED;	/* Important ;) */
	      read_skin_level_colour_override ();
	      load_soundskin (skins[selected_skin].soundskin);
	    }
	  /* Save rcfile now if requested; recommended for battery operated or unstable devices */
	  if (rcfile.save_frequency == RCFILE_SAVE_ON_CHANGE)
	    {
	      save_resource_file (path_resource_file, FALSE);
#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
	      system ("sync");
#elif defined(PLATFORM_ZAURUS)
#elif defined(PLATFORM_PSP)
#elif defined(PLATFORM_PSVITA)
#endif
	    }
	  /* Some of the above will require the entire page to be redrawn so always redraw everything on save */
	  options_screen.redraw |= REDRAW_EVERYTHING;
	  strcpy (temp_msg_box.name, MESSAGE_BOX_SAVING_ID);
	  strcpy (temp_msg_box.message, txt_Changes_saved);
	  temp_msg_box.timeout = DELAY_MESSAGE_BOX_SAVING;
	  temp_msg_box.dynamic = TRUE;
	  temp_msg_box.w = temp_msg_box.h = 0;
	  show_message_box (MESSAGE_BOX_SUB_INITIALISE, &temp_msg_box);
	}
      else if (optionid % 13 == OPTIONS_MENUPOS_EXIT)
	{
	  /* Return to calling state */
	  game_mode = calling_game_mode;
	  if (game_mode == INTRO_SCREEN)
	    {
	      intro_screen.redraw |= REDRAW_INITIALISE;
	    }
	  else if (game_mode == GAME_ON)
	    {
	      game_area.redraw |= REDRAW_INITIALISE;
	    }
	}
      else if (optionid % 13 == OPTIONS_MENUPOS_RIGHT)
	{
	  /* Next page */
	  if (optionspage < OPTIONS_SCREEN_PAGES - 1)
	    {
	      optionspage++;
	      options_screen.redraw |= REDRAW_EVERYTHING;
	    }
	}
      else if (optionid == OPTIONS_RESTORE_DEFAULT_CONTROLS)
	{
	  /* Restore default controls */
	  set_default_user_controls (temp_user_controls);
	  options_screen.redraw |= REDRAW_INTERMEDIATE;
	  strcpy (temp_msg_box.name, MESSAGE_BOX_RESTORE_ID);
	  strcpy (temp_msg_box.message, txt_Default_controls_restored);
	  temp_msg_box.timeout = DELAY_MESSAGE_BOX_RESTORE;
	  temp_msg_box.dynamic = TRUE;
	  temp_msg_box.w = temp_msg_box.h = 0;
	  show_message_box (MESSAGE_BOX_SUB_INITIALISE, &temp_msg_box);
	}
      else if (optionid >= OPTIONS_ACTION_UP
	       && optionid <= OPTIONS_ACTION_PRIMARY_CLICK)
	{
	  /* Initiate assigning a new key or button to a control */
	  getkey_timeout = DELAY_MESSAGE_BOX_GETKEY - 1;	/* Minus one makes the transition smooth if the error box gets shown */
	  getkey_state = gua_state;
	  getkey_count = 0;
	  options_screen.redraw |= REDRAW_INTERMEDIATE;
	  strcpy (temp_msg_box.name, MESSAGE_BOX_GETKEY_ID);
	  strcpy (temp_msg_box.message, txt_Press_something_and_release);
	  temp_msg_box.timeout = DELAY_MESSAGE_BOX_GETKEY;
	  temp_msg_box.dynamic = TRUE;
	  temp_msg_box.w = temp_msg_box.h = 0;
	  show_message_box (MESSAGE_BOX_SUB_INITIALISE, &temp_msg_box);
	}
      else
	{
	  options_screen.redraw |= REDRAW_INTERMEDIATE;
	}
    }
}

/***************************************************************************
 * Manage Options First                                                    *
 ***************************************************************************/
/* This function makes it easy to manage the modification of dozens of
 * options across dozens of pages either using an up/down menu item selector
 * or by targeting the items directly with a pointer */

void manage_options_first (int optionid)
{
	if (options[optionid])
	{
		if (optionid % 13 >= OPTIONS_MENUPOS_LEFT &&
			optionid % 13 <= OPTIONS_MENUPOS_RIGHT)
		{
			/* Do nothing; these should have been dealt with before this function */
		}
		else if (optionid == OPTIONS_GAME_SPEED)
		{
			temp_game_cycle_limit = GAME_CYCLE_LIMIT_MAX;
		}
		else if (optionid == OPTIONS_DEFAULT_JOYSTICK)
		{
			temp_default_joystick = 0;
		}
		else if (optionid == OPTIONS_JOYSTICK_AXES_DEAD_ZONE)
		{
			temp_joystick_dead_zone = 1;
		}
		else if (optionid == OPTIONS_KEY_REPEAT_DELAY)
		{
			temp_key_repeat_delay = KEY_REPEAT_MIN;
		}
		else if (optionid == OPTIONS_KEY_REPEAT_INTERVAL)
		{
			temp_key_repeat_interval = KEY_REPEAT_MIN;
		}
		else if (optionid == OPTIONS_LANGUAGE)
		{
			temp_selected_locale = 0;
		}
		else if (optionid == OPTIONS_SAVE_FREQUENCY)
		{
			temp_rcfile_save_frequency = RCFILE_SAVE_ON_EXIT;
		}
		else if (optionid >= OPTIONS_ACTION_UP &&
			optionid <= OPTIONS_ACTION_PRIMARY_CLICK)
		{
			if (temp_user_controls[optionid - OPTIONS_ACTION_UP -
				((optionid - OPTIONS_ACTION_UP) / 13) * 4].device !=
				UNDEFINED && (optionid - OPTIONS_ACTION_UP - ((optionid -
				OPTIONS_ACTION_UP) / 13) * 4 < ACTION_MODIFIER1 ||
				optionid - OPTIONS_ACTION_UP - ((optionid -
				OPTIONS_ACTION_UP) / 13) * 4 > ACTION_MODIFIER4))
			{
				temp_user_controls[optionid - OPTIONS_ACTION_UP -
					((optionid - OPTIONS_ACTION_UP) / 13) * 4].mod =
					UNDEFINED;
			}
		}
		else if (optionid == OPTIONS_SYSTEM_POINTER)
		{
			temp_gnurobbo_op_env.systempointer = FALSE;
		}
		else if (optionid == OPTIONS_POINTER_CONTROLS_PAD_TYPE)
		{
			temp_pointer_controls.pad_type =
				POINTER_CONTROLS_PAD_TYPE_VIEWPORT;
		}
		else if (optionid == OPTIONS_SOUND)
		{
			temp_game_sound = sound = 0;
			#ifdef HAVE_MUSIC
				music_stop();
			#endif
		}
		else if (optionid == OPTIONS_SFX_VOL)
		{
			temp_sfx_vol = sfx_vol = 0;
			play_sound (SFX_SCREW, SND_NORM);
		}
		else if (optionid == OPTIONS_SKIN)
		{
			temp_selected_skin = 0;
		}
		options_screen.redraw |= REDRAW_INTERMEDIATE;
	}
}

/***************************************************************************
 * Manage Options Last                                                     *
 ***************************************************************************/
/* This function makes it easy to manage the modification of dozens of
 * options across dozens of pages either using an up/down menu item selector
 * or by targeting the items directly with a pointer */

void manage_options_last (int optionid)
{
	if (options[optionid])
	{
		if (optionid % 13 >= OPTIONS_MENUPOS_LEFT &&
			optionid % 13 <= OPTIONS_MENUPOS_RIGHT)
		{
			/* Do nothing; these should have been dealt with before this function */
		}
		else if (optionid == OPTIONS_GAME_SPEED)
		{
			temp_game_cycle_limit = GAME_CYCLE_LIMIT_MIN;
		}
		else if (optionid == OPTIONS_DEFAULT_JOYSTICK)
		{
			temp_default_joystick = joystick_count - 1;
		}
		else if (optionid == OPTIONS_JOYSTICK_AXES_DEAD_ZONE)
		{
			temp_joystick_dead_zone = 99;
		}
		else if (optionid == OPTIONS_KEY_REPEAT_DELAY)
		{
			temp_key_repeat_delay = KEY_REPEAT_MAX;
		}
		else if (optionid == OPTIONS_KEY_REPEAT_INTERVAL)
		{
			temp_key_repeat_interval = KEY_REPEAT_MAX;
		}
		else if (optionid == OPTIONS_LANGUAGE)
		{
			temp_selected_locale = locale_count - 1;
		}
		else if (optionid == OPTIONS_SAVE_FREQUENCY)
		{
			temp_rcfile_save_frequency = RCFILE_SAVE_ON_CHANGE;
		}
		else if (optionid >= OPTIONS_ACTION_UP &&
			optionid <= OPTIONS_ACTION_PRIMARY_CLICK)
		{
			if (temp_user_controls [optionid - OPTIONS_ACTION_UP -
				((optionid - OPTIONS_ACTION_UP) / 13) * 4].device !=
				UNDEFINED && (optionid - OPTIONS_ACTION_UP - ((optionid -
				OPTIONS_ACTION_UP) / 13) * 4 < ACTION_MODIFIER1 ||
				optionid - OPTIONS_ACTION_UP - ((optionid -
				OPTIONS_ACTION_UP) / 13) * 4 > ACTION_MODIFIER4))
			{
				temp_user_controls[optionid - OPTIONS_ACTION_UP -
					((optionid - OPTIONS_ACTION_UP) / 13) * 4].mod =
					ACTION_MODIFIER4;
			}
		}
		else if (optionid == OPTIONS_SYSTEM_POINTER)
		{
			temp_gnurobbo_op_env.systempointer = TRUE;
		}
		else if (optionid == OPTIONS_POINTER_CONTROLS_PAD_TYPE)
		{
			temp_pointer_controls.pad_type =
				POINTER_CONTROLS_PAD_TYPE_SCREEN;
		}
		else if (optionid == OPTIONS_SOUND)
		{
			temp_game_sound = sound = 1;
			play_sound (SFX_SCREW, SND_NORM);
		}
		else if (optionid == OPTIONS_SFX_VOL)
		{
			temp_sfx_vol = sfx_vol = 128;
			play_sound (SFX_SCREW, SND_NORM);
		}
		else if (optionid == OPTIONS_SKIN)
		{
			temp_selected_skin = skin_count - 1;
		}
		options_screen.redraw |= REDRAW_INTERMEDIATE;
	}
}

/***************************************************************************
 * Manage Options Decrement                                                *
 ***************************************************************************/
/* This function makes it easy to manage the modification of dozens of
 * options across dozens of pages either using an up/down menu item selector
 * or by targeting the items directly with a pointer */

void manage_options_decrement (int optionid)
{
	if (options[optionid])
	{
		if (optionid % 13 >= OPTIONS_MENUPOS_LEFT &&
			optionid % 13 <= OPTIONS_MENUPOS_RIGHT)
		{
			/* Do nothing; these should have been dealt with before this function */
		}
		else if (optionid == OPTIONS_GAME_SPEED)
		{
			if (temp_game_cycle_limit < GAME_CYCLE_LIMIT_MAX)
				temp_game_cycle_limit = 100 / (100 /
				temp_game_cycle_limit - 1);
		}
		else if (optionid == OPTIONS_DEFAULT_JOYSTICK)
		{
			if (temp_default_joystick > 0) temp_default_joystick--;
		}
		else if (optionid == OPTIONS_JOYSTICK_AXES_DEAD_ZONE)
		{
			if (temp_joystick_dead_zone > 1) temp_joystick_dead_zone--;
		}
		else if (optionid == OPTIONS_KEY_REPEAT_DELAY)
		{
			if (temp_key_repeat_delay > KEY_REPEAT_MIN)
				temp_key_repeat_delay -= 20;
		}
		else if (optionid == OPTIONS_KEY_REPEAT_INTERVAL)
		{
			if (temp_key_repeat_interval > KEY_REPEAT_MIN)
				temp_key_repeat_interval -= 20;
		}
		else if (optionid == OPTIONS_LANGUAGE)
		{
			if (temp_selected_locale > 0) temp_selected_locale--;
		}
		else if (optionid == OPTIONS_SAVE_FREQUENCY)
		{
			if (temp_rcfile_save_frequency > 0)
				temp_rcfile_save_frequency--;
		}
		else if (optionid >= OPTIONS_ACTION_UP &&
			optionid <= OPTIONS_ACTION_PRIMARY_CLICK)
		{
			if (temp_user_controls [optionid - OPTIONS_ACTION_UP -
				((optionid - OPTIONS_ACTION_UP) / 13) * 4].device !=
				UNDEFINED && (optionid - OPTIONS_ACTION_UP - ((optionid -
				OPTIONS_ACTION_UP) / 13) * 4 < ACTION_MODIFIER1 ||
				optionid - OPTIONS_ACTION_UP - ((optionid -
				OPTIONS_ACTION_UP) / 13) * 4 > ACTION_MODIFIER4))
			{
				if (temp_user_controls [optionid - OPTIONS_ACTION_UP -
					((optionid - OPTIONS_ACTION_UP) / 13) * 4].mod >
					ACTION_MODIFIER1)
				{
					temp_user_controls[optionid - OPTIONS_ACTION_UP -
						((optionid - OPTIONS_ACTION_UP) / 13) * 4].mod--;
				}
				else if (temp_user_controls [optionid - OPTIONS_ACTION_UP -
					((optionid - OPTIONS_ACTION_UP) / 13) * 4].mod ==
					ACTION_MODIFIER1)
				{
					temp_user_controls[optionid - OPTIONS_ACTION_UP -
						((optionid - OPTIONS_ACTION_UP) / 13) * 4].mod =
						UNDEFINED;
				}
			}
		}
		else if (optionid == OPTIONS_SYSTEM_POINTER)
		{
			if (temp_gnurobbo_op_env.systempointer == TRUE)
				temp_gnurobbo_op_env.systempointer = FALSE;
		}
		else if (optionid == OPTIONS_POINTER_CONTROLS_PAD_TYPE)
		{
			if (temp_pointer_controls.pad_type ==
				POINTER_CONTROLS_PAD_TYPE_SCREEN)
				temp_pointer_controls.pad_type =
					POINTER_CONTROLS_PAD_TYPE_VIEWPORT;
		}
		else if (optionid == OPTIONS_SOUND)
		{
			if (temp_game_sound > 0)
				temp_game_sound--;
			if (temp_game_sound == 0 && sound == 1)
				sound=0;
			#ifdef HAVE_MUSIC
				music_stop();
			#endif
		}
		else if (optionid == OPTIONS_SFX_VOL)
		{
			if (temp_sfx_vol > 0)
				temp_sfx_vol--;
			sfx_vol = temp_sfx_vol;
			play_sound (SFX_SCREW, SND_NORM);
		}
		else if (optionid == OPTIONS_SKIN)
		{
			if (temp_selected_skin > 0) temp_selected_skin--;
		}
		options_screen.redraw |= REDRAW_INTERMEDIATE;
	}
}

/***************************************************************************
 * Manage Options Increment                                                *
 ***************************************************************************/
/* This function makes it easy to manage the modification of dozens of
 * options across dozens of pages either using an up/down menu item selector
 * or by targeting the items directly with a pointer */

void manage_options_increment (int optionid)
{
	if (options[optionid])
	{
		if (optionid % 13 >= OPTIONS_MENUPOS_LEFT &&
			optionid % 13 <= OPTIONS_MENUPOS_RIGHT)
		{
			/* Do nothing; these should have been dealt with before this function */
		}
		else if (optionid == OPTIONS_GAME_SPEED)
		{
			if (temp_game_cycle_limit > GAME_CYCLE_LIMIT_MIN)
				temp_game_cycle_limit = 100 / (100 /
				temp_game_cycle_limit + 1);
		}
		else if (optionid == OPTIONS_DEFAULT_JOYSTICK)
		{
			if (temp_default_joystick < joystick_count - 1)
				temp_default_joystick++;
		}
		else if (optionid == OPTIONS_JOYSTICK_AXES_DEAD_ZONE)
		{
			if (temp_joystick_dead_zone < 99) temp_joystick_dead_zone++;
		}
		else if (optionid == OPTIONS_KEY_REPEAT_DELAY)
		{
			if (temp_key_repeat_delay < KEY_REPEAT_MAX)
				temp_key_repeat_delay += 20;
		}
		else if (optionid == OPTIONS_KEY_REPEAT_INTERVAL)
		{
			if (temp_key_repeat_interval < KEY_REPEAT_MAX)
				temp_key_repeat_interval += 20;
		}
		else if (optionid == OPTIONS_LANGUAGE)
		{
			if (temp_selected_locale < locale_count - 1)
				temp_selected_locale++;
		}
		else if (optionid == OPTIONS_SAVE_FREQUENCY)
		{
			if (temp_rcfile_save_frequency < 1)
				temp_rcfile_save_frequency++;
		}
		else if (optionid >= OPTIONS_ACTION_UP &&
			optionid <= OPTIONS_ACTION_PRIMARY_CLICK)
		{
			if (temp_user_controls [optionid - OPTIONS_ACTION_UP -
				((optionid - OPTIONS_ACTION_UP) / 13) * 4].device !=
				UNDEFINED && (optionid - OPTIONS_ACTION_UP - ((optionid -
				OPTIONS_ACTION_UP) / 13) * 4 < ACTION_MODIFIER1 ||
				optionid - OPTIONS_ACTION_UP - ((optionid -
				OPTIONS_ACTION_UP) / 13) * 4 > ACTION_MODIFIER4))
			{
				if (temp_user_controls [optionid - OPTIONS_ACTION_UP -
					((optionid - OPTIONS_ACTION_UP) / 13) * 4].mod ==
					UNDEFINED)
				{
					temp_user_controls[optionid - OPTIONS_ACTION_UP -
						((optionid - OPTIONS_ACTION_UP) / 13) * 4].mod =
						ACTION_MODIFIER1;
				}
				else if (temp_user_controls [optionid - OPTIONS_ACTION_UP -
					((optionid - OPTIONS_ACTION_UP) / 13) * 4].mod <
					ACTION_MODIFIER4)
				{
					temp_user_controls[optionid - OPTIONS_ACTION_UP -
						((optionid - OPTIONS_ACTION_UP) / 13) * 4].mod++;
				}
			}
		}
		else if (optionid == OPTIONS_SYSTEM_POINTER)
		{
			if (temp_gnurobbo_op_env.systempointer == FALSE)
				temp_gnurobbo_op_env.systempointer = TRUE;
		}
		else if (optionid == OPTIONS_POINTER_CONTROLS_PAD_TYPE)
		{
			if (temp_pointer_controls.pad_type ==
				POINTER_CONTROLS_PAD_TYPE_VIEWPORT)
				temp_pointer_controls.pad_type =
					POINTER_CONTROLS_PAD_TYPE_SCREEN;
		}
		else if (optionid == OPTIONS_SOUND)
		{
			if (temp_game_sound < 1)
				temp_game_sound = 1;
			if (sound == 0 && temp_game_sound == 1)
			{
				sound=1;	
				play_sound (SFX_SCREW, SND_NORM);
			}
		}
		else if (optionid == OPTIONS_SFX_VOL)
		{
			if (temp_sfx_vol < 128)
				temp_sfx_vol++;
			sfx_vol = temp_sfx_vol;
			play_sound (SFX_SCREW, SND_NORM);
		}
		else if (optionid == OPTIONS_SKIN)
		{
			if (temp_selected_skin < skin_count - 1) temp_selected_skin++;
		}
		options_screen.redraw |= REDRAW_INTERMEDIATE;
	}
}

/***************************************************************************
 * Intro Screen Event Processor                                            *
 ***************************************************************************/
/* This receives events from the ROB engine for the hotspots */

void intro_screen_event_processor (ROB_Event * rob_event)
{
	int menuitemid, selected = FALSE;

	/* We're only interested in Intro Screen hotspots */
	if (rob_event->rob_object->id > HOTSPOTS_ID &&
		rob_event->rob_object->id <
		HOTSPOTS_ID + 1 + INTRO_SCREEN_MENU_ITEM_COUNT * 3)
	{
		/* We're only interested in click and select events */
		if (rob_event->klasse == ROB_EVENT_CLICK ||
			rob_event->klasse == ROB_EVENT_SELECT)
		{
			/* Calculate the raw menuitemid from the object id */
			menuitemid = rob_event->rob_object->id - 1 - HOTSPOTS_ID;
			
			#ifdef DEBUG_ROB_HOTSPOTS
				ROB_ShowEvent (rob_event, __func__);
				printf ("%i: menuitemid=%i\n", cycle_count, menuitemid);
			#endif
			
			if (menuitemid / INTRO_SCREEN_MENU_ITEM_COUNT == 0)
			{
				/* This will be the textual part of a menu item */
				if (rob_event->klasse == ROB_EVENT_CLICK)
				{
					/* Records item's current selected state */
					if (introscreenselecteditem == menuitemid)
						selected = TRUE;
					/* Change the selected item to that which was clicked */
					introscreenselecteditem = menuitemid;
					/* Single click to activate everything except pack and
					 * level */
					if (menuitemid < INTRO_SCREEN_MENU_ITEM_LEVEL ||
						menuitemid > INTRO_SCREEN_MENU_ITEM_PACK)
					{
						manage_intro_screen_select (menuitemid);
					}
					else if (!selected)
					{
						/* For pack and level, the first click selects
						 * it to allow for adjusting */
						intro_screen.redraw |= REDRAW_INTERMEDIATE;
					}
					else
					{
						/* For pack and level, the second click activates
						 * the code */
						manage_intro_screen_select (menuitemid);
					}
				}
			}
			else if (menuitemid / INTRO_SCREEN_MENU_ITEM_COUNT == 1)
			{
				/* This will be the "<" part of a menu item if it exists */
				if (rob_event->klasse == ROB_EVENT_SELECT)
				{
					manage_intro_screen_decrement (menuitemid %
						INTRO_SCREEN_MENU_ITEM_COUNT);
				}
			}
			else if (menuitemid / INTRO_SCREEN_MENU_ITEM_COUNT == 2)
			{
				/* This will be the ">" part of a menu item if it exists */
				if (rob_event->klasse == ROB_EVENT_SELECT)
				{
					manage_intro_screen_increment (menuitemid %
						INTRO_SCREEN_MENU_ITEM_COUNT);
				}
			}
		}
	}
}

/***************************************************************************
 * Help Screen Event Processor                                             *
 ***************************************************************************/
/* This receives events from the ROB engine for the hotspots */

void help_screen_event_processor (ROB_Event * rob_event)
{
	int selecteditem;

	/* We're only interested in Help Screen hotspots */
	if (rob_event->rob_object->id > HOTSPOTS_ID &&
		rob_event->rob_object->id < HOTSPOTS_ID + 1 + 3)
	{
		/* We're only interested in click events */
		if (rob_event->klasse == ROB_EVENT_CLICK)
		{
			/* Calculate the selecteditem from the object id */
			selecteditem = rob_event->rob_object->id - 1 - HOTSPOTS_ID;
			
			#ifdef DEBUG_ROB_HOTSPOTS
				ROB_ShowEvent (rob_event, __func__);
				printf ("%i: selecteditem=%i\n", cycle_count, selecteditem);
			#endif
			
			manage_help_select (selecteditem);
		}
	}
}

/***************************************************************************
 * Options Screen Event Processor                                          *
 ***************************************************************************/
/* This receives events from the ROB engine for the hotspots.
 * 
 * I've just finished this function (20090405) and even though I put a
 * lot of effort into the design and preparation of the system, I wasn't
 * expecting it to result in something so simple! I am pleasantly surprised :) */

void options_screen_event_processor (ROB_Event * rob_event)
{
	int optionid, selected = FALSE;

	/* We're only interested in Options Screen hotspots */
	if (rob_event->rob_object->id > HOTSPOTS_ID &&
		rob_event->rob_object->id < HOTSPOTS_ID + 1 + OPTIONS_COUNT * 3)
	{
		/* We're only interested in click and select events */
		if (rob_event->klasse == ROB_EVENT_CLICK ||
			rob_event->klasse == ROB_EVENT_SELECT)
		{
			/* Calculate the raw optionid from the object id */
			optionid = rob_event->rob_object->id - 1 - HOTSPOTS_ID;
			
			#ifdef DEBUG_ROB_HOTSPOTS
				ROB_ShowEvent (rob_event, __func__);
				printf ("%i: optionid=%i\n", cycle_count, optionid);
			#endif
			
			if (optionid / OPTIONS_COUNT == 0)
			{
				/* This will be the textual part of an option */
				if (rob_event->klasse == ROB_EVENT_CLICK)
				{
					/* Records item's current selected state */
					if (optionspageselecteditem[optionspage] ==
						optionid % 13) selected = TRUE;
					/* Change the selected item to that which was clicked */
					optionspageselecteditem[optionspage] = optionid % 13;
					/* Single click to activate non-actions */
					if (optionid < OPTIONS_ACTION_UP ||
						optionid > OPTIONS_ACTION_PRIMARY_CLICK ||
						optionid % 13 >= OPTIONS_MENUPOS_LEFT)
					{
						manage_options_select (optionid);
					}
					else if (!selected)
					{
						/* For actions, the first click selects it to
						 * allow for resetting */
						options_screen.redraw |= REDRAW_INTERMEDIATE;
					}
					else
					{
						/* For actions, the second click activates the code */
						manage_options_select (optionid);
					}
				}
			}
			else if (optionid / OPTIONS_COUNT == 1)
			{
				/* This will be the "<" part of an option if it exists */
				if (rob_event->klasse == ROB_EVENT_SELECT)
				{
					manage_options_decrement (optionid % OPTIONS_COUNT);
				}
			}
			else if (optionid / OPTIONS_COUNT == 2)
			{
				/* This will be the ">" part of an option if it exists */
				if (rob_event->klasse == ROB_EVENT_SELECT)
				{
					manage_options_increment (optionid % OPTIONS_COUNT);
				}
			}
		}
	}
}

#ifdef PLATFORM_PSP
	/***************************************************************************
	 * Exit Callback                                                           *
	 ***************************************************************************/
int
exit_callback (int arg1, int arg2, void *common)
{
  sceKernelExitGame ();
  return 0;
}

	/***************************************************************************
	 * Power Callback                                                          *
	 ***************************************************************************/
int
power_callback (int unknown, int pwrflags, void *common)
{
  if (pwrflags & PSP_POWER_CB_POWER_SWITCH)
    {
      if (font)
	TTF_CloseFont (font);
    }
  else if (pwrflags & PSP_POWER_CB_RESUME_COMPLETE)
    {
      load_TTF_font ();
    }

  return 0;
}

	/***************************************************************************
	 * Callback Thread                                                         *
	 ***************************************************************************/
static int
callback_thread (SceSize args, void *argp)
{
  int cbid;

  cbid = sceKernelCreateCallback ("exit callback", exit_callback, NULL);
  sceKernelRegisterExitCallback (cbid);

  cbid = sceKernelCreateCallback ("power callback", power_callback, NULL);
  scePowerRegisterCallback (0, cbid);

  sceKernelSleepThreadCB ();
  return 0;
}

	/***************************************************************************
	 * Setup Callbacks                                                         *
	 ***************************************************************************/
static int
setup_callbacks (void)
{
  int thid = 0;

  thid =
    sceKernelCreateThread ("update_thread", callback_thread, 0x11, 0xFA0,
			   PSP_THREAD_ATTR_USER, 0);
  if (thid >= 0)
    {
      sceKernelStartThread (thid, 0, 0);
    }

  return thid;
}
#endif
