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
#define MAX_W 32		/* max width of the board */
#define MAX_H 31		/* max height of the board */
#define MAX_ICONS 32
#define MAX_EFFECTS 32

#define DEFAULT_VIEWPORT_WIDTH 16
#define DEFAULT_VIEWPORT_HEIGHT 12

/* Object delays */
//#undef _SMOOTH_SCRL_
#ifdef _SMOOTH_SCRL_ 
#define DELAY_RADIOACTIVE_FIELD  (3*SCRL_MULT1)
#define DELAY_BIRD (4*SCRL_MULT1)
#define DELAY_LITTLE_BOOM (2*SCRL_MULT1)	/* Thunor: I changed this from 1, as it's more like half a BIG_BOOM */
#define DELAY_BEAR (4*SCRL_MULT1)
#define DELAY_BUTTERFLY (4*SCRL_MULT1)
#define DELAY_BIGBOOM (3*SCRL_MULT1)
#define DELAY_ROBBO (4*SCRL_MULT1)
#define DELAY_TELEPORT (8*SCRL_MULT1)	/* Thunor: was 15, 8 is similar to Robbo */
#define DELAY_LASER (4*SCRL_MULT1)
#define DELAY_CAPSULE (10*SCRL_MULT1)
#define DELAY_GUN (8*SCRL_MULT1)
#define DELAY_ROTATION (20*SCRL_MULT1)
#define DELAY_BLASTER (4*SCRL_MULT1)
#define DELAY_PUSHBOX (4*SCRL_MULT1)
#define DELAY_BARRIER (4*SCRL_MULT1)
#define DELAY_TELEPORTING (3*SCRL_MULT1)
#define DELAY_BOMB_TARGET (DELAY_BIGBOOM + 2)	/* Was DELAY_BIGBOOM * 2 (6) but 5 is spot on: use RobboIX-003 and Original-020 to test */
#define DELAY_ROBBO_ANIMATE (DELAY_ROBBO / 2)
#define DELAY_MAGNET_ATTRACT DELAY_ROBBO	/* Was DELAY_ROBBO * 2 but it's the same as Robbo moving: use Original-011 to test */
#define DELAY_BLINKSCREEN (6*SCRL_MULT1)	/* how long screen should blink after exit opening */
#define DELAY_RESTART (DELAY_BIGBOOM * 8)
#else
#define DELAY_RADIOACTIVE_FIELD  3
#define DELAY_BIRD 4
#define DELAY_LITTLE_BOOM 2	/* Thunor: I changed this from 1, as it's more like half a BIG_BOOM */
#define DELAY_BEAR 4
#define DELAY_BUTTERFLY 4
#define DELAY_BIGBOOM 3
#define DELAY_ROBBO 4
#define DELAY_TELEPORT 8	/* Thunor: was 15, 8 is similar to Robbo */
#define DELAY_LASER 4
#define DELAY_CAPSULE 10
#define DELAY_GUN 8
#define DELAY_ROTATION 20
#define DELAY_BLASTER 4
#define DELAY_PUSHBOX 4
#define DELAY_BARRIER 4
#define DELAY_TELEPORTING 3
#define DELAY_BOMB_TARGET (DELAY_BIGBOOM + 2)	/* Was DELAY_BIGBOOM * 2 (6) but 5 is spot on: use RobboIX-003 and Original-020 to test */
#define DELAY_ROBBO_ANIMATE (DELAY_ROBBO / 2)
#define DELAY_MAGNET_ATTRACT DELAY_ROBBO	/* Was DELAY_ROBBO * 2 but it's the same as Robbo moving: use Original-011 to test */
#define DELAY_BLINKSCREEN 6	/* how long screen should blink after exit opening */
#define DELAY_RESTART (DELAY_BIGBOOM * 8)
#endif
/* Object ids */
#define EMPTY_FIELD 0
#define ROBBO 1
#define WALL 2
#define WALL_RED 3
#define SCREW 4
#define BULLET 5
#define BOX 6
#define KEY 7
#define BOMB 8
#define DOOR 9
#define QUESTIONMARK 10
#define BEAR 11
#define BIRD 13
#define CAPSULE 15
#define LITTLE_BOOM 21
#define GROUND 24
#define WALL_GREEN 25
#define BEAR_B 26
#define BUTTERFLY 28
#define LASER_L 30
#define LASER_D 32
#define SOLID_LASER_L 34	/* Thunor: The solid laser uses only one frame so I'm seperating it so that I can create different laser/gun images for skins */
#define SOLID_LASER_D 36	/* Thunor: The solid laser uses only one frame so I'm seperating it so that I can create different laser/gun images for skins */
#define TELEPORT 40
#define TELEPORTING 41
#define BIG_BOOM 42
#define GUN 50
#define MAGNET 54
#define BLASTER 58
#define BLACK_WALL 59
#define PUSH_BOX 60
#define BARRIER 61
#define FAT_WALL 63
#define ROUND_WALL 64
#define BOULDER_WALL 65
#define SQUARE_WALL 66
#define LATTICE_WALL 67
#define RADIOACTIVE_FIELD  68
#define STOP 69
#define BOMB2 70      /* neurocyp: Robbo alex has two types of bombs, we'd like to support it */
#define S_WALL 71   // Special type of the wall

