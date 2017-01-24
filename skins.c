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
#define DEBUG_READ_SKIN_LEVEL_COLOUR_OVERRIDE
#define DEBUG_READ_SKINRC_FILE
#define DEBUG_READ_SKIN_PROJECT_COLOURS
*/

#define SKINS_DIR "skins"
#define SKINRC_FILE "skinrc"
#ifdef PLATFORM_ZAURUS
  #define IMG_ICONS32 "icons32.bmp"
  #define IMG_ICONS16 "icons16.bmp"
  #define IMG_KICONS32 "k_icons32.bmp"
  #define IMG_KICONS16 "k_icons16.bmp"
  #define IMG_BGRD32 "background32.bmp"
  #define IMG_BGRD16 "background16.bmp"
#else
  #define IMG_EFX32   "EFX32.png"    
  #define IMG_EFX16   "EFX16.png"    
    
  #define IMG_ICONS32   "icons32.png"
  #define IMG_ICONS16    "icons16.png"
  #define IMG_KICONS32	"k_icons32.png"
  #define IMG_KICONS16	"k_icons16.png"
  #define IMG_BGRD32	"background32.png"
  #define IMG_BGRD16	"background16.png"
#endif
#define IMG_CIPHERS16 "ciphers16.bmp"
#define IMG_CIPHERS32 "ciphers32.bmp"
#define IMG_ALPHA16 "alpha16.bmp"
#define IMG_ALPHA32 "alpha32.bmp"
#define TTF_ROBBO "robbo.ttf"
#define IMG_FONT_LOWRES "font12x16.bmp"
#define IMG_FONT_HIRES "font24x32.bmp"
#define IMG_WM_ICON "wm_icon.bmp"

/* Variables */
char *data_state[] =
  { "DATA_UNREAD", "DATA_READING", "DATA_READ", "DATA_INCOMPLETE" };

/* Function prototypes */
void read_skin_project_colours (void);


/***************************************************************************
 * Find All Skins                                                          *
 ***************************************************************************/
/* Searches PACKAGE_DATA_DIR SKINS_DIR for skin folders.
   On exit: returns 1 on error */

int
find_all_skins (void)
{
#ifdef PLATFORM_PSVITA
  FILE *fp;
  char filename[256];
  int dir;
  int default_skin_found = FALSE;
  SceIoDirent entry;

  /* Open the skins directory */
  if ((dir = sceIoDopen (PACKAGE_DATA_DIR "/" SKINS_DIR)) <= 0)
    {
      fprintf (stdout, "Couldn't open skins folder: %s\n",
	       PACKAGE_DATA_DIR "/" SKINS_DIR);
      return 1;
    }

  /* Now search the directory for skin folders */
  while (sceIoDread(dir, &entry) > 0)
    {
      if ((strcmp (entry.d_name, "."))
	  && (strcmp (entry.d_name, "..")))
	{
	  if (skin_count < MAX_SKINS)
	    {
	      /* Attempt to open entry.d_name/skinrc to verify it's a valid skin folder  */
	      strcpy (filename, PACKAGE_DATA_DIR "/" SKINS_DIR "/");
	      strcat (filename, entry.d_name);
	      strcat (filename, "/");
	      strcat (filename, SKINRC_FILE);
	      if ((fp = fopen (filename, "r")) != NULL)
		{
                    if (!strcmp (entry.d_name, DEFAULT_SKIN))
		    {
		      default_skin_found = TRUE;
		    }
		  else
		    {
		      strncpy (skins[skin_count].foldername,
			       entry.d_name, 100);
		      skins[skin_count].foldername[99] = 0;	/* Just in case we read all chars */
		      skin_count++;
		    }
		  fclose (fp);
		}
	    }
	}
    }
  sceIoDclose(dir);

  if (!default_skin_found)
    {
      fprintf (stdout, "Cannot find the default skin: %s\n",
	       PACKAGE_DATA_DIR "/" SKINS_DIR "/" DEFAULT_SKIN);
      return 1;
    }
  else
#else
  FILE *fp;
  char filename[256];
  DIR *dir;
  struct dirent *currentdirent;
  int default_skin_found = FALSE;

  /* Open the skins directory */
  if ((dir = opendir (PACKAGE_DATA_DIR "/" SKINS_DIR)) == NULL)
    {
      fprintf (stdout, "Couldn't open skins folder: %s\n",
	       PACKAGE_DATA_DIR "/" SKINS_DIR);
      return 1;
    }

  /* Now search the directory for skin folders */
  while ((currentdirent = readdir (dir)) != NULL)
    {
      if ((strcmp (currentdirent->d_name, "."))
	  && (strcmp (currentdirent->d_name, "..")))
	{
	  if (skin_count < MAX_SKINS)
	    {
	      /* Attempt to open currentdirent->d_name/skinrc to verify it's a valid skin folder  */
	      strcpy (filename, PACKAGE_DATA_DIR "/" SKINS_DIR "/");
	      strcat (filename, currentdirent->d_name);
	      strcat (filename, "/");
	      strcat (filename, SKINRC_FILE);
	      if ((fp = fopen (filename, "r")) != NULL)
		{
                  #if defined(PLATFORM_PSP)
		  	if (!strcasecmp (currentdirent->d_name, DEFAULT_SKIN))
                  #else
		  	if (!strcmp (currentdirent->d_name, DEFAULT_SKIN))
                  #endif
		    {
		      default_skin_found = TRUE;
		    }
		  else
		    {
		      strncpy (skins[skin_count].foldername,
			       currentdirent->d_name, 100);
		      skins[skin_count].foldername[99] = 0;	/* Just in case we read all chars */
		      skin_count++;
		    }
		  fclose (fp);
		}
	    }
	}
    }
  closedir (dir);

  if (!default_skin_found)
    {
      fprintf (stdout, "Cannot find the default skin: %s\n",
	       PACKAGE_DATA_DIR "/" SKINS_DIR "/" DEFAULT_SKIN);
      return 1;
    }
  else
#endif
    {
      return 0;
    }
}

