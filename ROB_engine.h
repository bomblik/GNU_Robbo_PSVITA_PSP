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
/*
#define DEBUG_ROB_HOTSPOTS
*/

/* Miscellaneous defines */
#define ROB_UNDEFINED -1
#define ROB_INHERITED -2

/* System objects */
#define ROB_LYR_ROOT 0
#define ROB_LYR_CANVAS 1
#define ROB_REC_POINTER 98
#define ROB_LYR_POINTER 99

/* Hit points */
#define ROB_HIT_POINT_TOP_LEFT 0
#define ROB_HIT_POINT_CENTRE 1

/* Object drag points */
#define ROB_DRAG_POINT_ANYWHERE 0
#define ROB_DRAG_POINT_MIDDLE 1

/* Object drag types */
#define ROB_DRAG_ANYWHERE 1
#define ROB_DRAG_RESTRICT_TO_PARENT 2
#define ROB_DRAG_RESTRICT_TO_PARENT_ORIGIN 3

/* ZOrder positions */
#define ROB_ZORDER_ABOVE_ALL 0
#define ROB_ZORDER_ABOVE 1
#define ROB_ZORDER_BELOW 2

/* Pointer modes */
#define ROB_POINTER_MODE_OFF 0
#define ROB_POINTER_MODE_PHYSICAL 1
#define ROB_POINTER_MODE_SIMULATED 2

/* Events */
#define ROB_EVENT_SEND_PRE_RENDER

#define ROB_EVENT_OVER 10
#define ROB_EVENT_OUT 11
#define ROB_EVENT_PRESS 12
#define ROB_EVENT_CLICK 13
#define ROB_EVENT_RELEASE 14
#define ROB_EVENT_DRAG 15
#define ROB_EVENT_SELECT 38

/* Error codes */
#define ROB_ERROR_NONE 0
#define ROB_ERROR_TOO_MANY_OBJECTS 100
#define ROB_ERROR_INVALID_OBJECT 200
#define ROB_ERROR_INVALID_OBJECT_ID 300
#define ROB_ERROR_INVALID_TARGET_OBJECT 400
#define ROB_ERROR_CANNOT_SET_SURFACE_ALPHA 500
#define ROB_ERROR_CANNOT_FILL_SURFACE 600
#define ROB_ERROR_CANNOT_BLIT_SURFACE 700
#define ROB_ERROR_INVALID_EVENT 800


/* Variables */
typedef struct robobject {
	int id;
	int pid;			/* Once object is loaded, use *pob instead; the engine does not use this after loading */
	int visible;		/* -2 (inherited), 0 or 1: Invisible objects are not manipulatable */
	int enabled;		/* -2 (inherited), 0 or 1: Disabled objects are not manipulatable */
	int alpha;			/* -2 (inherited) or 0 (transparent) to 255 (opaque) */
	int alpha_over;
	int alpha_press;
	int bgcolour;
	int bgcolour_over;
	int bgcolour_press;
	int x;				/* Object coordinates and dimensions */
	int y;
	int w;
	int h;
	SDL_Surface *img_surface;
	int img_x;			/* Image region */
	int img_y;
	int img_w;
	int img_h;
	SDL_Surface *img_over_surface;
	int img_over_x;		/* Image over region */
	int img_over_y;
	int img_over_w;
	int img_over_h;
	SDL_Surface *img_press_surface;
	int img_press_x;	/* Image press region */
	int img_press_y;
	int img_press_w;
	int img_press_h;
	int draggable;
	int drag_point;
	int drag_granularity;
	/* The properties below are set by the system when the object is created */
	int zorder;
	void *pob;			/* Parent object pointer */
} ROB_Object;

ROB_Object *rob_lyr_pointer;

typedef struct robevent {
	ROB_Object *rob_object;
	int klasse;
} ROB_Event;

typedef struct robopenv {
	int systempointer;								/* True or false */
	int pointer;									/* True or false */
	int pointer_mode;								/* Pointer mode */
	int pointer_move_unit_low;						/* Unit pointer should move unaccelerated */
	int pointer_move_unit_high;						/* Unit pointer should move accelerated */
	int pointer_move_unit_threshold;				/* Number of units marking the low/high threshold */
	void (*event_processor)(ROB_Event *rob_event);	/* The function that receives events */
} ROB_OpEnv;

/* Function prototypes */
void ROB_Init(ROB_OpEnv *op_env);
ROB_Object *ROB_GetObject(int id);
int ROB_CreateObject(ROB_Object *rob_object);
int ROB_SetObjectDefaults(ROB_Object *rob_object);
char *ROB_GetError(void);
int ROB_Quit(void);
int ROB_FreeAllChildObjects(ROB_Object *pob, int free_pob);
int ROB_FreeAllDescendantObjects(ROB_Object *ancestor, int free_ancestor);
int ROB_FreeObject(ROB_Object *rob_object, int free_images_only);
int ROB_RenderObjects(void);
void ROB_GenerateEvents(int *actionid);
int ROB_ShowEvent(ROB_Event *rob_event, const char *func);
int ROB_SetOpEnvSystemPointer(int systempointer);
int ROB_SetOpEnvPointer(int pointer, int pointer_mode);
int ROB_SetOpEnvEventProcessor(void (*event_processor)(ROB_Event *rob_event));







