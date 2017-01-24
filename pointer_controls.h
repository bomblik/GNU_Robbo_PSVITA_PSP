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
/* Pointer controls ids */
#define POINTER_CONTROLS_ID 500
#define POINTER_CONTROLS_SHOOT_ID 501
#define POINTER_CONTROLS_UP_ID 502
#define POINTER_CONTROLS_RIGHT_ID 503
#define POINTER_CONTROLS_DOWN_ID 504
#define POINTER_CONTROLS_LEFT_ID 505
#define POINTER_CONTROLS_TOOLBAR_ID 510
#define POINTER_CONTROLS_TOOLBAR_INTRO_SCREEN_ID 511
#define POINTER_CONTROLS_TOOLBAR_OPTIONS_ID 512
#define POINTER_CONTROLS_TOOLBAR_DESIGNER_ID 513
#define POINTER_CONTROLS_TOOLBAR_RESTART_ID 514
#define POINTER_CONTROLS_TOOLBAR_HIDE_ID 515

/* Pointer controls operation modes */
#define POINTER_CONTROLS_MODE_OFF 0
#define POINTER_CONTROLS_MODE_ON 1

/* Pointer controls pad types */
#define POINTER_CONTROLS_PAD_TYPE_VIEWPORT 0
#define POINTER_CONTROLS_PAD_TYPE_SCREEN 1

/* Pointer controls pad shoot states */
#define POINTER_CONTROLS_SHOOT_OFF 0
#define POINTER_CONTROLS_SHOOT_OFF_DRAG 1
#define POINTER_CONTROLS_SHOOT_CLICK_DRAG 2
#define POINTER_CONTROLS_SHOOT_CLICK 3

/* Variables */
struct pointercontrols {
	int state;
	int shoot_state;
	int pad_type;
};
struct pointercontrols pointer_controls;
struct pointercontrols temp_pointer_controls;

/* Function prototypes */
void pointer_controls_event_processor(ROB_Event *rob_event);
void create_pointer_controls(void);
void show_pointer_controls(void);
void show_pointer_controls_pad(void);
void show_pointer_controls_toolbar(void);
void hide_pointer_controls(void);
void hide_pointer_controls_pad(void);
void hide_pointer_controls_toolbar(void);
void update_pointer_controls_pad(int robbo_xoffset, int robbo_yoffset);
void update_pointer_controls_toolbar(void);