/***************************************************************************
 * Read skinrc Files                                                       *
 ***************************************************************************/
/* Reads all skinrc files and populates the skin list. At the time this is
   called, we already know that the default skin has been found so nothing
   here is fatal. Anything not found will be initialised with some defaults */

void
read_skinrc_files (void)
{
  FILE *fp;
  char filename[256], line[1024];	/* 1024 because if fgets returns a partial line then line_count will be incremented again */
  int count, count2, line_count = 0, row = 0;
  int value_read[1];
  int name_read = DATA_UNREAD;
  int author_read = DATA_UNREAD;
  int about_read = DATA_UNREAD;
  int project_colours_read = DATA_UNREAD;
  int help_tile_read = DATA_UNREAD;
  int soundskin_read = DATA_UNREAD;

#ifdef DEBUG_READ_SKINRC_FILE
  printf ("*** Start %s ***\n", __func__);
#endif

  for (count = 0; count < skin_count; count++)
    {
      /* Reset these because we'll be using them more than once */
      line_count = 0;
      name_read = DATA_UNREAD;
      author_read = DATA_UNREAD;
      about_read = DATA_UNREAD;
      project_colours_read = DATA_UNREAD;
      help_tile_read = DATA_UNREAD;
      soundskin_read = DATA_UNREAD;
      /* Initialise everything to some defaults */
      strcpy (skins[count].name, SINGLE_SPACE);
      strcpy (skins[count].author, SINGLE_SPACE);
      strcpy (skins[count].about[0], SINGLE_SPACE);
      strcpy (skins[count].about[1], SINGLE_SPACE);
      strcpy (skins[count].about[2], SINGLE_SPACE);
      strcpy (skins[count].soundskin, SINGLE_SPACE);

      skins[count].help_tile_colour = HELP_TILE_COLOUR;
      /* Attempt to open the skinrc file */
      strcpy (filename, PACKAGE_DATA_DIR "/" SKINS_DIR "/");
      strcat (filename, skins[count].foldername);
      strcat (filename, "/");
      strcat (filename, SKINRC_FILE);
      if ((fp = fopen (filename, "r")) == NULL)
	{
	  fprintf (stdout, "Cannot read from file %s\n", filename);
	}
      else
	{
	  /* Read lines including the [CR]LFs until we have what we need or reach the end of file (line == NULL).
	     We are looking for all of these things :-
	     [name]
	     [author]
	     [about]
	     [project_colours]
	     [help_tile]
	     [project_colours_end] */
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
		  (!strncmp (line, "[author]", 8)) ||
		  (!strncmp (line, "[about]", 7)) ||
		  (!strncmp (line, "[project_colours]", 17)) ||
		  (!strncmp (line, "[soundskin]", 11)) ||
		  (!strncmp (line, "[help_tile]", 11)
		   && project_colours_read != DATA_UNREAD))
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
		  else if (about_read == DATA_READING)
		    {
		      about_read = DATA_INCOMPLETE;
		    }
		  else if (project_colours_read == DATA_READING)
		    {
		      project_colours_read = DATA_READ;	/* A dataless tag */
		    }
		  else if (help_tile_read == DATA_READING)
		    {
		      help_tile_read = DATA_INCOMPLETE;
		    }
		  else if (soundskin_read == DATA_READING)
		    {
		      soundskin_read = DATA_INCOMPLETE;
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
		  else if (!strncmp (line, "[about]", 7))
		    {
		      if (about_read == DATA_UNREAD)
			{
			  about_read = DATA_READING;
			  row = 0;	/* This is a good place to reset variables before reading the data */
			}
		    }
		  else if (!strncmp (line, "[soundskin]", 10))
		    {
		      if (soundskin_read == DATA_UNREAD)
			soundskin_read = DATA_READING;
		    }
		  else if (!strncmp (line, "[project_colours]", 17))
		    {
		      if (project_colours_read == DATA_UNREAD)
			project_colours_read = DATA_READING;
		    }
		  else if (!strncmp (line, "[help_tile]", 11))
		    {
		      if (help_tile_read == DATA_UNREAD)
			help_tile_read = DATA_READING;
		    }
		  else if (!strncmp (line, "[project_colours_end]", 21))
		    {
		      break;	/* Stop reading the file now */
		    }
		}
	      else
		{
		  /* It's not a tag but data (or newline) so we need to find what to do with it */
		  if (name_read == DATA_READING)
		    {
		      if (strlen (line) != 0)
			{
			  strncpy (skins[count].name, line, 60);	/* Prevent overruns */
			  skins[count].name[59] = 0;	/* Just in case we read all chars */
			}
		      name_read = DATA_READ;
#ifdef DEBUG_READ_SKINRC_FILE
		      printf ("%i: name=%s\n", line_count, line);
#endif
		    }
		  else if (author_read == DATA_READING)
		    {
		      if (strlen (line) != 0)
			{
			  strncpy (skins[count].author, line, 60);	/* Prevent overruns */
			  skins[count].author[59] = 0;	/* Just in case we read all chars */
			}
		      author_read = DATA_READ;
#ifdef DEBUG_READ_SKINRC_FILE
		      printf ("%i: author=%s\n", line_count, line);
#endif
		    }
		  else if (about_read == DATA_READING)
		    {
		      /* row is used exclusively here whilst reading this data */
		      if (strlen (line) != 0)
			{
			  strncpy (skins[count].about[row], line, 60);	/* Prevent overruns */
			  skins[count].about[row][59] = 0;	/* Just in case we read all chars */
			}
#ifdef DEBUG_READ_SKINRC_FILE
		      printf ("%i: about[%i]=%s\n", line_count, row, line);
#endif
		      if (++row >= 3)
			about_read = DATA_READ;
		    }
		  else if (soundskin_read == DATA_READING)
		    {		/* sound skin file reading */
		      if (strlen (line) != 0)
			{
			  strncpy (skins[count].soundskin, line, 60);	/* Prevent overruns */
			  skins[count].soundskin[59] = 0;	/* just in case we read all chars */
			}
		    }
		  else if (help_tile_read == DATA_READING)
		    {
		      if (sscanf (line, "%x", &value_read[0]) == 1)
			{
			  skins[count].help_tile_colour = value_read[0];
			  help_tile_read = DATA_READ;
#ifdef DEBUG_READ_SKINRC_FILE
			  printf ("%i: help_tile_colour=0x%06X\n", line_count,
				  value_read[0]);
#endif
			}
		      else
			{
			  help_tile_read = DATA_INCOMPLETE;
			}
		    }
		}
	    }

	  /* Dump info to the console */
	  fprintf (stdout, "Skin found: %s\n", skins[count].name);

	  fclose (fp);

#ifdef DEBUG_READ_SKINRC_FILE
	  printf ("name_read=%s\n", data_state[name_read]);
	  printf ("author_read=%s\n", data_state[author_read]);
	  printf ("about_read=%s\n", data_state[about_read]);
	  printf ("project_colours_read=%s\n",
		  data_state[project_colours_read]);
	  printf ("help_tile_read=%s\n\n", data_state[help_tile_read]);
#endif

	}
    }
