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
#define DEBUG_CONTROLS
#define DEBUG_SDL_JOYAXISMOTION
#define DEBUG_SDL_ACTIVEEVENT
*/

#define JOYDEADZONE (32767 * joystick_dead_zone / 100)
#define MAX_JOY_AXES 20

/* GP2X button IDs */
#define GP2X_JOY_N 0x00
#define GP2X_JOY_NW 0x01
#define GP2X_JOY_W 0x02
#define GP2X_JOY_SW 0x03
#define GP2X_JOY_S 0x04
#define GP2X_JOY_SE 0x05
#define GP2X_JOY_E 0x06
#define GP2X_JOY_NE 0x07
#define GP2X_START 0x08
#define GP2X_SELECT 0x09
#define GP2X_LTRIG 0x0A
#define GP2X_RTRIG 0x0B
#define GP2X_BTN_A 0x0C
#define GP2X_BTN_B 0x0D
#define GP2X_BTN_X 0x0E
#define GP2X_BTN_Y 0x0F
#define GP2X_VOL_UP 0x10
#define GP2X_VOL_DN 0x11
#define GP2X_BTN_JOY 0x12

/* Zaurus button IDs */
#define ZAURUS_UP SDLK_UP
#define ZAURUS_DOWN SDLK_DOWN
#define ZAURUS_LEFT SDLK_LEFT
#define ZAURUS_RIGHT SDLK_RIGHT
#define ZAURUS_SPACE SDLK_SPACE
#define ZAURUS_CANCEL SDLK_ESCAPE
#define ZAURUS_OK 0
#define ZAURUS_CALENDAR SDLK_F9
#define ZAURUS_ADDRESS SDLK_F10
#define ZAURUS_HOME SDLK_F12
#define ZAURUS_MENU SDLK_F11
#define ZAURUS_EMAIL SDLK_F13

/* PSP button IDs */
#define PSP_TRIANGLE 0
#define PSP_CIRCLE 1
#define PSP_CROSS 2
#define PSP_SQUARE 3
#define PSP_LEFT_TRIGGER 4
#define PSP_RIGHT_TRIGGER 5
#define PSP_DOWN 6
#define PSP_LEFT 7
#define PSP_UP 8
#define PSP_RIGHT 9
#define PSP_SELECT 10
#define PSP_START 11
#define PSP_HOME 12
#define PSP_HOLD 13
#define PSP_JOYSTICK_LEFT 14
#define PSP_JOYSTICK_RIGHT 15
#define PSP_JOYSTICK_UP 16
#define PSP_JOYSTICK_DOWN 17
#define PSP_VIRTUAL_MODIFIER 1000

/* PSVITA button IDs */
#define PSVITA_TRIANGLE 0
#define PSVITA_CIRCLE 1
#define PSVITA_CROSS 2
#define PSVITA_SQUARE 3
#define PSVITA_LEFT_TRIGGER 4
#define PSVITA_RIGHT_TRIGGER 5
#define PSVITA_DOWN 6
#define PSVITA_LEFT 7
#define PSVITA_UP 8
#define PSVITA_RIGHT 9
#define PSVITA_SELECT 10
#define PSVITA_START 11
#define PSVITA_HOME 12
#define PSVITA_LEFT_JOYSTICK_LEFT 13
#define PSVITA_LEFT_JOYSTICK_RIGHT 14
#define PSVITA_LEFT_JOYSTICK_UP 15
#define PSVITA_LEFT_JOYSTICK_DOWN 16
#define PSVITA_RIGHT_JOYSTICK_LEFT 17
#define PSVITA_RIGHT_JOYSTICK_RIGHT 18
#define PSVITA_RIGHT_JOYSTICK_UP 19
#define PSVITA_RIGHT_JOYSTICK_DOWN 20
#define PSVITA_VIRTUAL_MODIFIER 1000

/* Variables */
SDL_Event event;

/* Function prototypes */


/***************************************************************************
 * Get User Action v5.4                                                    *
 ***************************************************************************/
/* This reports whether an action has been triggered via user input.
 * Controls with modifiers take precedence over single key controls.
 * Controls must be unique although you can combine actions in your code.
 * Mixing of input devices and control reconfiguration is trivial.
 * user_controls holds all the controls. Each control has its pressed state
 * recorded so you won't need to do it yourself. It's also possible to
 * gain access to the device, id and state of individual events but in normal
 * operation you should only need to know the action triggered.
 * This function manages key/button repeat which can be different for each
 * control. If you don't want key/button repeat then set delay to -1.
 * It's designed to be called every game cycle because it counts in
 * game cycle units for key/button repeat calculation.
 * The event loop code in END SCREEN in main is almost empty and makes a good
 * template.
 * 
 * To debug this function, define one or all of the following :-
 * DEBUG_CONTROLS, DEBUG_SDL_ACTIVEEVENT, DEBUG_SDL_JOYAXISMOTION
 * 
 * UPDATE: Joystick axis support added. Axes are converted into additional
 * virtual buttons (two per axis) and require a dead zone to be set up which
 * should be user configurable. For example, if a joystick has 10 physical
 * buttons then axis0 will be virtual buttons 10 and 11, axis1 12 and 13 etc.
 * 
 * v5.1 UPDATE: pollall now breaks when an action is triggered to prevent the
 * possibility that an action is pressed and released before exiting (as
 * experienced on my Zaurus).
 * 
 * v5.2 UPDATE: Added mouse support. Initially I was intending to substitute
 * this version with another from an event/object based SDL engine I've been
 * working on but it won't work. In the end adding mouse/stylus support was
 * easier than I was expecting. I've added a ACTION_PRIMARY_CLICK action that
 * can be set-up for use with a mouse or stylus.
 * 
 * v5.3 UPDATE: Added hooks to the GNU Robbo Rectangular Object Engine (ROB).
 * ROB likes to be informed of released and triggered actions so that it can
 * generate events and manage the simulated pointer via the cursor events.
 * 
 * v5.4 UPDATE: Modified the code that deals with analogue axis events so
 * that they become remapped as SDL_JOYBUTTONUP/DOWN virtual button events.
 * 
 * struct control {
 *     int device;          Keyboard, joystick or mouse
 *     int id;              Key or button id
 *     int mod;             Modifier control
 *     int state;           Pressed or released
 *     int cyclesactive;    How many cycles this has been pressed for
 *     int delay;           The initial delay in cycles before repeating -
 *                              0 disables the delay
 *     int interval;        The repeat interval in cycles - 0 disables the interval
 * };
 * struct control user_controls[USER_CONTROLS];    Holds the assignment of user controls 
 * 
 * On entry: actionid will hold any actionid found
 *           pollall = TRUE to poll all events until an action is triggered
 *           pollall = FALSE to poll one event (used to retrieve the device,
 *               id and state for each polled event)
 *           device will hold the device of the last event or UNDEFINED (-1)
 *           id will hold the id of the last event or UNDEFINED
 *           state will hold the state of the last event or UNDEFINED
 *  On exit: actionid will contain an actionid or UNDEFINED
 *           returns 1 on SDL_QUIT else 0 */

