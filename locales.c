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

#include "game.h"

#ifdef PLATFORM_PSVITA
#include "psp2/io/dirent.h"
#endif

/* Defines */
/*
#define DEBUG_LOAD_SELECTED_LOCALE
#define DEBUG_READ_LOCALERC_FILE
#define DEBUG_SET_LOCALE_TO_LANG
*/

#define LOCALES_DIR "locales"
#define LOCALERC_FILE "localerc"
#define LOCALE_TEXT_FILE "text"


/* Variables */
extern char *data_state[];

/* Function prototypes */



/***************************************************************************
 * Find All Locales                                                        *
 ***************************************************************************/
/* Searches PACKAGE_DATA_DIR LOCALES_DIR for locale folders.
   On exit: returns 1 on error */

int
find_all_locales (void)
{
#ifdef PLATFORM_PSVITA
  FILE *fp;
  char filename[256];
  int dir;
  int default_locale_found = FALSE;
  SceIoDirent entry;

  if ((dir = sceIoDopen(PACKAGE_DATA_DIR "/" LOCALES_DIR)) == 0)
  {
    printf ("Couldn't open locales folder: %s\n",
           PACKAGE_DATA_DIR "/" LOCALES_DIR);
    return 1;
  }

  while (sceIoDread(dir, &entry) > 0)
  {
    if ((strcmp (entry.d_name, "."))
     && (strcmp (entry.d_name, "..")))
    {
      if (locale_count < MAX_LOCALES)
      {
        /* Attempt to open currentdirent->d_name/localerc to verify it's a valid locale folder  */
        strcpy (filename, PACKAGE_DATA_DIR "/" LOCALES_DIR "/");
        strcat (filename, entry.d_name);
        strcat (filename, "/");
        strcat (filename, LOCALERC_FILE);
        if ((fp = fopen (filename, "r")) != NULL)
        {
          if (!strcmp (entry.d_name, DEFAULT_LOCALE))
          {
            default_locale_found = TRUE;
          }
          else
          {
            strncpy (locales[locale_count].foldername,
                     entry.d_name, 100);
            locales[locale_count].foldername[99] = 0; /* Just in case we read all chars */
            locale_count++;
          }
          fclose (fp);
        }
      }
    }
  }
  sceIoDclose(dir);

  if (!default_locale_found)
  {
    fprintf (stdout, "Cannot find the default locale: %s\n",
             PACKAGE_DATA_DIR "/" LOCALES_DIR "/" DEFAULT_LOCALE);
    return 1;
  }
#else
  FILE *fp;
  char filename[256];
  DIR *dir;
  struct dirent *currentdirent;
  int default_locale_found = FALSE;

  /* Open the locales directory */
  if ((dir = opendir (PACKAGE_DATA_DIR "/" LOCALES_DIR)) == NULL)
    {
      fprintf (stdout, "Couldn't open locales folder: %s\n",
	       PACKAGE_DATA_DIR "/" LOCALES_DIR);
      return 1;
    }

  /* Now search the directory for locale folders */
  while ((currentdirent = readdir (dir)) != NULL)
    {
      if ((strcmp (currentdirent->d_name, "."))
	  && (strcmp (currentdirent->d_name, "..")))
	{
	  if (locale_count < MAX_LOCALES)
	    {
	      /* Attempt to open currentdirent->d_name/localerc to verify it's a valid locale folder  */
	      strcpy (filename, PACKAGE_DATA_DIR "/" LOCALES_DIR "/");
	      strcat (filename, currentdirent->d_name);
	      strcat (filename, "/");
	      strcat (filename, LOCALERC_FILE);
	      if ((fp = fopen (filename, "r")) != NULL)
		{
		  if (!strcmp (currentdirent->d_name, DEFAULT_LOCALE))
		    {
		      default_locale_found = TRUE;
		    }
		  else
		    {
		      strncpy (locales[locale_count].foldername,
			       currentdirent->d_name, 100);
		      locales[locale_count].foldername[99] = 0;	/* Just in case we read all chars */
		      locale_count++;
		    }
		  fclose (fp);
		}
	    }
	}
    }
  closedir (dir);

  if (!default_locale_found)
    {
      fprintf (stdout, "Cannot find the default locale: %s\n",
	       PACKAGE_DATA_DIR "/" LOCALES_DIR "/" DEFAULT_LOCALE);
      return 1;
    }
  else
#endif
    {
      return 0;
    }
}

/***************************************************************************
 * Sort Locales                                                            *
 ***************************************************************************/
/* Sorts found locales alphanumerically by name remembering to preserve the selected locale :)
   This must be done last after find_all_locales(), read_localerc_files() and read_resource_file() */

void
sort_locales (void)
{
  struct locale swap_locale;
  int swapped, count;

  /* Bubble sort */
  do
    {
      swapped = FALSE;
      for (count = 0; count < locale_count - 1; count++)
	{
	  if (strcmp (locales[count].name, locales[count + 1].name) > 0)
	    {
	      swapped = TRUE;

	      swap_locale = locales[count + 1];
	      locales[count + 1] = locales[count];
	      locales[count] = swap_locale;

	      /* Preserve selected locale */
	      if (selected_locale == count + 1)
		{
		  selected_locale = count;
		}
	      else if (selected_locale == count)
		{
		  selected_locale = count + 1;
		}
	    }
	}
    }
  while (swapped);
}

/***************************************************************************
 * Read localerc Files                                                     *
 ***************************************************************************/