#ifdef DEBUG_READ_SKINRC_FILE
  printf ("*** Stop %s ***\n", __func__);
#endif
}

/***************************************************************************
 * Read Skin Project Colours                                               *
 ***************************************************************************/
/* Reads the projects colours from the skinrc file for the selected skin.
   All the colours will be preset with some defaults in case they are missing.

   This function is the final (for now) rc reader that is needed for this project
   and is an accumulation of experience from writing the others (the last being
   load_selected_locale). If I need to write another of these then I'd be close to
   having a universal rc reader which is what I could have done with from the outset :)
   The only thing this isn't doing is selecting a tag or group of tags that equal a
   certain value as in reading a record but it's not difficult to fix that */

void
read_skin_project_colours (void)
{
  FILE *fp;
  char filename[256], line[1024];	/* 1024 because if fgets returns a partial line then line_count will be incremented again */
  int count, count2, line_count = 0, tag_found, tag_index, row, quit = FALSE;
  Uint32 value_read;
#define CONFIG_ITEM_COUNT 18
#define CONFIG_ITEM_DATALESS 1	/* Only a tag, no following data */
#define CONFIG_ITEM_DEPENDENT 2	/* Dependent on another tag being DATA_READ first */
#define CONFIG_ITEM_QUIT_ON_READ 4	/* Quit once tag has been DATA_READ */
#define CONFIG_ITEM_TYPE_UINT32_HEX 8
#define CONFIG_ITEM_TYPE_STRING 16
  struct config_item
  {
    char name[60];
    int state;
    Uint32 *destination;	/* If we were reading other types too then this would have to be a void pointer */
    int flags;			/* An ORed combination of CONFIG_ITEM_* bits */
    int depends_on;		/* If CONFIG_ITEM_DEPENDENT then this is an index into this struct */
  };
  struct config_item config_items[CONFIG_ITEM_COUNT];

  /* Initialise the struct with some defaults and most common settings to reduce the initialisation code below */
  for (count = 0; count < CONFIG_ITEM_COUNT; count++)
    {
      config_items[count].state = DATA_UNREAD;
      config_items[count].flags =
	CONFIG_ITEM_DEPENDENT | CONFIG_ITEM_TYPE_UINT32_HEX;
      config_items[count].depends_on = 0;	/* Would normally default to UNDEFINED */
    }

  /* Initialise config_item struct with search and destination data */
  row = -1;
  strcpy (config_items[++row].name, "[project_colours]");
  config_items[row].flags = CONFIG_ITEM_DATALESS;
  strcpy (config_items[++row].name, "[background]");
  config_items[row].destination = &skins[selected_skin].background_colour;
  strcpy (config_items[++row].name, "[version_text]");
  config_items[row].destination = &skins[selected_skin].version_text_colour;
  strcpy (config_items[++row].name, "[general_text]");
  config_items[row].destination = &skins[selected_skin].general_text_colour;
  strcpy (config_items[++row].name, "[menu_text]");
  config_items[row].destination = &skins[selected_skin].menu_text_colour;
  strcpy (config_items[++row].name, "[menu_selected_text]");
  config_items[row].destination =
    &skins[selected_skin].menu_selected_text_colour;
  strcpy (config_items[++row].name, "[menu_selected_limit_text]");
  config_items[row].destination =
    &skins[selected_skin].menu_selected_limit_text_colour;
  strcpy (config_items[++row].name, "[menu_greyed_text]");
  config_items[row].destination =
    &skins[selected_skin].menu_greyed_text_colour;
  strcpy (config_items[++row].name, "[default_controls_text]");
  config_items[row].destination =
    &skins[selected_skin].default_controls_text_colour;
  strcpy (config_items[++row].name, "[credits_text]");
  config_items[row].destination = &skins[selected_skin].credits_text_colour;
  strcpy (config_items[++row].name, "[help_tile]");
  config_items[row].destination = &skins[selected_skin].help_tile_colour;
  strcpy (config_items[++row].name, "[fade]");
  config_items[row].destination = &skins[selected_skin].fade_colour;
  strcpy (config_items[++row].name, "[author_text]");
  config_items[row].destination = &skins[selected_skin].author_text_colour;
  strcpy (config_items[++row].name, "[designer_text]");
  config_items[row].destination = &skins[selected_skin].designer_text_colour;
  strcpy (config_items[++row].name, "[designer_GAMEOBJ_tile]");
  config_items[row].destination = &skins[selected_skin].designer_GAMEOBJ_tile_colour;
  strcpy (config_items[++row].name, "[designer_OBJS_tile]");
  config_items[row].destination = &skins[selected_skin].designer_OBJS_tile_colour;
  strcpy (config_items[++row].name, "[designer_OBJV_tile]");
  config_items[row].destination = &skins[selected_skin].designer_OBJV_tile_colour;
  strcpy (config_items[++row].name, "[project_colours_end]");
  config_items[row].flags =
    CONFIG_ITEM_DATALESS | CONFIG_ITEM_DEPENDENT | CONFIG_ITEM_QUIT_ON_READ;

#ifdef DEBUG_READ_SKIN_PROJECT_COLOURS
  printf ("*** Start %s ***\n", __func__);
#endif

  /* Set some default colours */
  skins[selected_skin].background_colour = BACKGROUND_COLOUR;
  skins[selected_skin].version_text_colour = VERSION_TEXT_COLOUR;
  skins[selected_skin].general_text_colour = GENERAL_TEXT_COLOUR;
  skins[selected_skin].menu_text_colour = MENU_TEXT_COLOUR;
  skins[selected_skin].menu_selected_text_colour = MENU_SELECTED_TEXT_COLOUR;
  skins[selected_skin].menu_selected_limit_text_colour =
    MENU_SELECTED_LIMIT_TEXT_COLOUR;
  skins[selected_skin].menu_greyed_text_colour = MENU_GREYED_TEXT_COLOUR;
  skins[selected_skin].default_controls_text_colour =
    DEFAULT_CONTROLS_TEXT_COLOUR;
  skins[selected_skin].credits_text_colour = CREDITS_TEXT_COLOUR;
  skins[selected_skin].help_tile_colour = HELP_TILE_COLOUR;
  skins[selected_skin].fade_colour = FADE_COLOUR;
  skins[selected_skin].author_text_colour = AUTHOR_TEXT_COLOUR;
  skins[selected_skin].designer_text_colour = DESIGNER_TEXT_COLOUR;
  skins[selected_skin].designer_GAMEOBJ_tile_colour = DESIGNER_GAMEOBJ_TILE_COLOUR;
  skins[selected_skin].designer_OBJS_tile_colour = DESIGNER_OBJS_TILE_COLOUR;
  skins[selected_skin].designer_OBJV_tile_colour = DESIGNER_OBJV_TILE_COLOUR;

  /* Attempt to open the skinrc file */
  strcpy (filename, PACKAGE_DATA_DIR "/" SKINS_DIR "/");
  strcat (filename, skins[selected_skin].foldername);
  strcat (filename, "/");
  strcat (filename, SKINRC_FILE);
  if ((fp = fopen (filename, "r")) == NULL)
    {
      fprintf (stdout, "Cannot read from file %s\n", filename);
    }
  else
    {
      /* Read lines including the [CR]LFs until we have what we need or reach the end of file (line == NULL) */
      while (((fgets (line, 1024, fp)) != NULL) && !quit)
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
	      if (!strncmp
		  (line, config_items[tag_index].name,
		   strlen (config_items[tag_index].name)))
		{
		  if ((!
		       (config_items[tag_index].flags &
			CONFIG_ITEM_DEPENDENT))
		      ||
		      ((config_items[tag_index].flags & CONFIG_ITEM_DEPENDENT)
		       && config_items[tag_index].depends_on >= 0
		       && config_items[config_items[tag_index].
				       depends_on].state == DATA_READ))
		    {
		      tag_found = TRUE;
		      break;
		    }
		}
	    }
	  if (tag_found)
	    {
	      /* If a tag is being read and we read another tag then mark it as DATA_INCOMPLETE */
	      for (count = 0; count < CONFIG_ITEM_COUNT; count++)
		{
		  if (config_items[count].state == DATA_READING)
		    {
		      if (config_items[count].flags & CONFIG_ITEM_DATALESS)
			{
			  config_items[count].state = DATA_READ;
			}
		      else
			{
			  config_items[count].state = DATA_INCOMPLETE;
			}
		    }
		}
	      /* If the found tag is DATA_UNREAD then mark it for DATA_READING */
	      if (config_items[tag_index].state == DATA_UNREAD)
		{
		  if (config_items[tag_index].flags & CONFIG_ITEM_DATALESS)
		    {
		      config_items[tag_index].state = DATA_READ;
		      if (config_items[tag_index].flags &
			  CONFIG_ITEM_QUIT_ON_READ)
			quit = TRUE;
		    }
		  else
		    {
		      config_items[tag_index].state = DATA_READING;
		    }
		}
	    }
	  else
	    {
	      /* It's not a tag but data (or newline) so we need to find what to do with it */
	      for (count = 0; count < CONFIG_ITEM_COUNT; count++)
		{
		  if (config_items[count].state == DATA_READING)
		    {
		      if (config_items[count].flags &
			  CONFIG_ITEM_TYPE_UINT32_HEX)
			{
			  if (sscanf (line, "%x", &value_read) == 1)
			    {
			      *config_items[count].destination = value_read;
			      config_items[count].state = DATA_READ;
#ifdef DEBUG_READ_SKIN_PROJECT_COLOURS
			      printf ("%s: %p=%08X\n",
				      config_items[count].name,
				      config_items[count].destination,
				      *config_items[count].destination);
#endif
			    }
			  else
			    {
			      config_items[count].state = DATA_INCOMPLETE;
			    }
			}
		      else if (config_items[count].flags &
			       CONFIG_ITEM_TYPE_STRING)
			{
			  /* For the future */
			}
		      if (config_items[count].state == DATA_READ
			  && (config_items[count].flags &
			      CONFIG_ITEM_QUIT_ON_READ))
			quit = TRUE;
		      break;
		    }
		}
	    }
	}

      fclose (fp);

