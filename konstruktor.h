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

#ifdef _SMOOTH_SCRL_
#define DELAY_KONSTRUKTOR (6*SCRL_MULT1)
#define DELAY_K_BLINK (3*SCRL_MULT1)
#define DELAY_K_BLINK2 (3*SCRL_MULT1)
#else
#define DELAY_KONSTRUKTOR 6
#define DELAY_K_BLINK 3
#define DELAY_K_BLINK2 3
#endif

#define K_MAXICONS  60

#define K_RESETLSELECT -10 /* reset last selected */
#define K_NOTSELECTED -1 /* nothing selected */
#define KGAMEOBJ   1  /* game objects */
#define KOBJ_S     2 /* control objects */
#define KOBJ_V     3  /* vertical objects like scroll bars and so on */
#define KBREAKLINE 4

#define K_RELOAD   127		/* reload level */
#define K_LTYPE    117
#define K_DIRECTION 118
#define K_RMSTATE 119		/* ratable/movable */
#define K_ERASER 120		/* clear level */
#define K_SAVE   121		/* save level */
#define K_INSERT  122		/* Insert new level */
#define K_SIZE    123
#define K_SCROLL 124
#define K_SCROLL2 125
#define K_EXIT		130
#define K_LASER		131  /* we want also shots to be included, so we create a laser object */
#define K_SCREWUP	132
#define K_SCREWDOWN	133


#define K_GAMECOLOR 0x0000f0
#define K_OBJSCOLOR 0x6f006f
#define K_OBJVCOLOR 0x202020

#define K_FOUND -5
#define K_UNDEFINED -10
#define K_WRITTEN -11

/*
 * one object can be type: game object, action icon, this determines the
 * source surface, that would be used
 */
struct k_icons {
    int             x;
    int             y;
    int             type;
    int             game_obj;	/* this is a game object it could be
				 * wall,capsule and so on... */
    int             direction;
    int             scrx;
    int             scry;
    int             blink;
    int		     blink2;
    int             redraw;
};
struct k_icon_init {
	int x;  /* coordinates on the k_icons file */
	int y; 
	int type;
	int game_obj;
	int state; 
};

/*
 * direction 0 -> 1 \/ 2 <- 3 /\ 
 */


/* Thunor: This is now being defined and declared at the same
 * time since we only require one of them */
struct {
	int x;
	int y;
	int w;
	int h;
	int offsetx;
	int offsety;
} k_view;







SDL_Surface    *k_icons;

/*
 * variables 
 */
int K_exit;
int             lastclick;
int             K_direction;
char            infostring[255];
int             inforedraw;
int             kmx,
                kmy;
/*
 * prototypes 
 */

int             konstruktor_init();

int             konstruktor_end();
void            konstruktor_animate(void);
void            konstruktor_scroll(int direction);
void            konstruktor_show_game_area(void);
void            konstruktor_click();
void            konstruktor_draw_icons();
void            k_geticoncoords(int x, int y, SDL_Rect * rect);
void            k_save_map();
int             k_count_obj(int obj);
void            k_redraw_all();

