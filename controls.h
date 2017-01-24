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
#define MAX_JOYSTICKS 10
#define MAX_JOYSTICK_NAME_LENGTH 256

#define DEVICE_KEYBOARD 1
#define DEVICE_JOYSTICK 2
#define DEVICE_MOUSE 3

#define DEFAULT_KEY_REPEAT_DELAY 300		/* This is ms */
#define DEFAULT_KEY_REPEAT_INTERVAL 100		/* This is ms */
#define KEY_REPEAT_MIN 20					/* This is ms */
#define KEY_REPEAT_MAX 1000					/* This is ms */

#define KEY_REPEAT_DELAY (ceil((float)key_repeat_delay / (1000 / game_cycle_limit)))		/* This equates to cycles */
#define KEY_REPEAT_INTERVAL (ceil((float)key_repeat_interval / (1000 / game_cycle_limit)))	/* This equates to cycles */

#define JOYSTICK_DEAD_ZONE 50

#define USER_CONTROLS 37

/* User actions. These are used as an index into user_controls */
#define ACTION_UP 0	
#define ACTION_UP_RIGHT 1
#define ACTION_RIGHT 2
#define ACTION_DOWN_RIGHT 3
#define ACTION_DOWN 4
#define ACTION_DOWN_LEFT 5
#define ACTION_LEFT 6
#define ACTION_UP_LEFT 7
#define ACTION_RESTART 8

#define ACTION_SHOOT_UP 9
#define ACTION_SHOOT_DOWN 10
#define ACTION_SHOOT_LEFT 11
#define ACTION_SHOOT_RIGHT 12
#define ACTION_SELECT 13
#define ACTION_EXIT 14
#define ACTION_HELP 15
#define ACTION_OPTIONS 16
#define ACTION_TOGGLE_FULLSCREEN 17

#define ACTION_PREVIOUS_LEVEL 18
#define ACTION_NEXT_LEVEL 19
#define ACTION_PREVIOUS_PACK 20
#define ACTION_NEXT_PACK 21
#define ACTION_HOME 22
#define ACTION_END 23
#define ACTION_PAGEUP 24
#define ACTION_PAGEDOWN 25
#define ACTION_TOGGLE_DESIGNER 26

#define ACTION_VOLUP 27
#define ACTION_VOLDOWN 28
#define ACTION_MODIFIER1 29
#define ACTION_MODIFIER2 30
#define ACTION_MODIFIER3 31
#define ACTION_MODIFIER4 32
#define ACTION_SCROLL_UP 33
#define ACTION_SCROLL_DOWN 34
#define ACTION_PRIMARY_CLICK 35

#define ACTION_NOT_USED1 36		/* Restore Default Controls */

#define ACTION_SHOOT_REPEAT_DELAY_PERIOD (game_cycle_limit / 2)


/* Variables */
SDL_Joystick *joystick;

char default_joystick_name[MAX_JOYSTICK_NAME_LENGTH];
int joystick_count;
char joystick_list[MAX_JOYSTICKS][MAX_JOYSTICK_NAME_LENGTH];

/* Configurable options */
int key_repeat_delay;			/* 20 to 1000 ms */
int temp_key_repeat_delay;		/* 20 to 1000 ms */
int key_repeat_interval;		/* 20 to 1000 ms */
int temp_key_repeat_interval;	/* 20 to 1000 ms */
int default_joystick;
int temp_default_joystick;
int joystick_dead_zone;
int temp_joystick_dead_zone;

struct control {
	int device;			/* Keyboard, joystick or mouse */
	int id;				/* Key or button id */
	int mod;			/* Modifier control */
	int state;			/* Pressed or released */
	int cyclesactive;	/* How many cycles this has been pressed for */
	int delay;			/* The initial delay in cycles before repeating - 0 disables the delay */
	int interval;		/* The repeat interval in cycles - 0 disables the interval */
};
struct control user_controls[USER_CONTROLS];
struct control temp_user_controls[USER_CONTROLS];

ROB_OpEnv gnurobbo_op_env;
ROB_OpEnv temp_gnurobbo_op_env;

/* Function prototypes */
int get_user_action(int *actionid, int pollall, int *device, int *id, int *state);
void show_user_action(int *action_found);
void set_default_user_controls(struct control user_controls[USER_CONTROLS]);
int initialise_joystick(int joyid, char *joyname, int show);
int get_joystick_name(int joyid, char *joyname);
int get_joystick_list(char joystick_list[MAX_JOYSTICKS][MAX_JOYSTICK_NAME_LENGTH], int show);
void get_input_device_text(int device, int id, char *joyname, char *text);
int set_key_repeat(int delay, int interval);