#ifdef DEBUG_READ_SKIN_PROJECT_COLOURS
      printf ("\n");
      for (count = 0; count < CONFIG_ITEM_COUNT; count++)
	{
	  printf ("%s=%s\n", config_items[count].name,
		  data_state[config_items[count].state]);
	}
      printf ("\n");
#endif
    }

#ifdef DEBUG_READ_SKIN_PROJECT_COLOURS
  printf ("*** Stop %s ***\n", __func__);
#endif
}

/***************************************************************************
 * Sort Skins                                                              *
 ***************************************************************************/
/* Sorts found skins alphanumerically by name remembering to preserve the selected skin :)
   This must be done last after find_all_skins(), read_skinrc_files() and read_resource_file() */

void
sort_skins (void)
{
  struct skin swap_skin;
  int swapped, count;

  /* Bubble sort */
  do
    {
      swapped = FALSE;
      for (count = 0; count < skin_count - 1; count++)
	{
	  if (strcmp (skins[count].name, skins[count + 1].name) > 0)
	    {
	      swapped = TRUE;

	      swap_skin = skins[count + 1];
	      skins[count + 1] = skins[count];
	      skins[count] = swap_skin;

	      /* Preserve selected skin */
	      if (selected_skin == count + 1)
		{
		  selected_skin = count;
		}
	      else if (selected_skin == count)
		{
		  selected_skin = count + 1;
		}
	    }
	}
    }
  while (swapped);
}