int get_user_action (int *actionid, int pollall, int *device, int *id, int *state)
{
	static int axisstates[MAX_JOY_AXES * 2], init = FALSE;
	int found = FALSE, quitfound = FALSE;
	SDL_Event virtualevent;
	int axis_end = 0;
	int rob_actionid;
	int count;

	/* Do some first time variable initialisation */
	if (!init)
	{
		init = TRUE;
		for (count = 0; count < MAX_JOY_AXES; count++)
		{
			axisstates[count * 2] = axisstates[count * 2 + 1] = SDL_RELEASED;
		}
	}
	
	*actionid = UNDEFINED;

	/* Get and process some (not necessarily all) SDL events */
	while (SDL_PollEvent (&event))
	{
		*device = *id = *state = UNDEFINED;
		switch (event.type)
		{
			case SDL_KEYUP:
			case SDL_KEYDOWN:
				*device = DEVICE_KEYBOARD;
				*id = event.key.keysym.sym;
				*state = event.key.state;
				break;
			case SDL_JOYBUTTONUP:
				#if defined(SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS)
				#if defined(PLATFORM_PSVITA) || defined(PLATFORM_PSP)
				lastclick=0;
				#endif
				#endif
			case SDL_JOYBUTTONDOWN:
				*device = DEVICE_JOYSTICK;
				*id = event.jbutton.button;
				*state = event.jbutton.state;

				#if defined(SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS)
				#if defined(PLATFORM_PSVITA)
				if ((event.jbutton.button == PSVITA_CROSS) && (game_mode == DESIGNER_ON))
				{
					*id = user_controls[ACTION_PRIMARY_CLICK].id;
				}
				#endif

				#if defined(PLATFORM_PSP)
				if ((event.jbutton.button == PSP_CROSS) && (game_mode == DESIGNER_ON))
				{
					*id = user_controls[ACTION_PRIMARY_CLICK].id;
				}
				#endif
				#endif
				break;
			case SDL_MOUSEBUTTONUP:
				lastclick=0;	/* Added by neurocyp for the designer */
			case SDL_MOUSEBUTTONDOWN:
				*device = DEVICE_MOUSE;
				*id = event.button.button;
				*state = event.button.state;
				break;
			case SDL_MOUSEMOTION:
				rob_lyr_pointer->x = event.motion.x;
				rob_lyr_pointer->y = event.motion.y;
				kmx = event.motion.x;	/* Added by neurocyp for the designer */
				kmy = event.motion.y;	/* Added by neurocyp for the designer */
				break;
			case SDL_QUIT:
				quitfound = TRUE;
				break;
			#ifdef DEBUG_SDL_ACTIVEEVENT
				case SDL_ACTIVEEVENT:
					printf ("%i: states: ", cycle_count);
					if (event.active.state & SDL_APPMOUSEFOCUS)
						printf ("SDL_APPMOUSEFOCUS ");
					if (event.active.state & SDL_APPINPUTFOCUS)
						printf ("SDL_APPINPUTFOCUS ");
					if (event.active.state & SDL_APPACTIVE)
						printf ("SDL_APPACTIVE ");
					printf (": gain=%i\n", event.active.gain);
					break;
			#endif
			case SDL_JOYAXISMOTION:
				/* The axis motion will be reported as a value from -32768 to +32767
				 * with 0 being the theoretical middle point. Some analogue joysticks
				 * are actually digital and offer limited analogue functionality by
				 * firing off a few values throughout this range (these joysticks will
				 * tend to rest at 0) whilst a truly analogue joystick will fire lots
				 * of different values with the middle firing anything but 0. The
				 * JOYDEADZONE offers us a reasonable user configurable middle region.
				 * 
				 * To simulate digital on/off button functionality from an analogue
				 * axis we need to record previous axis motion activity so that we can
				 * see if we've already generated an SDL_JOYBUTTONDOWN/UP event. This
				 * activity is stored within the axisstates array as pairs of values
				 * representing both ends of an axis, with each value belonging to a
				 * virtual button. The first integer records the up/down state of the
				 * negative end of an axis with the positive end stored in the other.
				 * 
				 * Following are the situations that this code will account for :-
				 *   SDL_JOYBUTTONDOWN - axis middle to any axis end
				 *   SDL_JOYBUTTONDOWN - one axis end to another axis end with the
				 *   opposite end generating SDL_JOYBUTTONUP
				 *   SDL_JOYBUTTONUP - any axis end to the middle */

				#ifdef DEBUG_SDL_JOYAXISMOTION
					printf ("%i: axis=%i value=%i\n", cycle_count,
						event.jaxis.axis, event.jaxis.value);
				#endif

				if (abs (event.jaxis.value) >= JOYDEADZONE)
				{
					/* Identify which end of the axis we are operating upon:
					 * 0 for -32768 to JOYDEADZONE, 1 for JOYDEADZONE to +32767 */
					axis_end = 0;
					if (event.jaxis.value >= JOYDEADZONE) axis_end = 1;

					if (axisstates[event.jaxis.axis * 2 + axis_end] == SDL_RELEASED)
					{
						/* Generate a virtual joystick button event */
						virtualevent.jbutton.button = event.jaxis.axis * 2 +
							axis_end + SDL_JoystickNumButtons (joystick);
						virtualevent.type = SDL_JOYBUTTONDOWN;
						virtualevent.jbutton.state = SDL_PRESSED;
						SDL_PushEvent(&virtualevent);

						/* Record its SDL_PRESSED state within axisstates */
						axisstates[event.jaxis.axis * 2 + axis_end] = SDL_PRESSED;
					}

					/* Do we need to release the opposite end of the axis? */
					if (axisstates[event.jaxis.axis * 2 + (1 - axis_end)] == SDL_PRESSED)
					{
						/* Generate a virtual joystick button event */
						virtualevent.jbutton.button = event.jaxis.axis * 2 +
							(1 - axis_end) + SDL_JoystickNumButtons (joystick);
						virtualevent.type = SDL_JOYBUTTONUP;
						virtualevent.jbutton.state = SDL_RELEASED;
						SDL_PushEvent(&virtualevent);

						/* Record its SDL_RELEASED state within axisstates */
						axisstates[event.jaxis.axis * 2 + (1 - axis_end)] = SDL_RELEASED;
					}
				}
				else
				{
					/* Do we need to release either end of the axis? */
					for (count = 0; count < 2; count++)
					{
						if (axisstates[event.jaxis.axis * 2 + count] == SDL_PRESSED)
						{
							/* Generate a virtual joystick button event */
							virtualevent.jbutton.button = event.jaxis.axis * 2 +
								count + SDL_JoystickNumButtons (joystick);
							virtualevent.type = SDL_JOYBUTTONUP;
							virtualevent.jbutton.state = SDL_RELEASED;
							SDL_PushEvent(&virtualevent);

							/* Record its SDL_RELEASED state within axisstates */
							axisstates[event.jaxis.axis * 2 + count] = SDL_RELEASED;
						}
					}
				}
				break;
			default:
				break;
		}

		found = FALSE;
		if (*device != UNDEFINED)
		{
			/* Iterate through user_controls and record the state for all matching controls */
			#ifdef DEBUG_CONTROLS
				printf ("device=%i; id=%03i; state=%i; all control states:", *device, *id, *state);
			#endif
			for (count = 0; count < USER_CONTROLS; count++)
			{
				if (user_controls[count].device == *device && user_controls[count].id == *id)
				{
					user_controls[count].state = *state;
					if (*state == SDL_RELEASED)
					{
						user_controls[count].cyclesactive = 0;
						/* ROB will want to know what action was released. Actually this
						 * isn't reporting action releases but control releases which is
						 * not the same thing. The reporting of action releases was not
						 * part of the requirements when designing this version of
						 * get_user_action() for GNU Robbo, but what is done here more
						 * than suffices */
						rob_actionid = count | 0x80;
						ROB_GenerateEvents (&rob_actionid);
					}
					found = TRUE;
				}
				#ifdef DEBUG_CONTROLS
					printf (" %i", user_controls[count].state);
				#endif
			}
			#ifdef DEBUG_CONTROLS
				printf ("\n");
			#endif

			if (found || !pollall) break;	/* Stop polling events now as we've got something to work with */
		}
	}

	/* Now we know what's pressed we can see if an action has been triggered.
	 * Controls with modifiers take precedence so search for these first */
	found = FALSE;
	for (count = 0; count < USER_CONTROLS; count++)
	{
		if (user_controls[count].state == SDL_PRESSED &&
			user_controls[count].mod != UNDEFINED &&
			user_controls[user_controls[count].mod].state == SDL_PRESSED)
		{
			found = TRUE;
			*actionid = count;
			#ifdef DEBUG_CONTROLS
				show_user_action (actionid);
			#endif
			break;
		}
	}
	if (!found)
	{
		/* Now search for controls without modifiers */
		for (count = 0; count < USER_CONTROLS; count++)
		{
			if (user_controls[count].state == SDL_PRESSED &&
				user_controls[count].mod == UNDEFINED)
			{
				found = TRUE;
				*actionid = count;
				#ifdef DEBUG_CONTROLS
					show_user_action (actionid);
				#endif
				break;
			}
		}
	}

	/* Because this function is called every game cycle, if the user
	 * is pressing a control then it would trigger an action every game
	 * cycle. cyclesactive, delay and interval are used to decide the
	 * frequency of individual key/button repeats */
	if (found)
	{
		if (user_controls[*actionid].cyclesactive == 0)
		{
			/* Allow this action */
		}
		else if (user_controls[*actionid].cyclesactive == user_controls[*actionid].delay)
		{
			/* Allow this action */
		}
		else if (user_controls[*actionid].cyclesactive > user_controls[*actionid].delay)
		{
			if (user_controls[*actionid].interval == 0)
			{
				/* Allow this action */
			}
			else if ((user_controls[*actionid].cyclesactive -
				user_controls[*actionid].delay) % user_controls[*actionid].interval == 0)
			{
				/* Allow this action */
			}
			else
			{
				found = FALSE;
			}
		}
		else
		{
			found = FALSE;
		}
		user_controls[*actionid].cyclesactive++;
		if (!found) *actionid = UNDEFINED;
	}

	/* ROB will want to know what action was triggered */
	ROB_GenerateEvents (actionid);

	return quitfound;
}

