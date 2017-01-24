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
#define DEBUG_LOAD_LEVEL_DATA
*/

#define DELAY_LEVEL_INIT_SCROLL (2 * game_cycle_limit)

/* Variables */
extern char *data_state[];

/* Function prototypes */
int transform_char (char c);


/***************************************************************************
 * Level Initialisation                                                    *
 ***************************************************************************/
/* Initialises a level (and Robbo) and reads level data from a dat file.
 * 
 *  On exit: returns 0 if successful
 *           returns 1 if level not found */

int level_init (void)
{
	int retval = FALSE;

	/* Initialise Robbo */
	init_robbo ();

	/* Initialise the level */
	level.w = 0;
	level.h = 0;
	level.author[0] = 0;
	level.notes[0] = 0;
	level.now_is_blinking = 0;
	/* This is Uint32 so -1 == 0xffffffff and I won't be using that colour */
	level.colour = UNDEFINED;
	/* This is Uint32 so -1 == 0xffffffff and I won't be using that colour */
	level.colour_override = UNDEFINED;
	/* Reset the pointer controls shoot state */
	pointer_controls.shoot_state = POINTER_CONTROLS_SHOOT_OFF;

	/* Clear board */
	clear_entire_board ();

	/* Attempt to load the level */
	if (load_level_data (level_packs[selected_pack].level_selected))
	{
		retval = TRUE;
	}
	else
	{
		read_skin_level_colour_override ();

		#ifdef DEBUG_LOAD_LEVEL_DATA
			printf ("*** Start %s ***\n", __func__);
			printf ("level.colour=%08X\n", level.colour);
			printf ("level.colour_override=%08X\n", level.colour_override);
			printf ("*** Stop %s ***\n", __func__);
		#endif

		#ifdef DEBUG_COLOUR_SELECT
			debug_colour_select_r = (level.colour_override >> 16) & 0xff;
			debug_colour_select_g = (level.colour_override >> 8) & 0xff;
			debug_colour_select_b = level.colour_override & 0xff;
		#endif

		/* This is only going to happen on levels with no screws */
		if (robbo.screws == 0) open_exit ();

		/* Set the id_questionmark property for all questionmarks */
		init_questionmarks ();

		/* Set the size of the viewport */
		/* Set viewport width */
		if (level.w >= viewport.max_w && !viewport.maximise)
		{
			viewport.w = viewport.max_w;
		}
		else
		{
			viewport.w = level.w;
		}
#ifdef _SMOOTH_SCRL_
		viewport.xs=0;
		viewport.ys=0;
#endif
		
		
		/* Now adjust it to fit within the dimensions of the screen */
		if (viewport.w > screen->w / video.field_size)
			viewport.w = screen->w / video.field_size;
		/* Set the xoffset and centre the viewport */
		viewport.xoffset = (screen->w - viewport.w *
			video.field_size) / 2 + video.xshift;

		/* Set viewport height */
		if (level.h >= viewport.max_h && !viewport.maximise)
		{
			viewport.h = viewport.max_h;
		}
		else
		{
			viewport.h = level.h;
		}
		/* Now adjust it to fit within the dimensions of the screen */
		if (viewport.h > screen->h / video.field_size - 3)
			viewport.h = screen->h / video.field_size - 3;
		/* Set the yoffset and centre the viewport */
		viewport.yoffset = (screen->h - viewport.h *
			video.field_size) / 2 + video.yshift;

		/* Set the scoreline screen offset */
		scoreline.xoffset = (screen->w - video.field_size *
			DEFAULT_VIEWPORT_WIDTH) / 2 + video.xshift;
		/* A special case when screen->w = 240 or 480 */
		if (scoreline.xoffset < 0) scoreline.xoffset = 0;
		scoreline.yoffset = screen->h / 2 + video.field_size *
			viewport.h / 2 + video.field_size / 4 + video.yshift;

		/* Set the authorline screen offset */
		authorline.xoffset = scoreline.xoffset + 2 *
			video.field_size / 16;
		authorline.yoffset = screen->h / 2 - video.field_size *
			viewport.h / 2 - video.field_size -
			video.field_size / 4 + video.yshift;

		/* Start scrolling from the opposite end of the board to Robbo */
		if (robbo.x < level.w / 2)
		{
			viewport.x = level.w - viewport.w;
		}
		else
		{
			viewport.x = 0;
		}
		if (robbo.y < level.h / 2)
		{
			viewport.y = level.h - viewport.h;
		}
		else
		{
			viewport.y = 0;
		}
		viewport.cycles_to_dest = DELAY_LEVEL_INIT_SCROLL;
#ifdef _SMOOTH_SCRL_
		viewport.x=viewport.x*SCRL_MULT2;
		viewport.y=viewport.y*SCRL_MULT2;
#endif
		
		#ifdef DEBUG_RECORD_DEMO
			demo_mode (DEMO_MODE_RECORD_INITIALISE, 0);
		#endif
	}

	return retval;
}

/***************************************************************************
 * Create User Pack                                                        *
 ***************************************************************************/