#define MECHANIC_SENSIBLE_BEARS TRUE
#define MECHANIC_SENSIBLE_QUESTIONMARKS TRUE
#define MECHANIC_SENSIBLE_SOLID_LASERS TRUE


#define EFFECT_NONE   -1

/* Variables */
int restart_timeout;		/* Time to wait before restarting a level after Robbo dies */

struct Coords
{
  int x;
  int y;
};

struct object
{
  int type;
  int state;			/* This is for animation and is used as an index into the icon Coords array lower down */
  int direction;		/* 0r 1d 2l 3u */
  int destroyable;		/* can be destroyed */
  int blowable;			/* can be blowed up */
  int killing;			/* is object dangerous for robbo */
  int moved;			/* When last object was moved (all movable) */
  int blowed;			/* Should object be blowed up? */
  int shooted;			/* When lately object shooted (guns) */
  int rotated;			/* When object lately was rotated (guns) */
  int solidlaser;		/* Does gun shoots solid or normal laser */
  int rotable;			/* If object can be rotated ? (guns) */
  int randomrotated;		/* When object has undetermining rotation */
  int teleportnumber;		/* Number of teleport (kind of theleport) */
  int teleportnumber2;		/* ID of teleport (for teleports with the same number */
  int id_questionmark;		/* What object is covered under questionmark */
  int direction2;		/* direction of moveing (for guns) (if direction2 for birds != (direction+-1) bird shoots */
  int movable;			/* Is object moving (only for guns all animals do) */
  int returnlaser;		/* only for solid lasers... */
  int shooting;			/* if birds can shoot */
  int processed;		/* Time stamped with cycle_count once processed in update_game() */
  int redraw;			/* Set to 1 if this object/board location needs redrawing in show_game_area() */
#ifdef LIGHTNINGENABLED
  int effect;			/* Visual effect on the object */        
#endif
  struct Coords icon[MAX_ICONS];	/* Coords of left-up point of icons drawed on bitmap */
};
struct object board[MAX_W][MAX_H];	/* This is the game area. Each board location holds one and only one of the above objects */

struct
{
  int x;			/* Board x position */
  int y;			/* Board y position */
  int alive;			/* if Robbo is alive */
  int state;			/* Robbo's state (0 or 1 for exchange 2 icons for each direction)  */
  int direction;		/* Robbo's direction 0,2,4,6  + state => icon */
  int screws;			/* The initial number of screws to collect at level start */
  int keys;			/* Keys collected */
  int bullets;			/* Bullets collected */
  int moved;			/* A delay countdown till next move */
  int shooted;			/* A delay countdown till next shot */
  int exitopened;		/* Set to TRUE when all the required screws are collected */
  int blocked;			/* robbo cannot move - possible magnet moving */
  int blocked_direction;	/* where robbo should be moved after blocking */
  int teleporting;		/* Set to TRUE when Robbo is teleporting */
} robbo;

/* What is shown of the board is seen through this viewport */
struct
{
  int x;			/* Board x position */
  int y;			/* Board y position */
#ifdef _SMOOTH_SCRL_
  int xs;
  int ys;
#endif
  int w;
  int h;
  int max_w;
  int max_h;
  int xoffset;
  int yoffset;
  int cycles_to_dest;
  int maximise;
} viewport;

/* Some game mechanics that I have made available for modification via the rcfile */
struct
{
  int sensible_bears;
  int sensible_questionmarks;
  int sensible_solid_lasers;
} game_mechanics;

/* Function prototypes */
void update_game (void);
void init_questionmarks (void);
void open_exit (void);
void init_robbo (void);
void move_robbo (int x, int y);
void shoot_robbo (int x, int y);
void viewport_needs_redrawing (void);
void create_object (int x, int y, int type);
void clear_entire_board (void);
void set_coords (struct Coords *coords, int x, int y);
int coords_out_of_range (struct Coords coords);
void negate_state (int x, int y);
void redraw_field (int x, int y);
void clear_field (int x, int y);
int in_viewport(int x, int y);
int check_wall(int x, int y);
int rearange_walls();
int gcoord(int a);