/***************************************************************************
 * Show User Action                                                        *
 ***************************************************************************/
/* Used for debugging */

void show_user_action (int *actionid)
{
	switch (*actionid)
	{
		case ACTION_UP:
			printf ("ACTION_UP");
			break;
		case ACTION_UP_RIGHT:
			printf ("ACTION_UP_RIGHT");
			break;
		case ACTION_RIGHT:
			printf ("ACTION_RIGHT");
			break;
		case ACTION_DOWN_RIGHT:
			printf ("ACTION_DOWN_RIGHT");
			break;
		case ACTION_DOWN:
			printf ("ACTION_DOWN");
			break;
		case ACTION_DOWN_LEFT:
			printf ("ACTION_DOWN_LEFT");
			break;
		case ACTION_LEFT:
			printf ("ACTION_LEFT");
			break;
		case ACTION_UP_LEFT:
			printf ("ACTION_UP_LEFT");
			break;
		case ACTION_SHOOT_UP:
			printf ("ACTION_SHOOT_UP");
			break;
		case ACTION_SHOOT_DOWN:
			printf ("ACTION_SHOOT_DOWN");
			break;
		case ACTION_SHOOT_LEFT:
			printf ("ACTION_SHOOT_LEFT");
			break;
		case ACTION_SHOOT_RIGHT:
			printf ("ACTION_SHOOT_RIGHT");
			break;
		case ACTION_SELECT:
			printf ("ACTION_SELECT");
			break;
		case ACTION_EXIT:
			printf ("ACTION_EXIT");
			break;
		case ACTION_HELP:
			printf ("ACTION_HELP");
			break;
		case ACTION_OPTIONS:
			printf ("ACTION_OPTIONS");
			break;
		case ACTION_RESTART:
			printf ("ACTION_RESTART");
			break;
		case ACTION_PREVIOUS_LEVEL:
			printf ("ACTION_PREVIOUS_LEVEL");
			break;
		case ACTION_NEXT_LEVEL:
			printf ("ACTION_NEXT_LEVEL");
			break;
		case ACTION_PREVIOUS_PACK:
			printf ("ACTION_PREVIOUS_PACK");
			break;
		case ACTION_NEXT_PACK:
			printf ("ACTION_NEXT_PACK");
			break;
		case ACTION_TOGGLE_FULLSCREEN:
			printf ("ACTION_TOGGLE_FULLSCREEN");
			break;
		case ACTION_HOME:
			printf ("ACTION_HOME");
			break;
		case ACTION_END:
			printf ("ACTION_END");
			break;
		case ACTION_PAGEUP:
			printf ("ACTION_PAGEUP");
			break;
		case ACTION_PAGEDOWN:
			printf ("ACTION_PAGEDOWN");
			break;
		case ACTION_TOGGLE_DESIGNER:
			printf ("ACTION_TOGGLE_DESIGNER");
			break;
		case ACTION_VOLUP:
			printf ("ACTION_VOLUP");
			break;
		case ACTION_VOLDOWN:
			printf ("ACTION_VOLDOWN");
			break;
		case ACTION_MODIFIER1:
			printf ("ACTION_MODIFIER1");
			break;
		case ACTION_MODIFIER2:
			printf ("ACTION_MODIFIER2");
			break;
		case ACTION_MODIFIER3:
			printf ("ACTION_MODIFIER3");
			break;
		case ACTION_MODIFIER4:
			printf ("ACTION_MODIFIER4");
			break;
		case ACTION_SCROLL_UP:
			printf ("ACTION_SCROLL_UP");
			break;
		case ACTION_SCROLL_DOWN:
			printf ("ACTION_SCROLL_DOWN");
			break;
		case ACTION_PRIMARY_CLICK:
			printf ("ACTION_PRIMARY_CLICK");
			break;
		case ACTION_NOT_USED1:
			printf ("ACTION_NOT_USED1");
			break;
		case UNDEFINED:
			printf ("UNDEFINED");
			break;
		default:
			printf ("Unknown action");
			break;
	}
	printf (": cyclesactive=%i\n", user_controls[*actionid].cyclesactive);
}

/***************************************************************************
 * Set Default User Controls                                               *
 ***************************************************************************/