/* This will create the default user level pack for use with the designer
 * (it needs to be writeable and so its location is platform dependent).
 * If the target platform requires a local folder structure to accommodate
 * this file then it will be created too, otherwise the user pack will be
 * created within the normal location of PACKAGE_DATA_DIR LEVELS_DIR */

void create_userpack (void)
{
	FILE *fp;
	char fullpath[256];
	#if defined(PLATFORM_WIN32)
	#elif defined(PLATFORM_PC) || defined(PLATFORM_ZAURUS) || defined(PLATFORM_FREMANTLE)
		char foldername[256];
	#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
	#elif defined(PLATFORM_PSP)
	#endif

	/* Build a platform dependent full path to the DEFAULT_USER_LEVEL_PACK */
	fullpath[0] = 0;
	#if defined(PLATFORM_WIN32)
	#elif defined(PLATFORM_PC) || defined(PLATFORM_ZAURUS) || defined(PLATFORM_FREMANTLE)
		strcpy (fullpath, getenv ("HOME"));
		strcat (fullpath, "/" LOCAL_DATA_DIR "/" LEVELS_DIR "/" DEFAULT_USER_LEVEL_PACK);
	#elif defined(PLATFORM_GP2X) || defined(PLATFORM_PSP)  || defined(PLATFORM_CAANOO)
		strcpy (fullpath, PACKAGE_DATA_DIR "/" LEVELS_DIR "/" DEFAULT_USER_LEVEL_PACK);
	#elif defined(PLATFORM_PSVITA)
		strcpy (fullpath, CONFIG_DATA_DIR "/" LEVELS_DIR "/" DEFAULT_USER_LEVEL_PACK);
	#endif
	
	if (fullpath[0] != 0)
	{
		/* Attempt to open the DEFAULT_USER_LEVEL_PACK to test its presence */
		if ((fp = fopen (fullpath, "r")) == NULL)
		{
			/* Failure, so attempt to create everything necessary */
			fprintf (stdout, "Creating %s\n", fullpath);

			/* Create directory structure ignoring errors */
			#if defined(PLATFORM_WIN32)
			#elif defined(PLATFORM_PC) || defined(PLATFORM_ZAURUS) || defined(PLATFORM_FREMANTLE)
				strcpy (foldername, getenv ("HOME"));
				strcat (foldername, "/" LOCAL_DATA_DIR);
				mkdir (foldername, 0755);
				strcat (foldername, "/" LEVELS_DIR);
				mkdir (foldername, 0755);
			#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
			#elif defined(PLATFORM_PSP)
			#elif defined(PLATFORM_PSVITA)
			#endif

			/* Create and populate the DEFAULT_USER_LEVEL_PACK */
			if ((fp = fopen (fullpath, "w")) == NULL)
			{
		      fprintf (stdout, "Cannot write to file: %s\n", fullpath);
			}
			else 
			{
				fprintf (fp, "%s\n",
					"[name]\nMyLevels\n\n"
					"[last_level]\n1\n\n"
					"[default_level_colour]\n608050\n\n"
					"[notes]\nDefault user level pack for use with the designer.\n\n\n"
					"[level]\n1\n"
					"[colour]\n608050\n"
					"[size]\n16.31\n"
					"[author]\n"
					"[level_notes]\n"
					"[data]\n"
					"................\n"
					".R..............\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"........T.......\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"................\n"
					"..............!.\n"
					"................\n"
					"[additional]\n0\n"
					"[end]\n\n");
				fclose (fp);
			}
		}
		else 
		{
			fclose (fp);
		}
	}
}

/***************************************************************************
 * Find All DAT Files                                                      *
 ***************************************************************************/
/* This will search either one or two locations (platform dependent) for dat
 * files. Searching PACKAGE_DATA_DIR LEVELS_DIR is mandatory but if the
 * LOCAL_DATA_DIR LEVELS_DIR needs searching too then this should be set-up
 * below by the maintainer of the particular build.
 * On exit: returns 1 on error */