/***************************************************************************
 * Read Skin Level Colour Override                                         *
 ***************************************************************************/
/* This will attempt to fetch a level colour override for the current pack/level
   from the selected skinrc.

	On exit: returns 0 if successful with level.colour_override set to the found colour
             returns 1 if not found with level.colour_override unchanged */

int
read_skin_level_colour_override (void)
{
  FILE *fp;
  char filename[256], line[1024];	/* 1024 because if fgets returns a partial line then line_count will be incremented again */
  int count, line_count = 0;
  int value_read[1];
  int level_colour_overrides_read = DATA_UNREAD;
  int default_level_colour_read = DATA_UNREAD;
  int level_pack_name_read = DATA_UNREAD;
  int level_read = DATA_UNREAD;
  int colour_read = DATA_UNREAD;

  /* Attempt to open the skinrc file */
  strcpy (filename, PACKAGE_DATA_DIR "/" SKINS_DIR "/");
  strcat (filename, skins[selected_skin].foldername);
  strcat (filename, "/");
  strcat (filename, SKINRC_FILE);
  if ((fp = fopen (filename, "r")) == NULL)
    {
      fprintf (stdout, "Cannot read from file: %s\n", filename);
      return TRUE;
    }

#ifdef DEBUG_READ_SKIN_LEVEL_COLOUR_OVERRIDE
  printf ("*** Start %s ***\n", __func__);
#endif

  /* Read lines including the [CR]LFs until we have what we need or reach the end of file (line == NULL).
     We are looking for some or all of these things :-
     [level_colour_overrides]
     [level_pack_name]
     [default_level_colour] - OPTIONAL
     [level] - OPTIONAL
     [colour] - may be missing
     [level_colour_overrides_end] */
  while ((fgets (line, 1024, fp)) != NULL)
    {
      line_count++;
      /* fgets will get [CR]LFs too so they should be removed */
      for (count = strlen (line) - 1; count >= 0; count--)
	if (line[count] == 13 || line[count] == 10)
	  line[count] = 0;
      /* Use the tags as switches so we know what to read */
      /* NOTE: Reading consecutive (dataless) tags means that the previous tag may still be marked as DATA_READING */
      if ((!strncmp (line, "[level_colour_overrides]", 24)) ||
	  (!strncmp (line, "[level_pack_name]", 17)
	   && level_colour_overrides_read != DATA_UNREAD)
	  || (!strncmp (line, "[default_level_colour]", 22)
	      && level_pack_name_read == DATA_READ)
	  || (!strncmp (line, "[level]", 7)
	      && level_pack_name_read == DATA_READ)
	  || (!strncmp (line, "[colour]", 8) && level_read == DATA_READ)
	  || (!strncmp (line, "[level_colour_overrides_end]", 28)
	      && level_read == DATA_READ))
	{
	  /* If a tag is being read and we read another tag then mark it as something else */
	  if (level_colour_overrides_read == DATA_READING)
	    {
	      level_colour_overrides_read = DATA_READ;	/* A dataless tag */
	    }
	  else if (default_level_colour_read == DATA_READING)
	    {
	      default_level_colour_read = DATA_INCOMPLETE;
	    }
	  else if (level_pack_name_read == DATA_READING)
	    {
	      level_pack_name_read = DATA_UNREAD;	/* May need to read another later */
	    }
	  else if (level_read == DATA_READING)
	    {
	      level_read = DATA_UNREAD;	/* May need to read another later */
	    }
	  else if (colour_read == DATA_READING)
	    {
	      colour_read = DATA_INCOMPLETE;
	    }
	  /* If the found tag is DATA_UNREAD then mark it for DATA_READING */
	  if (!strncmp (line, "[level_colour_overrides]", 24))
	    {
	      if (level_colour_overrides_read == DATA_UNREAD)
		level_colour_overrides_read = DATA_READING;
	    }
	  else if (!strncmp (line, "[level_pack_name]", 17))
	    {
	      if (level_pack_name_read == DATA_UNREAD)
		{
		  level_pack_name_read = DATA_READING;
		}
	      else if (level_pack_name_read == DATA_READ)
		{
		  break;	/* We've already read this; something is wrong */
		}
	    }
	  else if (!strncmp (line, "[default_level_colour]", 22))
	    {
	      if (default_level_colour_read == DATA_UNREAD)
		default_level_colour_read = DATA_READING;
	    }
	  else if (!strncmp (line, "[level]", 7))
	    {
	      if (level_read == DATA_UNREAD)
		{
		  level_read = DATA_READING;
		}
	      else if (level_read == DATA_READ)
		{
		  break;	/* We've already read this; something is wrong */
		}
	    }
	  else if (!strncmp (line, "[colour]", 8))
	    {
	      if (colour_read == DATA_UNREAD)
		colour_read = DATA_READING;
	    }
	  else if (!strncmp (line, "[level_colour_overrides_end]", 28))
	    {
	      break;		/* Stop reading the file now */
	    }
	}
      else
	{
	  /* It's not a tag but data (or newline) so we need to find what to do with it */
	  /* level_colour_overrides_read is not here because it has no data */
	  if (default_level_colour_read == DATA_READING)
	    {
	      if (sscanf (line, "%x", &value_read[0]) == 1)
		{
		  level.colour_override = value_read[0];
		  default_level_colour_read = DATA_READ;
#ifdef DEBUG_READ_SKIN_LEVEL_COLOUR_OVERRIDE
		  printf ("%i: default_level_colour=0x%06X\n", line_count,
			  value_read[0]);
#endif
		}
	      else
		{
		  default_level_colour_read = DATA_INCOMPLETE;
		}
	    }
	  else if (level_pack_name_read == DATA_READING)
	    {
	      if (!strcmp (level_packs[selected_pack].name, line))
		{
		  level_pack_name_read = DATA_READ;
#ifdef DEBUG_READ_SKIN_LEVEL_COLOUR_OVERRIDE
		  printf ("%i: level_pack_name=%s\n", line_count, line);
#endif
		}
	      else
		{
		  level_pack_name_read = DATA_UNREAD;	/* We need to keep reading these tags */
		}
	    }
	  else if (level_read == DATA_READING)
	    {
	      if (sscanf (line, "%i", &value_read[0]) == 1)
		{
		  if (value_read[0] ==
		      level_packs[selected_pack].level_selected)
		    {
		      level_read = DATA_READ;
#ifdef DEBUG_READ_SKIN_LEVEL_COLOUR_OVERRIDE
		      printf ("%i: level=%i\n", line_count, value_read[0]);
#endif
		    }
		  else
		    {
		      level_read = DATA_UNREAD;
		    }
		}
	      else
		{
		  level_read = DATA_UNREAD;	/* We need to keep reading these tags */
		}
	    }
	  else if (colour_read == DATA_READING)
	    {
	      if (sscanf (line, "%x", &value_read[0]) == 1)
		{
		  level.colour_override = value_read[0];
		  colour_read = DATA_READ;
#ifdef DEBUG_READ_SKIN_LEVEL_COLOUR_OVERRIDE
		  printf ("%i: colour=0x%06X\n", line_count, value_read[0]);
#endif
		}
	      else
		{
		  colour_read = DATA_INCOMPLETE;
		}
	    }
	}
    }

  fclose (fp);

#ifdef DEBUG_READ_SKIN_LEVEL_COLOUR_OVERRIDE
  printf ("level_colour_overrides_read=%s\n",
	  data_state[level_colour_overrides_read]);
  printf ("level_pack_name_read=%s\n", data_state[level_pack_name_read]);
  printf ("default_level_colour_read=%s\n",
	  data_state[default_level_colour_read]);
  printf ("level_read=%s\n", data_state[level_read]);
  printf ("colour_read=%s\n", data_state[colour_read]);
  printf ("*** Stop %s ***\n", __func__);
#endif

  if (default_level_colour_read == DATA_READ || colour_read == DATA_READ)
    return FALSE;

  return TRUE;
}