void set_default_user_controls (struct control user_controls[USER_CONTROLS])
{
	int count;

	/* Set some defaults */
	for (count = 0; count < USER_CONTROLS; count++)
	{
		#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
			user_controls[count].device = DEVICE_KEYBOARD;
		#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
			user_controls[count].device = DEVICE_JOYSTICK;
		#elif defined(PLATFORM_ZAURUS)
			user_controls[count].device = DEVICE_KEYBOARD;
		#elif defined(PLATFORM_FREMANTLE)
			user_controls[count].device = DEVICE_KEYBOARD;
		#elif defined(PLATFORM_PSP)
			user_controls[count].device = DEVICE_JOYSTICK;
		#elif defined(PLATFORM_PSVITA)
			user_controls[count].device = DEVICE_JOYSTICK;
		#endif
		user_controls[count].id = UNDEFINED;
		user_controls[count].mod = UNDEFINED;
		user_controls[count].state = SDL_RELEASED;
		user_controls[count].cyclesactive = 0;
		user_controls[count].delay = KEY_REPEAT_DELAY;
		user_controls[count].interval = KEY_REPEAT_INTERVAL;
	}

	/* Now tune to a platform. NOTE that so far device is the only property
	 * that has been initialised to something (keyboard or joystick) so if
	 * you don't want to use a control then just set its device to UNDEFINED
	 * (id and mod are already set to UNDEFINED from the above code) otherwise
	 * within Options you'll see the device shown on its own which looks weird */
	#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
		user_controls[ACTION_UP].id = SDLK_UP;
		user_controls[ACTION_UP_RIGHT].device = UNDEFINED;
		user_controls[ACTION_RIGHT].id = SDLK_RIGHT;
		user_controls[ACTION_DOWN_RIGHT].device = UNDEFINED;
		user_controls[ACTION_DOWN].id = SDLK_DOWN;
		user_controls[ACTION_DOWN_LEFT].device = UNDEFINED;
		user_controls[ACTION_LEFT].id = SDLK_LEFT;
		user_controls[ACTION_UP_LEFT].device = UNDEFINED;
		user_controls[ACTION_RESTART].id = SDLK_r;
		user_controls[ACTION_SHOOT_UP].id = SDLK_UP;
		user_controls[ACTION_SHOOT_UP].mod = ACTION_MODIFIER2;
		user_controls[ACTION_SHOOT_DOWN].id = SDLK_DOWN;
		user_controls[ACTION_SHOOT_DOWN].mod = ACTION_MODIFIER2;
		user_controls[ACTION_SHOOT_LEFT].id = SDLK_LEFT;
		user_controls[ACTION_SHOOT_LEFT].mod = ACTION_MODIFIER2;
		user_controls[ACTION_SHOOT_RIGHT].id = SDLK_RIGHT;
		user_controls[ACTION_SHOOT_RIGHT].mod = ACTION_MODIFIER2;
		user_controls[ACTION_SELECT].id = SDLK_RETURN;
		user_controls[ACTION_EXIT].id = SDLK_ESCAPE;
		user_controls[ACTION_HELP].id = SDLK_F1;
		user_controls[ACTION_OPTIONS].id = SDLK_F4;
		user_controls[ACTION_TOGGLE_FULLSCREEN].id = SDLK_RETURN;
		user_controls[ACTION_TOGGLE_FULLSCREEN].mod = ACTION_MODIFIER1;
		user_controls[ACTION_PREVIOUS_LEVEL].id = SDLK_F5;
		user_controls[ACTION_NEXT_LEVEL].id = SDLK_F6;
		user_controls[ACTION_PREVIOUS_PACK].id = SDLK_F7;
		user_controls[ACTION_NEXT_PACK].id = SDLK_F8;
		user_controls[ACTION_HOME].id = SDLK_HOME;
		user_controls[ACTION_END].id = SDLK_END;
		user_controls[ACTION_PAGEUP].id = SDLK_PAGEUP;
		user_controls[ACTION_PAGEDOWN].id = SDLK_PAGEDOWN;
		user_controls[ACTION_TOGGLE_DESIGNER].id = SDLK_F9;
		user_controls[ACTION_VOLUP].id = SDLK_EQUALS;
		user_controls[ACTION_VOLDOWN].id = SDLK_MINUS;
		user_controls[ACTION_MODIFIER1].id = SDLK_LALT;
		user_controls[ACTION_MODIFIER2].id = SDLK_RSHIFT;
		user_controls[ACTION_MODIFIER3].device = UNDEFINED;
		user_controls[ACTION_MODIFIER4].device = UNDEFINED;
		user_controls[ACTION_SCROLL_UP].device = DEVICE_MOUSE;
		user_controls[ACTION_SCROLL_UP].id = SDL_BUTTON_WHEELUP;
		user_controls[ACTION_SCROLL_DOWN].device = DEVICE_MOUSE;
		user_controls[ACTION_SCROLL_DOWN].id = SDL_BUTTON_WHEELDOWN;
		user_controls[ACTION_PRIMARY_CLICK].device = DEVICE_MOUSE;
		user_controls[ACTION_PRIMARY_CLICK].id = SDL_BUTTON_LEFT;
		user_controls[ACTION_NOT_USED1].device = UNDEFINED;
	#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
		user_controls[ACTION_UP].id = GP2X_JOY_N;
		user_controls[ACTION_UP_RIGHT].device = UNDEFINED;
		user_controls[ACTION_RIGHT].id = GP2X_JOY_E;
		user_controls[ACTION_DOWN_RIGHT].device = UNDEFINED;
		user_controls[ACTION_DOWN].id = GP2X_JOY_S;
		user_controls[ACTION_DOWN_LEFT].device = UNDEFINED;
		user_controls[ACTION_LEFT].id = GP2X_JOY_W;
		user_controls[ACTION_UP_LEFT].device = UNDEFINED;
		user_controls[ACTION_RESTART].id = GP2X_SELECT;
		user_controls[ACTION_SHOOT_UP].id = GP2X_JOY_N;
		user_controls[ACTION_SHOOT_UP].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_DOWN].id = GP2X_JOY_S;
		user_controls[ACTION_SHOOT_DOWN].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_LEFT].id = GP2X_JOY_W;
		user_controls[ACTION_SHOOT_LEFT].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_RIGHT].id = GP2X_JOY_E;
		user_controls[ACTION_SHOOT_RIGHT].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SELECT].id = GP2X_BTN_B;
		user_controls[ACTION_EXIT].id = GP2X_BTN_X;
		user_controls[ACTION_HELP].id = GP2X_BTN_Y;
		user_controls[ACTION_HELP].mod = ACTION_MODIFIER1;
		user_controls[ACTION_OPTIONS].id = GP2X_START;
		user_controls[ACTION_TOGGLE_FULLSCREEN].device = UNDEFINED;
		user_controls[ACTION_PREVIOUS_LEVEL].id = GP2X_LTRIG;
		user_controls[ACTION_PREVIOUS_LEVEL].mod = ACTION_MODIFIER1;
		user_controls[ACTION_NEXT_LEVEL].id = GP2X_RTRIG;
		user_controls[ACTION_NEXT_LEVEL].mod = ACTION_MODIFIER1;
		user_controls[ACTION_PREVIOUS_PACK].id = GP2X_LTRIG;
		user_controls[ACTION_PREVIOUS_PACK].mod = ACTION_MODIFIER2;
		user_controls[ACTION_NEXT_PACK].id = GP2X_RTRIG;
		user_controls[ACTION_NEXT_PACK].mod = ACTION_MODIFIER2;
		user_controls[ACTION_HOME].id = GP2X_VOL_DN;
		user_controls[ACTION_HOME].mod = ACTION_MODIFIER1;
		user_controls[ACTION_END].id = GP2X_VOL_UP;
		user_controls[ACTION_END].mod = ACTION_MODIFIER1;
		user_controls[ACTION_PAGEUP].id = GP2X_LTRIG;
		user_controls[ACTION_PAGEDOWN].id = GP2X_RTRIG;
		user_controls[ACTION_TOGGLE_DESIGNER].device = UNDEFINED;
		user_controls[ACTION_VOLUP].id = GP2X_VOL_UP;
		user_controls[ACTION_VOLDOWN].id = GP2X_VOL_DN;
		user_controls[ACTION_MODIFIER1].id = GP2X_BTN_A;
		user_controls[ACTION_MODIFIER2].id = GP2X_BTN_Y;
		user_controls[ACTION_MODIFIER3].device = UNDEFINED;
		user_controls[ACTION_MODIFIER4].device = UNDEFINED;
		user_controls[ACTION_SCROLL_UP].device = UNDEFINED;
		user_controls[ACTION_SCROLL_DOWN].device = UNDEFINED;
		#ifdef HAVE_DESIGNER
			/* F100 doesn't support the mouse, F200 does (or can) so
			 * what happens here depends on HAVE_DESIGNER in the Makefile */
			user_controls[ACTION_PRIMARY_CLICK].device = DEVICE_MOUSE;
			user_controls[ACTION_PRIMARY_CLICK].id = SDL_BUTTON_LEFT;
		#else
			user_controls[ACTION_PRIMARY_CLICK].device = UNDEFINED;
		#endif
		user_controls[ACTION_NOT_USED1].device = UNDEFINED;
	#elif defined(PLATFORM_ZAURUS)
		user_controls[ACTION_UP].id = ZAURUS_UP;
		user_controls[ACTION_UP_RIGHT].device = UNDEFINED;
		user_controls[ACTION_RIGHT].id = ZAURUS_RIGHT;
		user_controls[ACTION_DOWN_RIGHT].device = UNDEFINED;
		user_controls[ACTION_DOWN].id = ZAURUS_DOWN;
		user_controls[ACTION_DOWN_LEFT].device = UNDEFINED;
		user_controls[ACTION_LEFT].id = ZAURUS_LEFT;
		user_controls[ACTION_UP_LEFT].device = UNDEFINED;
		user_controls[ACTION_RESTART].id = ZAURUS_HOME;
		user_controls[ACTION_SHOOT_UP].id = ZAURUS_UP;
		user_controls[ACTION_SHOOT_UP].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_DOWN].id = ZAURUS_DOWN;
		user_controls[ACTION_SHOOT_DOWN].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_LEFT].id = ZAURUS_LEFT;
		user_controls[ACTION_SHOOT_LEFT].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_RIGHT].id = ZAURUS_RIGHT;
		user_controls[ACTION_SHOOT_RIGHT].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SELECT].id = ZAURUS_SPACE;
		user_controls[ACTION_EXIT].id = ZAURUS_CANCEL;
		user_controls[ACTION_HELP].id = ZAURUS_MENU;
		user_controls[ACTION_OPTIONS].id = ZAURUS_EMAIL;
		user_controls[ACTION_TOGGLE_FULLSCREEN].device = UNDEFINED;
		user_controls[ACTION_PREVIOUS_LEVEL].id = ZAURUS_CALENDAR;
		user_controls[ACTION_PREVIOUS_LEVEL].mod = ACTION_MODIFIER1;
		user_controls[ACTION_NEXT_LEVEL].id = ZAURUS_ADDRESS;
		user_controls[ACTION_NEXT_LEVEL].mod = ACTION_MODIFIER1;
		user_controls[ACTION_PREVIOUS_PACK].device = UNDEFINED;
		user_controls[ACTION_NEXT_PACK].device = UNDEFINED;
		user_controls[ACTION_HOME].device = UNDEFINED;
		user_controls[ACTION_END].device = UNDEFINED;
		user_controls[ACTION_PAGEUP].id = ZAURUS_CALENDAR;
		user_controls[ACTION_PAGEDOWN].id = ZAURUS_ADDRESS;
		user_controls[ACTION_TOGGLE_DESIGNER].device = UNDEFINED;
		user_controls[ACTION_VOLUP].device = UNDEFINED;
		user_controls[ACTION_VOLDOWN].device = UNDEFINED;
		user_controls[ACTION_MODIFIER1].id = ZAURUS_SPACE;
		user_controls[ACTION_MODIFIER2].device = UNDEFINED;
		user_controls[ACTION_MODIFIER3].device = UNDEFINED;
		user_controls[ACTION_MODIFIER4].device = UNDEFINED;
		user_controls[ACTION_SCROLL_UP].device = UNDEFINED;
		user_controls[ACTION_SCROLL_DOWN].device = UNDEFINED;
		user_controls[ACTION_PRIMARY_CLICK].device = DEVICE_MOUSE;
		user_controls[ACTION_PRIMARY_CLICK].id = SDL_BUTTON_LEFT;
		user_controls[ACTION_NOT_USED1].device = UNDEFINED;
	#elif defined(PLATFORM_FREMANTLE)
		user_controls[ACTION_UP].id = SDLK_UP;
		user_controls[ACTION_UP_RIGHT].device = UNDEFINED;
		user_controls[ACTION_RIGHT].id = SDLK_RIGHT;
		user_controls[ACTION_DOWN_RIGHT].device = UNDEFINED;
		user_controls[ACTION_DOWN].id = SDLK_DOWN;
		user_controls[ACTION_DOWN_LEFT].device = UNDEFINED;
		user_controls[ACTION_LEFT].id = SDLK_LEFT;
		user_controls[ACTION_UP_LEFT].device = UNDEFINED;
		user_controls[ACTION_RESTART].id = SDLK_r;
		user_controls[ACTION_SHOOT_UP].id = SDLK_UP;
		user_controls[ACTION_SHOOT_UP].mod = ACTION_MODIFIER2;
		user_controls[ACTION_SHOOT_DOWN].id = SDLK_DOWN;
		user_controls[ACTION_SHOOT_DOWN].mod = ACTION_MODIFIER2;
		user_controls[ACTION_SHOOT_LEFT].id = SDLK_LEFT;
		user_controls[ACTION_SHOOT_LEFT].mod = ACTION_MODIFIER2;
		user_controls[ACTION_SHOOT_RIGHT].id = SDLK_RIGHT;
		user_controls[ACTION_SHOOT_RIGHT].mod = ACTION_MODIFIER2;
		user_controls[ACTION_SELECT].id = SDLK_KP_ENTER;
		user_controls[ACTION_EXIT].id = SDLK_BACKSPACE;
		user_controls[ACTION_HELP].id = SDLK_h;
		user_controls[ACTION_OPTIONS].id = SDLK_o;
		user_controls[ACTION_TOGGLE_FULLSCREEN].id = SDLK_RETURN;
		user_controls[ACTION_TOGGLE_FULLSCREEN].mod = ACTION_MODIFIER1;
		user_controls[ACTION_PREVIOUS_LEVEL].id = SDLK_F5;
		user_controls[ACTION_NEXT_LEVEL].id = SDLK_F6;
		user_controls[ACTION_PREVIOUS_PACK].id = SDLK_F7;
		user_controls[ACTION_NEXT_PACK].id = SDLK_F8;
		user_controls[ACTION_HOME].id = SDLK_HOME;
		user_controls[ACTION_END].id = SDLK_END;
		user_controls[ACTION_PAGEUP].id = SDLK_PAGEUP;
		user_controls[ACTION_PAGEDOWN].id = SDLK_PAGEDOWN;
		user_controls[ACTION_TOGGLE_DESIGNER].id = SDLK_F9;
		user_controls[ACTION_VOLUP].id = SDLK_EQUALS;
		user_controls[ACTION_VOLDOWN].id = SDLK_MINUS;
		user_controls[ACTION_MODIFIER1].id = SDLK_MODE;
		user_controls[ACTION_MODIFIER2].id = SDLK_LSHIFT;
		user_controls[ACTION_MODIFIER3].device = UNDEFINED;
		user_controls[ACTION_MODIFIER4].device = UNDEFINED;
		user_controls[ACTION_SCROLL_UP].device = DEVICE_MOUSE;
		user_controls[ACTION_SCROLL_UP].id = SDL_BUTTON_WHEELUP;
		user_controls[ACTION_SCROLL_DOWN].device = DEVICE_MOUSE;
		user_controls[ACTION_SCROLL_DOWN].id = SDL_BUTTON_WHEELDOWN;
		user_controls[ACTION_PRIMARY_CLICK].device = DEVICE_MOUSE;
		user_controls[ACTION_PRIMARY_CLICK].id = SDL_BUTTON_LEFT;
		user_controls[ACTION_NOT_USED1].device = UNDEFINED;
	#elif defined(PLATFORM_PSP)
		user_controls[ACTION_UP].id = PSP_UP;
		user_controls[ACTION_UP_RIGHT].device = UNDEFINED;
		user_controls[ACTION_RIGHT].id = PSP_RIGHT;
		user_controls[ACTION_DOWN_RIGHT].device = UNDEFINED;
		user_controls[ACTION_DOWN].id = PSP_DOWN;
		user_controls[ACTION_DOWN_LEFT].device = UNDEFINED;
		user_controls[ACTION_LEFT].id = PSP_LEFT;
		user_controls[ACTION_UP_LEFT].device = UNDEFINED;
		user_controls[ACTION_RESTART].id = PSP_START;
		user_controls[ACTION_SHOOT_UP].id = PSP_UP;
		user_controls[ACTION_SHOOT_UP].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_DOWN].id = PSP_DOWN;
		user_controls[ACTION_SHOOT_DOWN].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_LEFT].id = PSP_LEFT;
		user_controls[ACTION_SHOOT_LEFT].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_RIGHT].id = PSP_RIGHT;
		user_controls[ACTION_SHOOT_RIGHT].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SELECT].id = PSP_CROSS;
		user_controls[ACTION_EXIT].id = PSP_CIRCLE;
		user_controls[ACTION_HELP].device = UNDEFINED;
		user_controls[ACTION_OPTIONS].device = UNDEFINED;
		user_controls[ACTION_TOGGLE_FULLSCREEN].device = UNDEFINED;
		user_controls[ACTION_PREVIOUS_LEVEL].device = UNDEFINED;
		user_controls[ACTION_NEXT_LEVEL].device = UNDEFINED;
		user_controls[ACTION_PREVIOUS_PACK].id = PSP_LEFT_TRIGGER;
		user_controls[ACTION_NEXT_PACK].id = PSP_RIGHT_TRIGGER;
		user_controls[ACTION_HOME].device = UNDEFINED;
		user_controls[ACTION_END].device = UNDEFINED;
		user_controls[ACTION_PAGEUP].device = UNDEFINED;
		user_controls[ACTION_PAGEDOWN].device = UNDEFINED;
		user_controls[ACTION_TOGGLE_DESIGNER].id = PSP_TRIANGLE;
		user_controls[ACTION_VOLUP].device = UNDEFINED;
		user_controls[ACTION_VOLDOWN].device = UNDEFINED;
		user_controls[ACTION_MODIFIER1].id = PSP_CROSS;
		user_controls[ACTION_MODIFIER2].device = UNDEFINED;
		user_controls[ACTION_MODIFIER3].device = UNDEFINED;
		user_controls[ACTION_MODIFIER4].device = UNDEFINED;
		user_controls[ACTION_SCROLL_UP].id = PSP_JOYSTICK_UP;
		user_controls[ACTION_SCROLL_DOWN].id = PSP_JOYSTICK_DOWN;
		user_controls[ACTION_PRIMARY_CLICK].id = PSP_VIRTUAL_MODIFIER;
		#ifdef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
		user_controls[ACTION_SCROLL_LEFT].id = PSP_JOYSTICK_LEFT;
		user_controls[ACTION_SCROLL_RIGHT].id = PSP_JOYSTICK_RIGHT;
		#endif
		user_controls[ACTION_NOT_USED1].device = UNDEFINED;
	#elif defined(PLATFORM_PSVITA)
		user_controls[ACTION_UP].id = PSVITA_UP;
		user_controls[ACTION_UP_RIGHT].device = UNDEFINED;
		user_controls[ACTION_RIGHT].id = PSVITA_RIGHT;
		user_controls[ACTION_DOWN_RIGHT].device = UNDEFINED;
		user_controls[ACTION_DOWN].id = PSVITA_DOWN;
		user_controls[ACTION_DOWN_LEFT].device = UNDEFINED;
		user_controls[ACTION_LEFT].id = PSVITA_LEFT;
		user_controls[ACTION_UP_LEFT].device = UNDEFINED;
		user_controls[ACTION_RESTART].id = PSVITA_START;
		user_controls[ACTION_SHOOT_UP].id = PSVITA_UP;
		user_controls[ACTION_SHOOT_UP].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_DOWN].id = PSVITA_DOWN;
		user_controls[ACTION_SHOOT_DOWN].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_LEFT].id = PSVITA_LEFT;
		user_controls[ACTION_SHOOT_LEFT].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SHOOT_RIGHT].id = PSVITA_RIGHT;
		user_controls[ACTION_SHOOT_RIGHT].mod = ACTION_MODIFIER1;
		user_controls[ACTION_SELECT].id = PSVITA_CROSS;
		user_controls[ACTION_EXIT].id = PSVITA_CIRCLE;
		user_controls[ACTION_HELP].device = UNDEFINED;
		user_controls[ACTION_OPTIONS].device = UNDEFINED;
		user_controls[ACTION_PREVIOUS_LEVEL].device = UNDEFINED;
		user_controls[ACTION_NEXT_LEVEL].device = UNDEFINED;
		user_controls[ACTION_PREVIOUS_PACK].id = PSVITA_LEFT_TRIGGER;;
		user_controls[ACTION_NEXT_PACK].id = PSVITA_RIGHT_TRIGGER;
		user_controls[ACTION_HOME].device = UNDEFINED;
		user_controls[ACTION_END].device = UNDEFINED;
		user_controls[ACTION_PAGEUP].device = UNDEFINED;
		user_controls[ACTION_PAGEDOWN].device = UNDEFINED;
		user_controls[ACTION_TOGGLE_DESIGNER].id = PSVITA_TRIANGLE;
		user_controls[ACTION_VOLUP].device = UNDEFINED;
		user_controls[ACTION_VOLDOWN].device = UNDEFINED;
		user_controls[ACTION_MODIFIER1].id = PSVITA_CROSS;
		user_controls[ACTION_MODIFIER2].device = PSVITA_SQUARE;
		user_controls[ACTION_MODIFIER3].device = UNDEFINED;
		user_controls[ACTION_MODIFIER4].device = UNDEFINED;
		user_controls[ACTION_SCROLL_UP].id = PSVITA_LEFT_JOYSTICK_UP;
		user_controls[ACTION_SCROLL_DOWN].id = PSVITA_LEFT_JOYSTICK_DOWN;
		user_controls[ACTION_PRIMARY_CLICK].id = PSVITA_VIRTUAL_MODIFIER;
		#ifdef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
		user_controls[ACTION_SCROLL_LEFT].id = PSVITA_LEFT_JOYSTICK_LEFT;
		user_controls[ACTION_SCROLL_RIGHT].id = PSVITA_LEFT_JOYSTICK_RIGHT;
		#endif
		user_controls[ACTION_NOT_USED1].device = UNDEFINED;
	#endif
}