/* Reads all localerc files and populates the locale list. At the time this is
   called, we already know that the default locale has been found so nothing
   here is fatal. Anything not found will be set to SINGLE_SPACE */

void
read_localerc_files (void)
{
  FILE *fp;
  char filename[256], line[1024];	/* 1024 because if fgets returns a partial line then line_count will be incremented again */
  int count, count2, line_count = 0;
  int name_read = DATA_UNREAD;
  int author_read = DATA_UNREAD;

#ifdef DEBUG_READ_LOCALERC_FILE
  printf ("*** Start %s ***\n", __func__);
#endif

  for (count = 0; count < locale_count; count++)
    {
      /* Reset these because we'll be using them more than once */
      line_count = 0;
      name_read = DATA_UNREAD;
      author_read = DATA_UNREAD;
      /* Initialise everything to some defaults */
      strcpy (locales[count].name, SINGLE_SPACE);
      strcpy (locales[count].author, SINGLE_SPACE);
      /* Attempt to open the localerc file */
      strcpy (filename, PACKAGE_DATA_DIR "/" LOCALES_DIR "/");
      strcat (filename, locales[count].foldername);
      strcat (filename, "/");
      strcat (filename, LOCALERC_FILE);
      if ((fp = fopen (filename, "r")) == NULL)
	{
	  fprintf (stdout, "Cannot read from file %s\n", filename);
	}
      else
	{
	  /* Read lines including the [CR]LFs until we have what we need or reach the end of file (line == NULL).
	     We are looking for all of these things :-
	     [name]
	     [author] */
	  while ((fgets (line, 1024, fp)) != NULL)
	    {
	      line_count++;
	      /* fgets will get [CR]LFs too so they should be removed */
	      for (count2 = strlen (line) - 1; count2 >= 0; count2--)
		if (line[count2] == 13 || line[count2] == 10)
		  line[count2] = 0;
	      /* Use the tags as switches so we know what to read */
	      /* NOTE: Reading consecutive (dataless) tags means that the previous tag may still be marked as DATA_READING */
	      if ((!strncmp (line, "[name]", 6)) ||
		  (!strncmp (line, "[author]", 8)))
		{
		  /* If a tag is being read and we read another tag then mark it as something else */
		  if (name_read == DATA_READING)
		    {
		      name_read = DATA_INCOMPLETE;
		    }
		  else if (author_read == DATA_READING)
		    {
		      author_read = DATA_INCOMPLETE;
		    }
		  /* If the found tag is DATA_UNREAD then mark it for DATA_READING */
		  if (!strncmp (line, "[name]", 6))
		    {
		      if (name_read == DATA_UNREAD)
			name_read = DATA_READING;
		    }
		  else if (!strncmp (line, "[author]", 8))
		    {
		      if (author_read == DATA_UNREAD)
			author_read = DATA_READING;
		    }
		}
	      else
		{
		  /* It's not a tag but data (or newline) so we need to find what to do with it */
		  if (name_read == DATA_READING)
		    {
		      if (strlen (line) != 0)
			{
			  strncpy (locales[count].name, line, 60);	/* Prevent overruns */
			  locales[count].name[59] = 0;	/* Just in case we read all chars */
			}
		      name_read = DATA_READ;
#ifdef DEBUG_READ_LOCALERC_FILE
		      printf ("%i: name=%s\n", line_count, line);
#endif
		    }
		  else if (author_read == DATA_READING)
		    {
		      if (strlen (line) != 0)
			{
			  strncpy (locales[count].author, line, 60);	/* Prevent overruns */
			  locales[count].author[59] = 0;	/* Just in case we read all chars */
			}
		      author_read = DATA_READ;
#ifdef DEBUG_READ_LOCALERC_FILE
		      printf ("%i: author=%s\n", line_count, line);
#endif
		    }
		}
	    }

	  /* Dump info to the console */
	  fprintf (stdout, "Locale found: %s\n", locales[count].foldername);

	  fclose (fp);

#ifdef DEBUG_READ_LOCALERC_FILE
	  printf ("name_read=%s\n", data_state[name_read]);
	  printf ("author_read=%s\n\n", data_state[author_read]);
#endif

	}
    }
#ifdef DEBUG_READ_LOCALERC_FILE
  printf ("*** Stop %s ***\n", __func__);
#endif
}

/***************************************************************************
 * Load Selected Locale                                                    *
 ***************************************************************************/
/* Loads all text data from the selected locale folder.
 * Now fixes all strings to the size TXT_MAX_COLS to make the code easier to
 * read and to make initialisation of the config_items less prone to errors.
 * We're only dealing with 6KB of memory so even if half of it's empty it
 * won't matter :)
 * 
 * This function :-
 * * Won't write further than the bounds of the array (strings will be
 * * chopped off)
 * * Can place one or more strings into a char array
 * * Will at least place a space into each destination so not to cause seg
 *   faults with SDL_ttf
 * * Records what was unread, read and partially read (incomplete) if you
 *   require this information */

