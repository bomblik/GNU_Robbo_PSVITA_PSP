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

/* Defines */
/*
#define DEBUG_READ_RESOURCE_FILE
*/

#define UNDEFINED2 -2

/* Variables */


/* Function prototypes */


/***************************************************************************
 * Read Resource File                                                      *
 ***************************************************************************/
/* Reads resource file data.
 * On entry: filename is the full path to the rc file
 * On exit: returns 0 if successful
 *          returns 1 if file not found
 *          returns 2 if rc data corrupt */

int read_resource_file (char *filename)
{
	FILE *fp;
	char line[1024];
	int read_game_cycle_limit = UNDEFINED, read_game_cycle_delay = UNDEFINED;
	int read_default_joystick = UNDEFINED, read_joystick_dead_zone = UNDEFINED;
	char read_default_joystick_name[MAX_JOYSTICK_NAME_LENGTH];
	int read_key_repeat_delay = UNDEFINED, read_key_repeat_interval = UNDEFINED;
	char read_selected_skin[100], read_selected_locale[100];
	int read_sensible_bears = UNDEFINED, read_sensible_questionmarks =
		UNDEFINED, read_sensible_solid_lasers = UNDEFINED;
	int read_save_frequency = UNDEFINED;
	struct pack read_level_packs[MAX_LEVEL_PACKS];
	int read_pack_count = 0, count, count2, error_found = FALSE, found;
	struct control read_user_controls[USER_CONTROLS];
	ROB_OpEnv read_op_env;
	char error_setting[50];
	struct pointercontrols read_pointer_controls;
	int read_sound = UNDEFINED;
	int read_volume = UNDEFINED;
	int read_sfx_vol = UNDEFINED;
	
	strcpy (read_default_joystick_name, "-1");
	strcpy (read_selected_skin, "-1");
	strcpy (read_selected_locale, "-1");

	fprintf (stdout, "Reading from %s\n", filename);
	if ((fp = fopen (filename, "r")) == NULL)
	{
		fprintf (stdout, "%s not found\n", filename);
		return 1;
	}

	/* Reset read_level_packs so we can test for missing settings */
	for (count = 0; count < MAX_LEVEL_PACKS; count++)
	{
		strcpy (read_level_packs[count].filename, "-1");
		read_level_packs[count].level_reached = UNDEFINED;
		read_level_packs[count].level_selected = UNDEFINED;
		read_level_packs[count].selected = UNDEFINED;
	}

	/* Reset read_user_controls so we can test for missing settings */
	for (count = 0; count < USER_CONTROLS; count++)
	{
		/* Note use of UNDEFINED2 (-2) as UNDEFINED (-1) is used */
		read_user_controls[count].device = UNDEFINED2;
		read_user_controls[count].id = UNDEFINED2;
		read_user_controls[count].mod = UNDEFINED2;
	}

	/* Reset read_op_env so we can test for missing settings */
	read_op_env.systempointer = UNDEFINED;
	read_op_env.pointer = UNDEFINED;
	read_op_env.pointer_mode = UNDEFINED;
	read_op_env.pointer_move_unit_low = UNDEFINED;
	read_op_env.pointer_move_unit_high = UNDEFINED;
	read_op_env.pointer_move_unit_threshold = UNDEFINED;

	/* Reset read_pointer_controls so we can test for missing settings */
	read_pointer_controls.state = UNDEFINED;
	read_pointer_controls.shoot_state = UNDEFINED;
	read_pointer_controls.pad_type = UNDEFINED;

	/* Read the rc data */
	while (fscanf (fp, "%s", line) == 1)
	{
		if (!strncmp (line, "[game_cycle_limit]", 18))
		{
			fscanf (fp, "%i", &read_game_cycle_limit);
		}
		else if (!strncmp (line, "[sfx_vol]", 9))
		{
			fscanf (fp, "%i", &read_sfx_vol);
		}
		else if (!strncmp(line,"[volume]",8))
		{
			fscanf(fp,"%i",&read_volume);
		}
		else if (!strncmp (line, "[sound]", 7))
		{
			fscanf (fp, "%i", &read_sound);
			#ifdef DEBUG_SOUND
				printf ("sound %d %s\n", read_sound, line);
			#endif
		}
		else if (!strncmp (line, "[game_cycle_delay]", 18))
		{
			fscanf (fp, "%i", &read_game_cycle_delay);
		}
		else if (!strncmp (line, "[default_joystick]", 18))
		{
			fscanf (fp, "%i", &read_default_joystick);
		}
		else if (!strncmp (line, "[default_joystick_name]", 23))
		{
			/* Have to use fgets here although it isn't perfect either.
			 * The problem is the name can have spaces in it and fgets
			 * gets [CR]LFs too so we have to clean up the line end */
			/* Reads the [CR]LF at the end of the previous line */
			fgets (read_default_joystick_name, MAX_JOYSTICK_NAME_LENGTH, fp);
			/* Reads the name + one or more [CR]LFs */
			fgets (read_default_joystick_name, MAX_JOYSTICK_NAME_LENGTH, fp);
			/* MacOSX and Unix is LF, Win and Mac is CRLF */
			for (count = strlen (read_default_joystick_name) - 1; count >= 0; count--)
				if (read_default_joystick_name[count] == 13
					|| read_default_joystick_name[count] == 10)
					read_default_joystick_name[count] = 0;
		}
		else if (!strncmp (line, "[joystick_dead_zone]", 20))
		{
			fscanf (fp, "%i", &read_joystick_dead_zone);
		}
		else if (!strncmp (line, "[key_repeat_delay]", 18))
		{
			fscanf (fp, "%i", &read_key_repeat_delay);
		}
		else if (!strncmp (line, "[key_repeat_interval]", 21))
		{
			fscanf (fp, "%i", &read_key_repeat_interval);
		}
		else if (!strncmp (line, "[selected_skin]", 15))
		{
			fscanf (fp, "%s", read_selected_skin);
		}
		else if (!strncmp (line, "[selected_locale]", 17))
		{
			fscanf (fp, "%s", read_selected_locale);
		}
		else if (!strncmp (line, "[save_frequency]", 16))
		{
			fscanf (fp, "%i", &read_save_frequency);
		}
		else if (!strncmp (line, "[op_env.systempointer]", 22))
		{
			fscanf (fp, "%i", &read_op_env.systempointer);
		}
		else if (!strncmp (line, "[pointer_controls.state]", 24))
		{
			fscanf (fp, "%i", &read_pointer_controls.state);
		}
		else if (!strncmp (line, "[pointer_controls.pad_type]", 27))
		{
			fscanf (fp, "%i", &read_pointer_controls.pad_type);
		}
		else if (!strncmp (line, "[sensible_bears]", 16))
		{
			fscanf (fp, "%i", &read_sensible_bears);
		}
		else if (!strncmp (line, "[sensible_questionmarks]", 24))
		{
			fscanf (fp, "%i", &read_sensible_questionmarks);
		}
		else if (!strncmp (line, "[sensible_solid_lasers]", 23))
		{
			fscanf (fp, "%i", &read_sensible_solid_lasers);
		}
		else if (!strncmp (line, "[filename]", 10))
		{
			if (read_pack_count < MAX_LEVEL_PACKS)
				fscanf (fp, "%s", read_level_packs[read_pack_count].filename);
		}
		else if (!strncmp (line, "[level_reached]", 15))
		{
			if (read_pack_count < MAX_LEVEL_PACKS)
				fscanf (fp, "%i", &read_level_packs[read_pack_count].level_reached);
		}
		else if (!strncmp (line, "[level_selected]", 16))
		{
			if (read_pack_count < MAX_LEVEL_PACKS)
				fscanf (fp, "%i", &read_level_packs[read_pack_count].level_selected);
		}
		else if (!strncmp (line, "[selected]", 10))
		{
			if (read_pack_count < MAX_LEVEL_PACKS)
			{
				fscanf (fp, "%i", &read_level_packs[read_pack_count].selected);
				read_pack_count++;
				if (read_pack_count >= MAX_LEVEL_PACKS)
					fprintf (stdout, "There are too many level packs recorded "
						"within the rcfile (max %i). Possibly there are some "
						"historical entries that you can delete.\n", MAX_LEVEL_PACKS);
			}
		}
		else if (!strncmp (line, "[ACTION_UP.device]", 18))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_UP].device);
		}
		else if (!strncmp (line, "[ACTION_UP.id]", 14))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_UP].id);
		}
		else if (!strncmp (line, "[ACTION_UP.mod]", 15))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_UP].mod);
		}
		/* else if (!strncmp (line, "[ACTION_UP_RIGHT.device]", 24))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_UP_RIGHT].device);
		}
		else if (!strncmp (line, "[ACTION_UP_RIGHT.id]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_UP_RIGHT].id);
		}
		else if (!strncmp (line, "[ACTION_UP_RIGHT.mod]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_UP_RIGHT].mod);
		} temp temp Not currently implemented but maybe in future */
		else if (!strncmp (line, "[ACTION_RIGHT.device]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_RIGHT].device);
		}
		else if (!strncmp (line, "[ACTION_RIGHT.id]", 17))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_RIGHT].id);
		}
		else if (!strncmp (line, "[ACTION_RIGHT.mod]", 18))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_RIGHT].mod);
		}
		/* else if (!strncmp (line, "[ACTION_DOWN_RIGHT.device]", 26))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_DOWN_RIGHT].device);
		}
		else if (!strncmp (line, "[ACTION_DOWN_RIGHT.id]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_DOWN_RIGHT].id);
		}
		else if (!strncmp (line, "[ACTION_DOWN_RIGHT.mod]", 23))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_DOWN_RIGHT].mod);
		} temp temp Not currently implemented but maybe in future */
		else if (!strncmp (line, "[ACTION_DOWN.device]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_DOWN].device);
		}
		else if (!strncmp (line, "[ACTION_DOWN.id]", 16))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_DOWN].id);
		}
		else if (!strncmp (line, "[ACTION_DOWN.mod]", 17))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_DOWN].mod);
		}
		/* else if (!strncmp (line, "[ACTION_DOWN_LEFT.device]", 25))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_DOWN_LEFT].device);
		}
		else if (!strncmp (line, "[ACTION_DOWN_LEFT.id]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_DOWN_LEFT].id);
		}
		else if (!strncmp (line, "[ACTION_DOWN_LEFT.mod]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_DOWN_LEFT].mod);
		} temp temp Not currently implemented but maybe in future */
		else if (!strncmp (line, "[ACTION_LEFT.device]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_LEFT].device);
		}
		else if (!strncmp (line, "[ACTION_LEFT.id]", 16))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_LEFT].id);
		}
		else if (!strncmp (line, "[ACTION_LEFT.mod]", 17))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_LEFT].mod);
		}
		/* else if (!strncmp (line, "[ACTION_UP_LEFT.device]", 23))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_UP_LEFT].device);
		}
		else if (!strncmp (line, "[ACTION_UP_LEFT.id]", 19))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_UP_LEFT].id);
		}
		else if (!strncmp (line, "[ACTION_UP_LEFT.mod]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_UP_LEFT].mod);
		} temp temp Not currently implemented but maybe in future */
		else if (!strncmp (line, "[ACTION_RESTART.device]", 23))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_RESTART].device);
		}
		else if (!strncmp (line, "[ACTION_RESTART.id]", 19))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_RESTART].id);
		}
		else if (!strncmp (line, "[ACTION_RESTART.mod]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_RESTART].mod);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_UP.device]", 24))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_UP].device);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_UP.id]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_UP].id);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_UP.mod]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_UP].mod);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_DOWN.device]", 26))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_DOWN].device);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_DOWN.id]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_DOWN].id);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_DOWN.mod]", 23))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_DOWN].mod);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_LEFT.device]", 26))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_LEFT].device);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_LEFT.id]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_LEFT].id);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_LEFT.mod]", 23))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_LEFT].mod);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_RIGHT.device]", 27))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_RIGHT].device);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_RIGHT.id]", 23))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_RIGHT].id);
		}
		else if (!strncmp (line, "[ACTION_SHOOT_RIGHT.mod]", 24))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SHOOT_RIGHT].mod);
		}
		else if (!strncmp (line, "[ACTION_SELECT.device]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SELECT].device);
		}
		else if (!strncmp (line, "[ACTION_SELECT.id]", 18))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SELECT].id);
		}
		else if (!strncmp (line, "[ACTION_SELECT.mod]", 19))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SELECT].mod);
		}
		else if (!strncmp (line, "[ACTION_EXIT.device]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_EXIT].device);
		}
		else if (!strncmp (line, "[ACTION_EXIT.id]", 16))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_EXIT].id);
		}
		else if (!strncmp (line, "[ACTION_EXIT.mod]", 17))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_EXIT].mod);
		}
		else if (!strncmp (line, "[ACTION_HELP.device]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_HELP].device);
		}
		else if (!strncmp (line, "[ACTION_HELP.id]", 16))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_HELP].id);
		}
		else if (!strncmp (line, "[ACTION_HELP.mod]", 17))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_HELP].mod);
		}
		else if (!strncmp (line, "[ACTION_OPTIONS.device]", 23))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_OPTIONS].device);
		}
		else if (!strncmp (line, "[ACTION_OPTIONS.id]", 19))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_OPTIONS].id);
		}
		else if (!strncmp (line, "[ACTION_OPTIONS.mod]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_OPTIONS].mod);
		}
		else if (!strncmp (line, "[ACTION_TOGGLE_FULLSCREEN.device]", 33))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_TOGGLE_FULLSCREEN].device);
		}
		else if (!strncmp (line, "[ACTION_TOGGLE_FULLSCREEN.id]", 29))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_TOGGLE_FULLSCREEN].id);
		}
		else if (!strncmp (line, "[ACTION_TOGGLE_FULLSCREEN.mod]", 30))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_TOGGLE_FULLSCREEN].mod);
		}
		else if (!strncmp (line, "[ACTION_PREVIOUS_LEVEL.device]", 30))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PREVIOUS_LEVEL].device);
		}
		else if (!strncmp (line, "[ACTION_PREVIOUS_LEVEL.id]", 26))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PREVIOUS_LEVEL].id);
		}
		else if (!strncmp (line, "[ACTION_PREVIOUS_LEVEL.mod]", 27))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PREVIOUS_LEVEL].mod);
		}
		else if (!strncmp (line, "[ACTION_NEXT_LEVEL.device]", 26))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_NEXT_LEVEL].device);
		}
		else if (!strncmp (line, "[ACTION_NEXT_LEVEL.id]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_NEXT_LEVEL].id);
		}
		else if (!strncmp (line, "[ACTION_NEXT_LEVEL.mod]", 23))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_NEXT_LEVEL].mod);
		}
		else if (!strncmp (line, "[ACTION_PREVIOUS_PACK.device]", 29))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PREVIOUS_PACK].device);
		}
		else if (!strncmp (line, "[ACTION_PREVIOUS_PACK.id]", 25))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PREVIOUS_PACK].id);
		}
		else if (!strncmp (line, "[ACTION_PREVIOUS_PACK.mod]", 26))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PREVIOUS_PACK].mod);
		}
		else if (!strncmp (line, "[ACTION_NEXT_PACK.device]", 25))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_NEXT_PACK].device);
		}
		else if (!strncmp (line, "[ACTION_NEXT_PACK.id]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_NEXT_PACK].id);
		}
		else if (!strncmp (line, "[ACTION_NEXT_PACK.mod]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_NEXT_PACK].mod);
		}
		else if (!strncmp (line, "[ACTION_HOME.device]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_HOME].device);
		}
		else if (!strncmp (line, "[ACTION_HOME.id]", 16))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_HOME].id);
		}
		else if (!strncmp (line, "[ACTION_HOME.mod]", 17))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_HOME].mod);
		}
		else if (!strncmp (line, "[ACTION_END.device]", 19))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_END].device);
		}
		else if (!strncmp (line, "[ACTION_END.id]", 15))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_END].id);
		}
		else if (!strncmp (line, "[ACTION_END.mod]", 16))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_END].mod);
		}
		else if (!strncmp (line, "[ACTION_PAGEUP.device]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PAGEUP].device);
		}
		else if (!strncmp (line, "[ACTION_PAGEUP.id]", 18))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PAGEUP].id);
		}
		else if (!strncmp (line, "[ACTION_PAGEUP.mod]", 19))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PAGEUP].mod);
		}
		else if (!strncmp (line, "[ACTION_PAGEDOWN.device]", 24))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PAGEDOWN].device);
		}
		else if (!strncmp (line, "[ACTION_PAGEDOWN.id]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PAGEDOWN].id);
		}
		else if (!strncmp (line, "[ACTION_PAGEDOWN.mod]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PAGEDOWN].mod);
		}
		else if (!strncmp (line, "[ACTION_TOGGLE_DESIGNER.device]", 31))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_TOGGLE_DESIGNER].device);
		}
		else if (!strncmp (line, "[ACTION_TOGGLE_DESIGNER.id]", 27))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_TOGGLE_DESIGNER].id);
		}
		else if (!strncmp (line, "[ACTION_TOGGLE_DESIGNER.mod]", 28))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_TOGGLE_DESIGNER].mod);
		}
		else if (!strncmp (line, "[ACTION_VOLUP.device]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_VOLUP].device);
		}
		else if (!strncmp (line, "[ACTION_VOLUP.id]", 17))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_VOLUP].id);
		}
		else if (!strncmp (line, "[ACTION_VOLUP.mod]", 18))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_VOLUP].mod);
		}
		else if (!strncmp (line, "[ACTION_VOLDOWN.device]", 23))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_VOLDOWN].device);
		}
		else if (!strncmp (line, "[ACTION_VOLDOWN.id]", 19))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_VOLDOWN].id);
		}
		else if (!strncmp (line, "[ACTION_VOLDOWN.mod]", 20))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_VOLDOWN].mod);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER1.device]", 25))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER1].device);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER1.id]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER1].id);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER1.mod]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER1].mod);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER2.device]", 25))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER2].device);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER2.id]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER2].id);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER2.mod]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER2].mod);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER3.device]", 25))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER3].device);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER3.id]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER3].id);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER3.mod]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER3].mod);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER4.device]", 25))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER4].device);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER4.id]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER4].id);
		}
		else if (!strncmp (line, "[ACTION_MODIFIER4.mod]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_MODIFIER4].mod);
		}
		else if (!strncmp (line, "[ACTION_SCROLL_UP.device]", 25))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SCROLL_UP].device);
		}
		else if (!strncmp (line, "[ACTION_SCROLL_UP.id]", 21))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SCROLL_UP].id);
		}
		else if (!strncmp (line, "[ACTION_SCROLL_UP.mod]", 22))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SCROLL_UP].mod);
		}
		else if (!strncmp (line, "[ACTION_SCROLL_DOWN.device]", 27))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SCROLL_DOWN].device);
		}
		else if (!strncmp (line, "[ACTION_SCROLL_DOWN.id]", 23))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SCROLL_DOWN].id);
		}
		else if (!strncmp (line, "[ACTION_SCROLL_DOWN.mod]", 24))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_SCROLL_DOWN].mod);
		}
		else if (!strncmp (line, "[ACTION_PRIMARY_CLICK.device]", 29))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PRIMARY_CLICK].device);
		}
		else if (!strncmp (line, "[ACTION_PRIMARY_CLICK.id]", 25))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PRIMARY_CLICK].id);
		}
		else if (!strncmp (line, "[ACTION_PRIMARY_CLICK.mod]", 26))
		{
			fscanf (fp, "%i", &read_user_controls[ACTION_PRIMARY_CLICK].mod);
		}
	}

	fclose (fp);

	#ifdef DEBUG_READ_RESOURCE_FILE
		printf ("*** Start %s ***\n", __func__);
		printf ("read_game_cycle_limit=%i\n", read_game_cycle_limit);
		printf ("read_game_cycle_delay=%i\n", read_game_cycle_delay);
		printf ("read_default_joystick=%i\n", read_default_joystick);
		printf ("read_default_joystick_name=%s\n", read_default_joystick_name);
		printf ("read_joystick_dead_zone=%i\n", read_joystick_dead_zone);
		printf ("read_key_repeat_delay=%i\n", read_key_repeat_delay);
		printf ("read_key_repeat_interval=%i\n", read_key_repeat_interval);
		printf ("read_selected_skin=%s\n", read_selected_skin);
		printf ("read_selected_locale=%s\n", read_selected_locale);
		printf ("read_save_frequency=%i\n", read_save_frequency);
		printf ("read_op_env.systempointer=%i\n", read_op_env.systempointer);
		printf ("read_pointer_controls.state=%i\n", read_pointer_controls.state);
		printf ("read_pointer_controls.pad_type=%i\n", read_pointer_controls.pad_type);
		printf ("read_sensible_bears=%i\n", read_sensible_bears);
		printf ("read_sensible_questionmarks=%i\n", read_sensible_questionmarks);
		printf ("read_sensible_solid_lasers=%i\n", read_sensible_solid_lasers);
		printf ("read_sound=%i\n", read_sound);
		printf ("read_sfx_vol=%i\n", read_sfx_vol);
		printf ("read_volume=%i\n", read_volume);
		for (count = 0; count < read_pack_count; count++)
		{
			printf ("read_level_packs[%i].filename=%s\n", count,
				read_level_packs[count].filename);
			printf ("read_level_packs[%i].level_reached=%i\n", count,
				read_level_packs[count].level_reached);
			printf ("read_level_packs[%i].level_selected=%i\n", count,
				read_level_packs[count].level_selected);
			printf ("read_level_packs[%i].selected=%i\n", count,
				read_level_packs[count].selected);
		}
		for (count = 0; count < USER_CONTROLS; count++)
		{
			printf ("read_user_controls[%i].device=%i\n", count,
				read_user_controls[count].device);
			printf ("read_user_controls[%i].id=%i\n", count,
				read_user_controls[count].id);
			printf ("read_user_controls[%i].mod=%i\n", count,
				read_user_controls[count].mod);
		}
		printf ("*** Stop %s ***\n", __func__);
	#endif

	/* Now verify that there are no FATAL missing settings.
	 * Forcing the defaults here would erase historical data so we'll
	 * just get the user to fix it */
	for (count = 0; count < read_pack_count; count++)
	{
		if (strcmp (read_level_packs[count].filename, "-1") == 0)
		{
			error_found = TRUE;
			strcpy (error_setting, "[filename]");
		}
		else if (read_level_packs[count].level_reached == UNDEFINED)
		{
			error_found = TRUE;
			strcpy (error_setting, "[level_reached]");
		}
		else if (read_level_packs[count].level_selected == UNDEFINED)
		{
			error_found = TRUE;
			strcpy (error_setting, "[level_selected]");
		}
		else if (read_level_packs[count].selected == UNDEFINED)
		{
			error_found = TRUE;
			strcpy (error_setting, "[selected]");
		}
		if (error_found)
		{
			/* Missing setting found */
			fprintf (stdout, "There is a missing %s setting in %s. ",
				error_setting, filename);
			fprintf (stdout,
				"Please fix it or delete the file to have gnurobbo create a new one. ");
			fprintf (stdout,
				"Deleting the file will reset all levels reached for all packs played!\n");
			return 2;
		}
	}
	
	/* Store settings */
	/* sound stuff */
	if (read_sound != UNDEFINED) sound = read_sound;
	if (read_sfx_vol != UNDEFINED) sfx_vol = read_sfx_vol;
	if (read_volume != UNDEFINED) volume = read_volume;
	/* game_cycle_limit */