/***************************************************************************
 * Load Selected Skin                                                      *
 ***************************************************************************/
/* Loads all image data from the selected skin folder. If any of the SDL
   surfaces are found to contain non NULL pointers then these surfaces will
   be freed first. */

void
load_selected_skin (void)
{
	char filename[256];
	int count;
	SDL_Surface *unconverted, *converted;

	/* Free SDL surfaces first if they are not NULL */
	if (icons)
		SDL_FreeSurface (icons);
#ifdef LIGHTNINGENABLED
	if (efx)
		SDL_FreeSurface (efx);
#endif
	if (ciphers)
		SDL_FreeSurface (ciphers);
	if (alpha)
		SDL_FreeSurface (alpha);
	if (font)
		TTF_CloseFont (font);
	if (bgrnd)
		SDL_FreeSurface (bgrnd);
	bgrnd = NULL;	/* Because the background tile is optional, this now invalid pointer is nullified */
	if (k_icons)
		SDL_FreeSurface (k_icons);
#ifdef LIGHTNINGENABLED
	for (count = 0; count < 7; count++)
#else
	for (count = 0; count < 6; count++)
#endif
	{
		strcpy (filename, PACKAGE_DATA_DIR "/" SKINS_DIR "/");
		strcat (filename, skins[selected_skin].foldername);
		strcat (filename, "/");
		if (count == 0)
		{
			/* Load the icons (sprites) */
			if (video.field_size == 16)
			{
				strcat (filename, IMG_ICONS16);
			}
			else if (video.field_size == 32)
			{
				strcat (filename, IMG_ICONS32);
			}
		}
		else if (count == 1)
		{
			/* Load the ciphers (numbers) */
			if (video.field_size == 16)
			{
				strcat (filename, IMG_CIPHERS16);
			}
			else if (video.field_size == 32)
			{
				strcat (filename, IMG_CIPHERS32);
			}
		}
		else if (count == 2)
		{
			/* Load the alphabet (A to Z and some miscellaneous chars) */
			if (video.field_size == 16)
			{
				strcat (filename, IMG_ALPHA16);
			}
			else if (video.field_size == 32)
			{
				strcat (filename, IMG_ALPHA32);
			}
		}
		else if (count == 3)
		{
			/* Load the font */
			load_TTF_font ();
		}
		else if (count == 4)
		{
			/* Load the background (optional) */
			if(video.field_size==32) 
				strcat (filename, IMG_BGRD32);
			else
				strcat(filename,IMG_BGRD16);
		}
		else if (count == 5)
		{
			/* Load the konstruktor icons */
			if (video.field_size == 32)
				strcat (filename, IMG_KICONS32);
			else if (video.field_size == 16)
				strcat (filename, IMG_KICONS16);
		}
#ifdef LIGHTNINGENABLED
		else if (count == 6)
		{
			/* Load the konstruktor icons */
			if (video.field_size == 32)
				strcat (filename, IMG_EFX32);
			else if (video.field_size == 16)
				strcat (filename, IMG_EFX16);
		}
#endif
		/* now we have the icons files in the PNG format, so we treat these differently */
#ifndef PLATFORM_ZAURUS                                     
		if (count == 0 || count == 4 || count == 5 ||count==6) 
		{
#ifndef PLATFORM_PSVITA
			if ((unconverted = IMG_Load (filename)) == NULL)
#else
			SDL_RWops *rwop = SDL_RWFromFile(filename, "rb");
			if ((unconverted = IMG_LoadPNG_RW(rwop)) == NULL)
#endif
			{
				fprintf (stdout, "Cannot load PNG image %s\n", filename);
				if (count == 0 || count == 5) exit (1);
				bgrnd = NULL;	/* no background file? well, this is not important, let's skip this one */
				continue;
			}
			converted = SDL_DisplayFormatAlpha (unconverted);
			SDL_FreeSurface (unconverted);
			if (converted == NULL)
			{
				fprintf (stdout, "Cannot create surface: %s\n",
					SDL_GetError ());
				if (count == 0 || count == 5) exit (1);
				bgrnd = NULL;	/* no background file? well, this is not important, let's skip this one */
				break;
			}
			if (count == 0)
			{
				icons = converted;
			}
			else if (count == 4)
			{
				bgrnd = converted;
			}
			else if (count == 5)
			{
				k_icons = converted;
			}
#ifdef LIGHTNINGENABLED 
			else if (count==6) {
			    efx=converted;
			}
#endif
		}
		else if (count == 1 || count == 2)
#else
		if (count != 3)
#endif
		{
			/* Load the bitmap */
			if ((unconverted = SDL_LoadBMP (filename)) == NULL)
			{
				fprintf (stdout, "Cannot load bitmap: %s\n", filename);
				exit (1);
			}
			/* Set the transparent colour */
			if (SDL_SetColorKey (unconverted, SDL_SRCCOLORKEY,
				SDL_MapRGB (unconverted->format, 0xff, 0x00, 0xff)) < 0)
			{
				fprintf (stdout, "Cannot set surface colour key: %s\n",
					SDL_GetError ());
				exit (1);
			}
			/* Convert the loaded surface to the same pixel format as the screen for fast blitting */
			converted = SDL_DisplayFormat (unconverted);
			SDL_FreeSurface (unconverted);
			if (converted == NULL)
			{
				fprintf (stdout, "Cannot create surface: %s\n",
					SDL_GetError ());
				exit (1);
			}
			if (count == 1)
			{
				ciphers = converted;
			}
			else if (count == 2)
			{
				alpha = converted;
			}
#ifdef PLATFORM_ZAURUS
			else if (count == 0)
			{
				icons = converted;
			}
			else if (count == 4)
			{
				bgrnd = converted;
				if ((SDL_SetAlpha(bgrnd, SDL_SRCALPHA, 31)) < 0) {	/* 12% */
					fprintf(stdout, "Cannot set surface alpha: %s\n",
						SDL_GetError());
					exit (1);
				}
			}
			else if (count == 5)
			{
				k_icons = converted;
			}
#endif
		}
	}

	/* Read the colours used throughout the project from the skinrc */
	read_skin_project_colours ();

	/* This is what is left of load_bitmaps() */
	/* These SDL_Rects replace the previous duplication of surfaces */
	if (video.field_size == 16)
	{
		score_screw_srcrect =
			set_rect (69, 52, video.field_size, video.field_size);
		score_key_srcrect =
			set_rect (103, 52, video.field_size, video.field_size);
		score_bullet_srcrect =
			set_rect (120, 52, video.field_size, video.field_size);
		score_level_srcrect =
			set_rect (137, 52, video.field_size, video.field_size);
	}
	else if (video.field_size == 32)
	{
		score_screw_srcrect =
			set_rect (138, 104, video.field_size, video.field_size);
		score_key_srcrect =
			set_rect (206, 104, video.field_size, video.field_size);
		score_bullet_srcrect =
			set_rect (240, 104, video.field_size, video.field_size);
		score_level_srcrect =
			set_rect (274, 104, video.field_size, video.field_size);
	}

	for (count = 0; count < 8; count++)
	{
		robbo_images_srcrect[count] =
			set_rect ((video.field_size / 16 +
			count * (video.field_size + video.field_size / 16)),
			((video.field_size + video.field_size / 16) * 5 +
			video.field_size / 16), video.field_size,
			video.field_size);
	}

	for (count = 0; count < 10; count++)
	{
		score_images_srcrect[count] =
			set_rect (count * (video.field_size / 2 + video.field_size / 16), 0,
			video.field_size / 2 + video.field_size / 16,
			video.field_size);
	}

	/* and the last, but not least, we are to call soundskin init */
	/*
	***** disabled
	sound_init(skins[selected_skin].soundskin); 
	***** disabled
	*/
}