void load_selected_locale (void)
{
	FILE *fp;
	/* line[1024] because if fgets returns a partial line then line_count
	 * will be incremented again */
	char filename[256], line[1024];
	int count, count2, line_count = 0, tag_found, tag_index, row = 0;
	#define CONFIG_ITEM_COUNT (6+7+59+6+2+5+5+41+49+1)
	struct config_item
	{
		char name[60];
		int state;
		/* Points to an array of chars which could hold more than one string */
		char *destination;
		/* 1 or more rows within this array of chars of fixed length TXT_MAX_COLS */
		int max_rows;
	};
	struct config_item config_items[CONFIG_ITEM_COUNT];

	/* Initialise the state and most common max_rows to reduce the
	 * initialisation code below */
	for (count = 0; count < CONFIG_ITEM_COUNT; count++)
	{
		config_items[count].state = DATA_UNREAD;
		config_items[count].max_rows = 1;
	}

	/* Initialise config_item struct with search and destination data */
	row = -1;
	strcpy (config_items[++row].name, "[txt_intro_story]");
	config_items[row].destination = txt_intro_story;
	config_items[row].max_rows = TXT_INTRO_STORY_ROWS;
	#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
		strcpy (config_items[++row].name, "[txt_intro_keys_PLATFORM_PC]");
		config_items[row].destination = txt_intro_keys;
		config_items[row].max_rows = TXT_INTRO_KEYS_ROWS;
	#elif defined(PLATFORM_GP2X) || defined(PLATFORM_CAANOO)
		strcpy (config_items[++row].name, "[txt_intro_keys_PLATFORM_GP2X]");
		config_items[row].destination = txt_intro_keys;
		config_items[row].max_rows = TXT_INTRO_KEYS_ROWS;
	#elif defined(PLATFORM_ZAURUS)
		strcpy (config_items[++row].name, "[txt_intro_keys_PLATFORM_ZAURUS]");
		config_items[row].destination = txt_intro_keys;
		config_items[row].max_rows = TXT_INTRO_KEYS_ROWS;
	#elif defined(PLATFORM_FREMANTLE)
		strcpy (config_items[++row].name, "[txt_intro_keys_PLATFORM_FREMANTLE]");
		config_items[row].destination = txt_intro_keys;
		config_items[row].max_rows = TXT_INTRO_KEYS_ROWS;
	#elif defined(PLATFORM_PSP)
		strcpy (config_items[++row].name, "[txt_intro_keys_PLATFORM_PSP]");
		config_items[row].destination = txt_intro_keys;
		config_items[row].max_rows = TXT_INTRO_KEYS_ROWS;
	#elif defined(PLATFORM_PSVITA)
		strcpy (config_items[++row].name, "[txt_intro_keys_PLATFORM_PSVITA]");
		config_items[row].destination = txt_intro_keys;
		config_items[row].max_rows = TXT_INTRO_KEYS_ROWS;
	#endif
	strcpy (config_items[++row].name, "[txt_intro_keys_description]");
	config_items[row].destination = txt_intro_keys_description;
	config_items[row].max_rows = TXT_INTRO_KEYS_DESCRIPTION_ROWS;
	strcpy (config_items[++row].name, "[txt_intro_menu]");
	config_items[row].destination = txt_intro_menu;
	config_items[row].max_rows = TXT_INTRO_MENU_ROWS;
	strcpy (config_items[++row].name, "[txt_of]");
	config_items[row].destination = txt_of;
	strcpy (config_items[++row].name, "[txt_intro_credits]");
	config_items[row].destination = txt_intro_credits;
	config_items[row].max_rows = TXT_INTRO_CREDITS_ROWS;

	strcpy (config_items[++row].name, "[txt_help_page0]");
	config_items[row].destination = txt_help_page0;
	config_items[row].max_rows = TXT_HELP_PAGE0_ROWS;
	strcpy (config_items[++row].name, "[txt_help_page1]");
	config_items[row].destination = txt_help_page1;
	config_items[row].max_rows = TXT_HELP_PAGE1_ROWS;
	strcpy (config_items[++row].name, "[txt_help_page2]");
	config_items[row].destination = txt_help_page2;
	config_items[row].max_rows = TXT_HELP_PAGE2_ROWS;
	strcpy (config_items[++row].name, "[txt_help_page3]");
	config_items[row].destination = txt_help_page3;
	config_items[row].max_rows = TXT_HELP_PAGE3_ROWS;
	strcpy (config_items[++row].name, "[txt_Back]");
	config_items[row].destination = txt_Back;
	strcpy (config_items[++row].name, "[txt_Next]");
	config_items[row].destination = txt_Next;
	strcpy (config_items[++row].name, "[txt_Exit]");
	config_items[row].destination = txt_Exit;

	strcpy (config_items[++row].name, "[txt_options_page1]");
	config_items[row].destination = txt_options_page1;
	config_items[row].max_rows = TXT_OPTIONS_PAGE1_ROWS;
	strcpy (config_items[++row].name, "[txt_Save]");
	config_items[row].destination = txt_Save;
	strcpy (config_items[++row].name, "[txt_Game_Speed]");
	config_items[row].destination = txt_Game_Speed;
	strcpy (config_items[++row].name, "[txt_Fast]");
	config_items[row].destination = txt_Fast;
	strcpy (config_items[++row].name, "[txt_Normal]");
	config_items[row].destination = txt_Normal;
	strcpy (config_items[++row].name, "[txt_Slow]");
	config_items[row].destination = txt_Slow;
	strcpy (config_items[++row].name, "[txt_Key_Repeat]");
	config_items[row].destination = txt_Key_Repeat;
	strcpy (config_items[++row].name, "[txt_Delay]");
	config_items[row].destination = txt_Delay;
	strcpy (config_items[++row].name, "[txt_Interval]");
	config_items[row].destination = txt_Interval;
	strcpy (config_items[++row].name, "[txt_Default_Joystick]");
	config_items[row].destination = txt_Default_Joystick;
	strcpy (config_items[++row].name, "[txt_No_joystick_found]");
	config_items[row].destination = txt_No_joystick_found;
	strcpy (config_items[++row].name, "[txt_Joystick_Axes_Dead_Zone]");
	config_items[row].destination = txt_Joystick_Axes_Dead_Zone;
	strcpy (config_items[++row].name, "[txt_Language]");
	config_items[row].destination = txt_Language;
	strcpy (config_items[++row].name, "[txt_Translation_by]");
	config_items[row].destination = txt_Translation_by;
	strcpy (config_items[++row].name, "[txt_Save_Frequency]");
	config_items[row].destination = txt_Save_Frequency;
	strcpy (config_items[++row].name, "[txt_On_Exit]");
	config_items[row].destination = txt_On_Exit;
	strcpy (config_items[++row].name, "[txt_On_Change]");
	config_items[row].destination = txt_On_Change;
	strcpy (config_items[++row].name, "[txt_Move_Up]");
	config_items[row].destination = txt_Move_Up;
	strcpy (config_items[++row].name, "[txt_Move_Up_Right]");
	config_items[row].destination = txt_Move_Up_Right;
	strcpy (config_items[++row].name, "[txt_Move_Right]");
	config_items[row].destination = txt_Move_Right;
	strcpy (config_items[++row].name, "[txt_Move_Down_Right]");
	config_items[row].destination = txt_Move_Down_Right;
	strcpy (config_items[++row].name, "[txt_Move_Down]");
	config_items[row].destination = txt_Move_Down;
	strcpy (config_items[++row].name, "[txt_Move_Down_Left]");
	config_items[row].destination = txt_Move_Down_Left;
	strcpy (config_items[++row].name, "[txt_Move_Left]");
	config_items[row].destination = txt_Move_Left;
	strcpy (config_items[++row].name, "[txt_Move_Up_Left]");
	config_items[row].destination = txt_Move_Up_Left;
	strcpy (config_items[++row].name, "[txt_Shoot_Up]");
	config_items[row].destination = txt_Shoot_Up;
	strcpy (config_items[++row].name, "[txt_Shoot_Down]");
	config_items[row].destination = txt_Shoot_Down;
	strcpy (config_items[++row].name, "[txt_Shoot_Left]");
	config_items[row].destination = txt_Shoot_Left;
	strcpy (config_items[++row].name, "[txt_Shoot_Right]");
	config_items[row].destination = txt_Shoot_Right;
	strcpy (config_items[++row].name, "[txt_Restart]");
	config_items[row].destination = txt_Restart;
	strcpy (config_items[++row].name, "[txt_Confirm]");
	config_items[row].destination = txt_Confirm;
	strcpy (config_items[++row].name, "[txt_Help]");
	config_items[row].destination = txt_Help;
	strcpy (config_items[++row].name, "[txt_Options]");
	config_items[row].destination = txt_Options;
	strcpy (config_items[++row].name, "[txt_Previous_Level]");
	config_items[row].destination = txt_Previous_Level;
	strcpy (config_items[++row].name, "[txt_Next_Level]");
	config_items[row].destination = txt_Next_Level;
	strcpy (config_items[++row].name, "[txt_Previous_Pack]");
	config_items[row].destination = txt_Previous_Pack;
	strcpy (config_items[++row].name, "[txt_Next_Pack]");
	config_items[row].destination = txt_Next_Pack;
	strcpy (config_items[++row].name, "[txt_Toggle_Fullscreen]");
	config_items[row].destination = txt_Toggle_Fullscreen;
	strcpy (config_items[++row].name, "[txt_Home]");
	config_items[row].destination = txt_Home;
	strcpy (config_items[++row].name, "[txt_End]");
	config_items[row].destination = txt_End;
	strcpy (config_items[++row].name, "[txt_Page_Up]");
	config_items[row].destination = txt_Page_Up;
	strcpy (config_items[++row].name, "[txt_Page_Down]");
	config_items[row].destination = txt_Page_Down;
	strcpy (config_items[++row].name, "[txt_Volume_Up]");
	config_items[row].destination = txt_Volume_Up;
	strcpy (config_items[++row].name, "[txt_Volume_Down]");
	config_items[row].destination = txt_Volume_Down;
	strcpy (config_items[++row].name, "[txt_Modifier]");
	config_items[row].destination = txt_Modifier;
	strcpy (config_items[++row].name, "[txt_Restore_Default_Controls]");
	config_items[row].destination = txt_Restore_Default_Controls;
	strcpy (config_items[++row].name, "[txt_Toggle_Designer]");
	config_items[row].destination = txt_Toggle_Designer;
	strcpy (config_items[++row].name, "[txt_Scroll_Up]");
	config_items[row].destination = txt_Scroll_Up;
	strcpy (config_items[++row].name, "[txt_Scroll_Down]");
	config_items[row].destination = txt_Scroll_Down;
	strcpy (config_items[++row].name, "[txt_Primary_Click]");
	config_items[row].destination = txt_Primary_Click;
	strcpy (config_items[++row].name, "[txt_System_Pointer");
	config_items[row].destination = txt_System_Pointer;
	strcpy (config_items[++row].name, "[txt_Disabled]");
	config_items[row].destination = txt_Disabled;
	strcpy (config_items[++row].name, "[txt_Enabled]");
	config_items[row].destination = txt_Enabled;
	strcpy (config_items[++row].name, "[txt_Pointer_Controls_Pad_Type]");
	config_items[row].destination = txt_Pointer_Controls_Pad_Type;
	strcpy (config_items[++row].name, "[txt_Viewport]");
	config_items[row].destination = txt_Viewport;
	strcpy (config_items[++row].name, "[txt_Screen]");
	config_items[row].destination = txt_Screen;
	strcpy (config_items[++row].name, "[txt_Sound]");
	config_items[row].destination = txt_Sound;
	strcpy (config_items[++row].name, "[txt_Sfx_Volume]");
	config_items[row].destination = txt_Sfx_Volume;
	strcpy (config_items[++row].name, "[txt_Simulated_Pointer]");
	config_items[row].destination = txt_Simulated_Pointer;

	strcpy (config_items[++row].name, "[txt_Key]");
	config_items[row].destination = txt_Key;
	strcpy (config_items[++row].name, "[txt_Joy]");
	config_items[row].destination = txt_Joy;
	strcpy (config_items[++row].name, "[txt_Ptr]");
	config_items[row].destination = txt_Ptr;
	strcpy (config_items[++row].name, "[txt_Mod]");
	config_items[row].destination = txt_Mod;
	strcpy (config_items[++row].name, "[txt_Skin]");
	config_items[row].destination = txt_Skin;
	strcpy (config_items[++row].name, "[txt_By]");
	config_items[row].destination = txt_By;

	strcpy (config_items[++row].name, "[txt_Level_Author]");
	config_items[row].destination = txt_Level_Author;
	strcpy (config_items[++row].name, "[txt_endscreen_congrats]");
	config_items[row].destination = txt_endscreen_congrats;
	config_items[row].max_rows = TXT_ENDSCREEN_CONGRATS_ROWS;

	strcpy (config_items[++row].name, "[txt_Volume]");
	config_items[row].destination = txt_Volume;
	strcpy (config_items[++row].name, "[txt_Changes_saved]");
	config_items[row].destination = txt_Changes_saved;
	strcpy (config_items[++row].name, "[txt_Default_controls_restored]");
	config_items[row].destination = txt_Default_controls_restored;
	strcpy (config_items[++row].name, "[txt_Press_something_and_release]");
	config_items[row].destination = txt_Press_something_and_release;
	strcpy (config_items[++row].name, "[txt_No_input_was_detected]");
	config_items[row].destination = txt_No_input_was_detected;

	strcpy (config_items[++row].name, "[txt_ptr_BtnLeft]");
	config_items[row].destination = txt_ptr_BtnLeft;
	strcpy (config_items[++row].name, "[txt_ptr_BtnMiddle]");
	config_items[row].destination = txt_ptr_BtnMiddle;
	strcpy (config_items[++row].name, "[txt_ptr_BtnRight]");
	config_items[row].destination = txt_ptr_BtnRight;
	strcpy (config_items[++row].name, "[txt_ptr_WheelUp]");
	config_items[row].destination = txt_ptr_WheelUp;
	strcpy (config_items[++row].name, "[txt_ptr_WheelDown]");
	config_items[row].destination = txt_ptr_WheelDown;

	strcpy (config_items[++row].name, "[txt_key_Backspace]");
	config_items[row].destination = txt_key_Backspace;
	strcpy (config_items[++row].name, "[txt_key_Tab]");
	config_items[row].destination = txt_key_Tab;
	strcpy (config_items[++row].name, "[txt_key_Clear]");
	config_items[row].destination = txt_key_Clear;
	strcpy (config_items[++row].name, "[txt_key_Return]");
	config_items[row].destination = txt_key_Return;
	strcpy (config_items[++row].name, "[txt_key_Pause]");
	config_items[row].destination = txt_key_Pause;
	strcpy (config_items[++row].name, "[txt_key_Escape]");
	config_items[row].destination = txt_key_Escape;
	strcpy (config_items[++row].name, "[txt_key_Space]");
	config_items[row].destination = txt_key_Space;
	strcpy (config_items[++row].name, "[txt_key_Delete]");
	config_items[row].destination = txt_key_Delete;
	strcpy (config_items[++row].name, "[txt_key_KP]");
	config_items[row].destination = txt_key_KP;
	strcpy (config_items[++row].name, "[txt_key_Up]");
	config_items[row].destination = txt_key_Up;
	strcpy (config_items[++row].name, "[txt_key_Down]");
	config_items[row].destination = txt_key_Down;
	strcpy (config_items[++row].name, "[txt_key_Right]");
	config_items[row].destination = txt_key_Right;
	strcpy (config_items[++row].name, "[txt_key_Left]");
	config_items[row].destination = txt_key_Left;
	strcpy (config_items[++row].name, "[txt_key_Insert]");
	config_items[row].destination = txt_key_Insert;
	strcpy (config_items[++row].name, "[txt_key_Home]");
	config_items[row].destination = txt_key_Home;
	strcpy (config_items[++row].name, "[txt_key_End]");
	config_items[row].destination = txt_key_End;
	strcpy (config_items[++row].name, "[txt_key_PgUp]");
	config_items[row].destination = txt_key_PgUp;
	strcpy (config_items[++row].name, "[txt_key_PgDn]");
	config_items[row].destination = txt_key_PgDn;
	strcpy (config_items[++row].name, "[txt_key_NumLk]");
	config_items[row].destination = txt_key_NumLk;
	strcpy (config_items[++row].name, "[txt_key_CapsLk]");
	config_items[row].destination = txt_key_CapsLk;
	strcpy (config_items[++row].name, "[txt_key_ScrlLk]");
	config_items[row].destination = txt_key_ScrlLk;
	strcpy (config_items[++row].name, "[txt_key_RShift]");
	config_items[row].destination = txt_key_RShift;
	strcpy (config_items[++row].name, "[txt_key_LShift]");
	config_items[row].destination = txt_key_LShift;
	strcpy (config_items[++row].name, "[txt_key_RCtrl]");
	config_items[row].destination = txt_key_RCtrl;
	strcpy (config_items[++row].name, "[txt_key_LCtrl]");
	config_items[row].destination = txt_key_LCtrl;
	strcpy (config_items[++row].name, "[txt_key_RAlt]");
	config_items[row].destination = txt_key_RAlt;
	strcpy (config_items[++row].name, "[txt_key_LAlt]");
	config_items[row].destination = txt_key_LAlt;
	strcpy (config_items[++row].name, "[txt_key_RMeta]");
	config_items[row].destination = txt_key_RMeta;
	strcpy (config_items[++row].name, "[txt_key_LMeta]");
	config_items[row].destination = txt_key_LMeta;
	strcpy (config_items[++row].name, "[txt_key_LSuper]");
	config_items[row].destination = txt_key_LSuper;
	strcpy (config_items[++row].name, "[txt_key_RSuper]");
	config_items[row].destination = txt_key_RSuper;

	#if defined(PLATFORM_FREMANTLE)
		strcpy (config_items[++row].name, "[txt_key_AltGr_PLATFORM_FREMANTLE]");
		config_items[row].destination = txt_key_AltGr;
	#else
		strcpy (config_items[++row].name, "[txt_key_AltGr]");
		config_items[row].destination = txt_key_AltGr;
	#endif

	strcpy (config_items[++row].name, "[txt_key_Compose]");
	config_items[row].destination = txt_key_Compose;
	strcpy (config_items[++row].name, "[txt_key_Help]");
	config_items[row].destination = txt_key_Help;
	strcpy (config_items[++row].name, "[txt_key_PrScr]");
	config_items[row].destination = txt_key_PrScr;
	strcpy (config_items[++row].name, "[txt_key_SysRq]");
	config_items[row].destination = txt_key_SysRq;
	strcpy (config_items[++row].name, "[txt_key_Break]");
	config_items[row].destination = txt_key_Break;
	strcpy (config_items[++row].name, "[txt_key_Menu]");
	config_items[row].destination = txt_key_Menu;
	strcpy (config_items[++row].name, "[txt_key_Power]");
	config_items[row].destination = txt_key_Power;
	strcpy (config_items[++row].name, "[txt_key_Euro]");
	config_items[row].destination = txt_key_Euro;
	strcpy (config_items[++row].name, "[txt_key_Undo]");
	config_items[row].destination = txt_key_Undo;

	strcpy (config_items[++row].name, "[txt_konstruktor_Ground]");
	config_items[row].destination = txt_konstruktor_Ground;
	strcpy (config_items[++row].name, "[txt_konstruktor_Stop]");
	config_items[row].destination = txt_konstruktor_Stop;
	strcpy (config_items[++row].name, "[txt_konstruktor_Radioactive_Field]");
	config_items[row].destination = txt_konstruktor_Radioactive_Field;
	strcpy (config_items[++row].name, "[txt_konstruktor_Push_Box]");
	config_items[row].destination = txt_konstruktor_Push_Box;
	strcpy (config_items[++row].name, "[txt_konstruktor_Capsule]");
	config_items[row].destination = txt_konstruktor_Capsule;
	strcpy (config_items[++row].name, "[txt_konstruktor_Bomb]");
	config_items[row].destination = txt_konstruktor_Bomb;
	strcpy (config_items[++row].name, "[txt_konstruktor_Bomb2]");
	config_items[row].destination = txt_konstruktor_Bomb2;
	strcpy (config_items[++row].name, "[txt_konstruktor_Door]");
	config_items[row].destination = txt_konstruktor_Door;
	strcpy (config_items[++row].name, "[txt_konstruktor_Box]");
	config_items[row].destination = txt_konstruktor_Box;
	strcpy (config_items[++row].name, "[txt_konstruktor_Empty_Field]");
	config_items[row].destination = txt_konstruktor_Empty_Field;
	strcpy (config_items[++row].name, "[txt_konstruktor_Butterfly]");
	config_items[row].destination = txt_konstruktor_Butterfly;
	strcpy (config_items[++row].name, "[txt_konstruktor_Gun_Fire]");
	config_items[row].destination = txt_konstruktor_Gun_Fire;
	strcpy (config_items[++row].name, "[txt_konstruktor_Force_Field]");
	config_items[row].destination = txt_konstruktor_Force_Field;
	strcpy (config_items[++row].name, "[txt_konstruktor_Screw]");
	config_items[row].destination = txt_konstruktor_Screw;
	strcpy (config_items[++row].name, "[txt_konstruktor_Wall]");
	config_items[row].destination = txt_konstruktor_Wall;
	strcpy (config_items[++row].name, "[txt_konstruktor_Black_Wall]");
	config_items[row].destination = txt_konstruktor_Black_Wall;
	strcpy (config_items[++row].name, "[txt_konstruktor_Teleport]");
	config_items[row].destination = txt_konstruktor_Teleport;
	strcpy (config_items[++row].name, "[txt_konstruktor_Gun]");
	config_items[row].destination = txt_konstruktor_Gun;
	strcpy (config_items[++row].name, "[txt_konstruktor_Magnet]");
	config_items[row].destination = txt_konstruktor_Magnet;
	strcpy (config_items[++row].name, "[txt_konstruktor_Bear]");
	config_items[row].destination = txt_konstruktor_Bear;
	strcpy (config_items[++row].name, "[txt_konstruktor_Black_Bear]");
	config_items[row].destination = txt_konstruktor_Black_Bear;
	strcpy (config_items[++row].name, "[txt_konstruktor_Bird]");
	config_items[row].destination = txt_konstruktor_Bird;
	strcpy (config_items[++row].name, "[txt_konstruktor_Key]");
	config_items[row].destination = txt_konstruktor_Key;
	strcpy (config_items[++row].name, "[txt_konstruktor_Bullets]");
	config_items[row].destination = txt_konstruktor_Bullets;
	strcpy (config_items[++row].name, "[txt_konstruktor_on_map]");
	config_items[row].destination = txt_konstruktor_on_map;
	strcpy (config_items[++row].name, "[txt_konstruktor_Laser_Gun]");
	config_items[row].destination = txt_konstruktor_Laser_Gun;
	strcpy (config_items[++row].name, "[txt_konstruktor_Blaster_Gun]");
	config_items[row].destination = txt_konstruktor_Blaster_Gun;
	strcpy (config_items[++row].name, "[txt_konstruktor_Regular_Gun]");
	config_items[row].destination = txt_konstruktor_Regular_Gun;
	strcpy (config_items[++row].name, "[txt_konstruktor_Fixed]");
	config_items[row].destination = txt_konstruktor_Fixed;
	strcpy (config_items[++row].name, "[txt_konstruktor_Moveable]");
	config_items[row].destination = txt_konstruktor_Moveable;
	strcpy (config_items[++row].name, "[txt_konstruktor_Rotating]");
	config_items[row].destination = txt_konstruktor_Rotating;
	strcpy (config_items[++row].name, "[txt_konstruktor_Moveable_Rotating]");
	config_items[row].destination = txt_konstruktor_Moveable_Rotating;
	strcpy (config_items[++row].name, "[txt_konstruktor_Direction_right]");
	config_items[row].destination = txt_konstruktor_Direction_right;
	strcpy (config_items[++row].name, "[txt_konstruktor_Direction_down]");
	config_items[row].destination = txt_konstruktor_Direction_down;
	strcpy (config_items[++row].name, "[txt_konstruktor_Direction_left]");
	config_items[row].destination = txt_konstruktor_Direction_left;
	strcpy (config_items[++row].name, "[txt_konstruktor_Direction_up]");
	config_items[row].destination = txt_konstruktor_Direction_up;
	strcpy (config_items[++row].name, "[txt_konstruktor_Welcome_to_GNU_Robbo_Designer]");
	config_items[row].destination = txt_konstruktor_Welcome_to_GNU_Robbo_Designer;
	strcpy (config_items[++row].name, "[txt_konstruktor_Exit_designer_select_again_to_exit]");
	config_items[row].destination = txt_konstruktor_Exit_designer_select_again_to_exit;
	strcpy (config_items[++row].name, "[txt_konstruktor_Reload_level_select_again_to_reload]");
	config_items[row].destination = txt_konstruktor_Reload_level_select_again_to_reload;
	strcpy (config_items[++row].name, "[txt_konstruktor_Level_reloaded]");
	config_items[row].destination = txt_konstruktor_Level_reloaded;
	strcpy (config_items[++row].name, "[txt_konstruktor_New_level_select_again_to_create]");
	config_items[row].destination = txt_konstruktor_New_level_select_again_to_create;
	strcpy (config_items[++row].name, "[txt_konstruktor_Level_created]");
	config_items[row].destination = txt_konstruktor_Level_created;
	strcpy (config_items[++row].name, "[txt_konstruktor_Save_select_again_to_save_new_level]");
	config_items[row].destination = txt_konstruktor_Save_select_again_to_save_new_level;
	strcpy (config_items[++row].name, "[txt_konstruktor_New_level_appended_to_pack]");
	config_items[row].destination = txt_konstruktor_New_level_appended_to_pack;
	strcpy (config_items[++row].name, "[txt_konstruktor_Clear_level_select_again_to_clear]");
	config_items[row].destination = txt_konstruktor_Clear_level_select_again_to_clear;
	strcpy (config_items[++row].name, "[txt_konstruktor_Level_cleared]");
	config_items[row].destination = txt_konstruktor_Level_cleared;
	strcpy (config_items[++row].name, "[txt_konstruktor_Save_select_again_to_save_changes]");
	config_items[row].destination = txt_konstruktor_Save_select_again_to_save_changes;
	strcpy (config_items[++row].name, "[txt_konstruktor_Changes_saved]");
	config_items[row].destination = txt_konstruktor_Changes_saved;
	strcpy (config_items[++row].name, "[txt_konstruktor_Level_resized_to]");
	config_items[row].destination = txt_konstruktor_Level_resized_to;
	strcpy(config_items[++row].name, "[txt_konstruktor_screws]");
	config_items[row].destination = txt_konstruktor_Screws;
	#ifdef DEBUG_LOAD_SELECTED_LOCALE
		printf ("*** Start %s ***\n", __func__);
	#endif

	/* Initialise all the config_item destination strings to SINGLE_SPACE */
	for (count = 0; count < CONFIG_ITEM_COUNT; count++)
	{
		for (row = 0; row < config_items[count].max_rows; row++)
		{
			strcpy (&(config_items[count].destination)
				[TXT_MAX_COLS * row], SINGLE_SPACE);
			#ifdef DEBUG_LOAD_SELECTED_LOCALE
				printf ("%s: %p=\"%s\"\n", config_items[count].name,
					&(config_items[count].destination)[TXT_MAX_COLS * row],
					&(config_items[count].destination[TXT_MAX_COLS * row]));
			#endif
		}
	}
	#ifdef DEBUG_LOAD_SELECTED_LOCALE
		printf ("\n");
	#endif

	/* Attempt to open the text file */
	strcpy (filename, PACKAGE_DATA_DIR "/" LOCALES_DIR "/");
	strcat (filename, locales[selected_locale].foldername);
	strcat (filename, "/");
	strcat (filename, LOCALE_TEXT_FILE);
	if ((fp = fopen (filename, "r")) == NULL)
	{
		fprintf (stdout, "Cannot read from file %s\n", filename);
	}
	else
	{
		/* Read lines including the [CR]LFs until we have what we need
		 * or reach the end of file (line == NULL) */
		while ((fgets (line, 1024, fp)) != NULL)
		{
			line_count++;
			/* fgets will get [CR]LFs too so they should be removed */
			for (count2 = strlen (line) - 1; count2 >= 0; count2--)
				if (line[count2] == 13 || line[count2] == 10)
					line[count2] = 0;
			/* Are we reading a tag? */
			tag_found = FALSE;
			for (tag_index = 0; tag_index < CONFIG_ITEM_COUNT; tag_index++)
			{
				if (!strncmp (line, config_items[tag_index].name,
					strlen (config_items[tag_index].name)))
				{
					tag_found = TRUE;
					break;
				}
			}
			if (tag_found)
			{
				/* If a tag is being read and we read another tag
				 * then mark it as something else */
				for (count = 0; count < CONFIG_ITEM_COUNT; count++)
				{
					if (config_items[count].state == DATA_READING)
						config_items[count].state = DATA_INCOMPLETE;
				}
				/* If the found tag is DATA_UNREAD then mark it for DATA_READING */
				if (config_items[tag_index].state == DATA_UNREAD)
				{
					config_items[tag_index].state = DATA_READING;
					row = 0;
				}
			}
			else
			{
				/* It's not a tag but data (or newline) so we need
				 * to find what to do with it */
				for (count = 0; count < CONFIG_ITEM_COUNT; count++)
				{
					if (config_items[count].state == DATA_READING)
					{
						if (strlen (line) > 0)
						{	/* We've already set the strings to
							* SINGLE_SPACE, don't empty them here */
							strncpy (&(config_items[count].destination)
								[TXT_MAX_COLS * row], line, TXT_MAX_COLS);	/* Prevent overruns */
							config_items[count].destination[TXT_MAX_COLS *
								(row + 1) - 1] = 0;	/* Just in case we read all chars */
						}
						#ifdef DEBUG_LOAD_SELECTED_LOCALE
							printf ("%s: %p=\"%s\"\n", config_items[count].name,
								&(config_items[count].destination)[TXT_MAX_COLS * row],
								&(config_items[count].destination[TXT_MAX_COLS * row]));
						#endif
						if (++row == config_items[count].max_rows)
							config_items[count].state = DATA_READ;
						break;
					}
				}
			}
		}
		fclose (fp);

		#ifdef DEBUG_LOAD_SELECTED_LOCALE
			printf ("\n");
			for (count = 0; count < CONFIG_ITEM_COUNT; count++)
			{
				printf ("%s=%s\n", config_items[count].name,
					data_state[config_items[count].state]);
			}
			printf ("\n");
		#endif
	}

	#ifdef DEBUG_LOAD_SELECTED_LOCALE
		printf ("*** Stop %s ***\n", __func__);
	#endif
}