#ifdef _SMOOTH_SCRL_	
	if (read_game_cycle_limit != UNDEFINED) {
	game_cycle_limit = read_game_cycle_limit*SCRL_MULT1;
	}
#else
	if (read_game_cycle_limit != UNDEFINED) game_cycle_limit = read_game_cycle_limit;
#endif
	/* game_cycle_delay */
	if (read_game_cycle_delay != UNDEFINED) game_cycle_delay = read_game_cycle_delay;
	/* Joystick */
	if (read_default_joystick != UNDEFINED) default_joystick = read_default_joystick;
	if ((strcmp (read_default_joystick_name, "-1")) != 0)
		strcpy (default_joystick_name, read_default_joystick_name);
	if (read_joystick_dead_zone != UNDEFINED) joystick_dead_zone = read_joystick_dead_zone;
	/* Key repeat */
	if (read_key_repeat_delay != UNDEFINED)
	{
		key_repeat_delay = read_key_repeat_delay;
		for (count = 0; count < USER_CONTROLS; count++)
			user_controls[count].delay = KEY_REPEAT_DELAY;
	}
	if (read_key_repeat_interval != UNDEFINED)
	{
		key_repeat_interval = read_key_repeat_interval;
		for (count = 0; count < USER_CONTROLS; count++)
			user_controls[count].interval = KEY_REPEAT_INTERVAL;
	}
	/* Selected skin */
	if ((strcmp (read_selected_skin, "-1")) != 0)
	{
		for (count = 0; count < skin_count; count++)
		{
			if (!strcmp (skins[count].foldername, read_selected_skin))
			{
				selected_skin = count;
				break;
			}
		}
	}
	/* Selected locale */
	if ((strcmp (read_selected_locale, "-1")) != 0)
	{
		for (count = 0; count < locale_count; count++)
		{
			if (!strcmp (locales[count].foldername, read_selected_locale))
			{
				selected_locale = count;
				break;
			}
		}
	}
	/* rcfile save frequency */
	if (read_save_frequency != UNDEFINED)
		rcfile.save_frequency = read_save_frequency;
	/* System pointer state */
	if (read_op_env.systempointer != UNDEFINED)
		gnurobbo_op_env.systempointer = read_op_env.systempointer;
	/* Pointer controls */
	if (read_pointer_controls.state != UNDEFINED)
		pointer_controls.state = read_pointer_controls.state;
	if (read_pointer_controls.pad_type != UNDEFINED)
		pointer_controls.pad_type = read_pointer_controls.pad_type;
	/* Game mechanics */
	if (read_sensible_bears != UNDEFINED)
		game_mechanics.sensible_bears = read_sensible_bears;
	if (read_sensible_questionmarks != UNDEFINED)
		game_mechanics.sensible_questionmarks = read_sensible_questionmarks;
	if (read_sensible_solid_lasers != UNDEFINED)
		game_mechanics.sensible_solid_lasers = read_sensible_solid_lasers;
	/* User controls */
	for (count = 0; count < USER_CONTROLS; count++)
	{
		if (read_user_controls[count].device != UNDEFINED2)
			user_controls[count].device = read_user_controls[count].device;
		if (read_user_controls[count].id != UNDEFINED2)
			user_controls[count].id = read_user_controls[count].id;
		if (read_user_controls[count].mod != UNDEFINED2)
			user_controls[count].mod = read_user_controls[count].mod;
	}

	/* Now we need to match up the filenames with those already in the pack
	 * list and amend the entries. Any that don't match will be historical
	 * entries i.e. the user played a pack that's now been deleted and we
	 * don't want to lose these */
	for (count = 0; count < read_pack_count; count++)
	{
		found = FALSE;
		for (count2 = 0; count2 < level_pack_count; count2++)
		{
			if (strcmp(level_packs[count2].filename,
				read_level_packs[count].filename) == 0)
			{
				/* A pack from the rc matches with one found so update the pack list */
				level_packs[count2].level_reached =
					read_level_packs[count].level_reached;
				level_packs[count2].level_selected =
					read_level_packs[count].level_selected;
				level_packs[count2].selected = read_level_packs[count].selected;
				found = TRUE;
			}
		}
		if (!found && level_pack_count < MAX_LEVEL_PACKS)
		{
			/* The pack from the rc hasn't been found so add it to
			 * the end of the pack list as an historical entry */
			strcpy (level_packs[level_pack_count].filename,
				read_level_packs[count].filename);
			strcpy (level_packs[level_pack_count].name, "-1");
			level_packs[level_pack_count].last_level = UNDEFINED;
			level_packs[level_pack_count].level_reached =
				read_level_packs[count].level_reached;
			level_packs[level_pack_count].level_selected =
				read_level_packs[count].level_selected;
			level_packs[level_pack_count].selected =
				read_level_packs[count].selected;
			level_pack_count++;
		}
	}

	/* There may be more than one, none or a missing pack selected so we'll fix that here */
	found = FALSE;
	for (count = 0; count < level_pack_count; count++)
	{
		/* If a missing pack is selected then deselect it */
		if (level_packs[count].selected == TRUE
			&& level_packs[count].last_level == UNDEFINED)
		{
			level_packs[count].selected = FALSE;
		}
		else if (level_packs[count].selected == TRUE)
		{
			/* A found pack is selected so deselect all the others */
			selected_pack = count;
			for (count2 = 0; count2 < level_pack_count; count2++)
			{
				/* Don't deselect the selected pack! */
				if (count2 != count) level_packs[count2].selected = FALSE;
			}
			found = TRUE;
		}
		if (found) break;
	}
	/* If no pack was found to be selected then select the default pack */
	if (!found)
	{
		level_packs[0].selected = TRUE;
		selected_pack = 0;
	}

	/* It's also possible that level_selected > level_reached or both
	 * are greater than last_level for found packs so we'll fix that here */
	for (count = 0; count < level_pack_count; count++)
	{
		if (level_packs[count].last_level != UNDEFINED)
		{
			if (level_packs[count].level_reached > level_packs[count].last_level)
				level_packs[count].level_reached = level_packs[count].last_level;
			if (level_packs[count].level_selected > level_packs[count].level_reached)
				level_packs[count].level_selected = level_packs[count].level_reached;
		}
	}

	return 0;
}

