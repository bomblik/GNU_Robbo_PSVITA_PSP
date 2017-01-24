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
#define DEBUG_POINTER_CONTROLS
*/

#define BUTTON_ALPHA_NORMAL 48
#define BUTTON_ALPHA_MOVE_SHOOT 80
#define BUTTON_COLOUR_SHOOT_PRESS 0xff8000
#define BUTTON_COLOUR_SHOOT_CLICK 0xff0000
#define BUTTON_COLOUR_DIRECTIONS 0xffffff
#define BUTTON_COLOUR_HIGHLIGHT 0x505050

/* Variables */
SDL_Rect toolbar_intro_screen_image_srcrect;
SDL_Rect toolbar_options_image_srcrect;
SDL_Rect toolbar_designer_image_srcrect;
SDL_Rect toolbar_restart_image_srcrect;
SDL_Rect toolbar_hide_image_srcrect;

/* Function prototypes */


/***************************************************************************
 * Pointer Controls Event Processor                                        *
 ***************************************************************************/
/* This receives events from the ROB engine for the pointer controls */

void pointer_controls_event_processor(ROB_Event *rob_event) {
	int move_actionid = 0, shoot_actionid = 0, set = FALSE;
		
	#ifdef DEBUG_POINTER_CONTROLS
		ROB_ShowEvent(rob_event, __func__);
	#endif

	/* We're only interested in pointer controls events */
	if (rob_event->rob_object->id >= POINTER_CONTROLS_ID &&
		rob_event->rob_object->id <= POINTER_CONTROLS_TOOLBAR_HIDE_ID) {

		/* Most of this is state changing but if something needs to be executed on-click
		   then it is done from here */
		switch (rob_event->rob_object->id) {
			case POINTER_CONTROLS_ID:
				if (rob_event->klasse == ROB_EVENT_CLICK) {
				 	pointer_controls.state = POINTER_CONTROLS_MODE_ON;
		 			show_pointer_controls_toolbar();
				} else if (rob_event->klasse == ROB_EVENT_RELEASE) {
					if (pointer_controls.shoot_state == POINTER_CONTROLS_SHOOT_OFF_DRAG) {
						pointer_controls.shoot_state = POINTER_CONTROLS_SHOOT_OFF;
					} else if (pointer_controls.shoot_state == POINTER_CONTROLS_SHOOT_CLICK_DRAG) {
						pointer_controls.shoot_state = POINTER_CONTROLS_SHOOT_CLICK;
					}
				}
				break;
			case POINTER_CONTROLS_SHOOT_ID:
				if (rob_event->klasse == ROB_EVENT_PRESS) {
					if (pointer_controls.shoot_state == POINTER_CONTROLS_SHOOT_OFF) {
						pointer_controls.shoot_state = POINTER_CONTROLS_SHOOT_OFF_DRAG;
					} else if (pointer_controls.shoot_state == POINTER_CONTROLS_SHOOT_CLICK) {
						pointer_controls.shoot_state = POINTER_CONTROLS_SHOOT_CLICK_DRAG;
					}
				} else if (rob_event->klasse == ROB_EVENT_CLICK) {
					if (pointer_controls.shoot_state == POINTER_CONTROLS_SHOOT_OFF_DRAG) {
						pointer_controls.shoot_state = POINTER_CONTROLS_SHOOT_CLICK;
					} else if (pointer_controls.shoot_state == POINTER_CONTROLS_SHOOT_CLICK_DRAG) {
						pointer_controls.shoot_state = POINTER_CONTROLS_SHOOT_OFF;
					}
				}
				break;
			case POINTER_CONTROLS_UP_ID:
				move_actionid = ACTION_UP; shoot_actionid = ACTION_SHOOT_UP; set = TRUE;
			case POINTER_CONTROLS_RIGHT_ID:
				if (!set) {
					move_actionid = ACTION_RIGHT; shoot_actionid = ACTION_SHOOT_RIGHT; set = TRUE;
				}
			case POINTER_CONTROLS_DOWN_ID:
				if (!set) {
					move_actionid = ACTION_DOWN; shoot_actionid = ACTION_SHOOT_DOWN; set = TRUE;
				}
			case POINTER_CONTROLS_LEFT_ID:
				if (!set) {
					move_actionid = ACTION_LEFT; shoot_actionid = ACTION_SHOOT_LEFT; set = TRUE;
				}
				if (rob_event->klasse == ROB_EVENT_RELEASE) {
					if (pointer_controls.shoot_state == POINTER_CONTROLS_SHOOT_OFF_DRAG) {
						pointer_controls.shoot_state = POINTER_CONTROLS_SHOOT_OFF;
						manage_game_on_input(shoot_actionid);
					} else if (pointer_controls.shoot_state == POINTER_CONTROLS_SHOOT_CLICK_DRAG) {
						pointer_controls.shoot_state = POINTER_CONTROLS_SHOOT_CLICK;
						manage_game_on_input(shoot_actionid);
					}
				} else if (rob_event->klasse == ROB_EVENT_SELECT) {
					if (pointer_controls.shoot_state == POINTER_CONTROLS_SHOOT_CLICK) {
						manage_game_on_input(shoot_actionid);
					} else {
						manage_game_on_input(move_actionid);
					}
				}
				break;
			case POINTER_CONTROLS_TOOLBAR_ID:
				if (rob_event->klasse == ROB_EVENT_CLICK) {
				}
				break;
			case POINTER_CONTROLS_TOOLBAR_INTRO_SCREEN_ID:
				if (rob_event->klasse == ROB_EVENT_CLICK) {
					manage_game_on_input(ACTION_EXIT);
				}
				break;
			case POINTER_CONTROLS_TOOLBAR_OPTIONS_ID:
				if (rob_event->klasse == ROB_EVENT_CLICK) {
					manage_game_on_input(ACTION_OPTIONS);
				}
				break;
			case POINTER_CONTROLS_TOOLBAR_DESIGNER_ID:
				if (rob_event->klasse == ROB_EVENT_CLICK) {
					manage_game_on_input(ACTION_TOGGLE_DESIGNER);
				}
				break;
			case POINTER_CONTROLS_TOOLBAR_RESTART_ID:
				if (rob_event->klasse == ROB_EVENT_CLICK) {
					manage_game_on_input(ACTION_RESTART);
				}
				break;
			case POINTER_CONTROLS_TOOLBAR_HIDE_ID:
				if (rob_event->klasse == ROB_EVENT_CLICK) {
					pointer_controls.state = POINTER_CONTROLS_MODE_OFF;
					hide_pointer_controls_pad();
					hide_pointer_controls_toolbar();
					game_area.redraw |= REDRAW_EVERYTHING;
				}
				break;
		}
	}
}