int
find_all_dat_files (void)
{
#ifdef PLATFORM_PSVITA
    int dir;
    int default_dat_found = FALSE;
    char foldername[256];
    int count;
    int i;
    char tmp[256];
    char *str;
    char *filename;
    int add_pack = TRUE;
    SceIoDirent entry;

    for (count = 0; count < 2; count++)
    {
      /* precedence of modified CONFIG_DATA_DIR level packs over PACKAGE_DATA_DIR level packs */
      if (count == 0)
      {
        strcpy (foldername, CONFIG_DATA_DIR "/" LEVELS_DIR);
      }
      else if (count == 1)
      {
        strcpy (foldername, PACKAGE_DATA_DIR "/" LEVELS_DIR);
      }

      if ((dir = sceIoDopen(foldername)) > 0)
      {
        while (sceIoDread(dir, &entry) > 0)
        {
          if (strstr (entry.d_name, ".dat") != NULL)
          {
            printf("---- count = %d, %s\n", count, entry.d_name);

            if (level_pack_count < MAX_LEVEL_PACKS)
            {
              if (strcmp (entry.d_name, DEFAULT_LEVEL_PACK) == 0)
              {
                default_dat_found = TRUE;
              }

              add_pack = TRUE;
              if (count == 0)
              {
                if (strcmp (entry.d_name, DEFAULT_LEVEL_PACK) == 0)
                {
                  strcpy (level_packs[0].filename, foldername);
                  strcat (level_packs[0].filename, "/");
                  strcat (level_packs[0].filename, DEFAULT_LEVEL_PACK);

                  add_pack = FALSE;
                }
              }

              if (count == 1)
              {
                for (i = 0 ; i < level_pack_count - 1 ; i++)
                {
                  sprintf (tmp, "%s", level_packs[i].filename);
                  str = strtok(tmp, "/");
                  while (str != NULL)
                  {
                    filename = str;
                    str = strtok (NULL, "/");
                  }

                  if (strcmp (entry.d_name, filename) == 0)
                  {
                    add_pack = FALSE;
                    printf("@@@ pack %s already included\n", filename);
                  }
                }
              }

              if (add_pack)
              {
                strcpy (level_packs[level_pack_count].filename, foldername);
                strcat (level_packs[level_pack_count].filename, "/");
                strcat (level_packs[level_pack_count].filename, entry.d_name);
                level_pack_count++;
                found_pack_count++;
              }
            }
          }
        }
        sceIoDclose(dir);
      }
      else
      {
        printf ("Couldn't open levels folder: %s\n", foldername);
      }
    }

    printf ("find_all_dat_files:\n");
    for (i = 0 ; i < level_pack_count - 1 ; i++)
    {
      printf ("level_packs[%d].filename = %s", i, level_packs[i].filename);
    }

    if (!default_dat_found)
    {
        printf ("Cannot find the default level file: %s\n",
            PACKAGE_DATA_DIR "/" LEVELS_DIR "/" DEFAULT_LEVEL_PACK);
        return 1;
    }
#else
	DIR *dir;
	struct dirent *currentdirent;
	int default_dat_found = FALSE;
	char foldername[256];
	int count;

	#if defined(PLATFORM_WIN32)
		for (count = 0; count < 1; count++)
	#elif defined(PLATFORM_PC) || defined(PLATFORM_ZAURUS) || defined(PLATFORM_FREMANTLE)
		for (count = 0; count < 2; count++)
	#elif defined(PLATFORM_GP2X) || defined(PLATFORM_PSP)  || defined(PLATFORM_CAANOO)
		for (count = 0; count < 1; count++)
	#endif
	{
		if (count == 0)
		{
			strcpy (foldername, PACKAGE_DATA_DIR "/" LEVELS_DIR);
		}
		else if (count == 1)
		{
			#if defined(PLATFORM_WIN32)
			#elif defined(PLATFORM_PC) || defined(PLATFORM_ZAURUS) || defined(PLATFORM_FREMANTLE)
				strcpy (foldername, getenv ("HOME"));
				strcat (foldername, "/" LOCAL_DATA_DIR "/" LEVELS_DIR);
			#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
			#elif defined(PLATFORM_PSP)
			#endif
		}

		/* Open the installation or current directory */
		if ((dir = opendir (foldername)) != NULL)
		{
			/* Now search the directory for dat files */
			while ((currentdirent = readdir (dir)) != NULL)
			{
                                #if defined(PLATFORM_PSP)
                                	if ((strstr (currentdirent->d_name, ".dat") != NULL) || (strstr (currentdirent->d_name, ".DAT") != NULL))
                                #else
                                	if (strstr (currentdirent->d_name, ".dat") != NULL)
                                #endif
				{
					if (level_pack_count < MAX_LEVEL_PACKS)
					{
                                                #if defined(PLATFORM_PSP)
                                                      if (strcasecmp (currentdirent->d_name, DEFAULT_LEVEL_PACK) == 0)
                                                #else
                                                      if (strcmp (currentdirent->d_name, DEFAULT_LEVEL_PACK) == 0)        
                                                #endif
						{
							default_dat_found = TRUE;
						}
						else
						{
							strcpy (level_packs[level_pack_count].filename,
								foldername);
							strcat (level_packs[level_pack_count].filename,
								"/");
							strcat (level_packs[level_pack_count].filename,
								currentdirent->d_name);
							level_pack_count++;
							found_pack_count++;
						}
					}
				}
			}
			closedir (dir);
		}
		else
		{
			fprintf (stdout, "Couldn't open levels folder: %s\n", foldername);
		}
	}

	if (!default_dat_found)
	{
		fprintf (stdout, "Cannot find the default level file: %s\n",
			PACKAGE_DATA_DIR "/" LEVELS_DIR "/" DEFAULT_LEVEL_PACK);
		return 1;
	}
#endif
	return 0;
}

/***************************************************************************
 * Read Level Packs                                                        *
 ***************************************************************************/
/* Reads all level packs and populates the level pack list */

