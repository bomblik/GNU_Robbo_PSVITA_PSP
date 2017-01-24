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

/* The GNU Robbo Rectangular Object Engine
   =======================================
   
   The purpose of this engine is to manage a GUI and widget set
   capable of assisting in the development of a level designer.
   It's a cut down version of ENG, a project I've been working on
   and only recently (2009-03-22) published on Sourceforge.
   
   The main differences between this and ENG are :-
   * No objects will be loaded from a file, only created in code
   * There is no image subsystem although ROB_FreeObject will free attached images
   * There is no actions subsystem as it already exists within GNU Robbo in a simpler form
   * No screen or viewport system objects, just a screen sized canvas
   * The sorted id array is dispensed with because there won't be many objects
   * Only GNU Robbo developers will be creating objects, not users
   * Much of the idiot-proof error checking is no longer necessary :)

*/

/* Defines */
/*
#define DEBUG_ROB_OBJECTS
#define DEBUG_ROB_OBJECT_ZORDER
#define DEBUG_ROB_OBJECTS_ALL_DRAGGABLE
#define DEBUG_ROB_GET_VISIBILITY
#define DEBUG_ROB_GET_ENABILITY
#define DEBUG_ROB_EVENTS
#define DEBUG_ROB_PROGRESS_DUMP
*/

#define ROB_MAX_OBJECTS 1000
#define ROB_FILE_LENGTH 256
#define ROB_ERROR_LENGTH 256

#define ROB_DIR "rob"
#define ROB_SPRITES "robsprites.bmp"

#define ROB_SPRITES_POINTER_XOFFSET 0
#define ROB_SPRITES_POINTER_W 13
#define ROB_SPRITES_POINTER_H 19
#define ROB_SPRITES_POINTER_SHADOW_XOFFSET 13
#define ROB_SPRITES_POINTER_SHADOW_ALPHA 50
#define ROB_SPRITES_POINTER_SHADOW_X 4
#define ROB_SPRITES_POINTER_SHADOW_Y 4
#define ROB_SPRITES_POINTER_SHADOW_W 13
#define ROB_SPRITES_POINTER_SHADOW_H 19

/* Variables */
ROB_OpEnv rob_op_env;
SDL_Surface *rob_blend_screen = NULL;
SDL_Surface *rob_sprites = NULL;
ROB_Object *rob_objects[ROB_MAX_OBJECTS];
ROB_Object *rob_lyr_root = NULL;
ROB_Object *rob_lyr_canvas = NULL;
ROB_Object *rob_rec_pointer = NULL;
ROB_Object *rob_oup = NULL;
ROB_Object *rob_pressed_object = NULL;
int rob_object_count = 0;
char rob_last_error[ROB_ERROR_LENGTH];

/* Function prototypes */
int ROB_SetObjectZOrder(ROB_Object *rob_object, int position, ROB_Object *target);
void ROB_FreeAllObjects(void);
int ROB_FillRectA(SDL_Surface *surface, SDL_Rect *dstrect, Uint32 colour, Uint8 alpha);
int ROB_GetCanvasOffset(ROB_Object *rob_object, int *cx, int *cy);
int ROB_GetVisibility(ROB_Object *rob_object);
int ROB_GetEnability(ROB_Object *rob_object);
int ROB_GetAlpha(ROB_Object *rob_object);
ROB_Object *ROB_GetObjectUnderObject(ROB_Object *rob_object, int hitpoint);
int ROB_FindAncestor(ROB_Object *rob_object, ROB_Object *target);
void ROB_ManagePointerInput(int pxu, int pyu);
int ROB_SendEvent(ROB_Event *rob_event);


/***************************************************************************
 * Initialise                                                              *
 ***************************************************************************/
/* On entry: op_env points to the operating environment data which is copied locally */