/***************************************************************************
 * Save Resource File                                                      *
 ***************************************************************************/
/* Saves resource file data.
 * 
 * On entry: filename is the full path to the rc file
 *           inform = true to inform the user via the console
 *  On exit: returns 1 on error */

int save_resource_file (char *filename, int inform)
{
	FILE *fp;
	int count;

	if ((fp = fopen (filename, "w")) == NULL)
	{
		fprintf (stdout, "Cannot write to file: %s\n", filename);
		return 1;
	}

	if (inform) fprintf (stdout, "Writing to %s\n", filename);

	fprintf (fp, "[version]\n%s\n", VERSION);
	fprintf (fp, "[sound]\n%d\n", sound);
	fprintf (fp, "[sfx_vol]\n%d\n", sfx_vol);
	fprintf (fp, "[volume]\n%d\n",volume);
#ifdef _SMOOTH_SCRL_	
	fprintf (fp, "[game_cycle_limit]\n%i\n", (int)floor(game_cycle_limit/SCRL_MULT1));
#else
	fprintf (fp, "[game_cycle_limit]\n%i\n", game_cycle_limit);

#endif
	fprintf (fp, "[game_cycle_delay]\n%i\n", game_cycle_delay);
	fprintf (fp, "[default_joystick]\n%i\n", default_joystick);
	if (default_joystick == UNDEFINED)
	{
		fprintf (fp, "[default_joystick_name]\n%s\n", "-1");
	}
	else
	{
		fprintf (fp, "[default_joystick_name]\n%s\n", default_joystick_name);
	}
	fprintf (fp, "[joystick_dead_zone]\n%i\n", joystick_dead_zone);
	fprintf (fp, "[key_repeat_delay]\n%i\n", key_repeat_delay);
	fprintf (fp, "[key_repeat_interval]\n%i\n", key_repeat_interval);
	fprintf (fp, "[selected_skin]\n%s\n", skins[selected_skin].foldername);
	fprintf (fp, "[selected_locale]\n%s\n", locales[selected_locale].foldername);
	fprintf (fp, "[save_frequency]\n%i\n", rcfile.save_frequency);
	fprintf (fp, "[op_env.systempointer]\n%i\n", gnurobbo_op_env.systempointer);
	fprintf (fp, "[pointer_controls.state]\n%i\n", pointer_controls.state);
	fprintf (fp, "[pointer_controls.pad_type]\n%i\n", pointer_controls.pad_type);
	fprintf (fp, "[sensible_bears]\n%i\n", game_mechanics.sensible_bears);
	fprintf (fp, "[sensible_questionmarks]\n%i\n", game_mechanics.sensible_questionmarks);
	fprintf (fp, "[sensible_solid_lasers]\n%i\n", game_mechanics.sensible_solid_lasers);
	for (count = 0; count < level_pack_count; count++)
	{
		fprintf (fp, "[filename]\n%s\n", level_packs[count].filename);
		fprintf (fp, "[level_reached]\n%i\n", level_packs[count].level_reached);
		fprintf (fp, "[level_selected]\n%i\n", level_packs[count].level_selected);
		fprintf (fp, "[selected]\n%i\n", level_packs[count].selected);
	}
	fprintf (fp, "[ACTION_UP.device]\n%i\n", user_controls[ACTION_UP].device);
	fprintf (fp, "[ACTION_UP.id]\n%i\n", user_controls[ACTION_UP].id);
	fprintf (fp, "[ACTION_UP.mod]\n%i\n", user_controls[ACTION_UP].mod);
	fprintf (fp, "[ACTION_UP_RIGHT.device]\n%i\n", user_controls[ACTION_UP_RIGHT].device);
	fprintf (fp, "[ACTION_UP_RIGHT.id]\n%i\n", user_controls[ACTION_UP_RIGHT].id);
	fprintf (fp, "[ACTION_UP_RIGHT.mod]\n%i\n", user_controls[ACTION_UP_RIGHT].mod);
	fprintf (fp, "[ACTION_RIGHT.device]\n%i\n", user_controls[ACTION_RIGHT].device);
	fprintf (fp, "[ACTION_RIGHT.id]\n%i\n", user_controls[ACTION_RIGHT].id);
	fprintf (fp, "[ACTION_RIGHT.mod]\n%i\n", user_controls[ACTION_RIGHT].mod);
	fprintf (fp, "[ACTION_DOWN_RIGHT.device]\n%i\n",
		user_controls[ACTION_DOWN_RIGHT].device);
	fprintf (fp, "[ACTION_DOWN_RIGHT.id]\n%i\n", user_controls[ACTION_DOWN_RIGHT].id);
	fprintf (fp, "[ACTION_DOWN_RIGHT.mod]\n%i\n", user_controls[ACTION_DOWN_RIGHT].mod);
	fprintf (fp, "[ACTION_DOWN.device]\n%i\n", user_controls[ACTION_DOWN].device);
	fprintf (fp, "[ACTION_DOWN.id]\n%i\n", user_controls[ACTION_DOWN].id);
	fprintf (fp, "[ACTION_DOWN.mod]\n%i\n", user_controls[ACTION_DOWN].mod);
	fprintf (fp, "[ACTION_DOWN_LEFT.device]\n%i\n",
		user_controls[ACTION_DOWN_LEFT].device);
	fprintf (fp, "[ACTION_DOWN_LEFT.id]\n%i\n", user_controls[ACTION_DOWN_LEFT].id);
	fprintf (fp, "[ACTION_DOWN_LEFT.mod]\n%i\n", user_controls[ACTION_DOWN_LEFT].mod);
	fprintf (fp, "[ACTION_LEFT.device]\n%i\n", user_controls[ACTION_LEFT].device);
	fprintf (fp, "[ACTION_LEFT.id]\n%i\n", user_controls[ACTION_LEFT].id);
	fprintf (fp, "[ACTION_LEFT.mod]\n%i\n", user_controls[ACTION_LEFT].mod);
	fprintf (fp, "[ACTION_UP_LEFT.device]\n%i\n", user_controls[ACTION_UP_LEFT].device);
	fprintf (fp, "[ACTION_UP_LEFT.id]\n%i\n", user_controls[ACTION_UP_LEFT].id);
	fprintf (fp, "[ACTION_UP_LEFT.mod]\n%i\n", user_controls[ACTION_UP_LEFT].mod);
	fprintf (fp, "[ACTION_RESTART.device]\n%i\n", user_controls[ACTION_RESTART].device);
	fprintf (fp, "[ACTION_RESTART.id]\n%i\n", user_controls[ACTION_RESTART].id);
	fprintf (fp, "[ACTION_RESTART.mod]\n%i\n", user_controls[ACTION_RESTART].mod);
	fprintf (fp, "[ACTION_SHOOT_UP.device]\n%i\n", user_controls[ACTION_SHOOT_UP].device);
	fprintf (fp, "[ACTION_SHOOT_UP.id]\n%i\n", user_controls[ACTION_SHOOT_UP].id);
	fprintf (fp, "[ACTION_SHOOT_UP.mod]\n%i\n", user_controls[ACTION_SHOOT_UP].mod);
	fprintf (fp, "[ACTION_SHOOT_DOWN.device]\n%i\n",
		user_controls[ACTION_SHOOT_DOWN].device);
	fprintf (fp, "[ACTION_SHOOT_DOWN.id]\n%i\n", user_controls[ACTION_SHOOT_DOWN].id);
	fprintf (fp, "[ACTION_SHOOT_DOWN.mod]\n%i\n", user_controls[ACTION_SHOOT_DOWN].mod);
	fprintf (fp, "[ACTION_SHOOT_LEFT.device]\n%i\n",
		user_controls[ACTION_SHOOT_LEFT].device);
	fprintf (fp, "[ACTION_SHOOT_LEFT.id]\n%i\n", user_controls[ACTION_SHOOT_LEFT].id);
	fprintf (fp, "[ACTION_SHOOT_LEFT.mod]\n%i\n", user_controls[ACTION_SHOOT_LEFT].mod);
	fprintf (fp, "[ACTION_SHOOT_RIGHT.device]\n%i\n",
		user_controls[ACTION_SHOOT_RIGHT].device);
	fprintf (fp, "[ACTION_SHOOT_RIGHT.id]\n%i\n", user_controls[ACTION_SHOOT_RIGHT].id);
	fprintf (fp, "[ACTION_SHOOT_RIGHT.mod]\n%i\n", user_controls[ACTION_SHOOT_RIGHT].mod);
	fprintf (fp, "[ACTION_SELECT.device]\n%i\n", user_controls[ACTION_SELECT].device);
	fprintf (fp, "[ACTION_SELECT.id]\n%i\n", user_controls[ACTION_SELECT].id);
	fprintf (fp, "[ACTION_SELECT.mod]\n%i\n", user_controls[ACTION_SELECT].mod);
	fprintf (fp, "[ACTION_EXIT.device]\n%i\n", user_controls[ACTION_EXIT].device);
	fprintf (fp, "[ACTION_EXIT.id]\n%i\n", user_controls[ACTION_EXIT].id);
	fprintf (fp, "[ACTION_EXIT.mod]\n%i\n", user_controls[ACTION_EXIT].mod);
	fprintf (fp, "[ACTION_HELP.device]\n%i\n", user_controls[ACTION_HELP].device);
	fprintf (fp, "[ACTION_HELP.id]\n%i\n", user_controls[ACTION_HELP].id);
	fprintf (fp, "[ACTION_HELP.mod]\n%i\n", user_controls[ACTION_HELP].mod);
	fprintf (fp, "[ACTION_OPTIONS.device]\n%i\n", user_controls[ACTION_OPTIONS].device);
	fprintf (fp, "[ACTION_OPTIONS.id]\n%i\n", user_controls[ACTION_OPTIONS].id);
	fprintf (fp, "[ACTION_OPTIONS.mod]\n%i\n", user_controls[ACTION_OPTIONS].mod);
	fprintf (fp, "[ACTION_TOGGLE_FULLSCREEN.device]\n%i\n",
		user_controls[ACTION_TOGGLE_FULLSCREEN].device);
	fprintf (fp, "[ACTION_TOGGLE_FULLSCREEN.id]\n%i\n",
		user_controls[ACTION_TOGGLE_FULLSCREEN].id);
	fprintf (fp, "[ACTION_TOGGLE_FULLSCREEN.mod]\n%i\n",
		user_controls[ACTION_TOGGLE_FULLSCREEN].mod);
	fprintf (fp, "[ACTION_PREVIOUS_LEVEL.device]\n%i\n",
		user_controls[ACTION_PREVIOUS_LEVEL].device);
	fprintf (fp, "[ACTION_PREVIOUS_LEVEL.id]\n%i\n",
		user_controls[ACTION_PREVIOUS_LEVEL].id);
	fprintf (fp, "[ACTION_PREVIOUS_LEVEL.mod]\n%i\n",
		user_controls[ACTION_PREVIOUS_LEVEL].mod);
	fprintf (fp, "[ACTION_NEXT_LEVEL.device]\n%i\n",
		user_controls[ACTION_NEXT_LEVEL].device);
	fprintf (fp, "[ACTION_NEXT_LEVEL.id]\n%i\n", user_controls[ACTION_NEXT_LEVEL].id);
	fprintf (fp, "[ACTION_NEXT_LEVEL.mod]\n%i\n", user_controls[ACTION_NEXT_LEVEL].mod);
	fprintf (fp, "[ACTION_PREVIOUS_PACK.device]\n%i\n",
		user_controls[ACTION_PREVIOUS_PACK].device);
	fprintf (fp, "[ACTION_PREVIOUS_PACK.id]\n%i\n",
		user_controls[ACTION_PREVIOUS_PACK].id);
	fprintf (fp, "[ACTION_PREVIOUS_PACK.mod]\n%i\n",
		user_controls[ACTION_PREVIOUS_PACK].mod);
	fprintf (fp, "[ACTION_NEXT_PACK.device]\n%i\n",
		user_controls[ACTION_NEXT_PACK].device);
	fprintf (fp, "[ACTION_NEXT_PACK.id]\n%i\n", user_controls[ACTION_NEXT_PACK].id);
	fprintf (fp, "[ACTION_NEXT_PACK.mod]\n%i\n", user_controls[ACTION_NEXT_PACK].mod);
	fprintf (fp, "[ACTION_HOME.device]\n%i\n", user_controls[ACTION_HOME].device);
	fprintf (fp, "[ACTION_HOME.id]\n%i\n", user_controls[ACTION_HOME].id);
	fprintf (fp, "[ACTION_HOME.mod]\n%i\n", user_controls[ACTION_HOME].mod);
	fprintf (fp, "[ACTION_END.device]\n%i\n", user_controls[ACTION_END].device);
	fprintf (fp, "[ACTION_END.id]\n%i\n", user_controls[ACTION_END].id);
	fprintf (fp, "[ACTION_END.mod]\n%i\n", user_controls[ACTION_END].mod);
	fprintf (fp, "[ACTION_PAGEUP.device]\n%i\n", user_controls[ACTION_PAGEUP].device);
	fprintf (fp, "[ACTION_PAGEUP.id]\n%i\n", user_controls[ACTION_PAGEUP].id);
	fprintf (fp, "[ACTION_PAGEUP.mod]\n%i\n", user_controls[ACTION_PAGEUP].mod);
	fprintf (fp, "[ACTION_PAGEDOWN.device]\n%i\n", user_controls[ACTION_PAGEDOWN].device);
	fprintf (fp, "[ACTION_PAGEDOWN.id]\n%i\n", user_controls[ACTION_PAGEDOWN].id);
	fprintf (fp, "[ACTION_PAGEDOWN.mod]\n%i\n", user_controls[ACTION_PAGEDOWN].mod);
	fprintf (fp, "[ACTION_TOGGLE_DESIGNER.device]\n%i\n",
		user_controls[ACTION_TOGGLE_DESIGNER].device);
	fprintf (fp, "[ACTION_TOGGLE_DESIGNER.id]\n%i\n",
		user_controls[ACTION_TOGGLE_DESIGNER].id);
	fprintf (fp, "[ACTION_TOGGLE_DESIGNER.mod]\n%i\n",
		user_controls[ACTION_TOGGLE_DESIGNER].mod);
	fprintf (fp, "[ACTION_VOLUP.device]\n%i\n", user_controls[ACTION_VOLUP].device);
	fprintf (fp, "[ACTION_VOLUP.id]\n%i\n", user_controls[ACTION_VOLUP].id);
	fprintf (fp, "[ACTION_VOLUP.mod]\n%i\n", user_controls[ACTION_VOLUP].mod);
	fprintf (fp, "[ACTION_VOLDOWN.device]\n%i\n", user_controls[ACTION_VOLDOWN].device);
	fprintf (fp, "[ACTION_VOLDOWN.id]\n%i\n", user_controls[ACTION_VOLDOWN].id);
	fprintf (fp, "[ACTION_VOLDOWN.mod]\n%i\n", user_controls[ACTION_VOLDOWN].mod);
	fprintf (fp, "[ACTION_MODIFIER1.device]\n%i\n",
		user_controls[ACTION_MODIFIER1].device);
	fprintf (fp, "[ACTION_MODIFIER1.id]\n%i\n", user_controls[ACTION_MODIFIER1].id);
	fprintf (fp, "[ACTION_MODIFIER1.mod]\n%i\n", user_controls[ACTION_MODIFIER1].mod);
	fprintf (fp, "[ACTION_MODIFIER2.device]\n%i\n",
		user_controls[ACTION_MODIFIER2].device);
	fprintf (fp, "[ACTION_MODIFIER2.id]\n%i\n", user_controls[ACTION_MODIFIER2].id);
	fprintf (fp, "[ACTION_MODIFIER2.mod]\n%i\n", user_controls[ACTION_MODIFIER2].mod);
	fprintf (fp, "[ACTION_MODIFIER3.device]\n%i\n",
		user_controls[ACTION_MODIFIER3].device);
	fprintf (fp, "[ACTION_MODIFIER3.id]\n%i\n", user_controls[ACTION_MODIFIER3].id);
	fprintf (fp, "[ACTION_MODIFIER3.mod]\n%i\n", user_controls[ACTION_MODIFIER3].mod);
	fprintf (fp, "[ACTION_MODIFIER4.device]\n%i\n",
		user_controls[ACTION_MODIFIER4].device);
	fprintf (fp, "[ACTION_MODIFIER4.id]\n%i\n", user_controls[ACTION_MODIFIER4].id);
	fprintf (fp, "[ACTION_MODIFIER4.mod]\n%i\n", user_controls[ACTION_MODIFIER4].mod);
	fprintf (fp, "[ACTION_SCROLL_UP.device]\n%i\n",
		user_controls[ACTION_SCROLL_UP].device);
	fprintf (fp, "[ACTION_SCROLL_UP.id]\n%i\n", user_controls[ACTION_SCROLL_UP].id);
	fprintf (fp, "[ACTION_SCROLL_UP.mod]\n%i\n", user_controls[ACTION_SCROLL_UP].mod);
	fprintf (fp, "[ACTION_SCROLL_DOWN.device]\n%i\n",
		user_controls[ACTION_SCROLL_DOWN].device);
	fprintf (fp, "[ACTION_SCROLL_DOWN.id]\n%i\n", user_controls[ACTION_SCROLL_DOWN].id);
	fprintf (fp, "[ACTION_SCROLL_DOWN.mod]\n%i\n", user_controls[ACTION_SCROLL_DOWN].mod);
	fprintf (fp, "[ACTION_PRIMARY_CLICK.device]\n%i\n",
		user_controls[ACTION_PRIMARY_CLICK].device);
	fprintf (fp, "[ACTION_PRIMARY_CLICK.id]\n%i\n",
		user_controls[ACTION_PRIMARY_CLICK].id);
	fprintf (fp, "[ACTION_PRIMARY_CLICK.mod]\n%i\n",
		user_controls[ACTION_PRIMARY_CLICK].mod);

	fclose (fp);

	return 0;
}