void
read_level_packs (void)
{
	FILE *fp;
	char line[1024];
	char filename[256];
	int name_found, last_level_found, count, count2;

	for (count = 0; count < level_pack_count; count++)
	{
		strcpy (filename, level_packs[count].filename);
		if ((fp = fopen (filename, "r")) == NULL)
		{
			fprintf (stdout, "Cannot read from file %s\n", filename);
			/* Set everything to uninitialised */
			strcpy (level_packs[count].name, SINGLE_SPACE);
			level_packs[count].last_level = DEFAULT_LEVEL_START;
			level_packs[count].level_reached = DEFAULT_LEVEL_START;
			level_packs[count].level_selected = DEFAULT_LEVEL_START;
			level_packs[count].selected = FALSE;
		}
		else
		{
			name_found = last_level_found = FALSE;
			while ((fgets (line, 1024, fp)) != NULL)
			{
				if (!strncmp (line, "[name]", 6))
				{
					fgets (line, 1024, fp);
					/* fgets will get [CR]LFs too so they should be removed */
					for (count2 = strlen (line) - 1; count2 >= 0; count2--)
						if (line[count2] == 13 || line[count2] == 10)
							line[count2] = 0;
					strncpy (level_packs[count].name, line, 20);
					level_packs[count].name[19] = 0;	/* Just in case we read all chars */
					name_found = TRUE;
				}
				else if (!strncmp (line, "[last_level]", 12))
				{
					fgets (line, 1024, fp);
					sscanf (line, "%i", &level_packs[count].last_level);
					last_level_found = TRUE;
				}
				if (name_found && last_level_found)
					break;
			}
			if (!name_found || !last_level_found)
			{
				if (!name_found)
				{
					fprintf (stdout, "[name] is missing from %s\n", filename);
					/* Default the name */
					strcpy (level_packs[count].name, SINGLE_SPACE);
				}
				if (!last_level_found)
				{
					fprintf (stdout, "[last_level] is missing from %s\n",
						filename);
					/* Default last_level */
					level_packs[count].last_level = DEFAULT_LEVEL_START;
				}
			}
			/* Default the rest */
			level_packs[count].level_reached = DEFAULT_LEVEL_START;
			level_packs[count].level_selected = DEFAULT_LEVEL_START;
			if (count)
				level_packs[count].selected = FALSE;
			/* Dump info to the console */
			fprintf (stdout, "Level pack found: %s with %i levels\n",
				level_packs[count].name, level_packs[count].last_level);
			fclose (fp);
		}
	}
}

/***************************************************************************
 * Sort Level Packs                                                        *
 ***************************************************************************/
/* Sorts found level packs alphanumerically by name remembering to
 * preserve the selected pack :) This must be done last after
 * find_all_dat_files(), read_level_packs() and read_resource_file() */

void sort_level_packs (void)
{
	struct pack swap_pack;
	int swapped, count;

	/* Bubble sort found level packs by name */
	do
	{
		swapped = FALSE;
		for (count = 0; count < found_pack_count - 1; count++)
		{
			if (strcmp (level_packs[count].name, level_packs[count + 1].name) > 0)
			{
				swapped = TRUE;
				swap_pack = level_packs[count + 1];
				level_packs[count + 1] = level_packs[count];
				level_packs[count] = swap_pack;
			}
		}
	}
	while (swapped);

	/* Find the selected pack */
	for (count = 0; count < found_pack_count; count++)
	{
		if (level_packs[count].selected)
			selected_pack = count;
	}
}

int create_wall(int type,int x,int y)
{
    switch (type)
    {
    case WALL_RED:
	create_object (x, y, WALL);
	board[x][y].state = 1;
	break;
    case WALL_GREEN:
	create_object (x, y, WALL);
	board[x][y].state = 2;
	break;
    case BLACK_WALL:
	create_object (x, y, WALL);
	board[x][y].state = 3;
	break;
    case FAT_WALL:
	create_object (x, y, WALL);
	board[x][y].state = 4;
	break;
    case ROUND_WALL:
	create_object (x, y, WALL);
	board[x][y].state = 5;
	break;
    case BOULDER_WALL:
	create_object (x, y, WALL);
	board[x][y].state = 6;
	break;
    case SQUARE_WALL:
	create_object (x, y, WALL);
	board[x][y].state = 7;
	break;
    case LATTICE_WALL:
	create_object (x, y, WALL);
	board[x][y].state = 8;
	break;
    case S_WALL:
	create_object(x,y,WALL);
	board[x][y].state = 9;
	break;
    }
    return 0;
    }


/***************************************************************************
 * Load Level Data                                                         *
 ***************************************************************************/
/* This function is called every time we need a new level.
 * It has been rewritten to be more tolerant of incomplete data and to
 * display warnings via the console when something is missing.
 * 
 * On entry: level_number is a level within the currently selected pack
 *  On exit: returns 0 if successful
 *           returns 1 if level not found */