/***************************************************************************
 * Set Key Repeat                                                          *
 ***************************************************************************/
/* Enable or disable keyboard key repeat.
 * 
 * On entry: delay = the delay before repeating or 0 to disable
 *           interval = the repeat interval
 *  On exit: returns 1 on error else 0 */

int set_key_repeat (int delay, int interval)
{
	if (SDL_EnableKeyRepeat (delay, interval) != 0)
	{
		fprintf (stdout, "Error setting key repeat: %s\n", SDL_GetError ());
		return 1;
	}

	return 0;
}

/***************************************************************************
 * Initialise Joystick                                                     *
 ***************************************************************************/
/* Enable a joystick for input. It's designed so that it's possible to restore
 * a saved joyid and joyname from an rc file and attempt to match them up
 * taking into account somebody might have two joysticks that may have been
 * switched.
 * 
 * On entry: joyid = UNDEFINED (-1) for any joystick with joyname irrelevant
 *           joyid = 0 to n for a specific joystick by joyid only (joyname = "-1")
 *           joyid = 0 to n for a specific joystick by joyid and/or joyname with
 *               joyname taking priority (joyname != "-1")
 *           show = TRUE to dump output to the terminal
 *  On exit: returns the joyid if successful (zero based) with joyname = joystick name
 *           returns UNDEFINED (-1) if unsuccessful with joyname = "-1" */