/***************************************************************************
 * Set Window Manager Icon                                                 *
 ***************************************************************************/
/* This will load and set a themed icon from the currently selected skin
 * folder as a window manager icon. This is done just the once before
 * calling SDL_SetVideoMode (as per SDL docs instructions) and so changing
 * the skin won't change the icon until quitting the program and restarting */

void set_wm_icon (void) {
	char filename[256];

	strcpy (filename, PACKAGE_DATA_DIR "/" SKINS_DIR "/");
	strcat (filename, skins[selected_skin].foldername);
	strcat (filename, "/");
	strcat (filename, IMG_WM_ICON);

	/* Load the bitmap */
	if ((wm_icon = SDL_LoadBMP(filename)) == NULL) {
		fprintf(stdout,"Cannot load window manager icon: %s\n", filename);
	} else {
		/* Set the transparent colour */
		if (SDL_SetColorKey(wm_icon, SDL_SRCCOLORKEY, SDL_MapRGB(wm_icon->format, 0xff, 0x00, 0xff)) < 0) {
			fprintf(stdout,"Cannot set surface colour key: %s\n", SDL_GetError());
		} else {
			SDL_WM_SetIcon (wm_icon, NULL);
		}
	}
}

/***************************************************************************
 * Create Skin Preview                                                     *
 ***************************************************************************/
/* Creates a 9 x 3 sprite preview of the temp_selected_skin.

	On exit: returns an SDL_Surface pointer if successful which the caller
			 is responsible for freeing with SDL_FreeSurface().
			 returns NULL on error */