int load_level_data (int level_number)
{
	FILE *fp;
	/* line[1024] because if fgets returns a partial line
	 * then line_count will be incremented again */
	char filename[256], symbol, line[1024];
	int count, line_count = 0, x, y = 0, rows = UNDEFINED;
	int additional_count = UNDEFINED, value_count;
	int value_read[16];
	int default_level_colour_read = DATA_UNREAD;
	int level_read = DATA_UNREAD;
	int colour_read = DATA_UNREAD;
	int size_read = DATA_UNREAD;
	int author_read = DATA_UNREAD;
	int data_read = DATA_UNREAD;
	int additional_read = DATA_UNREAD;
	int notes_read = DATA_UNREAD;
	int screws_read = DATA_UNREAD;
	int screws_to_set=-1;
//	int temp_symbol;
	/*int screws_read= DATA_UNREAD;	Thunor: redundant: not used - neurocyp:
	 * we want to have different number of screws, than default, some levels
	 * in robbo have different number for level, than real screws number */

	/* Attempt to open the level pack */
	strcpy (filename, level_packs[selected_pack].filename);
	if ((fp = fopen (filename, "r")) == NULL)
	{
		fprintf (stdout, "Cannot read from file: %s\n", filename);
		return TRUE;
	}

	#ifdef DEBUG_LOAD_LEVEL_DATA
		printf ("*** Start %s ***\n", __func__);
	#endif

	/* Read lines including the [CR]LFs until we have what we need
	 * or reach the end of file (line == NULL).
	 * We are looking for some or all of these things :-
	 * [default_level_colour] - OPTIONAL
	 * [level]
	 * [colour] - OPTIONAL
	 * [size]
	 * [author] - OPTIONAL
	 * [level_notes] - OPTIONAL
	 * [data]
	 * [additional] - if not applicable, this may be missing
	 * [end] */
	while ((fgets (line, 1024, fp)) != NULL)
	{
		line_count++;
		
		/* fgets will get [CR]LFs too so they should be removed */
		for (count = strlen (line) - 1; count >= 0; count--)
			if (line[count] == 13 || line[count] == 10) line[count] = 0;
			
		/* Use the tags as switches so we know what to read.
		 * NOTE: Reading consecutive (dataless) tags means that the
		 * previous tag may still be marked as DATA_READING */
		if ((!strncmp (line, "[default_level_colour]", 22)) ||
			(!strncmp (line, "[level]", 7) && level_read == DATA_UNREAD) ||
			(!strncmp (line, "[level_notes]", 13) && level_read == DATA_READ) ||
			(!strncmp (line, "[colour]", 8) && level_read == DATA_READ) ||
			(!strncmp (line, "[size]", 6) && level_read == DATA_READ) ||
			(!strncmp (line, "[author]", 8) && level_read == DATA_READ) ||
			(!strncmp (line, "[data]", 6) && level_read == DATA_READ) ||
			(!strncmp (line, "[additional]", 12) && level_read == DATA_READ) ||
			(!strncmp (line, "[screws]", 7) && level_read == DATA_READ) ||
			(!strncmp (line, "[end]", 5) && level_read == DATA_READ))
		{
			/* If a tag is being read and we read another tag then
			 * mark it as something else */
			if (notes_read == DATA_READING) 
			{
				notes_read = DATA_INCOMPLETE;
			} else if (screws_read == DATA_READING) 
			{
			    screws_read=DATA_INCOMPLETE;
			}
			else if (default_level_colour_read == DATA_READING)
			{
				default_level_colour_read = DATA_INCOMPLETE;
			}
			else if (level_read == DATA_READING)
			{
				level_read = DATA_UNREAD;	/* We need to keep reading these tags */
			}
			else if (colour_read == DATA_READING)
			{
				colour_read = DATA_INCOMPLETE;
			}
			else if (size_read == DATA_READING)
			{
				size_read = DATA_INCOMPLETE;
				fprintf (stdout, "%s:%i: Incomplete size data: width or "
					"height or both were found to be missing.\n",
					level_packs[selected_pack].filename, line_count);
			}
			else if (author_read == DATA_READING)
			{
				author_read = DATA_INCOMPLETE;
			}
			else if (data_read == DATA_READING)
			{
				data_read = DATA_INCOMPLETE;
				fprintf (stdout, "%s:%i: Insufficient data for declared "
					"height of %i.\n", level_packs[selected_pack].filename,
					line_count, level.h);
			}
			else if (additional_read == DATA_READING)
			{
				additional_read = DATA_INCOMPLETE;
				if (additional_count != UNDEFINED)
					fprintf (stdout, "%s:%i: Insufficient additional data "
						"objects for declared count of %i.\n",
						level_packs[selected_pack].filename, line_count,
						additional_count);
			}
			
			/* If the found tag is DATA_UNREAD then mark it for DATA_READING */
			if (!strncmp (line, "[default_level_colour]", 22))
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
				if (colour_read == DATA_UNREAD) colour_read = DATA_READING;
			}
			else if (!strncmp (line, "[size]", 6))
			{
				if (size_read == DATA_UNREAD) size_read = DATA_READING;
			}
			else if (!strncmp (line, "[author]", 8))
			{
				if (author_read == DATA_UNREAD) author_read = DATA_READING;
			}
			else if (!strncmp (line, "[data]", 6))
			{
				if (data_read == DATA_UNREAD)
				{
					data_read = DATA_READING;
					/* This is a good place to reset variables
					 * before reading the data */
					y = 0;
				}
			}
			else if (!strncmp (line, "[additional]", 12))
			{
				if (additional_read == DATA_UNREAD)
				{
					additional_read = DATA_READING;
					additional_count = rows = UNDEFINED;
				}
			} else if (!strncmp(line,"[screws]",8)) {
			    screws_read=DATA_READING;
			}
			else if (!strncmp(line,"[level_notes]",13))
			{
				if (notes_read == DATA_UNREAD) notes_read = DATA_READING;
			}
			else  if (!strncmp (line, "[end]", 5))
			{
				break;		/* Stop reading the file now */
			}
		}
		else
		{
			/* It's not a tag but data (or newline) so we
			 * need to find what to do with it */
			if (notes_read == DATA_READING)
			{
				if (strlen(level.notes) < 1024 - 3)
				{
					/* Append some or all line data */
					count = 1024 - 3 - strlen(level.notes);
					if (strlen(line) <= count)
					{
						strcat (level.notes, line);
					}
					else
					{
						strncat (level.notes, line, count);
						/* Force termination when using strncat */
						level.notes[1024 - 1] = 0;
					}
					/* Enforce a newline else it causes problems.
					 * 3 chars have been reserved for buffer termination */
					strcat (level.notes, "\n");
				}
				else
				{
						notes_read = DATA_READ;
				}
				#ifdef DEBUG_LOAD_LEVEL_DATA
					printf ("%i: level_notes=\"%s\"\n", line_count, level.notes);
				#endif
			} else if (screws_read==DATA_READING) {
			    if(sscanf(line,"%i",&value_read[0])==1) {
				screws_to_set= value_read[0];
				screws_read=DATA_READ;
			    }
			}
			else if (default_level_colour_read == DATA_READING)
			{
				if (sscanf (line, "%x", &value_read[0]) == 1)
				{
					level.colour = value_read[0];
					default_level_colour_read = DATA_READ;
					#ifdef DEBUG_LOAD_LEVEL_DATA
						printf ("%i: default_level_colour=0x%06X\n",
							line_count, value_read[0]);
					#endif
				}
				else
				{
					/* Read once, found to be incomplete, the end :) */
					default_level_colour_read = DATA_INCOMPLETE;
				}
			}
			else if (level_read == DATA_READING)
			{
				if (sscanf (line, "%i", &value_read[0]) == 1)
				{
					if (value_read[0] == level_number)
					{
						level_read = DATA_READ;
						#ifdef DEBUG_LOAD_LEVEL_DATA
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
					level.colour = value_read[0];
					colour_read = DATA_READ;
					#ifdef DEBUG_LOAD_LEVEL_DATA
						printf ("%i: colour=0x%06X\n", line_count, value_read[0]);
					#endif
				}
				else
				{
					colour_read = DATA_INCOMPLETE;
				}
			}
			else if (size_read == DATA_READING)
			{
				if (sscanf (line, "%i.%i", &value_read[0], &value_read[1]) == 2)
				{
					if ((value_read[0] > 0 && value_read[0] <= MAX_W) &&
						(value_read[1] > 0 && value_read[1] <= MAX_H))
					{
						level.w = value_read[0];
						level.h = value_read[1];
						size_read = DATA_READ;
						#ifdef DEBUG_LOAD_LEVEL_DATA
							printf ("%i: size w=%i h=%i\n", line_count,
								value_read[0], value_read[1]);
						#endif
					}
					else
					{
						size_read = DATA_INCOMPLETE;
						fprintf (stdout, "%s:%i: Invalid size data: width "
							"must be <= %i and height <= %i.\n",
							level_packs[selected_pack].filename,
							line_count, MAX_W, MAX_H);
					}
				}
				else
				{
					size_read = DATA_INCOMPLETE;
					fprintf (stdout, "%s:%i: Incomplete size data: width "
						"or height or both were found to be missing.\n",
						level_packs[selected_pack].filename, line_count);
				}
			}
			else if (author_read == DATA_READING)
			{
				strncpy (level.author, line, 60);	/* Prevent overruns */
				level.author[59] = 0;	/* Just in case we read all chars */
				author_read = DATA_READ;
				#ifdef DEBUG_LOAD_LEVEL_DATA
					printf ("%i: author=%s\n", line_count, line);
				#endif
			}
			else if (data_read == DATA_READING)
			{
				/* Only the quantity of data declared by size must be read.
				 * x and y will be used exclusively here whilst reading the data */
				x = 0;
				while (TRUE)
				{
					if (line[x] == 0)
					{		/* No data, end of string */
						data_read = DATA_INCOMPLETE;
						fprintf (stdout, "%s:%i: Insufficient data for "
							"declared width of %i.\n",
							level_packs[selected_pack].filename,
							line_count, level.w);
						break;
					}
					else
					{
						board[x][y].type = transform_char (line[x]);
						create_object (x, y, board[x][y].type);
						switch (board[x][y].type)
						{
							case -1:   // here we add support for more complicated type, of course in this place, we do not do anything
								break;
							case SCREW:
								if(screws_read==DATA_READ) {
								robbo.screws=screws_to_set;
								} else {
								    robbo.screws++;
								}
								break;
							case WALL_RED:
							case WALL_GREEN:
							case BLACK_WALL:
							case FAT_WALL:
							case ROUND_WALL:
							case BOULDER_WALL:
							case SQUARE_WALL:
							case LATTICE_WALL:
							case S_WALL:
							    create_wall(board[x][y].type,x,y);
							    break;
						}
						#ifdef DEBUG_LOAD_LEVEL_DATA
							printf ("%c", line[x]);
						#endif
					}
					if (++x >= level.w) break;
				}
				#ifdef DEBUG_LOAD_LEVEL_DATA
					printf ("\n");
				#endif
				if (++y >= level.h) data_read = DATA_READ;
			}
			else if (additional_read == DATA_READING)
			{
				/* rows is used exclusively here whilst reading the data.
				 * Are we reading the additional count? */
				if (additional_count == UNDEFINED)
				{
					/* If we read 2 then it's a data object and not a count */
					if (sscanf (line, "%i.%i", &value_read[0], &value_read[1]) == 1)
					{
						if (value_read[0] > 0)
						{
							additional_count = rows = value_read[0];
							#ifdef DEBUG_LOAD_LEVEL_DATA
								printf ("%i: additional=%i\n", line_count,
									value_read[0]);
							#endif
						}
						else
						{
							additional_read = DATA_INCOMPLETE;
						}
					}
					else
					{
						additional_read = DATA_INCOMPLETE;
						fprintf (stdout, "%s:%i: The additional data object "
							"count is missing. This should be directly below "
							"the [additional] tag.\n",
							level_packs[selected_pack].filename, line_count);
					}
				}
				else
				{
					/* It's not the additional count,
					 * so read the additional data objects */
					value_count = sscanf (line, "%i.%i.%c.%i.%i.%i.%i.%i.%i.%i.%i.%i.%i.%i",
						&x, &y, &symbol, &value_read[0], &value_read[1],
						&value_read[2], &value_read[3], &value_read[4],
						&value_read[5],&value_read[6],&value_read[7],
						&value_read[8],&value_read[9],&value_read[10]
						);
					#ifdef DEBUG_LOAD_LEVEL_DATA
						printf ("%i: %s\n", line_count, line);
					#endif
					/* Validate the values read */
					if (value_count <= 0)
					{
						additional_read = DATA_INCOMPLETE;
						fprintf (stdout, "%s:%i: Insufficient additional "
							"data objects for declared count of %i.\n",
							level_packs[selected_pack].filename,
							line_count, additional_count);
					}
					else if (value_count < 4)
					{
						fprintf (stdout, "%s:%i: Insufficient number of "
							"values defining additional data object.\n",
							level_packs[selected_pack].filename,
							line_count);
					}
					else if (x < 0 || x >= MAX_W)
					{
						fprintf (stdout, "%s:%i: Invalid value for x in "
							"additional data object.\n",
							level_packs[selected_pack].filename,
							line_count);
					}
					else if (y < 0 || y >= MAX_H)
					{
						fprintf (stdout, "%s:%i: Invalid value for y in "
							"additional data object.\n",
							level_packs[selected_pack].filename,
							line_count);
					}
					else
					{
						switch (transform_char (symbol))
						{
							case -1:
								switch (value_read[0]) {
								    case WALL_RED:
								    case WALL_GREEN:
								    case BLACK_WALL:
								    case FAT_WALL:
								    case ROUND_WALL:
								    case BOULDER_WALL:
								    case SQUARE_WALL:
								    case LATTICE_WALL:
								    case S_WALL:
									create_wall(value_read[0],x,y);
									//board[x][y].state=value_read[1];
									board[x][y].direction=value_read[2];
									board[x][y].destroyable=value_read[3];
									board[x][y].blowable=value_read[4];
									board[x][y].killing=value_read[5];
									board[x][y].movable=value_read[6];
									board[x][y].shooted=value_read[7];

								    break;
								    default:
									//fprintf(stdout,"%i %i %i %i %i\n",value_read[1],value_read[2],value_read[3],value_read[4],value_read[5]);
									create_object(x,y,value_read[0]);
									board[x][y].state=value_read[1];
									board[x][y].direction=value_read[2];
									board[x][y].destroyable=value_read[3];
									board[x][y].blowable=value_read[4];
									board[x][y].killing=value_read[5];
									board[x][y].movable=value_read[6];
									board[x][y].shooted=value_read[7];
								    break;	
								    }
								    break;
								
							case LASER_L:
							case LASER_D:
								board[x][y].direction=value_read[0];
								break;
							case TELEPORT:
								board[x][y].teleportnumber = value_read[0];
								board[x][y].teleportnumber2 = value_read[1];
								break;
							case GUN:
								board[x][y].direction = value_read[0];
								board[x][y].state = value_read[0];
								board[x][y].direction2 = value_read[1];
								board[x][y].solidlaser = value_read[2];
								board[x][y].movable = value_read[3];
								board[x][y].rotable = value_read[4];
								board[x][y].randomrotated = value_read[5];
								if(board[x][y].movable==1) {
									board[x][y].state+=4;
								}
								break;
							case MAGNET:
								board[x][y].state = value_read[0];
								if(value_count>4)   /* we would like to have rotable magnets */
									board[x][y].rotable=value_read[1];
								else	
									board[x][y].rotable=0;
							case BARRIER:
								board[x][y].direction = value_read[0];
								break;
							case BIRD:
								board[x][y].direction2 = value_read[1];
								board[x][y].shooting = value_read[2];
							case BEAR_B:
							case BEAR:
								board[x][y].direction = value_read[0];
								break;
							default:
								fprintf (stdout, "%s:%i: Invalid value for "
									"symbol in additional data object.\n",
									level_packs[selected_pack].filename,
									line_count);
								break;
						}
					}
					if (--rows <= 0) additional_read = DATA_READ;
				}
			}
		}
	}
	
	fclose (fp);
        rearange_walls();
	#ifdef DEBUG_LOAD_LEVEL_DATA
		printf ("default_level_colour_read=%s\n",
			data_state[default_level_colour_read]);
		printf ("level_read=%s\n", data_state[level_read]);
		printf ("colour_read=%s\n", data_state[colour_read]);
		printf ("size_read=%s\n", data_state[size_read]);
		printf ("author_read=%s\n", data_state[author_read]);
		printf ("notes_read=%s\n", data_state[notes_read]);
		printf ("data_read=%s\n", data_state[data_read]);
		printf ("additional_read=%s\n", data_state[additional_read]);
		printf ("*** Stop %s ***\n", __func__);
	#endif

	if (level_read == DATA_READ && size_read == DATA_READ &&
		data_read == DATA_READ)
	{
		return FALSE;
	}
	else
	{
		/* Show the user generally what caused the load abort. More
		 * descriptive messages may have already been displayed from
		 * the above code */
		fprintf (stdout, "Loading of pack %s level %i has been aborted due "
			"to :-\n", level_packs[selected_pack].filename, level_number);
		if (level_read != DATA_READ)
		{
			fprintf (stdout, "* The level was not found.\n");
		}
		else
		{
			if (size_read == DATA_UNREAD)
			{
				fprintf (stdout, "* The [size] tag was not found.\n");
			}
			else if (size_read == DATA_INCOMPLETE)
			{
				fprintf (stdout, "* Incomplete or invalid size data.\n");
			}
			if (data_read == DATA_UNREAD)
			{
				fprintf (stdout, "* The [data] tag was not found.\n");
			}
			else if (data_read == DATA_INCOMPLETE)
			{
				fprintf (stdout, "* Incomplete level data.\n");
			}
		}
	}

	return TRUE;
}