/***************************************************************************
 * Create Pointer Controls                                                 *
 ***************************************************************************/
/*
   ---------------------------------------- There's a transparent rectangle that covers the entire screen
   |                              |#|#|#|#| which is always manipulatable in-game. This rectangle is the
   |  ----------------------------------  | ancestor of all the pointer controls and when clicked will
   |  |              |  |              |  | make visible the pointer controls pad and toolbar.
   |  |              |  |              |  | 
   |  |              |  |              |  | Centred over Robbo is a shoot rectangle that is the parent to
   |  |              |  |              |  | the four directional rectangles that extend to the viewport
   |  |--------------|--|--------------|  | edges. Pressing the directional buttons sends Robbo off in the
   |  |              |  |              |  | corresponding direction. Dragging from the shoot button to one
   |  |--------------|--|--------------|  | of the direction buttons shoots once. Clicking the shoot button
   |  |              |  |              |  | toggles shoot mode on and off. If shoot mode is on then pressing
   |  |              |  |              |  | a direction makes Robbo repeatedly shoot instead of walk.
   |  |              |  |              |  | 
   |  |              |  |              |  | Top right is a toolbar parent with currently four buttons that
   |  ----------------------------------  | perform useful and necessary tasks. Clicking the left-most button
   |                                      | will make invisible the pad and toolbar.
   ----------------------------------------
   Note that when the pointer controls are visible, the entire viewport must be redrawn every frame which
   is not the norm and increases CPU usage. Normally only the tiles that have changed are redrawn. If the
   direction buttons extend outside of the viewport then the entire screen must be redrawn every frame which
   increases CPU usage even more and that is the reason why the default boundary is the viewport.
*/