/***************************************************************************
 * Set Locale to LANG                                                      *
 ***************************************************************************/
/* This will attempt to set the default locale to something equivalent to
 * whatever is present within the LANG environment variable. This must be
 * called after read_localerc_files and before read_resource_file so that
 * if an rcfile is found and a locale is set within it then that will
 * override any default set here */

void set_locale_to_LANG(void) {
	int count, length = 0, found = FALSE;
	char *envlang = getenv("LANG");

	#ifdef DEBUG_SET_LOCALE_TO_LANG
		printf("%s: locales[selected_locale].foldername=%s\n", __func__,
			locales[selected_locale].foldername);
		printf("%s: LANG=%s\n", __func__, envlang);
	#endif

	/* On my GP2X getenv("LANG") returns NULL and using any string
	 * functions on NULL will cause a segfault. Actually this is an
	 * issue for any platform and really the return value from getenv
	 * should always be checked for NULL before using it in any string
	 * functions */

	if (envlang != NULL) length = strlen(envlang);

	if (length >= 5) {
		length = 5;	/* en_GB is 5 chars */
	} else if (length >= 2) {
		length = 2;	/* en is 2 chars */
	} else {
		length = 0;
	}
	
	while (!found && length) {
		for (count = 0; count < locale_count; count++) {
			if (!strncmp(locales[count].foldername, envlang, length)) {
				found = TRUE;
				selected_locale = count;
				#ifdef DEBUG_SET_LOCALE_TO_LANG
					printf("%s: A %i-char match was found\n", __func__, length);
				#endif
			}
		}
		if (length > 2) {
			length = 2;
		} else {
			length = 0;
		}
	}

	#ifdef DEBUG_SET_LOCALE_TO_LANG
		printf("%s: locales[selected_locale].foldername=%s\n", __func__,
			locales[selected_locale].foldername);
		if (!found) printf("%s: No match was found\n", __func__);
	#endif
}