SDL_Surface *
create_skin_preview (void)
{
	int iconoffsets1[9 * 2] =
		{ 52, 1, 1, 1, 103, 1, 18, 18, 69, 1, 137, 35, 154, 1, 1, 18, 171, 1 };
	int iconoffsets2[9 * 2] =
		{ 1, 69, 1, 52, 69, 69, 103, 18, 52, 86, 137, 1, 18, 35, 1, 35, 120, 69 };
	int iconoffsets3[9 * 2] =
		{ 86, 35, 69, 35, 120, 1, 52, 18, 86, 1, 103, 35, 137, 18, 154, 52, 154,86 };
	Uint32 tilecolour = skins[temp_selected_skin].help_tile_colour;
	SDL_Surface *unconverted, *preview = NULL;
	SDL_Surface *converted;
	SDL_Rect srcrect, destrect;
	char filename[256];
	int xpos, ypos;

	strcpy (filename, PACKAGE_DATA_DIR "/" SKINS_DIR "/");
	strcat (filename, skins[temp_selected_skin].foldername);
	strcat (filename, "/");
	
	/* Load the icons (sprites) */
	if (video.field_size == 16)
	{
		strcat (filename, IMG_ICONS16);
	}
	else if (video.field_size == 32)
	{
		strcat (filename, IMG_ICONS32);
	}
	
#ifndef PLATFORM_ZAURUS
#ifndef PLATFORM_PSVITA
        if ((unconverted = IMG_Load (filename)) != NULL)
#else
	SDL_RWops *rwop = SDL_RWFromFile(filename, "rb");
	if ((unconverted = IMG_LoadPNG_RW(rwop)) != NULL)
#endif
	{
		/* Set the transparent colour */
		converted = SDL_DisplayFormatAlpha (unconverted);
		SDL_FreeSurface (unconverted);
		if (converted != NULL)
		{
#else
	if ((converted = SDL_LoadBMP(filename)) != NULL)
	{
		unconverted = converted;	/* Simply stops messages about unused variables */
		/* Set the transparent colour */
		if (SDL_SetColorKey(converted, SDL_SRCCOLORKEY,
			SDL_MapRGB(converted->format, 0xff, 0x00, 0xff)) == 0)
		{
#endif
			/* Create a surface for the preview */
			if ((preview =
				SDL_CreateRGBSurface (screen->flags,
				video.field_size * 9 + video.field_size / 4 * 8,
				video.field_size * 3 + video.field_size / 4 * 2,
				screen->format->BitsPerPixel,
				screen->format->Rmask,
				screen->format->Gmask,
				screen->format->Bmask,
				screen->format->Amask)) != NULL)
			{
				/* Fill it with the current background colour */
				SDL_FillRect (preview, NULL,
					SDL_MapRGB (screen->format,
					skins[selected_skin].background_colour >> 16 & 0xff,
					skins[selected_skin].background_colour >> 8 & 0xff,
					skins[selected_skin].background_colour & 0xff));
				/* Draw a selection of icons on the preview surface */
				for (ypos = 0; ypos < 3; ypos++)
				{
					for (xpos = 0; xpos < 9; xpos++)
					{
						/* Draw the background tiles for the icons */
						destrect =
							set_rect (xpos * (video.field_size + video.field_size / 4),
							ypos * (video.field_size + video.field_size / 4),
							video.field_size, video.field_size);
						SDL_FillRect (preview, &destrect,
							SDL_MapRGB (screen->format,
							tilecolour >> 16 & 0xff,
							tilecolour >> 8 & 0xff,
							tilecolour & 0xff));
						/* Draw the icons */
						if (ypos == 0)
						{
							srcrect =
								set_rect (iconoffsets1[xpos * 2] * video.field_size / 16,
								iconoffsets1[xpos * 2 + 1] * video.field_size / 16,
								video.field_size, video.field_size);
						}
						else if (ypos == 1)
						{
							srcrect =
								set_rect (iconoffsets2[xpos * 2] * video.field_size / 16,
								iconoffsets2[xpos * 2 + 1] * video.field_size / 16,
								video.field_size, video.field_size);
						}
						else if (ypos == 2)
						{
							srcrect =
								set_rect (iconoffsets3[xpos * 2] * video.field_size / 16,
								iconoffsets3[xpos * 2 + 1] * video.field_size / 16,
								video.field_size, video.field_size);
						}
						SDL_BlitSurface (converted, &srcrect, preview, &destrect);
					}
				}
			}
			else
			{
				fprintf (stdout, "Cannot create RGB surface: %s\n",
				SDL_GetError ());
			}
		}
		else
		{
			fprintf (stdout, "Cannot set surface colour key: %s\n",
			SDL_GetError ());
		}
		SDL_FreeSurface (converted);
	}
	else
	{
		fprintf (stdout, "Cannot load bitmap: %s\n", filename);
	}

	return preview;
}

/***************************************************************************
 * Load TTF Font                                                           *
 ***************************************************************************/
void
load_TTF_font ()
{
  char filename[256];
  int fontsize = 0;

  strcpy (filename, PACKAGE_DATA_DIR "/" SKINS_DIR "/");
  strcat (filename, skins[selected_skin].foldername);
  strcat (filename, "/");

#ifdef USE_PIXMAP_FONT
  fontsize = 0;			/* Not used */
  if (video.field_size == 16)
    {
      strcat (filename, IMG_FONT_LOWRES);
    }
  else if (video.field_size == 32)
    {
      strcat (filename, IMG_FONT_HIRES);
    }
#else
  strcat (filename, TTF_ROBBO);
  if (video.field_size == 16)
    {
      fontsize = FONTSIZE16;
    }
  else if (video.field_size == 32)
    {
      fontsize = FONTSIZE32;
    }
#endif

  if ((font = TTF_OpenFont (filename, fontsize)) == NULL)
    {
      fprintf (stdout, "Cannot load font: %s\n", filename);
      exit (1);
    }
}