void ROB_Init(ROB_OpEnv *op_env) {
	SDL_Surface *unconverted, *converted;
	ROB_Object rob_object;
	char filename[256];

	#ifdef DEBUG_ROB_PROGRESS_DUMP
		printf("%s: Initialising\n", __func__);
	#endif

	/* Create a local copy of the passed op_env */
	rob_op_env = *op_env;
	
	/* Initialise some defaults */
	rob_lyr_pointer = NULL;
	rob_last_error[0] = 0;
	
	/* Duplicate the screen surface for making alpha rectangles only */
	rob_blend_screen = SDL_CreateRGBSurface(screen->flags | SDL_SRCALPHA, screen->w, screen->h, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask, screen->format->Bmask, screen->format->Amask);
	if (rob_blend_screen == NULL) {
		fprintf(stdout, "%s: Cannot create blend surface: %s", __func__, SDL_GetError());
		exit(1);
	}

	/* Load the ROB system images */
	strcpy(filename, PACKAGE_DATA_DIR "/" ROB_DIR "/");
	strcat(filename, ROB_SPRITES);
	/* Load the bitmap */
	if ((unconverted = SDL_LoadBMP(filename)) == NULL) {
		fprintf(stdout, "Cannot load bitmap: %s\n", filename);
		exit(1);
	}
	/* Set the transparent colour */
	if (SDL_SetColorKey(unconverted, SDL_SRCCOLORKEY, SDL_MapRGB(unconverted->format, 0xff, 0x00, 0xff)) < 0) {
		fprintf(stdout, "Cannot set surface colour key: %s\n", SDL_GetError());
		exit(1);
	}
	/* Convert the loaded surface to the same pixel format as the screen for fast blitting */
	converted = SDL_DisplayFormat(unconverted);
	SDL_FreeSurface(unconverted);
	if (converted == NULL) {
		fprintf(stdout, "Cannot create surface: %s\n", SDL_GetError());
		exit(1);
	}
	rob_sprites = converted;

	/* Create the system objects */
	/* Create a root object */
	if (ROB_SetObjectDefaults(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	rob_object.id = ROB_LYR_ROOT ;
	rob_object.pid = ROB_LYR_ROOT;
	rob_object.visible = TRUE;
	rob_object.enabled = TRUE;
	rob_object.alpha = 255;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create a canvas object */
	if (ROB_SetObjectDefaults(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	rob_object.id = ROB_LYR_CANVAS;
	rob_object.pid = ROB_LYR_ROOT;
	rob_object.visible = TRUE;
	rob_object.enabled = TRUE;
	rob_object.alpha = 255;
	rob_object.w = screen->w;
	rob_object.h = screen->h;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create a pointer shadow */
	if (ROB_SetObjectDefaults(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	rob_object.id = ROB_REC_POINTER;
	rob_object.pid = ROB_LYR_POINTER;
	rob_object.alpha = ROB_SPRITES_POINTER_SHADOW_ALPHA;
	rob_object.x = ROB_SPRITES_POINTER_SHADOW_X;
	rob_object.y = ROB_SPRITES_POINTER_SHADOW_Y;
	rob_object.w = ROB_SPRITES_POINTER_SHADOW_W;
	rob_object.h = ROB_SPRITES_POINTER_SHADOW_H;
	rob_object.img_surface = rob_sprites;
	rob_object.img_x = ROB_SPRITES_POINTER_SHADOW_XOFFSET;
	rob_object.img_y = 0;
	rob_object.img_w = rob_object.w;
	rob_object.img_h = rob_object.h;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	/* Create a pointer */
	if (ROB_SetObjectDefaults(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	rob_object.id = ROB_LYR_POINTER;
	rob_object.pid = ROB_LYR_CANVAS;
	rob_object.visible = FALSE;
	rob_object.enabled = TRUE;
	rob_object.alpha = 255;
	rob_object.x = screen->w / 2;
	rob_object.y = screen->h / 2;
	rob_object.w = ROB_SPRITES_POINTER_W;
	rob_object.h = ROB_SPRITES_POINTER_H;
	rob_object.img_surface = rob_sprites;
	rob_object.img_x = ROB_SPRITES_POINTER_XOFFSET;
	rob_object.img_y = 0;
	rob_object.img_w = rob_object.w;
	rob_object.img_h = rob_object.h;
	if (ROB_CreateObject(&rob_object)) printf("%s: %s\n", __func__, ROB_GetError());
	#ifdef DEBUG_ROB_OBJECTS
		printf("%s: rob_lyr_root=%p\n", __func__, rob_lyr_root);
		printf("%s: rob_lyr_canvas=%p\n", __func__, rob_lyr_canvas);
		printf("%s: rob_rec_pointer=%p\n", __func__, rob_rec_pointer);
		printf("%s: rob_lyr_pointer=%p\n", __func__, rob_lyr_pointer);
	#endif
	
	/* Process the operating environment */
	ROB_SetOpEnvEventProcessor(rob_op_env.event_processor);
	ROB_SetOpEnvSystemPointer(rob_op_env.systempointer);
	ROB_SetOpEnvPointer(rob_op_env.pointer, rob_op_env.pointer_mode);
}

/***************************************************************************
 * Set Object ZOrder                                                       *
 ***************************************************************************/
/* User objects must be above ROB_LYR_CANVAS and below ROB_REC_POINTER to
   be accessible via the pointer. This function does not place any restrictions
   on the destinaton of any objects.

   Positions :-

   ROB_ZORDER_ABOVE_ALL
   --------------------
   The object will be moved above the target and above all other objects who's
   pob is also the target.
   If the object is or results in being the target then it is not moved.

   ROB_ZORDER_ABOVE
   ----------------
   The object will be moved above the target.
   If the object is the target then it is moved above itself.
   
   ROB_ZORDER_BELOW
   ----------------
   The object will be moved below the target.
   If the object is the target then it is moved below itself */
   
/* On entry: object is the object to move
			 position is one of the zorder positions detailed above
			 target is the target object of the move
	On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_SetObjectZOrder(ROB_Object *rob_object, int position, ROB_Object *target) {
	int count, retval = ROB_ERROR_NONE;
	int index = rob_object->zorder, targetindex = ROB_UNDEFINED, direction = 0;
	ROB_Object *swap_object;

	if (rob_object == NULL || sizeof(*rob_object) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: object is invalid", __func__);
		retval = ROB_ERROR_INVALID_OBJECT;
	} else if (target == NULL || sizeof(*target) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: target is invalid", __func__);
		retval = ROB_ERROR_INVALID_TARGET_OBJECT;
	} else {
		/* Set the target index */
		if (position == ROB_ZORDER_ABOVE_ALL) {
			/* Find the topmost object equal to target or its pob is equal to target */
			for (count = 0; count < rob_object_count; count++) {
				if (rob_objects[count] == target || rob_objects[count]->pob == target) targetindex = count;
			}
			/* Make some positional adjustments to the target index */
			if (targetindex == ROB_UNDEFINED) {
				targetindex = rob_object->zorder;	/* No seg faults please :) */
			} else if (rob_object->zorder > targetindex) {
				targetindex++;
			}
		} else if (position == ROB_ZORDER_ABOVE) {
			/* The target index is the zorder within the target */
			targetindex = target->zorder;
			/* Make some positional adjustments to the target index */
			if (rob_object->zorder == targetindex && targetindex < rob_object_count - 1) {
				targetindex++;
			} else if (rob_object->zorder > targetindex) {
				targetindex++;
			}
		} else if (position == ROB_ZORDER_BELOW) {
			/* The target index is the zorder within the target */
			targetindex = target->zorder;
			/* Make some positional adjustments to the target index */
			if (rob_object->zorder == targetindex && targetindex > 0) {
				targetindex--;
			} else if (rob_object->zorder < targetindex) {
				targetindex--;
			}
		}
		/* Set move direction */
		if (index < targetindex) {
			direction = 1;
		} else if (rob_object->zorder > targetindex) {
			direction = -1;
		}
		#ifdef DEBUG_ROB_OBJECT_ZORDER
			printf("%s: Moving id=%i zorder=%i targetindex=%i (rob_object_count=%i)\n", __func__, rob_object->id, rob_object->zorder, targetindex, rob_object_count);
		#endif
		while (index != targetindex) {
			#ifdef DEBUG_ROB_OBJECT_ZORDER
				printf("    id=%i pob=%p zorder=%i <--> id=%i pob=%p zorder=%i\n", rob_objects[index]->id, rob_objects[index]->pob, rob_objects[index]->zorder, rob_objects[index + direction]->id, rob_objects[index + direction]->pob, rob_objects[index + direction]->zorder);
			#endif
			swap_object = rob_objects[index];
			rob_objects[index] = rob_objects[index + direction];
			rob_objects[index]->zorder -= direction;
			index += direction;
			rob_objects[index] = swap_object;
			rob_objects[index]->zorder += direction;
		}
	}

	return retval;
}

/***************************************************************************
 * Get Object                                                              *
 ***************************************************************************/
/* On entry: id is the object id
	On exit: returns a pointer to the object on success else NULL */

ROB_Object *ROB_GetObject(int id) {
	int count;

	/* Linear search for id */
	for (count = 0; count < rob_object_count; count++) {
		if (rob_objects[count]->id == id) return rob_objects[count];
	}

	return NULL;
}

/***************************************************************************
 * Create Object                                                           *
 ***************************************************************************/
/* This will create the object.
   There may be existing objects within rob_objects whose pob is NULL because
   their parent was recently freed or hasn't been created yet. This function
   will attempt to fix these NULL pobs on introduction of this new object */
   
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_CreateObject(ROB_Object *rob_object) {
	int retval = ROB_ERROR_NONE, count;
	ROB_Object *newobject;
	
	if (rob_object == NULL || sizeof(*rob_object) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: object is invalid", __func__);
		retval = ROB_ERROR_INVALID_OBJECT;
	} else if (rob_object_count >= ROB_MAX_OBJECTS) {
		sprintf(rob_last_error, "%s: Too many objects (max %i)", __func__, ROB_MAX_OBJECTS);
		retval = ROB_ERROR_TOO_MANY_OBJECTS;
	} else {
		/* Check uniqueness of id */
		for (count = 0; count < rob_object_count; count++) {
			if (rob_object->id == rob_objects[count]->id) {
				sprintf(rob_last_error, "%s: Invalid duplicate id property", __func__);
				retval = ROB_ERROR_INVALID_OBJECT_ID;
				break;
			}
		}
		if (!retval) {
			/* Allocate memory for the new object */
			newobject = (ROB_Object*) malloc(sizeof(ROB_Object));
			*newobject = *rob_object;
			#ifdef DEBUG_ROB_OBJECTS_ALL_DRAGGABLE
				if (newobject->draggable == FALSE) newobject->draggable = ROB_DRAG_ANYWHERE;
			#endif
			#ifdef DEBUG_ROB_OBJECTS
				printf("*** Start %s ***\n", __func__);
				printf("%s: Creating %p (%i)\n", __func__, newobject, newobject->id);
			#endif
			/* Store the object pointer in the array */
			rob_objects[rob_object_count] = newobject;
			/* Set the zorder */
			newobject->zorder = rob_object_count;
			/* Update the array element count now */
			rob_object_count++;
			/* Store pointers to system objects */
			if (newobject->id == ROB_LYR_ROOT) {
				rob_lyr_root = newobject;
			} else if (newobject->id == ROB_LYR_CANVAS) {
				rob_lyr_canvas = newobject;
			} else if (newobject->id == ROB_REC_POINTER) {
				rob_rec_pointer = newobject;
			} else if (newobject->id == ROB_LYR_POINTER) {
				rob_lyr_pointer = newobject;
			}
			/* Iterate through rob_objects and fix the pob */
			for (count = 0; count < rob_object_count; count++) {
				if (rob_objects[count]->pob == NULL) {
					/* This may still result in a NULL pob if the parent never exists */
					rob_objects[count]->pob = ROB_GetObject(rob_objects[count]->pid);
				}
			}
			/* Maintain the highest zorder for the pointer and shadow */
			if (rob_lyr_pointer != NULL && rob_rec_pointer != NULL) {
				ROB_SetObjectZOrder(rob_lyr_pointer, ROB_ZORDER_ABOVE, newobject);
				ROB_SetObjectZOrder(rob_rec_pointer, ROB_ZORDER_BELOW, rob_lyr_pointer);
			}
			#ifdef DEBUG_ROB_OBJECTS
				printf("newobject :-\n");
				printf("id=%i\n", newobject->id);
				printf("pid=%i\n", newobject->pid);
				printf("visible=%i\n", newobject->visible);
				printf("enabled=%i\n", newobject->enabled);
				printf("alpha=%i\n", newobject->alpha);
				printf("alpha_over=%i\n", newobject->alpha_over);
				printf("alpha_press=%i\n", newobject->alpha_press);
				printf("bgcolour=%03X\n", newobject->bgcolour);
				printf("bgcolour_over=%03X\n", newobject->bgcolour_over);
				printf("bgcolour_press=%03X\n", newobject->bgcolour_press);
				printf("x=%i\n", newobject->x);
				printf("y=%i\n", newobject->y);
				printf("w=%i\n", newobject->w);
				printf("h=%i\n", newobject->h);
				printf("img_surface=%p\n", newobject->img_surface);
				printf("img_x=%i\n", newobject->img_x);
				printf("img_y=%i\n", newobject->img_y);
				printf("img_w=%i\n", newobject->img_w);
				printf("img_h=%i\n", newobject->img_h);
				printf("img_over_surface=%p\n", newobject->img_over_surface);
				printf("img_over_x=%i\n", newobject->img_over_x);
				printf("img_over_y=%i\n", newobject->img_over_y);
				printf("img_over_w=%i\n", newobject->img_over_w);
				printf("img_over_h=%i\n", newobject->img_over_h);
				printf("img_press_surface=%p\n", newobject->img_press_surface);
				printf("img_press_x=%i\n", newobject->img_press_x);
				printf("img_press_y=%i\n", newobject->img_press_y);
				printf("img_press_w=%i\n", newobject->img_press_w);
				printf("img_press_h=%i\n", newobject->img_press_h);
				printf("draggable=%i\n", newobject->draggable);
				printf("drag_point=%i\n", newobject->drag_point);
				printf("drag_granularity=%i\n", newobject->drag_granularity);
				printf("zorder=%i\n", newobject->zorder);
				printf("pob=%p\n", newobject->pob);
				printf("*** Stop %s ***\n", __func__);
			#endif
		}
	}

	return retval;
}

/***************************************************************************
 * Set Object Defaults                                                     *
 ***************************************************************************/
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_SetObjectDefaults(ROB_Object *rob_object) {
	int retval = ROB_ERROR_NONE;

	if (rob_object == NULL || sizeof(*rob_object) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: object is invalid", __func__);
		retval = ROB_ERROR_INVALID_OBJECT;
	} else {
		rob_object->id = ROB_UNDEFINED;
		rob_object->pid = ROB_LYR_CANVAS;
		rob_object->visible = ROB_INHERITED;
		rob_object->enabled = ROB_INHERITED;
		rob_object->alpha = ROB_INHERITED;
		rob_object->alpha_over = ROB_UNDEFINED;
		rob_object->alpha_press = ROB_UNDEFINED;
		rob_object->bgcolour = ROB_UNDEFINED;
		rob_object->bgcolour_over = ROB_UNDEFINED;
		rob_object->bgcolour_press = ROB_UNDEFINED;
		rob_object->x = 0;
		rob_object->y = 0;
		rob_object->w = 0;
		rob_object->h = 0;
		rob_object->img_surface = NULL;
		rob_object->img_x = ROB_UNDEFINED;
		rob_object->img_y = ROB_UNDEFINED;
		rob_object->img_w = ROB_UNDEFINED;
		rob_object->img_h = ROB_UNDEFINED;
		rob_object->img_over_surface = NULL;
		rob_object->img_over_x = ROB_UNDEFINED;
		rob_object->img_over_y = ROB_UNDEFINED;
		rob_object->img_over_w = ROB_UNDEFINED;
		rob_object->img_over_h = ROB_UNDEFINED;
		rob_object->img_press_surface = NULL;
		rob_object->img_press_x = ROB_UNDEFINED;
		rob_object->img_press_y = ROB_UNDEFINED;
		rob_object->img_press_w = ROB_UNDEFINED;
		rob_object->img_press_h = ROB_UNDEFINED;
		rob_object->draggable = FALSE;
		rob_object->drag_point = ROB_DRAG_POINT_ANYWHERE;
		rob_object->drag_granularity = 1;
		rob_object->zorder = ROB_UNDEFINED;
		rob_object->pob = NULL;
	}
	
	return retval;
}

/***************************************************************************
 * Get Error                                                               *
 ***************************************************************************/
/*  On exit: returns a pointer to a string containing the last internal
			 error plus the contents of SDL_GetError if relevant */

char *ROB_GetError(void) {
	static char last_error[ROB_ERROR_LENGTH];
	
	strcpy (last_error, rob_last_error);
	strcpy (rob_last_error, "");
	
	return last_error;
}

/***************************************************************************
 * Quit (ROB)                                                              *
 ***************************************************************************/
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_Quit(void) {
	int retval = ROB_ERROR_NONE;

	#ifdef DEBUG_ROB_PROGRESS_DUMP
		printf("%s: Quitting\n", __func__);
	#endif
	ROB_FreeAllObjects();
	if (rob_sprites) SDL_FreeSurface(rob_sprites);
	if (rob_blend_screen) SDL_FreeSurface(rob_blend_screen);

	return retval;
}

/***************************************************************************
 * Free All Objects                                                        *
 ***************************************************************************/
/* This mops-up any remaining objects before shutting down */

void ROB_FreeAllObjects(void) {
	int count;
	
	if (rob_object_count > 0) {
		#ifdef DEBUG_ROB_PROGRESS_DUMP
			printf("%s: Freeing %i objects\n", __func__, rob_object_count);
		#endif
		/* Free all remaining objects (ignoring errors) */
		for (count = rob_object_count - 1; count >= 0; count--) {
			ROB_FreeObject(rob_objects[count], FALSE);
		}
	}
}

/***************************************************************************
 * Free All Child Objects                                                  *
 ***************************************************************************/
/* This frees all child objects of a parent if any exist. This can be viewed
   as a way of freeing a group of objects that share a common parent */

/* On entry: pob is a pointer to a parent object
			 free_pob = TRUE to free the parent object also */
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_FreeAllChildObjects(ROB_Object *pob, int free_pob) {
	int retval = ROB_ERROR_NONE, count, free_count = 0;
	
	if (pob == NULL || sizeof(*pob) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: pob is invalid", __func__);
		retval = ROB_ERROR_INVALID_OBJECT;
	} else if (rob_object_count > 0) {
		for (count = rob_object_count - 1; count >= 0; count--) {
			if (rob_objects[count]->pob == pob) {
				ROB_FreeObject(rob_objects[count], FALSE);
				free_count++;
			}
		}
		#ifdef DEBUG_ROB_PROGRESS_DUMP
			printf("%s: Freeing %i objects", __func__, free_count);
			if (free_pob) printf(" and parent id=%i", pob->id);
			printf("\n");
		#endif
		if (free_pob) {
			ROB_FreeObject(pob, FALSE);
		}
	}

	return retval;
}

/***************************************************************************
 * Free All Descendant Objects                                             *
 ***************************************************************************/
/* This frees all descendant objects of an ancestor if any exist. This can be
   viewed as a way of freeing a group of objects that share a common ancestor */

/* On entry: ancestor is a pointer to an ancestor object
			 free_ancestor = TRUE to free the ancestor object also */
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_FreeAllDescendantObjects(ROB_Object *ancestor, int free_ancestor) {
	int retval = ROB_ERROR_NONE, count, free_count = 0;
	
	if (ancestor == NULL || sizeof(*ancestor) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: ancestor is invalid", __func__);
		retval = ROB_ERROR_INVALID_OBJECT;
	} else if (rob_object_count > 0) {
		for (count = rob_object_count - 1; count >= 0; count--) {
			if (rob_objects[count] != ancestor && ROB_FindAncestor(rob_objects[count], ancestor)) {
				ROB_FreeObject(rob_objects[count], FALSE);
				free_count++;
			}
		}
		#ifdef DEBUG_ROB_PROGRESS_DUMP
			printf("%s: Freeing %i objects", __func__, free_count);
			if (free_ancestor) printf(" and ancestor id=%i", ancestor->id);
			printf("\n");
		#endif
		if (free_ancestor) {
			ROB_FreeObject(ancestor, FALSE);
		}
	}

	return retval;
}

/***************************************************************************
 * Free Object                                                             *
 ***************************************************************************/
/* When freeing a parent object, the child objects will have their pob
   nullified up until the point when (or if) the parent is reinstated.
   Images declared within the object are freed and any objects that are
   also referencing these images through img_surface and img_over_surface
   will have these properties nullified */

/* On entry: object is the object to free
			 free_images_only = TRUE to free ONLY any declared images */
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_FreeObject(ROB_Object *rob_object, int free_images_only) {
	int retval = ROB_ERROR_NONE, count, count2;
	SDL_Surface *img_surface;

	if (rob_object_count <= 0) {
		sprintf(rob_last_error, "%s: There is no object to free", __func__);
		retval = ROB_ERROR_INVALID_OBJECT;
	} else if (rob_object == NULL || sizeof(*rob_object) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: object is invalid", __func__);
		retval = ROB_ERROR_INVALID_OBJECT;
	} else {
		#ifdef DEBUG_ROB_OBJECTS
			printf("%s: Freeing %p (%i)\n", __func__, rob_object, rob_object->id);
		#endif
		/* Free img_surface and img_over_surface if either are declared */
		for (count = 0; count < 2; count++) {
			if (count == 0) {
				img_surface = rob_object->img_surface;
			} else {
				img_surface = rob_object->img_over_surface;
			}
			if (img_surface != NULL) {
				/* Nullify all surface pointers within rob_objects that reference this surface */
				for (count2 = 0; count2 < rob_object_count; count2++) {
					if (rob_objects[count2]->img_surface == img_surface) {
						rob_objects[count2]->img_surface = NULL;
					}
					if (rob_objects[count2]->img_over_surface == img_surface) {
						rob_objects[count2]->img_over_surface = NULL;
					}
				}
				/* Free the SDL surface but not rob_sprites or GNU Robbo images */
				if (img_surface != rob_sprites && 
					img_surface != wm_icon && 
					img_surface != icons && 
					img_surface != ciphers && 
					img_surface != alpha && 
					img_surface != bgrnd && 
					img_surface != k_icons) {
					SDL_FreeSurface(img_surface);
				}
			}
		}
		if (!free_images_only) {
			/* If the current object is the parent to other objects then nullify their pob */
			for (count = 0; count < rob_object_count; count++) {
				if (rob_objects[count]->pob == rob_object) rob_objects[count]->pob = NULL;
			}
			/* If anything is above the current object then move it down the array */
			for (count = rob_object->zorder; count < rob_object_count - 1; count++) {
				rob_objects[count] = rob_objects[count + 1];
				rob_objects[count]->zorder--;
			}
			/* Nullify rob_oup if it's the same object */
			if (rob_oup == rob_object) rob_oup = NULL;

			/* Nullify rob_pressed_object if it's the same object */
			if (rob_pressed_object == rob_object) rob_pressed_object = NULL;

			/* Free the memory and update the array element count now */
			free(rob_object);
			rob_object_count--;
		}
	}

	return retval;
}

/***************************************************************************
 * Render Objects                                                          *
 ***************************************************************************/
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */
	
int ROB_RenderObjects(void) {
	int cx, cy, ix = 0, iy = 0, iw = 0, ih = 0, alpha;
	int overeventfound, presseventfound, use_rleaccel = 0;
	int count, retval = ROB_ERROR_NONE;
	SDL_Surface *img_surface = NULL;
	SDL_Rect srcrect, dstrect;
	Uint32 bgcolour = 0;
	
	/* Iterate through the objects */
	for (count = 0; count < rob_object_count; count++) {
		/* Get the object's visibility */
		if (ROB_GetVisibility(rob_objects[count])) {

			/* Is there an over event waiting for this object? */
			overeventfound = presseventfound = FALSE;
			if (rob_objects[count] == rob_oup && ROB_GetEnability(rob_objects[count])) {
				overeventfound = TRUE;
				/* Is there a press event waiting for this object? */
				if (rob_objects[count] == rob_pressed_object) presseventfound = TRUE;
			}

			/* Get object's canvas offset */
			ROB_GetCanvasOffset(rob_objects[count], &cx, &cy);

			/* Get object's alpha and alpha_{over|press} */
			alpha = ROB_GetAlpha(rob_objects[count]);
			if (overeventfound && rob_objects[count]->alpha_over != ROB_UNDEFINED)
				alpha = rob_objects[count]->alpha_over;
			if (presseventfound && rob_objects[count]->alpha_press != ROB_UNDEFINED)
				alpha = rob_objects[count]->alpha_press;

			/* Get object's bgcolour and bgcolour_{over|press} */
			bgcolour = rob_objects[count]->bgcolour;
			if (overeventfound && rob_objects[count]->bgcolour_over != ROB_UNDEFINED)
				bgcolour = rob_objects[count]->bgcolour_over;
			if (presseventfound && rob_objects[count]->bgcolour_press != ROB_UNDEFINED)
				bgcolour = rob_objects[count]->bgcolour_press;

			/* Get object's img_{over_|press_}surface */
			img_surface = rob_objects[count]->img_surface;
			ix = rob_objects[count]->img_x; iy = rob_objects[count]->img_y;
			iw = rob_objects[count]->img_w; ih = rob_objects[count]->img_h;
			if (overeventfound && rob_objects[count]->img_over_surface != NULL) {
				img_surface = rob_objects[count]->img_over_surface;
				ix = rob_objects[count]->img_over_x; iy = rob_objects[count]->img_over_y;
				iw = rob_objects[count]->img_over_w; ih = rob_objects[count]->img_over_h;
			}
			if (presseventfound && rob_objects[count]->img_press_surface != NULL) {
				img_surface = rob_objects[count]->img_press_surface;
				ix = rob_objects[count]->img_press_x; iy = rob_objects[count]->img_press_y;
				iw = rob_objects[count]->img_press_w; ih = rob_objects[count]->img_press_h;
			}

			/** Is there a colour to fill? */
			if (bgcolour != ROB_UNDEFINED) {
				bgcolour = SDL_MapRGB(screen->format, bgcolour >> 16 & 0xff,
					bgcolour >> 8 & 0xff, bgcolour & 0xff);
				if (alpha == 255) {
					/* Fill rectangle */
					dstrect.x = cx; dstrect.y = cy;
					dstrect.w = rob_objects[count]->w; dstrect.h = rob_objects[count]->h;
					if ((SDL_FillRect(screen, &dstrect, bgcolour)) < 0) {
						sprintf(rob_last_error, "%s: Cannot fill surface: %s", __func__, SDL_GetError());
						retval = ROB_ERROR_CANNOT_FILL_SURFACE;
						break;
					}
				} else {
					/* Fill rectangle */
					dstrect.x = cx; dstrect.y = cy;
					dstrect.w = rob_objects[count]->w; dstrect.h = rob_objects[count]->h;
					retval = ROB_FillRectA(screen, &dstrect, bgcolour, alpha);
					if (retval) break;
				}
			}

			/** Is there an image to attach? */
			if (img_surface != NULL) {
				/* If image region is unused then use entire image */
				if (ix == ROB_UNDEFINED && iy == ROB_UNDEFINED &&
					iw == ROB_UNDEFINED && ih == ROB_UNDEFINED) {
					ix = 0; iy = 0; iw = img_surface->w; ih = img_surface->h;
				}
				if (alpha != 255) {
					/* Record surface's use of SDL_RLEACCEL.
					   Because the surface is not compressed until the first blit,
					   SDL_RLEACCELOK must be checked for too, I imagine as compression is pending */
					if (img_surface->flags & (SDL_RLEACCEL | SDL_RLEACCELOK)) use_rleaccel = SDL_RLEACCEL;
					/* Set the image surface alpha. */
					/* Note that this removes SDL_RLEACCEL because using it with alpha
					   is very very slow; it's restored at the end of the function */
					if ((SDL_SetAlpha(img_surface, SDL_SRCALPHA, alpha)) < 0) {
						sprintf(rob_last_error, "%s: Cannot set surface alpha: %s", __func__, SDL_GetError());
						retval = ROB_ERROR_CANNOT_SET_SURFACE_ALPHA;
						break;
					}
				}
				srcrect.x = ix; srcrect.y = iy; srcrect.w = iw; srcrect.h = ih;
				dstrect.x = cx; dstrect.y = cy; dstrect.w = rob_objects[count]->w; dstrect.h = rob_objects[count]->h;
				/* Blit surface */
				if ((SDL_BlitSurface(img_surface, &srcrect, screen, &dstrect)) < 0) {
					sprintf(rob_last_error, "%s: Cannot blit surface: %s", __func__, SDL_GetError());
					retval = ROB_ERROR_CANNOT_BLIT_SURFACE;
					break;
				}
				if (alpha != 255) {
					/* Disable the image surface alpha and restore SDL_RLEACCEL if set.
					   Note that officially NOT passing the SDL_SRCALPHA flag disables
					   alpha blits from this surface but then it's not possible to reenable
					   SDL_RLEACCEL, so I simply set the alpha value to SDL_ALPHA_OPAQUE
					   (255) which achieves the same result. */
					if ((SDL_SetAlpha(img_surface, SDL_SRCALPHA | use_rleaccel, 255)) < 0) {
						sprintf(rob_last_error, "%s: Cannot set surface alpha: %s", __func__, SDL_GetError());
						retval = ROB_ERROR_CANNOT_SET_SURFACE_ALPHA;
						break;
					}
				}
			}
		}
	}

	return retval;
}

/***************************************************************************
 * FillRectA                                                               *
 ***************************************************************************/
/* SDL_BlitSurface alpha blends, so an alpha rectangle first needs to become
   a surface with per-surface-alpha set and then blitted to the screen */

/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_FillRectA(SDL_Surface *surface, SDL_Rect *dstrect, Uint32 colour, Uint8 alpha) {
	int retval = ROB_ERROR_NONE;
	
	/* Set the blend surface alpha */
	if ((SDL_SetAlpha(rob_blend_screen, SDL_SRCALPHA, alpha)) < 0) {
		sprintf(rob_last_error, "%s: Cannot set surface alpha: %s", __func__, SDL_GetError());
		retval = ROB_ERROR_CANNOT_SET_SURFACE_ALPHA;
	}
	if (!retval) {
		/* Fill rectangle */
		if ((SDL_FillRect(rob_blend_screen, dstrect, colour)) < 0) {
			sprintf(rob_last_error, "%s: Cannot fill surface: %s", __func__, SDL_GetError());
			retval = ROB_ERROR_CANNOT_FILL_SURFACE;
		}
	}
	if (!retval) {
		/* Blit surface */
		if ((SDL_BlitSurface(rob_blend_screen, dstrect, surface, dstrect)) < 0) {
			sprintf(rob_last_error, "%s: Cannot blit surface: %s", __func__, SDL_GetError());
			retval = ROB_ERROR_CANNOT_BLIT_SURFACE;
		}
	}

	return retval;
}

/***************************************************************************
 * Get Canvas Offset                                                       *
 ***************************************************************************/
/* This gets an object's canvas offset.
   If a NULL pob is found then it will be interpreted as rob_lyr_canvas */

int ROB_GetCanvasOffset(ROB_Object *rob_object, int *cx, int *cy) {
	int retval = ROB_ERROR_NONE;

	if (rob_object == NULL || sizeof(*rob_object) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: object is invalid", __func__);
		retval = ROB_ERROR_INVALID_OBJECT;
	} else {
		*cx = rob_object->x;
		*cy = rob_object->y;
		while (rob_object != rob_lyr_canvas && rob_object != rob_lyr_root) {
			if (rob_object->pob == NULL) {
				rob_object = rob_lyr_canvas;
			} else {
				rob_object = rob_object->pob;
			}
			*cx += rob_object->x;
			*cy += rob_object->y;
		}
	}
	
	return retval;
}

/***************************************************************************
 * Get Visibility                                                          *
 ***************************************************************************/
/* This gets an object's visibility.
   If a NULL pob is found then it will be interpreted as rob_lyr_canvas */
   
/*  On exit: returns TRUE or FALSE */

int ROB_GetVisibility(ROB_Object *rob_object) {
	
	if (rob_object == NULL || sizeof(*rob_object) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: object is invalid", __func__);
		return TRUE;	/* If NULL object this stops segfaults */
	} else {
		#ifdef DEBUG_ROB_GET_VISIBILITY
			printf("\n%s: Visibility for object %i\n", __func__, rob_object->id);
			printf("%s: id=%i pob=%p visible=%i\n", __func__, rob_object->id, rob_object->pob, rob_object->visible);
		#endif
		while (rob_object->visible == ROB_INHERITED) {
			if (rob_object == rob_lyr_root || rob_object == rob_lyr_canvas) {
				break;
			} else {
				if (rob_object->pob == NULL) {
					rob_object = rob_lyr_canvas;
				} else {
					rob_object = rob_object->pob;
				}
			}
			#ifdef DEBUG_ROB_GET_VISIBILITY
				printf("%s: id=%i pob=%p visible=%i\n", __func__, rob_object->id, rob_object->pob, rob_object->visible);
			#endif
		}
	}

	/* Something valid must be returned and this may not be the
	   case if the user has been fiddling with the system objects */
	if (rob_object->visible < 0 || rob_object->visible > 1) {
		return TRUE;
	} else {
		return rob_object->visible;
	}
}

/***************************************************************************
 * Get Enability                                                           *
 ***************************************************************************/
/* This gets an object's enability.
   If a NULL pob is found then it will be interpreted as rob_lyr_canvas */
   
/*  On exit: returns TRUE or FALSE */

int ROB_GetEnability(ROB_Object *rob_object) {
	
	if (rob_object == NULL || sizeof(*rob_object) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: object is invalid", __func__);
		return TRUE;	/* If NULL object this stops segfaults */
	} else {
		#ifdef DEBUG_ROB_GET_ENABILITY
			printf("\n%s: Enability for object %i\n", __func__, rob_object->id);
			printf("%s: id=%i pob=%p enabled=%i\n", __func__, rob_object->id, rob_object->pob, rob_object->enabled);
		#endif
		while (rob_object->enabled == ROB_INHERITED) {
			if (rob_object == rob_lyr_root || rob_object == rob_lyr_canvas) {
				break;
			} else {
				if (rob_object->pob == NULL) {
					rob_object = rob_lyr_canvas;
				} else {
					rob_object = rob_object->pob;
				}
			}
			#ifdef DEBUG_ROB_GET_ENABILITY
				printf("%s: id=%i pob=%p enabled=%i\n", __func__, rob_object->id, rob_object->pob, rob_object->enabled);
			#endif
		}
	}

	/* Something valid must be returned and this may not be the
	   case if the user has been fiddling with the system objects */
	if (rob_object->enabled < 0 || rob_object->enabled > 1) {
		return TRUE;
	} else {
		return rob_object->enabled;
	}
}

/***************************************************************************
 * Get Alpha                                                               *
 ***************************************************************************/
/* This gets an object's alpha.
   If a NULL pob is found then it will be interpreted as rob_lyr_canvas */
   
/*  On exit: returns 0 to 255 */

int ROB_GetAlpha(ROB_Object *rob_object) {
	
	if (rob_object == NULL || sizeof(*rob_object) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: object is invalid", __func__);
		return SDL_ALPHA_OPAQUE;	/* If NULL object this stops segfaults */
	} else {
		while (rob_object->alpha == ROB_INHERITED) {
			if (rob_object == rob_lyr_root || rob_object == rob_lyr_canvas) {
				break;
			} else {
				if (rob_object->pob == NULL) {
					rob_object = rob_lyr_canvas;
				} else {
					rob_object = rob_object->pob;
				}
			}
		}
	}

	/* Something valid must be returned and this may not be the
	   case if the user has been fiddling with the system objects */
	if (rob_object->alpha < 0 || rob_object->alpha > SDL_ALPHA_OPAQUE) {
		return SDL_ALPHA_OPAQUE;
	} else {
		return rob_object->alpha;
	}
}

/***************************************************************************
 * Get Object Under Object                                                 *
 ***************************************************************************/
/*  On exit: returns a pointer to the visible object under object on success else NULL */

ROB_Object *ROB_GetObjectUnderObject(ROB_Object *rob_object, int hitpoint) {
	int count, cx, cy, cx2, cy2;
	
	if (rob_object == NULL || sizeof(*rob_object) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: object is invalid", __func__);
	} else if (hitpoint != ROB_HIT_POINT_TOP_LEFT && hitpoint != ROB_HIT_POINT_CENTRE) {
		sprintf(rob_last_error, "%s: hitpoint is invalid", __func__);
	} else {
		/* Get hit point of source object */
		ROB_GetCanvasOffset(rob_object, &cx, &cy);
		if (hitpoint == ROB_HIT_POINT_TOP_LEFT) {
			/* cx and cy are ok */
		} else if (hitpoint == ROB_HIT_POINT_CENTRE) {
			cx += rob_object->w / 2;		
			cy += rob_object->h / 2;		
		}
		for (count = rob_object_count - 1; count >= 0; count--) {
			if (ROB_GetVisibility(rob_objects[count]) && count < rob_object->zorder) {
				ROB_GetCanvasOffset(rob_objects[count], &cx2, &cy2);
				if (cx >= cx2 && cx < cx2 + rob_objects[count]->w && cy >= cy2 && cy < cy2 + rob_objects[count]->h) {
					return rob_objects[count];
				}
			}
		}
	}

	return NULL;
}

/***************************************************************************
 * Find Ancestor                                                           *
 ***************************************************************************/
/* This traces back from (and including) an object looking for a
   particular ancestor.
   If a NULL pob is found then it will be interpreted as rob_lyr_canvas */

/*  On exit: returns TRUE if found else FALSE */

int ROB_FindAncestor(ROB_Object *rob_object, ROB_Object *target) {

	if (rob_object == NULL || sizeof(*rob_object) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: object is invalid", __func__);
	} else if (target == NULL || sizeof(*target) != sizeof(ROB_Object)) {
		sprintf(rob_last_error, "%s: target is invalid", __func__);
	} else {
		#ifdef DEBUG_ROB_FIND_ANCESTOR
			printf("\n%s: Looking for object %i's ancestor %i\n", __func__, rob_object->id, target->id);
			printf("%s: id=%i pob=%p\n", __func__, rob_object->id, rob_object->pob);
		#endif
		while (rob_object != target) {
			if (rob_object == rob_lyr_root || rob_object == rob_lyr_canvas) {
				return FALSE;
			} else {
				if (rob_object->pob == NULL) {
					rob_object = rob_lyr_canvas;
				} else {
					rob_object = rob_object->pob;
				}
			}
			#ifdef DEBUG_ROB_FIND_ANCESTOR
				printf("%s: id=%i pob=%p\n", __func__, rob_object->id, rob_object->pob);
			#endif
		}
	}

	return TRUE;
}

/***************************************************************************
 * Generate Events                                                         *
 ***************************************************************************/

void ROB_GenerateEvents(int *actionid) {
	static int last_pointer_x = 0, last_pointer_y = 0;
	static int press_point_x = 0, press_point_y = 0;
	int mx, my, px, py, pxu = 0, pyu = 0;
	ROB_Object *new_oup, *pob;
	ROB_Event rob_event;

	if (rob_op_env.pointer_mode == ROB_POINTER_MODE_OFF) {
		rob_oup = rob_lyr_root;
	} else {
		/* Get or set the object currently under the pointer. If an object is currently
		   being pressed and it's enabled and draggable then it is attached to the pointer */
		if (rob_pressed_object != NULL && ROB_GetEnability(rob_pressed_object) && rob_pressed_object->draggable) {
			/* Force it as rob_oup to prevent dragging-off through latency */
			rob_oup = rob_pressed_object;
			ROB_GetCanvasOffset(rob_oup->pob, &px, &py);
			/* Attach the object to the pointer */
			ROB_GetCanvasOffset(rob_lyr_pointer, &mx, &my);
			rob_oup->x = mx - px - press_point_x;
			rob_oup->y = my - py - press_point_y;
			/* Adjust x/y for the granularity: n = n + (n % g) / (g / 2) * g - (n % g) */
			if (rob_oup->drag_granularity > 1) {
				rob_oup->x += rob_oup->x % rob_oup->drag_granularity / (rob_oup->drag_granularity / 2) * rob_oup->drag_granularity - rob_oup->x % rob_oup->drag_granularity;
				rob_oup->y += rob_oup->y % rob_oup->drag_granularity / (rob_oup->drag_granularity / 2) * rob_oup->drag_granularity - rob_oup->y % rob_oup->drag_granularity;
			}
			/* Is dragging restricted in any way? */
			if (rob_oup->draggable == ROB_DRAG_RESTRICT_TO_PARENT || rob_oup->draggable == ROB_DRAG_RESTRICT_TO_PARENT_ORIGIN) {
				pob = rob_oup->pob;
				if (rob_oup->draggable == ROB_DRAG_RESTRICT_TO_PARENT) {
					if (rob_oup->x + rob_oup->w > pob->w) rob_oup->x = pob->w - rob_oup->w;
					if (rob_oup->y + rob_oup->h > pob->h) rob_oup->y = pob->h - rob_oup->h;
				}
				if (rob_oup->x < 0) rob_oup->x = 0;
				if (rob_oup->y < 0) rob_oup->y = 0;
			}
		} else {
			new_oup = ROB_GetObjectUnderObject(rob_lyr_pointer, ROB_HIT_POINT_TOP_LEFT);
			/* rob_oup could be NULL here because it may have been a freed object but
			   new_oup will most definitely contain at least a system object */
			if (rob_oup != new_oup) {
				/* Send one out event if the object is not NULL and enabled */
				if (rob_oup != NULL && ROB_GetEnability(rob_oup)) {
					rob_event.rob_object = rob_oup; rob_event.klasse = ROB_EVENT_OUT; ROB_SendEvent(&rob_event);
				}
				rob_oup = new_oup;
				/* Send one over event if the object is enabled */
				if (ROB_GetEnability(rob_oup)) {
					rob_event.rob_object = rob_oup; rob_event.klasse = ROB_EVENT_OVER; ROB_SendEvent(&rob_event);
				}
			}
			ROB_GetCanvasOffset(rob_oup->pob, &px, &py);
		}
		/* Detect dragging of the pointer across enabled objects */
		if (rob_pressed_object != NULL && rob_pressed_object == rob_oup && ROB_GetEnability(rob_oup)) {
			if (last_pointer_x != rob_lyr_pointer->x || last_pointer_y != rob_lyr_pointer->y) {
				rob_event.rob_object = rob_oup; rob_event.klasse = ROB_EVENT_DRAG; ROB_SendEvent(&rob_event);
			}
		}
		/* Record current pointer offsets for later comparison */
		last_pointer_x = rob_lyr_pointer->x;
		last_pointer_y = rob_lyr_pointer->y;
	
		/* Process any reported action */
		if (*actionid != ROB_UNDEFINED) {
			switch (*actionid) {
				case ACTION_UP:
					if (rob_op_env.pointer_mode == ROB_POINTER_MODE_SIMULATED) {
						pyu = -1;
					}
					break;
				case ACTION_UP_RIGHT:
					if (rob_op_env.pointer_mode == ROB_POINTER_MODE_SIMULATED) {
						pyu = -1; pxu = 1;
					}
					break;
				case ACTION_RIGHT:
					if (rob_op_env.pointer_mode == ROB_POINTER_MODE_SIMULATED) {
						pxu = 1;
					}
					break;
				case ACTION_DOWN_RIGHT:
					if (rob_op_env.pointer_mode == ROB_POINTER_MODE_SIMULATED) {
						pyu = 1; pxu = 1;
					}
					break;
				case ACTION_DOWN:
					if (rob_op_env.pointer_mode == ROB_POINTER_MODE_SIMULATED) {
						pyu = 1;
					}
					break;
				case ACTION_DOWN_LEFT:
					if (rob_op_env.pointer_mode == ROB_POINTER_MODE_SIMULATED) {
						pyu = 1; pxu = -1;
					}
					break;
				case ACTION_LEFT:
					if (rob_op_env.pointer_mode == ROB_POINTER_MODE_SIMULATED) {
						pxu = -1;
					}
					break;
				case ACTION_UP_LEFT:
					if (rob_op_env.pointer_mode == ROB_POINTER_MODE_SIMULATED) {
						pyu = -1; pxu = -1;
					}
					break;
				case ACTION_SELECT:
					if (rob_op_env.pointer_mode == ROB_POINTER_MODE_PHYSICAL) break;
				case ACTION_PRIMARY_CLICK:
					if (rob_pressed_object == NULL) {
						rob_pressed_object = rob_oup;
						/* Only generate events for enabled objects */
						if (ROB_GetEnability(rob_oup)) {
							rob_event.rob_object = rob_oup; rob_event.klasse = ROB_EVENT_PRESS; ROB_SendEvent(&rob_event);
							rob_event.rob_object = rob_oup; rob_event.klasse = ROB_EVENT_SELECT; ROB_SendEvent(&rob_event);
							/* For draggable [enabled] objects, calculate the drag points */
							if (rob_oup->draggable) {
								/* Get the current pointer coordinates */
								ROB_GetCanvasOffset(rob_lyr_pointer, &mx, &my);
								if (rob_oup->drag_point == ROB_DRAG_POINT_ANYWHERE) {
									press_point_x = mx - px - rob_oup->x;
									press_point_y = my - py - rob_oup->y;
								} else if (rob_oup->drag_point == ROB_DRAG_POINT_MIDDLE) {
									press_point_x = rob_oup->w / 2;
									press_point_y = rob_oup->h / 2;
								}
							}
						}
					} else {	/* Something is already being pressed */
						/* Only generate events for enabled objects */
						if (rob_pressed_object == rob_oup && ROB_GetEnability(rob_oup)) {
							rob_event.rob_object = rob_oup; rob_event.klasse = ROB_EVENT_SELECT; ROB_SendEvent(&rob_event);
						}
					}
					break;
				case ACTION_SELECT | 0x80:
					if (rob_op_env.pointer_mode == ROB_POINTER_MODE_PHYSICAL) break;
				case ACTION_PRIMARY_CLICK | 0x80:
					if (rob_pressed_object != NULL) {
						if (rob_pressed_object == rob_oup) {
							/* Only generate events for enabled objects */
							if (ROB_GetEnability(rob_oup)) {
								rob_event.rob_object = rob_oup; rob_event.klasse = ROB_EVENT_CLICK; ROB_SendEvent(&rob_event);
							}
						} else {
							/* Only generate events for enabled objects */
							if (ROB_GetEnability(rob_pressed_object)) {
								rob_event.rob_object = rob_oup; rob_event.klasse = ROB_EVENT_RELEASE; ROB_SendEvent(&rob_event);
							}
						}
					}
					rob_pressed_object = NULL;
					break;
			}
			/* Manage pointer input if required */
			if (rob_op_env.pointer_mode == ROB_POINTER_MODE_SIMULATED) ROB_ManagePointerInput(pxu, pyu);
		}
	}

}

/***************************************************************************
 * Manage Pointer Input                                                    *
 ***************************************************************************/

void ROB_ManagePointerInput(int pxu, int pyu) {
	static int lastpxu = 0, lastpyu = 0, unitcount = 0;
	int xmov, ymov;
	
	/* Is the x or y unit the same as last time? */
	if ((pxu != 0 && pxu == lastpxu) || (pyu != 0 && pyu == lastpyu)) {
		unitcount++;
		/* If the count reaches a threshold then change gear */
		if (unitcount < rob_op_env.pointer_move_unit_threshold) {
			xmov = pxu * rob_op_env.pointer_move_unit_low;
			ymov = pyu * rob_op_env.pointer_move_unit_low;
		} else {
			xmov = pxu * rob_op_env.pointer_move_unit_high;
			ymov = pyu * rob_op_env.pointer_move_unit_high;
		}
	} else {
		unitcount = 0;
		xmov = pxu * rob_op_env.pointer_move_unit_low;
		ymov = pyu * rob_op_env.pointer_move_unit_low;
	}
	lastpxu = pxu; lastpyu = pyu;
		
	/* Now move the pointer */
	rob_lyr_pointer->x += xmov;
	rob_lyr_pointer->y += ymov;

	/* Keep it within the bounds of the screen */
	if (rob_lyr_pointer->x < 0) {
		rob_lyr_pointer->x = 0;
	} else if (rob_lyr_pointer->x > screen->w - 1) {
		rob_lyr_pointer->x = screen->w - 1;
	}
	if (rob_lyr_pointer->y < 0) {
		rob_lyr_pointer->y = 0;
	} else if (rob_lyr_pointer->y > screen->h - 1) {
		rob_lyr_pointer->y = screen->h - 1;
	}

}

/***************************************************************************
 * Send Event                                                              *
 ***************************************************************************/
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_SendEvent(ROB_Event *rob_event) {
	int retval = ROB_ERROR_NONE;

	if (rob_event == NULL || sizeof(*rob_event) != sizeof(ROB_Event)) {
		sprintf(rob_last_error, "%s: event is invalid", __func__);
		retval = ROB_ERROR_INVALID_EVENT;
	} else {
		#ifdef DEBUG_ROB_EVENTS
			ROB_ShowEvent(rob_event, __func__);
		#endif
		rob_op_env.event_processor(rob_event);
	}
	
	return retval;
}

/***************************************************************************
 * Show Event                                                              *
 ***************************************************************************/

int ROB_ShowEvent(ROB_Event *rob_event, const char *func) {
	int retval = ROB_ERROR_NONE;

	if (rob_event == NULL || sizeof(*rob_event) != sizeof(ROB_Event)) {
		sprintf(rob_last_error, "%s: event is invalid", func);
		retval = ROB_ERROR_INVALID_EVENT;
	} else {
		printf("%i: ", cycle_count);
		switch (rob_event->klasse) {
			case ROB_EVENT_OVER:
				printf("%s: ROB_EVENT_OVER", func);
				break;
			case ROB_EVENT_OUT:
				printf("%s: ROB_EVENT_OUT", func);
				break;
			case ROB_EVENT_PRESS:
				printf("%s: ROB_EVENT_PRESS", func);
				break;
			case ROB_EVENT_CLICK:
				printf("%s: ROB_EVENT_CLICK", func);
				break;
			case ROB_EVENT_RELEASE:
				printf("%s: ROB_EVENT_RELEASE", func);
				break;
			case ROB_EVENT_DRAG:
				printf("%s: ROB_EVENT_DRAG", func);
				break;
			case ROB_EVENT_SELECT:
				printf("%s: ROB_EVENT_SELECT", func);
				break;
			default:
				printf("%s: Unknown", func);
				break;
		}
		printf(" object=%p id=%i x=%i y=%i w=%i h=%i\n", rob_event->rob_object, rob_event->rob_object->id, rob_event->rob_object->x, rob_event->rob_object->y, rob_event->rob_object->w, rob_event->rob_object->h);
	}

	return retval;	
}

/***************************************************************************
 * Set Operating Environment - System Pointer                              *
 ***************************************************************************/
/* On entry: systempointer is true or false */
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_SetOpEnvSystemPointer(int systempointer) {
	int retval = ROB_ERROR_NONE;
	
	#ifdef DEBUG_ROB_PROGRESS_DUMP
		printf("%s: systempointer=%i\n", __func__, systempointer);
	#endif
	/* Store new value(s) */
	rob_op_env.systempointer = systempointer;
	SDL_ShowCursor(systempointer);
	
	return retval;
}

/***************************************************************************
 * Set Operating Environment - Pointer                                     *
 ***************************************************************************/
/* On entry: pointer's visibility is true or false
			 pointer_mode is one of the modes as listed in ROB_engine.h */
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_SetOpEnvPointer(int pointer, int pointer_mode) {
	int retval = ROB_ERROR_NONE;
	#ifdef DEBUG_ROB_PROGRESS_DUMP
		char *pointer_modes[] = {"off", "physical", "simulated"};
	#endif

	#ifdef DEBUG_ROB_PROGRESS_DUMP
		printf("%s: pointer=%i pointer_mode=%s\n", __func__, pointer, pointer_modes[pointer_mode]);
	#endif
	/* Store new value(s) */
	rob_op_env.pointer = pointer;
	rob_op_env.pointer_mode = pointer_mode;
	if (rob_object_count > 0) rob_lyr_pointer->visible = pointer;
	
	return retval;
}

/***************************************************************************
 * Set Operating Environment - Event Processor                             *
 ***************************************************************************/
/* On entry: event_processor is a pointer to a function that receives events */
/*  On exit: returns ROB_ERROR_NONE on success else
			 any one of the error codes in ROB_engine.h */

int ROB_SetOpEnvEventProcessor(void (*event_processor)(ROB_Event *rob_event)) {
	int retval = ROB_ERROR_NONE;
	
	#ifdef DEBUG_ROB_PROGRESS_DUMP
		printf("%s: %p\n", __func__, event_processor);
	#endif
	/* Store new value(s) */
	rob_op_env.event_processor = event_processor;
	
	return retval;
}