void create_pointer_controls(void) {
	ROB_Object rob_object;
	
	if (video.field_size == 16) {
		toolbar_intro_screen_image_srcrect = set_rect(188, 1, video.field_size, video.field_size);
		toolbar_options_image_srcrect = set_rect(188, 18, video.field_size, video.field_size);
		toolbar_designer_image_srcrect = set_rect(188, 69, video.field_size, video.field_size);
		toolbar_restart_image_srcrect = set_rect(188, 35, video.field_size, video.field_size);
		toolbar_hide_image_srcrect = set_rect(188, 52, video.field_size, video.field_size);
	} else if (video.field_size == 32) {
		toolbar_intro_screen_image_srcrect = set_rect(376, 2, video.field_size, video.field_size);
		toolbar_options_image_srcrect = set_rect(376, 36, video.field_size, video.field_size);
		toolbar_designer_image_srcrect = set_rect(376, 138, video.field_size, video.field_size);
		toolbar_restart_image_srcrect = set_rect(376, 70, video.field_size, video.field_size);
		toolbar_hide_image_srcrect = set_rect(376, 104, video.field_size, video.field_size);
	}	

	/* Create the parent object which will be used to identify the group */
	if (ROB_SetObjectDefaults(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	rob_object.id = POINTER_CONTROLS_ID;
	rob_object.w = screen->w;
	rob_object.h = screen->h;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	
	/* Create the pointer controls pad */
	/* Create the shoot button which will be the parent to the directions */
	if (ROB_SetObjectDefaults(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	rob_object.id = POINTER_CONTROLS_SHOOT_ID;
	rob_object.pid = POINTER_CONTROLS_ID;
	rob_object.alpha = BUTTON_ALPHA_NORMAL;
	rob_object.alpha_press = BUTTON_ALPHA_MOVE_SHOOT;
	rob_object.bgcolour_press = BUTTON_COLOUR_SHOOT_PRESS;
	rob_object.x = 0;
	rob_object.y = 0;
	rob_object.w = video.field_size * 2;
	rob_object.h = video.field_size * 2;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create the up button */
	if (ROB_SetObjectDefaults(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	rob_object.id = POINTER_CONTROLS_UP_ID;
	rob_object.pid = POINTER_CONTROLS_SHOOT_ID;
	rob_object.alpha = BUTTON_ALPHA_NORMAL;
	rob_object.alpha_press = BUTTON_ALPHA_MOVE_SHOOT;
	rob_object.bgcolour = BUTTON_COLOUR_DIRECTIONS;
	rob_object.x = 0;
	rob_object.y = 0;
	rob_object.w = video.field_size * 2;
	rob_object.h = 0;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create the right button */
	rob_object.id = POINTER_CONTROLS_RIGHT_ID;
	rob_object.x = video.field_size * 2;
	rob_object.y = 0;
	rob_object.w = 0;
	rob_object.h = video.field_size * 2;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create the down button */
	rob_object.id = POINTER_CONTROLS_DOWN_ID;
	rob_object.x = 0;
	rob_object.y = video.field_size * 2;
	rob_object.w = video.field_size * 2;
	rob_object.h = 0;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create the left button */
	rob_object.id = POINTER_CONTROLS_LEFT_ID;
	rob_object.x = 0;
	rob_object.y = 0;
	rob_object.w = 0;
	rob_object.h = video.field_size * 2;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());

	/* Create the pointer controls toolbar */
	/* Create the toolbar which will be the parent to the toolbar buttons */
	if (ROB_SetObjectDefaults(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	rob_object.id = POINTER_CONTROLS_TOOLBAR_ID;
	rob_object.pid = POINTER_CONTROLS_ID;
	rob_object.w = video.field_size * 5 + video.field_size / 4 * 6;
	rob_object.h = video.field_size + video.field_size / 4 * 2;
	rob_object.x = screen->w - rob_object.w;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create the Intro Screen button */
	if (ROB_SetObjectDefaults(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	rob_object.id = POINTER_CONTROLS_TOOLBAR_INTRO_SCREEN_ID;
	rob_object.pid = POINTER_CONTROLS_TOOLBAR_ID;
	rob_object.w = rob_object.h = video.field_size;
	rob_object.x = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_ID)->w - rob_object.w - video.field_size / 4;
	rob_object.y = video.field_size / 4;
	rob_object.img_surface = icons;
	rob_object.img_x = toolbar_intro_screen_image_srcrect.x;
	rob_object.img_y = toolbar_intro_screen_image_srcrect.y;
	rob_object.img_w = toolbar_intro_screen_image_srcrect.w;
	rob_object.img_h = toolbar_intro_screen_image_srcrect.h;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create the Options button */
	rob_object.id = POINTER_CONTROLS_TOOLBAR_OPTIONS_ID;
	rob_object.x = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_ID)->w - rob_object.w * 2 - video.field_size / 4 * 2;
	rob_object.img_x = toolbar_options_image_srcrect.x;
	rob_object.img_y = toolbar_options_image_srcrect.y;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create the Designer button */
	rob_object.id = POINTER_CONTROLS_TOOLBAR_DESIGNER_ID;
	rob_object.x = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_ID)->w - rob_object.w * 3 - video.field_size / 4 * 3;
	rob_object.img_x = toolbar_designer_image_srcrect.x;
	rob_object.img_y = toolbar_designer_image_srcrect.y;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create the Restart button */
	rob_object.id = POINTER_CONTROLS_TOOLBAR_RESTART_ID;
	rob_object.x = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_ID)->w - rob_object.w * 4 - video.field_size / 4 * 4;
	rob_object.img_x = toolbar_restart_image_srcrect.x;
	rob_object.img_y = toolbar_restart_image_srcrect.y;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create the hide button */
	rob_object.id = POINTER_CONTROLS_TOOLBAR_HIDE_ID;
	rob_object.x = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_ID)->w - rob_object.w * 5 - video.field_size / 4 * 5;
	rob_object.img_x = toolbar_hide_image_srcrect.x;
	rob_object.img_y = toolbar_hide_image_srcrect.y;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());

	/* Set-up the controls depending on pointer_controls.state */
	hide_pointer_controls();
	if (pointer_controls.state == POINTER_CONTROLS_MODE_ON) {
		show_pointer_controls_pad();
		show_pointer_controls_toolbar();
	} else {
		hide_pointer_controls_pad();
		hide_pointer_controls_toolbar();
	}
	update_pointer_controls_toolbar();
}

/***************************************************************************
 * Show Pointer Controls                                                   *
 ***************************************************************************/
/* This shows the parent which could reveal the pad and toolbar if they haven't
   been independently hidden. It also currently enables/disables the parent
   depending on the demo_mode state */

void show_pointer_controls(void) {
	ROB_Object *rob_object;

	if ((rob_object = ROB_GetObject(POINTER_CONTROLS_ID)) != NULL) {
		rob_object->visible = rob_object->enabled = ROB_INHERITED;
		if (demo_mode(DEMO_MODE_IS_ACTIVE, 0)) rob_object->enabled = FALSE;
	}
}

/***************************************************************************
 * Show Pointer Controls Pad                                               *
 ***************************************************************************/
/* This shows the pad only */

void show_pointer_controls_pad(void) {
	ROB_Object *rob_object;

	if ((rob_object = ROB_GetObject(POINTER_CONTROLS_SHOOT_ID)) != NULL) rob_object->visible = ROB_INHERITED;
}

/***************************************************************************
 * Show Pointer Controls Toolbar                                           *
 ***************************************************************************/
/* This shows the toolbar only */

void show_pointer_controls_toolbar(void) {
	ROB_Object *rob_object;

	if ((rob_object = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_ID)) != NULL) rob_object->visible = ROB_INHERITED;
}

/***************************************************************************
 * Hide Pointer Controls                                                   *
 ***************************************************************************/
/* This will hide all the pointer controls because the visible property will be inherited */

void hide_pointer_controls(void) {
	ROB_Object *rob_object;

	if ((rob_object = ROB_GetObject(POINTER_CONTROLS_ID)) != NULL) rob_object->visible = FALSE;
}

/***************************************************************************
 * Hide Pointer Controls Pad                                               *
 ***************************************************************************/
/* This hides the pad only */
   
void hide_pointer_controls_pad(void) {
	ROB_Object *rob_object;

	if ((rob_object = ROB_GetObject(POINTER_CONTROLS_SHOOT_ID)) != NULL) rob_object->visible = FALSE;
}

/***************************************************************************
 * Hide Pointer Controls Toolbar                                           *
 ***************************************************************************/
/* This hides the toolbar only */

void hide_pointer_controls_toolbar(void) {
	ROB_Object *rob_object;

	if ((rob_object = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_ID)) != NULL) rob_object->visible = FALSE;
}

/***************************************************************************
 * Update Pointer Controls Pad                                             *
 ***************************************************************************/
/* This function :-
   * Centres the pad's shoot button over Robbo and adjusts it when he's up against the viewport edges if required
   * Extends the visible pad direction buttons right up to the viewport or screen boundary
   * Maintains object properties affected by the shoot state */

void update_pointer_controls_pad(int robbo_xoffset, int robbo_yoffset) {
	ROB_Object *pob, *rob_object;

	if (ROB_GetObject(POINTER_CONTROLS_ID) != NULL) {

		/* Update shoot button position */
		if (pointer_controls.pad_type == POINTER_CONTROLS_PAD_TYPE_VIEWPORT) {
			/* Centre the shoot button over Robbo and offset it at the viewport edges */
			pob = rob_object = ROB_GetObject(POINTER_CONTROLS_SHOOT_ID);
			if (robbo.x - viewport.x < 2) {
				rob_object->x = robbo_xoffset;
			} else if (robbo.x > viewport.x + viewport.w - 3) {
				rob_object->x = robbo_xoffset - rob_object->w / 2;
			} else {
				rob_object->x = robbo_xoffset - rob_object->w / 4;
			}
			if (robbo.y - viewport.y < 2) {
				rob_object->y = robbo_yoffset;
			} else if (robbo.y > viewport.y + viewport.h - 3) {
				rob_object->y = robbo_yoffset - rob_object->h / 2;
			} else {
				rob_object->y = robbo_yoffset - rob_object->h / 4;
			}
		} else { /* if (pointer_controls.pad_type == POINTER_CONTROLS_PAD_TYPE_SCREEN) { */
			/* Centre the shoot button over Robbo */
			pob = rob_object = ROB_GetObject(POINTER_CONTROLS_SHOOT_ID);
			rob_object->x = robbo_xoffset - rob_object->w / 4;
			rob_object->y = robbo_yoffset - rob_object->h / 4;
		}

		/* Update direction button positions and dimensions */
		if (pointer_controls.pad_type == POINTER_CONTROLS_PAD_TYPE_VIEWPORT) {
			/* Stretch the up button all the way to the edge of the viewport */
			rob_object = ROB_GetObject(POINTER_CONTROLS_UP_ID);
			rob_object->y = 0 - (pob->y - viewport.yoffset);
			rob_object->h = pob->y - viewport.yoffset;
			/* Stretch the right button all the way to the edge of the viewport */
			rob_object = ROB_GetObject(POINTER_CONTROLS_RIGHT_ID);
			rob_object->w = viewport.xoffset + viewport.w * video.field_size - pob->x - pob->w;
			/* Stretch the down button all the way to the edge of the viewport */
			rob_object = ROB_GetObject(POINTER_CONTROLS_DOWN_ID);
			rob_object->h = viewport.yoffset + viewport.h * video.field_size - pob->y - pob->h;
			/* Stretch the left button all the way to the edge of the viewport */
			rob_object = ROB_GetObject(POINTER_CONTROLS_LEFT_ID);
			rob_object->x = 0 - (pob->x - viewport.xoffset);
			rob_object->w = pob->x - viewport.xoffset;
		} else if (pointer_controls.pad_type == POINTER_CONTROLS_PAD_TYPE_SCREEN) {
			/* Stretch the up button all the way to the edge of the screen */
			rob_object = ROB_GetObject(POINTER_CONTROLS_UP_ID);
			rob_object->y = 0 - pob->y;
			rob_object->h = pob->y;
			/* Stretch the right button all the way to the edge of the screen */
			rob_object = ROB_GetObject(POINTER_CONTROLS_RIGHT_ID);
			rob_object->w = screen->w - pob->x - pob->w;
			/* Stretch the down button all the way to the edge of the screen */
			rob_object = ROB_GetObject(POINTER_CONTROLS_DOWN_ID);
			rob_object->h = screen->h - pob->y - pob->h;
			/* Stretch the left button all the way to the edge of the screen */
			rob_object = ROB_GetObject(POINTER_CONTROLS_LEFT_ID);
			rob_object->x = 0 - pob->x;
			rob_object->w = pob->x;
		}

		/* Update some of the object properties for the current shoot state */
		switch (pointer_controls.shoot_state) {
			case POINTER_CONTROLS_SHOOT_OFF:
				rob_object = ROB_GetObject(POINTER_CONTROLS_SHOOT_ID);
				rob_object->bgcolour = rob_object->bgcolour_over = ROB_UNDEFINED;
				user_controls[ACTION_PRIMARY_CLICK].delay = 0;
				user_controls[ACTION_PRIMARY_CLICK].interval = 0;
			case POINTER_CONTROLS_SHOOT_CLICK:
				ROB_GetObject(POINTER_CONTROLS_UP_ID)->alpha_over = ROB_UNDEFINED;
				ROB_GetObject(POINTER_CONTROLS_RIGHT_ID)->alpha_over = ROB_UNDEFINED;
				ROB_GetObject(POINTER_CONTROLS_DOWN_ID)->alpha_over = ROB_UNDEFINED;
				ROB_GetObject(POINTER_CONTROLS_LEFT_ID)->alpha_over = ROB_UNDEFINED;
				if (pointer_controls.shoot_state == POINTER_CONTROLS_SHOOT_OFF) break;
				rob_object = ROB_GetObject(POINTER_CONTROLS_SHOOT_ID);
				rob_object->bgcolour = BUTTON_COLOUR_SHOOT_CLICK;
				user_controls[ACTION_PRIMARY_CLICK].delay = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
				user_controls[ACTION_PRIMARY_CLICK].interval = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
				break;
			case POINTER_CONTROLS_SHOOT_OFF_DRAG:
			case POINTER_CONTROLS_SHOOT_CLICK_DRAG:
				ROB_GetObject(POINTER_CONTROLS_UP_ID)->alpha_over = BUTTON_ALPHA_MOVE_SHOOT;
				ROB_GetObject(POINTER_CONTROLS_RIGHT_ID)->alpha_over = BUTTON_ALPHA_MOVE_SHOOT;
				ROB_GetObject(POINTER_CONTROLS_DOWN_ID)->alpha_over = BUTTON_ALPHA_MOVE_SHOOT;
				ROB_GetObject(POINTER_CONTROLS_LEFT_ID)->alpha_over = BUTTON_ALPHA_MOVE_SHOOT;
				user_controls[ACTION_PRIMARY_CLICK].delay = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
				user_controls[ACTION_PRIMARY_CLICK].interval = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
				break;
		}
	}
}

/***************************************************************************
 * Update Pointer Controls Toolbar                                         *
 ***************************************************************************/
/* This function :-
   * Maintains the toolbar button colours to match the level
   * Maintains the toolbar button image surface pointers
   * Maintains the visibility of the toolbar buttons */

void update_pointer_controls_toolbar(void) {
	ROB_Object *rob_object;
	Uint32 level_colour;

	if (ROB_GetObject(POINTER_CONTROLS_ID) != NULL) {
		if (level.colour_override != UNDEFINED) {
			level_colour = level.colour_override;
		} else if (level.colour != UNDEFINED) {
			level_colour = level.colour;
		} else {
			level_colour = DEFAULT_LEVEL_COLOUR;
		}
		rob_object = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_INTRO_SCREEN_ID);
		rob_object->bgcolour = level_colour;
		rob_object->bgcolour_over = level_colour | BUTTON_COLOUR_HIGHLIGHT;
		rob_object->img_surface = icons;
		rob_object->visible = ROB_INHERITED;
		rob_object = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_OPTIONS_ID);
		rob_object->bgcolour = level_colour;
		rob_object->bgcolour_over = level_colour | BUTTON_COLOUR_HIGHLIGHT;
		rob_object->img_surface = icons;
		rob_object->visible = ROB_INHERITED;
		rob_object = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_DESIGNER_ID);
		rob_object->bgcolour = level_colour;
		rob_object->bgcolour_over = level_colour | BUTTON_COLOUR_HIGHLIGHT;
		rob_object->img_surface = icons;
		rob_object->visible = ROB_INHERITED;
		rob_object = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_RESTART_ID);
		rob_object->bgcolour = level_colour;
		rob_object->bgcolour_over = level_colour | BUTTON_COLOUR_HIGHLIGHT;
		rob_object->img_surface = icons;
		rob_object->visible = ROB_INHERITED;
		rob_object = ROB_GetObject(POINTER_CONTROLS_TOOLBAR_HIDE_ID);
		rob_object->bgcolour = level_colour;
		rob_object->bgcolour_over = level_colour | BUTTON_COLOUR_HIGHLIGHT;
		rob_object->img_surface = icons;
		rob_object->visible = ROB_INHERITED;
	}
}