/****************************************************/
/*** Transformation char for object value ***********/
/****************************************************/

int transform_char (char c)
{
	switch (c)
	{
		case 'a': 
			return S_WALL;
		case 'X':
			return STOP;
		case 'k':
			return RADIOACTIVE_FIELD;
		case '.':
			return EMPTY_FIELD;
		case 'R':
			return ROBBO;
		case 'O':
			return WALL;
		case 'Q':
			return WALL_RED;
		case 'T':
			return SCREW;
		case '\'':
			return BULLET;
		case '#':
			return BOX;
		case '%':
			return KEY;
		case 'B':
			return BOMB2;  /* robbo alex has at least two types of bombs */ 	
		case 'b':
			return BOMB;
		case 'D':
			return DOOR;
		case '?':
			return QUESTIONMARK;
		case '@':
			return BEAR;
			case '^':
		return BIRD;
		case '!':
			return CAPSULE;
		case 'H':
			return GROUND;
		case 'o':
			return WALL_GREEN;
		case '*':
			return BEAR_B;
		case 'V':
			return BUTTERFLY;
		case '&':
			return TELEPORT;
		case '}':
			return GUN;
		case 'M':
			return MAGNET;
		case '-':
			/* Thunor: I changed spaces to "-"
			 * because spaces are difficult to manage */
			return BLACK_WALL;
		case '~':
			return PUSH_BOX;
		case '=':
			return BARRIER;
		case 'q':
			return FAT_WALL;
		case 'p':
			return ROUND_WALL;
		case 'P':
			return BOULDER_WALL;
		case 's':
			return SQUARE_WALL;
		case 'S':
			return LATTICE_WALL;
		case '+':
			/* Thunor: "+" is a life which we don't support
			 * but remains in some levels */
 //			return EMPTY_FIELD;

/*neurocyp: STOP is more appropriate*/
			return STOP;
		case 'L':
			return LASER_L;
		case 'l':
			return LASER_D;	
		case ',':
			return -1; // This one must be additionally being resolved
		default:
			fprintf (stdout, "Unrecognised object identifier: \"%c\". "
				"Setting to EMPTY_FIELD\n", c);
			return EMPTY_FIELD;
	}
}