int initialise_joystick (int joyid, char *joyname, int show)
{
	int count, number_found, found = FALSE;

	number_found = SDL_NumJoysticks ();	/* Get number of joysticks attached */

	/* Are any joysticks available? */
	if (number_found > 0)
	{
		/* WARNING: attempting to close joystick 0 on the GP2X
		 * using the GPH SDK causes a seg fault */
		#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
			if (joystick)
			{
		#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
			if ((joystick) && ((SDL_JoystickIndex (joystick)) > 0))
			{
		#elif defined(PLATFORM_ZAURUS)
			if (joystick)
			{
		#elif defined(PLATFORM_FREMANTLE)
			if (joystick)
			{
		#elif defined(PLATFORM_PSP)
			if (joystick)
			{
		#elif defined(PLATFORM_PSVITA)
			if (joystick)
			{
		#endif
			if (show) fprintf (stdout, "Joystick closed: %i:%s\n",
				SDL_JoystickIndex (joystick),
				SDL_JoystickName (SDL_JoystickIndex (joystick)));
			SDL_JoystickClose (joystick);
		}

		/* Enable joystick event processing */
		SDL_JoystickEventState (SDL_ENABLE);

		/* Enable any joystick? */
		if (joyid == UNDEFINED)
		{
			for (count = 0; count < number_found; count++)
			{
				if ((joystick = SDL_JoystickOpen (count)))
				{
					if (show) fprintf (stdout, "Joystick opened: %i:%s\n",
						count, SDL_JoystickName (count));
					joyid = count;
					found = TRUE;
					break;
				}
				else
				{
				if (show) fprintf (stdout, "Couldn't open joystick %i:%s!\n",
					count, SDL_JoystickName (count));
				}
			}
			if (!found)
			{
				if (show) fprintf (stdout, "Couldn't open any of %i joystick(s)!\n",
					number_found);
				joyid = UNDEFINED;
			}

		}
		else if (!strcmp (joyname, "-1"))	/* Enable a joystick by joyid only? */
		{
			if ((joystick = SDL_JoystickOpen (joyid)))
			{
				if (show) fprintf (stdout, "Joystick opened: %i:%s\n",
					joyid, SDL_JoystickName (joyid));
			}
			else
			{
				if (show) fprintf (stdout, "Couldn't open joystick %i\n", joyid);
				joyid = UNDEFINED;
			}

		}
		else	/* Enable a joystick by joyid and/or joyname */
		{
			/* Firstly, attempt to find an exact match on joyid and joyname */
			for (count = 0; count < number_found; count++)
			{
				if ((joystick = SDL_JoystickOpen (count)))
				{
					if (joyid == count && !strcmp (joyname, SDL_JoystickName (count)))
					{
						if (show) fprintf (stdout, "Joystick opened: %i:%s\n",
							count, SDL_JoystickName (count));
						joyid = count;
						found = TRUE;
						break;
					}
				}
			}
			if (!found)
			{
				/* Exact match not found so search for joyname only */
				for (count = 0; count < number_found; count++)
				{
					if ((joystick = SDL_JoystickOpen (count)))
					{
						if (!strcmp (joyname, SDL_JoystickName (count)))
						{
							if (show) fprintf (stdout, "Joystick opened: %i:%s\n",
								count, SDL_JoystickName (count));
							joyid = count;
							found = TRUE;
							break;
						}
					}
				}
				if (!found)
				{
					if (show) fprintf (stdout, "Couldn't find joystick %i:%s\n",
						joyid, joyname);
					joyid = UNDEFINED;
				}
			}
		}
	}
	else
	{
		if (show) fprintf (stdout, "There is no joystick to initialise\n");
		joyid = UNDEFINED;
	}

	/* Flush events as opening a joystick releases all the buttons */
	if (joystick) while (SDL_PollEvent (&event));

	get_joystick_name (joyid, joyname);
	return joyid;
}

/***************************************************************************
 * Get Joystick Name                                                       *
 ***************************************************************************/
/* Gets a joystick name that isn't NULL ;) It's also truncated to
 * MAX_JOYSTICK_NAME_LENGTH which is currently 256 characters.
 * 
 * On entry: joyid = 0 to n for a specific joystick
 *  On exit: returns 0 if successful with joyname = joystick name
 *           returns 1 on error with joyname = "-1" */

int get_joystick_name (int joyid, char *joyname)
{
	if (SDL_JoystickName (joyid) != NULL)
	{
		strncpy (joyname, SDL_JoystickName (joyid), MAX_JOYSTICK_NAME_LENGTH);
		/* Force string null termination if max characters were copied */
		joyname[MAX_JOYSTICK_NAME_LENGTH - 1] = 0;
		return 0;
	}
	else
	{
		strcpy (joyname, "-1");
		return 1;
	}
}

/***************************************************************************
 * Get Joystick List                                                       *
 ***************************************************************************/
/* Get a list of attached joysticks if any. I have found that SDL does not
 * update the list if you subsequently plug or unplug a joystick.
 * 
 * On entry: joystick_list is an array of strings
 *           show = TRUE to dump output to the terminal
 *  On exit: returns number of joysticks found */

int get_joystick_list (char joystick_list[MAX_JOYSTICKS][MAX_JOYSTICK_NAME_LENGTH],
	int show)
{
	int count, number_found;

	for (count = 0; count < MAX_JOYSTICKS; count++)
		strcpy (joystick_list[count], "");

	number_found = SDL_NumJoysticks ();	/* Get number of joysticks attached */
	if (number_found > MAX_JOYSTICKS) number_found = MAX_JOYSTICKS;

	if (number_found)
	{
		SDL_JoystickEventState (SDL_ENABLE);	/* Enable joystick event processing */

		/* Initialise joystick_list */
		for (count = 0; count < number_found; count++)
		{
			get_joystick_name (count, joystick_list[count]);
			if (show) fprintf (stdout, "Joystick found: %i:%s\n",
				count, joystick_list[count]);
		}
	}
	else
	{
		if (show) fprintf (stdout, "No joystick found\n");
	}

	#ifdef DEBUG_CONTROLS
		strcpy (joystick_list[number_found++], "Micro Shaft Pretend Joystick");
		strcpy (joystick_list[number_found++], "Grevious Gamepad Pro");
		strcpy (joystick_list[number_found++], "Cheapo Joypad");
	#endif

	return number_found;
}

/***************************************************************************
 * Get Input Device Text                                                   *
 ***************************************************************************/
/* Translates an SDLKey value into a string for display in Options.
 * 
 * On entry: device = input device
 *           id = keysym.sym
 *           joyname = a string containing a possible joystick name
 *           text = the return string
 *  On exit: text = either a key/button legend or the id in text */

void get_input_device_text (int device, int id, char *joyname, char *text)
{
	if (device == DEVICE_MOUSE)
	{
		switch (id)
		{
			case SDL_BUTTON_LEFT:
				strcpy (text, txt_ptr_BtnLeft);
				break;
			case SDL_BUTTON_MIDDLE:
				strcpy (text, txt_ptr_BtnMiddle);
				break;
			case SDL_BUTTON_RIGHT:
				strcpy (text, txt_ptr_BtnRight);
				break;
			case SDL_BUTTON_WHEELUP:
				strcpy (text, txt_ptr_WheelUp);
				break;
			case SDL_BUTTON_WHEELDOWN:
				strcpy (text, txt_ptr_WheelDown);
				break;
			default:
				sprintf (text, "%i", id);
				break;
		}
	}
	else if (device == DEVICE_KEYBOARD)
	{
		switch (id)
		{
			case SDLK_UNKNOWN:
				strcpy (text, "SDLUnknown");
				break;
			case SDLK_BACKSPACE:
				strcpy (text, txt_key_Backspace);
				break;
			case SDLK_TAB:
				strcpy (text, txt_key_Tab);
				break;
			case SDLK_CLEAR:
				strcpy (text, txt_key_Clear);
				break;
			case SDLK_RETURN:
				strcpy (text, txt_key_Return);
				break;
			case SDLK_PAUSE:
				strcpy (text, txt_key_Pause);
				break;
			case SDLK_ESCAPE:
				strcpy (text, txt_key_Escape);
				break;
			case SDLK_SPACE:
				strcpy (text, txt_key_Space);
				break;
			case SDLK_EXCLAIM:
				strcpy (text, "!");
				break;
			case SDLK_QUOTEDBL:
				strcpy (text, "\"");
				break;
			case SDLK_HASH:
				strcpy (text, "#");
				break;
			case SDLK_DOLLAR:
				strcpy (text, "$");
				break;
			case SDLK_AMPERSAND:
				strcpy (text, "&");
				break;
			case SDLK_QUOTE:
				strcpy (text, "'");
				break;
			case SDLK_LEFTPAREN:
				strcpy (text, "(");
				break;
			case SDLK_RIGHTPAREN:
				strcpy (text, ")");
				break;
			case SDLK_ASTERISK:
				strcpy (text, "*");
				break;
			case SDLK_PLUS:
				strcpy (text, "+");
				break;
			case SDLK_COMMA:
				strcpy (text, ",");
				break;
			case SDLK_MINUS:
				strcpy (text, "-");
				break;
			case SDLK_PERIOD:
				strcpy (text, ".");
				break;
			case SDLK_SLASH:
				strcpy (text, "/");
				break;
			case SDLK_0:
				strcpy (text, "0");
				break;
			case SDLK_1:
				strcpy (text, "1");
				break;
			case SDLK_2:
				strcpy (text, "2");
				break;
			case SDLK_3:
				strcpy (text, "3");
				break;
			case SDLK_4:
				strcpy (text, "4");
				break;
			case SDLK_5:
				strcpy (text, "5");
				break;
			case SDLK_6:
				strcpy (text, "6");
				break;
			case SDLK_7:
				strcpy (text, "7");
				break;
			case SDLK_8:
				strcpy (text, "8");
				break;
			case SDLK_9:
				strcpy (text, "9");
				break;
			case SDLK_COLON:
				strcpy (text, ":");
				break;
			case SDLK_SEMICOLON:
				strcpy (text, ";");
				break;
			case SDLK_LESS:
				strcpy (text, "<");
				break;
			case SDLK_EQUALS:
				strcpy (text, "=");
				break;
			case SDLK_GREATER:
				strcpy (text, ">");
				break;
			case SDLK_QUESTION:
				strcpy (text, "?");
				break;
			case SDLK_AT:
				strcpy (text, "@");
				break;
			case SDLK_LEFTBRACKET:
				strcpy (text, "[");
				break;
			case SDLK_BACKSLASH:
				strcpy (text, "\\");
				break;
			case SDLK_RIGHTBRACKET:
				strcpy (text, "]");
				break;
			case SDLK_CARET:
				strcpy (text, "^");
				break;
			case SDLK_UNDERSCORE:
				strcpy (text, "_");
				break;
			case SDLK_BACKQUOTE:
				strcpy (text, "`");
				break;
			case SDLK_a:
				strcpy (text, "A");
				break;
			case SDLK_b:
				strcpy (text, "B");
				break;
			case SDLK_c:
				strcpy (text, "C");
				break;
			case SDLK_d:
				strcpy (text, "D");
				break;
			case SDLK_e:
				strcpy (text, "E");
				break;
			case SDLK_f:
				strcpy (text, "F");
				break;
			case SDLK_g:
				strcpy (text, "G");
				break;
			case SDLK_h:
				strcpy (text, "H");
				break;
			case SDLK_i:
				strcpy (text, "I");
				break;
			case SDLK_j:
				strcpy (text, "J");
				break;
			case SDLK_k:
				strcpy (text, "K");
				break;
			case SDLK_l:
				strcpy (text, "L");
				break;
			case SDLK_m:
				strcpy (text, "M");
				break;
			case SDLK_n:
				strcpy (text, "N");
				break;
			case SDLK_o:
				strcpy (text, "O");
				break;
			case SDLK_p:
				strcpy (text, "P");
				break;
			case SDLK_q:
				strcpy (text, "Q");
				break;
			case SDLK_r:
				strcpy (text, "R");
				break;
			case SDLK_s:
				strcpy (text, "S");
				break;
			case SDLK_t:
				strcpy (text, "T");
				break;
			case SDLK_u:
				strcpy (text, "U");
				break;
			case SDLK_v:
				strcpy (text, "V");
				break;
			case SDLK_w:
				strcpy (text, "W");
				break;
			case SDLK_x:
				strcpy (text, "X");
				break;
			case SDLK_y:
				strcpy (text, "Y");
				break;
			case SDLK_z:
				strcpy (text, "Z");
				break;
			case SDLK_DELETE:
				strcpy (text, txt_key_Delete);
				break;
			case SDLK_KP0:
				sprintf (text, "%s0", txt_key_KP);
				break;
			case SDLK_KP1:
				sprintf (text, "%s1", txt_key_KP);
				break;
			case SDLK_KP2:
				sprintf (text, "%s2", txt_key_KP);
				break;
			case SDLK_KP3:
				sprintf (text, "%s3", txt_key_KP);
				break;
			case SDLK_KP4:
				sprintf (text, "%s4", txt_key_KP);
				break;
			case SDLK_KP5:
				sprintf (text, "%s5", txt_key_KP);
				break;
			case SDLK_KP6:
				sprintf (text, "%s6", txt_key_KP);
				break;
			case SDLK_KP7:
				sprintf (text, "%s7", txt_key_KP);
				break;
			case SDLK_KP8:
				sprintf (text, "%s8", txt_key_KP);
				break;
			case SDLK_KP9:
				sprintf (text, "%s9", txt_key_KP);
				break;
			case SDLK_KP_PERIOD:
				sprintf (text, "%s.", txt_key_KP);
				break;
			case SDLK_KP_DIVIDE:
				sprintf (text, "%s/", txt_key_KP);
				break;
			case SDLK_KP_MULTIPLY:
				sprintf (text, "%s*", txt_key_KP);
				break;
			case SDLK_KP_MINUS:
				sprintf (text, "%s-", txt_key_KP);
				break;
			case SDLK_KP_PLUS:
				sprintf (text, "%s+", txt_key_KP);
				break;
			case SDLK_KP_ENTER:
				sprintf (text, "%sEnter", txt_key_KP);
				break;
			case SDLK_KP_EQUALS:
				sprintf (text, "%s=", txt_key_KP);
				break;
			case SDLK_UP:
				strcpy (text, txt_key_Up);
				break;
			case SDLK_DOWN:
				strcpy (text, txt_key_Down);
				break;
			case SDLK_RIGHT:
				strcpy (text, txt_key_Right);
				break;
			case SDLK_LEFT:
				strcpy (text, txt_key_Left);
				break;
			case SDLK_INSERT:
				strcpy (text, txt_key_Insert);
				break;
			case SDLK_HOME:
				strcpy (text, txt_key_Home);
				break;
			case SDLK_END:
				strcpy (text, txt_key_End);
				break;
			case SDLK_PAGEUP:
				strcpy (text, txt_key_PgUp);
				break;
			case SDLK_PAGEDOWN:
				strcpy (text, txt_key_PgDn);
				break;
			case SDLK_F1:
				strcpy (text, "F1");
				break;
			case SDLK_F2:
				strcpy (text, "F2");
				break;
			case SDLK_F3:
				strcpy (text, "F3");
				break;
			case SDLK_F4:
				strcpy (text, "F4");
				break;
			case SDLK_F5:
				strcpy (text, "F5");
				break;
			case SDLK_F6:
				strcpy (text, "F6");
				break;
			case SDLK_F7:
				strcpy (text, "F7");
				break;
			case SDLK_F8:
				strcpy (text, "F8");
				break;
			case SDLK_F9:
				strcpy (text, "F9");
				break;
			case SDLK_F10:
				strcpy (text, "F10");
				break;
			case SDLK_F11:
				strcpy (text, "F11");
				break;
			case SDLK_F12:
				strcpy (text, "F12");
				break;
			case SDLK_F13:
				strcpy (text, "F13");
				break;
			case SDLK_F14:
				strcpy (text, "F14");
				break;
			case SDLK_F15:
				strcpy (text, "F15");
				break;
			case SDLK_NUMLOCK:
				strcpy (text, txt_key_NumLk);
				break;
			case SDLK_CAPSLOCK:
				strcpy (text, txt_key_CapsLk);
				break;
			case SDLK_SCROLLOCK:
				strcpy (text, txt_key_ScrlLk);
				break;
			case SDLK_RSHIFT:
				strcpy (text, txt_key_RShift);
				break;
			case SDLK_LSHIFT:
				strcpy (text, txt_key_LShift);
				break;
			case SDLK_RCTRL:
				strcpy (text, txt_key_RCtrl);
				break;
			case SDLK_LCTRL:
				strcpy (text, txt_key_LCtrl);
				break;
			case SDLK_RALT:
				strcpy (text, txt_key_RAlt);
				break;
			case SDLK_LALT:
				strcpy (text, txt_key_LAlt);
				break;
			case SDLK_RMETA:
				strcpy (text, txt_key_RMeta);
				break;
			case SDLK_LMETA:
				strcpy (text, txt_key_LMeta);
				break;
			case SDLK_LSUPER:
				strcpy (text, txt_key_LSuper);
				break;
			case SDLK_RSUPER:
				strcpy (text, txt_key_RSuper);
				break;
			case SDLK_MODE:
				strcpy (text, txt_key_AltGr);
				break;
			case SDLK_COMPOSE:
				strcpy (text, txt_key_Compose);
				break;
			case SDLK_HELP:
				strcpy (text, txt_key_Help);
				break;
			case SDLK_PRINT:
				strcpy (text, txt_key_PrScr);
				break;
			case SDLK_SYSREQ:
				strcpy (text, txt_key_SysRq);
				break;
			case SDLK_BREAK:
				strcpy (text, txt_key_Break);
				break;
			case SDLK_MENU:
				strcpy (text, txt_key_Menu);
				break;
			case SDLK_POWER:
				strcpy (text, txt_key_Power);
				break;
			case SDLK_EURO:
				strcpy (text, txt_key_Euro);
				break;
			case SDLK_UNDO:
				strcpy (text, txt_key_Undo);
				break;
			default:
				sprintf (text, "%i", id);
				/*strcpy(text, "Unknown"); */
				break;
		}
	}
	else if (device == DEVICE_JOYSTICK)
	{
		if (strcmp (joyname, "Microsoft SideWinder Game Pad Pro USB version 1.0") == 0)
		{	/* Has a pseudo analogue (digital) D-pad with 2 axes
			* that's good for testing with but not much else ;) */
			switch (id)
			{
				case 0:
					strcpy (text, "A");
					break;
				case 1:
					strcpy (text, "B");
					break;
				case 2:
					strcpy (text, "C");
					break;
				case 3:
					strcpy (text, "X");
					break;
				case 4:
					strcpy (text, "Y");
					break;
				case 5:
					strcpy (text, "Z");
					break;
				case 6:
					strcpy (text, "LTrig");
					break;
				case 7:
					strcpy (text, "RTrig");
					break;
				case 8:
					strcpy (text, "Shift");
					break;
				case 10:
					strcpy (text, "Left");
					break;
				case 11:
					strcpy (text, "Right");
					break;
				case 12:
					strcpy (text, "Up");
					break;
				case 13:
					strcpy (text, "Down");
					break;
				default:
					sprintf (text, "%i", id);
					/*strcpy(text, "Unknown"); */
					break;
			}
		}
		else if (strcmp (joyname, "MM809 Commander Pad") == 0)
		{	/* Manta MM-809 */
			switch (id)
			{
				case 0:
					strcpy (text, "1");
					break;
				case 1:
					strcpy (text, "2");
					break;
				case 2:
					strcpy (text, "3");
					break;
				case 3:
					strcpy (text, "4");
					break;
				case 4:
					strcpy (text, "5");
					break;
				case 5:
					strcpy (text, "6");
					break;
				case 6:
					strcpy (text, "7");
					break;
				case 7:
					strcpy (text, "8");
					break;
				case 8:
					strcpy (text, "9");
					break;
				case 9:
					strcpy (text, "10");
					break;
				case 10:
					strcpy (text, "11");
					break;
				case 11:
					strcpy (text, "12");
					break;
				case 12:
					strcpy (text, "Left");
					break;
				case 13:
					strcpy (text, "Right");
					break;
				case 14:
					strcpy (text, "Up");
					break;
				case 15:
					strcpy (text, "Down");
					break;
				default:
					sprintf (text, "%i", id);
					break;
			}
		}
		else if ((strcmp (joyname, "PEP Joy") == 0) || (strcmp (joyname, "GP2X_JOYSTICK") == 0))
		{	/* GP2X F100/F200 joystick */
			switch (id)
			{
				case GP2X_JOY_N:
					strcpy (text, "Up");
					break;
				case GP2X_JOY_NW:
					strcpy (text, "UpLeft");
					break;
				case GP2X_JOY_W:
					strcpy (text, "Left");
					break;
				case GP2X_JOY_SW:
					strcpy (text, "DownLeft");
					break;
				case GP2X_JOY_S:
					strcpy (text, "Down");
					break;
				case GP2X_JOY_SE:
					strcpy (text, "DownRight");
					break;
				case GP2X_JOY_E:
					strcpy (text, "Right");
					break;
				case GP2X_JOY_NE:
					strcpy (text, "UpRight");
					break;
				case GP2X_START:
					strcpy (text, "Start");
					break;
				case GP2X_SELECT:
					strcpy (text, "Select");
					break;
				case GP2X_LTRIG:
					strcpy (text, "LTrig");
					break;
				case GP2X_RTRIG:
					strcpy (text, "RTrig");
					break;
				case GP2X_BTN_A:
					strcpy (text, "A");
					break;
				case GP2X_BTN_B:
					strcpy (text, "B");
					break;
				case GP2X_BTN_X:
					strcpy (text, "X");
					break;
				case GP2X_BTN_Y:
					strcpy (text, "Y");
					break;
				case GP2X_VOL_UP:
					strcpy (text, "VolUp");
					break;
				case GP2X_VOL_DN:
					strcpy (text, "VolDn");
					break;
				case GP2X_BTN_JOY:
					strcpy (text, "JoyPush");
					break;
				default:
					sprintf (text, "%i", id);
					/*strcpy(text, "Unknown"); */
					break;
			}
		}
		else
		{
			sprintf (text, "%i", id);
		}
	}
	else
	{
		sprintf (text, "%i", device);
	}
}


