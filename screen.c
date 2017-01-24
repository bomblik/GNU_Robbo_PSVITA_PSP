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
#define DEBUG_VIDEO
#define DEBUG_MESSAGE_BOX
#define DEBUG_CLEAR_SCREEN
*/

#define BANNER_COLOURS 22 * 3
#define DELAY_BANNER 2

#define REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT if (options[optionid]) register_hotspot(HOTSPOTS_ID, HOTSPOTS_ID + 1 + optionid, destrect)
#define REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC if (options[optionid]) register_hotspot(HOTSPOTS_ID, HOTSPOTS_ID + 1 + optionid + OPTIONS_COUNT, destrect)
#define REGISTER_HOTSPOT_OPTIONS_SCREEN_INC if (options[optionid]) register_hotspot(HOTSPOTS_ID, HOTSPOTS_ID + 1 + optionid + OPTIONS_COUNT * 2, destrect)


/* Variables */
SDL_Color bgcolor, fgcolor;	/* General purpose colours */
SDL_Surface *image;		/* General purpose surface */

/* Used for animating banners */
int bannerstate = 0;
int bannerframe = 0;
int bannercolour[BANNER_COLOURS] = {
  0x00ff00, 0x00f010, 0x00e020, 0x00d030, 0x00c040, 0x00b050, 0x00a060,
    0x009070, 0x008080, 0x007090, 0x0060a0, 0x0050b0, 0x0060a0, 0x007090,
    0x008080, 0x009070, 0x00a060, 0x00b050, 0x00c040, 0x00d030, 0x00e020,
    0x00f010,
  0x00ff00, 0x10f000, 0x20e000, 0x30d000, 0x40c000, 0x50b000, 0x60a000,
    0x709000, 0x808000, 0x907000, 0xa06000, 0xb05000, 0xa06000, 0x907000,
    0x808000, 0x709000, 0x60a000, 0x50b000, 0x40c000, 0x30d000, 0x20e000,
    0x10f000,
  0x00ff00, 0x10f010, 0x20e020, 0x30d030, 0x40c040, 0x50b050, 0x60a060,
    0x709070, 0x808080, 0x907090, 0xa060a0, 0xb050b0, 0xa060a0, 0x907090,
    0x808080, 0x709070, 0x60a060, 0x50b050, 0x40c040, 0x30d030, 0x20e020,
    0x10f010
};

/* Used for animating icons */
int capsulestate = 0;
int capsuleframe = 0;
int capsuleoffsets[2 * 2] = { 86, 18, 103, 18 };

int teleportstate = 0;
int teleportframe = 0;
int teleportoffsets[2 * 2] = { 1, 69, 18, 69 };

int barrierstate = 0;
int barrierframe = 0;
int barrieroffsets[2 * 2] = { 154, 52, 154, 69 };

int butterflystate = 0;
int butterflyframe = 0;
int butterflyoffsets[2 * 2] = { 137, 35, 154, 35 };

int bearstate = 0;
int bearframe = 0;
int bearoffsets[2 * 2] = { 18, 18, 35, 18 };

int blackbearstate = 0;
int blackbearframe = 0;
int blackbearoffsets[2 * 2] = { 103, 35, 120, 35 };

int birdstate = 0;
int birdframe = 0;
int birdoffsets[2 * 2] = { 52, 18, 69, 18 };

int gunstate = 0;
int gunframe = 0;
int gunoffsets[2 * 5 * 5] = {
  1, 52, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 18, 52, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 52, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 18, 52, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 52
};

int laserstate = 0;
int laserframe = 0;
int laseroffsets[2 * 5 * 14] = {
  1, 35, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 35, 1, 35, 0, 0, 0, 0, 0, 0,
  1, 35, 1, 35, 1, 35, 0, 0, 0, 0,
  1, 35, 1, 35, 1, 35, 1, 35, 0, 0,
  1, 35, 1, 35, 1, 35, 1, 35, 1, 35,
  1, 35, 1, 35, 1, 35, 1, 35, 0, 0,
  1, 35, 1, 35, 1, 35, 0, 0, 0, 0,
  1, 35, 1, 35, 0, 0, 0, 0, 0, 0,
  1, 35, 0, 0, 0, 0, 0, 0, 0, 0,
  35, 35, 0, 0, 0, 0, 0, 0, 0, 0,
  18, 35, 0, 0, 0, 0, 0, 0, 0, 0,
  35, 35, 0, 0, 0, 0, 0, 0, 0, 0,
  52, 35, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

int blasterstate = 0;
int blasterframe = 0;
int blasteroffsets[2 * 5 * 9] = {
  35, 69, 0, 0, 0, 0, 0, 0, 0, 0,
  52, 69, 35, 69, 0, 0, 0, 0, 0, 0,
  69, 69, 52, 69, 35, 69, 0, 0, 0, 0,
  52, 69, 69, 69, 52, 69, 35, 69, 0, 0,
  35, 69, 52, 69, 69, 69, 52, 69, 35, 69,
  0, 0, 35, 69, 52, 69, 69, 69, 52, 69,
  0, 0, 0, 0, 35, 69, 52, 69, 69, 69,
  0, 0, 0, 0, 0, 0, 35, 69, 52, 69,
  0, 0, 0, 0, 0, 0, 0, 0, 35, 69
};

int robbostate = 0;
int robboframe = 0;
int robbooffsets[2 * 2] = { 35, 86, 52, 86 };

/* Screen banners */
char *banner[8] = {
  ".OOOOO...OO...OOO..OOO..OOO.....oO#$oO..............oO#.......#$o...............",	/* 80 wide */
  "OOOOOOO..OOO..OOO..OOO..OOO.....O#$..#$.....O#$o....O#$.......$oO.........$oO#..",
  "OOO..OO..OOO..OOO..OOO..OOO.....#$o...oO...O#$.O#...#$oO#$....oO#$oO.....$oO.$o.",
  "OOO......OOOO.OOO..OOO..OOO.....$oO...O#..O#$...$o..$oO..oO...O#$..#$...$oO...O#",
  "OOO.OOO..OOOOOOOO..OOO..OOO.....oO#..O#...#$o...oO..oO#...#$..#$o...oO..oO#...#$",
  "OOO..OO..OOO.OOOO..OOO..OOO.....O#$oO#....$oO...O#..O#$...$o..$oO...O#..O#$...$o",
  "OOOOOOO..OOO..OOO..OOOOOOOO.....#$o..$o...oO#...#$..#$o...oO..oO#...#$..#$o...oO",
  ".OOOOO...OOO..OOO...OOOOOO......$oO...O#...#$oO#$...$oO#$oO...O#$oO#$....oO#$oO."
};

char *helpscreenbanner[10] = {
  "OOO..OOO............oO#..........",	/* 33 wide */
  "OOO..OOO....O#$o....O#$..........",
  "OOO..OOO...O#$.O#...#$o..OOOOOO..",
  "OOO..OOO..O#$...$o..$oO..OOO..OO.",
  "OOOOOOOO..#$oOOOoO..oO#..OOO...OO",
  "OOO..OOO..$oO.......$oO..OOO...OO",
  "OOO..OOO..oO#...#$..oO#..OOO..OO.",
  "OOO..OOO...#$oO#$...O#$..OOOOOO..",
  ".........................OOO.....",
  ".........................OOO....."
};

char *optionsscreenbanner[10] = {
  ".OOOOOO.............oO#....OOO..............................",	/* 60 wide */
  "OOOOOOOO............O#$....OOO....O#$o....ooo.........O#$o..",
  "OOO..OOO..OOOOOO....#$ooo........O#$.O#...#$oO#$.....O#$.O#.",
  "OOO..OOO..OOO..OO...$oO....OOO..O#$...$o..$oO..oO...O#$.....",
  "OOO..OOO..OOO...OO..oO#....OOO..#$o...oO..oO#...#$...O$oooO.",
  "OOO..OOO..OOO...OO..$oO....OOO..$oO...O#..O#$...$o........O#",
  "OOOOOOOO..OOO..OO...oO#....OOO..oO#...#$..#$o...oO..OO#...#$",
  ".OOOOOO...OOOOOO.....#$oo..OOO...#$oO#$...$oO...oO...OO$o#$.",
  "..........OOO...............................................",
  "..........OOO..............................................."
};

/* Function prototypes */
void show_scoreline (void);
Uint32 getpixel (SDL_Surface * surface, int x, int y);
void show_banner (char *banner[], int xpos, int ypos, int width, int height);
void write_lcd_char (char lcdchar, int xpos, int ypos);
void register_hotspot (int pid, int id, SDL_Rect destrect);
int efxcoord(int effect,int tile);

#ifdef _SMOOTH_SCRL_
/**********************************************************************************************
    Show game area, the smooth scrolling enabled version, the original is below in '#else' section
    if _SMOOTH_SCRL_ is undefined, than this procedure and the one belo should be inactive
 **********************************************************************************************/

int
show_game_area (void)
{
  int retval=0;


  int x, y, xoffset = viewport.xoffset, yoffset = viewport.yoffset;
  SDL_Rect srcrect, destrect, emptyrect;
  Uint32 level_colour;
  int vx,vx2,vy,vy2;
  int vpepx, vpepy;
  int sx,sx2,sy,sy2;
  int ex,ey;
  int x1,y1;
int tempdelta;

  if (game_area.redraw >= REDRAW_INITIALISE)
    {
      /* Maximum key/button repeat for all movement keys */
      user_controls[ACTION_UP].delay = 0;
      user_controls[ACTION_UP].interval = 0;
      user_controls[ACTION_UP_RIGHT].delay = 0;
      user_controls[ACTION_UP_RIGHT].interval = 0;
      user_controls[ACTION_RIGHT].delay = 0;
      user_controls[ACTION_RIGHT].interval = 0;
      user_controls[ACTION_DOWN_RIGHT].delay = 0;
      user_controls[ACTION_DOWN_RIGHT].interval = 0;
      user_controls[ACTION_DOWN].delay = 0;
      user_controls[ACTION_DOWN].interval = 0;
      user_controls[ACTION_DOWN_LEFT].delay = 0;
      user_controls[ACTION_DOWN_LEFT].interval = 0;
      user_controls[ACTION_LEFT].delay = 0;
      user_controls[ACTION_LEFT].interval = 0;
      user_controls[ACTION_UP_LEFT].delay = 0;
      user_controls[ACTION_UP_LEFT].interval = 0;
      user_controls[ACTION_PRIMARY_CLICK].delay = 0;
      user_controls[ACTION_PRIMARY_CLICK].interval = 0;

      /* Set a useable repeat for Robbo shoot keys */
      user_controls[ACTION_SHOOT_UP].delay = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
      user_controls[ACTION_SHOOT_UP].interval =
	ACTION_SHOOT_REPEAT_DELAY_PERIOD;
      user_controls[ACTION_SHOOT_DOWN].delay =
	ACTION_SHOOT_REPEAT_DELAY_PERIOD;
      user_controls[ACTION_SHOOT_DOWN].interval =
	ACTION_SHOOT_REPEAT_DELAY_PERIOD;
      user_controls[ACTION_SHOOT_LEFT].delay =
	ACTION_SHOOT_REPEAT_DELAY_PERIOD;
      user_controls[ACTION_SHOOT_LEFT].interval =
	ACTION_SHOOT_REPEAT_DELAY_PERIOD;
      user_controls[ACTION_SHOOT_RIGHT].delay =
	ACTION_SHOOT_REPEAT_DELAY_PERIOD;
      user_controls[ACTION_SHOOT_RIGHT].interval =
	ACTION_SHOOT_REPEAT_DELAY_PERIOD;

      /* Free all existing hotspots including the pob */
      ROB_FreeAllDescendantObjects (ROB_GetObject (HOTSPOTS_ID), TRUE);
      /* Point the event processor to manage pointer controls events */
      gnurobbo_op_env.event_processor = &pointer_controls_event_processor;
      if (ROB_SetOpEnvEventProcessor (gnurobbo_op_env.event_processor))
	printf ("%s: %s\n", __func__, ROB_GetError ());
      /* Pointer controls are created the once only */
      if (ROB_GetObject (POINTER_CONTROLS_ID) == NULL)
	create_pointer_controls ();
    }

#ifdef DEBUG_ROB_HOTSPOTS
  game_area.redraw |= REDRAW_EVERYTHING;
#endif

  /* Manage the pointer controls */
  if (pointer_controls.state == POINTER_CONTROLS_MODE_ON)
    {
      /* Hide the pad for now until Robbo gets drawn */
      hide_pointer_controls_pad ();
      /* What needs to be redrawn? */
      if (pointer_controls.pad_type == POINTER_CONTROLS_PAD_TYPE_VIEWPORT)
	{
	  /* The viewport will always need redrawing as alpha rectangles need to be combined with the tiles */
	  viewport_needs_redrawing ();
	}
      else if (pointer_controls.pad_type == POINTER_CONTROLS_PAD_TYPE_SCREEN)
	{
	  /* Everything will require redrawing as alpha rectangles extend right up to the screen edges */
	  game_area.redraw |= REDRAW_EVERYTHING;
	}
    }

  if (game_area.redraw >= REDRAW_EVERYTHING)
    {
      viewport_needs_redrawing ();
      clear_screen ();
      scoreline.redraw = 0xff;
      /* The Intro, Help and Options screens hide the pointer controls parent and so the parent's visibility
         is maintained here (it must always be visible in-game so that it can be activated with a click) */
      show_pointer_controls ();
      /* Maintain the toolbar button colours to match the level, image surface pointers (user may change
         skin) and visibility of the buttons (end screen hides some buttons) */
      update_pointer_controls_toolbar ();
    }

  /* Calculate the valid expected position of the viewport */
  vpepx = robbo.x - viewport.w / 2;
  if (vpepx < 0)
    {
      vpepx = 0;
    }
  else if (vpepx + viewport.w > level.w)
    {
      vpepx = (level.w - viewport.w);
    }
  vpepy = (robbo.y - viewport.h / 2);
  if (vpepy < 0)
    {
      vpepy = 0;
    }
  else if (vpepy + viewport.h > level.h)
    {
      vpepy = level.h - viewport.h;
    }
  /* If viewport.cycles_to_dest > 0 then move the viewport towards the expected position.
     viewport.cycles_to_dest tells us how many cycles we have to get the viewport over to the expected position
     and this is set to 1 on exit from this function. It is possible to lock the viewport at a set location
     by setting viewport.cycles_to_dest to 0 before calling this function. It's also possible to instantly
     place the viewport at its expected position by setting viewport.cycles_to_dest to 1 */
  vpepx=vpepx*SCRL_MULT2;
  vpepy=vpepy*SCRL_MULT2;
  if ((viewport.x != vpepx || viewport.y != vpepy)
      && viewport.cycles_to_dest > 0)
    {
      if (viewport.x != vpepx)
	{
	  if (viewport.x < vpepx)
	    {
		tempdelta=ceil ((float) (vpepx - viewport.x) / viewport.cycles_to_dest)/SCRL_MULT2;
		if(tempdelta<1) tempdelta=1;
	      viewport.x +=tempdelta;
	    }
	  else
	    {
		tempdelta=ceil ((float) (viewport.x - vpepx) / viewport.cycles_to_dest)/SCRL_MULT2;
		if(tempdelta<1) tempdelta=1;
	      viewport.x -=tempdelta;
	    }
	}
      if (viewport.y != vpepy)
	{
	  if (viewport.y < vpepy)
	    {
		tempdelta=ceil ((float) (vpepy - viewport.y) / viewport.cycles_to_dest)/SCRL_MULT2;
		if(tempdelta<1) tempdelta=1;
	      viewport.y +=tempdelta;
	    }
	  else
	    {
		tempdelta=ceil ((float) (viewport.y - vpepy) / viewport.cycles_to_dest)/SCRL_MULT2;
		if(tempdelta<1) tempdelta=1;
	      viewport.y -=tempdelta;
	    }
	}
      viewport.cycles_to_dest--;
      viewport_needs_redrawing ();
    }
  else
    {
      viewport.cycles_to_dest = 1;
    }

  vx=floor((float)(viewport.x)/SCRL_MULT2);
  vx2=ceil((float)(viewport.x)/SCRL_MULT2);
  vy=floor((float)(viewport.y)/SCRL_MULT2);
  vy2=ceil((float)(viewport.y)/SCRL_MULT2);

  sx=floor((float)((viewport.x-(vx*SCRL_MULT2))*video.field_size)/SCRL_MULT2);
  sy=floor((float)((viewport.y-(vy*SCRL_MULT2))*video.field_size)/SCRL_MULT2);

  /* Iterate through the board locations in the viewport and draw the objects if required */
  for (y = vy; y < vy2 + viewport.h; y++)
    {
	if(vy!=vy2) {
		if(y==vy) { /* first column */
			ey=(video.field_size-sy);
			if(ey>video.field_size)ey=video.field_size;
			sy2=0;
		} else { /* middle columns */
	  	ey=video.field_size;
	  	sy2=sy; // sy2 tiles offset 
		}
		if(y==vy+viewport.h) { /* last column */
			ey=sy;
		}
	} else 
	{
		ey=video.field_size;
		sy2=0;
	}
      for (x = vx; x < vx2 + viewport.w; x++)
	{
	if(vx!=vx2) {
		if(x==vx) { /* first column */
			ex=(video.field_size-sx);
			if(ex>video.field_size) ex=video.field_size;
			sx2=0;
		} else { /* middle columns */
	  	ex=video.field_size;
	  	sx2=sx; // sx2 tiles offset 
		}
		if(x==vx+viewport.w) { /* last column */
			ex=sx;
		}
	} else 
	{
		ex=video.field_size;
		sx2=0;
	}
	



	  destrect =
	    set_rect (xoffset - sx2 + (x - vx) * video.field_size,
		      yoffset -sy2  + (y - vy) * video.field_size,
		      ex, ey);
	  /* If the location is outside the dimensions of the level then draw a BACKGROUND_COLOUR tile instead */
	  if (x < 0 || y < 0 || x >= level.w || y >= level.h)
	    {
	      SDL_FillRect (screen, &destrect,
			    SDL_MapRGB (screen->format,
					skins[selected_skin].
					background_colour >> 16 & 0xff,
					skins[selected_skin].
					background_colour >> 8 & 0xff,
					skins[selected_skin].
					background_colour & 0xff));
	    }
	  else
	    {
	      /* The location is valid but only draw locations that have been marked for redrawing */
		//board[x][y].redraw=1;
    	     if (board[x][y].redraw)
		{
			retval++;
		  board[x][y].redraw = FALSE;
		  if (level.now_is_blinking > 1)
		    {
		      /* If the level is blinking then draw a white tile */
		      level_colour = 0xffffff;
		    }
		  else
		    {
		      if (level.colour_override != UNDEFINED)
			{
			  level_colour = level.colour_override;
			}
		      else if (level.colour != UNDEFINED)
			{
			  level_colour = level.colour;
			}
		      else
			{
			  level_colour = DEFAULT_LEVEL_COLOUR;
			}
		    }
		  SDL_FillRect (screen, &destrect,
				SDL_MapRGB (screen->format,
					    level_colour >> 16 & 0xff,
					    level_colour >> 8 & 0xff,
					    level_colour & 0xff));
		  /* Get icon surface offsets for object's current animation frame */
		  /*now first, let's manage the background file, we cut it into 4 pieces, so we would change backgrounds in different levels */
		  if (bgrnd != NULL)
		    {
		      int bgx = bgrnd->w / 2, bgy = bgrnd->h / 2;
		      switch (level_packs[selected_pack].level_selected % 4)
			{
			case 0:
			  emptyrect =
			    set_rect ((video.field_size * x) % bgx+((x==vx)?sx:0),
				      (video.field_size * y) % bgy+((y==vy)?sy:0),
				      ex, ey);
			  break;
			case 1:
			  emptyrect =
			    set_rect (((video.field_size * x) % bgx) + bgx+((x==vx)?sx:0),
				      (video.field_size * y) % bgy+((y==vy)?sy:0),
				      ex, ey);
			  break;
			case 2:
			  emptyrect =
			    set_rect ((video.field_size * x) % bgx+((x==vx)?sx:0),
				      ((video.field_size * y) % bgy) + bgy+((y==vy)?sy:0),
				      ex, ey);
			  break;
			case 3:
			  emptyrect =
			    set_rect (((video.field_size * x) % bgx) + bgx+((x==vx)?sx:0),
				      ((video.field_size * y) % bgy) + bgy+((y==vy)?sy:0),
				      ex, ey);
			  break;
			}
		      SDL_BlitSurface (bgrnd, &emptyrect, screen, &destrect);
		    };
/* light effects */
#ifdef LIGHTNINGENABLED
		    for(x1=(x>0)?x-1:0;x1<=x+1;x1++)	
			for(y1=(y>0)?y-1:0;y1<=y+1;y1++) {
			    switch (board[x1][y1].type) {
			    case (TELEPORTING):
				srcrect=set_rect(efxcoord(board[x1][y1].state,x-x1)+((x==vx)?sx:0),efxcoord(4,y-y1)+((y==vy)?sy:0),ex,ey);
			if (SDL_BlitSurface (efx, &srcrect, screen, &destrect)
				    < 0)
				    {
				    fprintf (stdout, "BlitSurface error: %s\n",
				    SDL_GetError ());
				    }

				break;
			    case (LITTLE_BOOM): 
				srcrect=set_rect(efxcoord(board[x1][y1].state,x-x1)+((x==vx)?sx:0),efxcoord(0,y-y1)+((y==vy)?sy:0),ex,ey);
			if (SDL_BlitSurface (efx, &srcrect, screen, &destrect)
				    < 0)
				    {
				    fprintf (stdout, "BlitSurface error: %s\n",
				    SDL_GetError ());
				    }

				break;
			    case (BIG_BOOM):
				srcrect=set_rect(efxcoord(board[x1][y1].state,x-x1)+((x==vx)?sx:0),efxcoord(1,y-y1)+((y==vy)?sy:0),ex,ey);
				if (SDL_BlitSurface (efx, &srcrect, screen, &destrect)
				    < 0)
				    {
				    fprintf (stdout, "BlitSurface error: %s\n",
				    SDL_GetError ());
				    }

				break;
                               
			    case (TELEPORT):
				srcrect=set_rect(efxcoord(board[x1][y1].state,x-x1)+((x==vx)?sx:0),efxcoord(2,y-y1)+((y==vy)?sy:0),ex,ey);
				if (SDL_BlitSurface (efx, &srcrect, screen, &destrect)
				    < 0)
				    {
				    fprintf (stdout, "BlitSurface error: %s\n",
				    SDL_GetError ());
				    }

				break;
			    case (CAPSULE):
				if(robbo.exitopened==TRUE) 
				    {
				srcrect=set_rect(efxcoord(board[x1][y1].state,x-x1)+((x==vx)?sx:0),efxcoord(3,y-y1)+((y==vy)?sy:0),ex,ey);
				    if (SDL_BlitSurface (efx, &srcrect, screen, &destrect)
					< 0)
					{
					fprintf (stdout, "BlitSurface error: %s\n",
					SDL_GetError ());
					}
                           	    }
				break;

                              }
		}
#endif
//		  if (board[x][y].effect>=0) board[x][y].effect--;
		  /* now we draw empty field */
		  if (board[x][y].type != EMPTY_FIELD)
		    {
		      srcrect =
			set_rect (board[x][y].icon[board[x][y].state].x+((x==vx)?sx:0),
				  board[x][y].icon[board[x][y].state].y+((y==vy)?sy:0),
				  ex, ey);
		      if (SDL_BlitSurface (icons, &srcrect, screen, &destrect)
			  < 0)
			{
			  fprintf (stdout, "BlitSurface error: %s\n",
				   SDL_GetError ());
			}
		    }
		  /* Are we drawing Robbo? */
		  if (robbo.x == x && robbo.y == y && robbo.alive &&	/* Is this his location and is he alive? */
		      (!robbo.teleporting
		       || (robbo.teleporting
			   && robbo.moved <= DELAY_TELEPORTING)))
		    {		/* Is he either not teleporting or just about to materialise from teleporting? */
		      srcrect =
			set_rect (robbo_images_srcrect
				  [robbo.direction + robbo.state].x+((x==vx)?sx:0),
				  robbo_images_srcrect[robbo.direction +
						       robbo.state].y+((y==vy)?sy:0),
				  ex,ey);
		      if (SDL_BlitSurface (icons, &srcrect, screen, &destrect)
			  < 0)
			{
			  fprintf (stdout, "BlitSurface error: %s\n",
				   SDL_GetError ());
			}
		      /* Reposition/resize the pointer controls pad over Robbo */
		      if (pointer_controls.state == POINTER_CONTROLS_MODE_ON)
			{
			  show_pointer_controls_pad ();
			  update_pointer_controls_pad (destrect.x,
						       destrect.y);
			}
		    }
		}
	    }
	}
    }

  /* If the level is blinking then mark the viewport for redrawing */
  if (level.now_is_blinking > 1)
    {
      level.now_is_blinking--;
      if (level.now_is_blinking == 1)
	{
	  viewport_needs_redrawing ();
	}
    }

  /* Redraw the parts of the scoreline that have been requested */
  if (scoreline.redraw)
    show_scoreline ();

  game_area.redraw = 0;
 return retval;

}

/***************************************************************************
 * Show Game Area Fade                                                     *
 ***************************************************************************/
/* Simulates the screen fading in. I think fades can become annoying, so I
   don't fade when manually changing levels and packs.
   
   On entry: subfunction = FADE_SUB_INITIALISE to initialise with
				type = 1 to 17 for a specific fade type
			 subfunction = FADE_SUB_SHOW to show the fade
				(type is ignored)
			 subfunction = FADE_SUB_KILL with
				type = 0 to kill an existing fade */

int 
show_game_area_fade (int subfunction, int type)
{
  int xoffset = viewport.xoffset, yoffset = viewport.yoffset, x, y;
  static int fadesize = 0, fadetype;
  SDL_Rect destrect;

  if (subfunction == FADE_SUB_INITIALISE)
    {
      /* Initialise a fade */
      fadetype = type;
      if (fadetype == 0)
	{
	  fadesize = 0;
	}
      else if (fadetype == 16)
	{
	  fadesize = viewport.w;
	}
      else
	{
	  fadesize = video.field_size;
	}
    }
  else if (subfunction == FADE_SUB_SHOW)
    {
      if (fadesize > 0)
	{
	  for (y = 0; y < viewport.h; y++)
	    {
	      for (x = 0; x < viewport.w; x++)
		{
		  if (fadetype == 0)
		    {
		      /* No fade */
		    }
		  else if (fadetype == 1)
		    {
		      /* Fade middle dot */
		      destrect =
			set_rect (x * video.field_size + xoffset +
				  (video.field_size - fadesize) / 2,
				  y * video.field_size + yoffset +
				  (video.field_size - fadesize) / 2, fadesize,
				  fadesize);
		    }
		  else if (fadetype == 2)
		    {
		      /* Fade bottom right */
		      destrect =
			set_rect (x * video.field_size + xoffset +
				  video.field_size - fadesize,
				  y * video.field_size + yoffset +
				  video.field_size - fadesize, fadesize,
				  fadesize);
		    }
		  else if (fadetype == 3)
		    {
		      /* Fade top left and bottom right */
		      if (x % 2 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		      else if (x % 2 == 1)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		    }
		  else if (fadetype == 4)
		    {
		      /* Fade bottom left and top right */
		      if (x % 2 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		      else if (x % 2 == 1)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		    }
		  else if (fadetype == 5)
		    {
		      /* Fade bottom */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset +
				  video.field_size - fadesize,
				  video.field_size, fadesize);
		    }
		  else if (fadetype == 6)
		    {
		      /* Fade top left */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset, fadesize,
				  fadesize);
		    }
		  else if (fadetype == 7)
		    {
		      /* Fade top right */
		      destrect =
			set_rect (x * video.field_size + xoffset +
				  video.field_size - fadesize,
				  y * video.field_size + yoffset, fadesize,
				  fadesize);
		    }
		  else if (fadetype == 8)
		    {
		      /* Fade bottom left */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset +
				  video.field_size - fadesize, fadesize,
				  fadesize);
		    }
		  else if (fadetype == 9)
		    {
		      /* Intermittent fade top left and bottom right */
		      if (fadesize % 4 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		      else if (fadesize % 4 == 2)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		    }
		  else if (fadetype == 10)
		    {
		      /* Fade left */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset, fadesize,
				  video.field_size);
		    }
		  else if (fadetype == 11)
		    {
		      /* Fade middle tall */
		      destrect =
			set_rect (x * video.field_size + xoffset +
				  (video.field_size - fadesize) / 2,
				  y * video.field_size + yoffset, fadesize,
				  video.field_size);
		    }
		  else if (fadetype == 12)
		    {
		      /* Fade middle wide */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset +
				  (video.field_size - fadesize) / 2,
				  video.field_size, fadesize);
		    }
		  else if (fadetype == 13)
		    {
		      /* Fade top */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset,
				  video.field_size, fadesize);
		    }
		  else if (fadetype == 14)
		    {
		      /* Fade right */
		      destrect =
			set_rect (x * video.field_size + xoffset +
				  video.field_size - fadesize,
				  y * video.field_size + yoffset, fadesize,
				  video.field_size);
		    }
		  else if (fadetype == 15)
		    {
		      /* Intermittent fade bottom left and top right */
		      if (fadesize % 4 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		      else if (fadesize % 4 == 2)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		    }
		  else if (fadetype == 16)
		    {
		      /* Board fades from the right */
		      if (x < fadesize)
			destrect =
			  set_rect (x * video.field_size + xoffset,
				    y * video.field_size + yoffset,
				    video.field_size, video.field_size);
		    }
		  else if (fadetype == 17)
		    {
		      /* Fade top left, top right, bottom left and bottom right */
		      if (x % 2 == 0 && y % 2 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		      else if (x % 2 == 1 && y % 2 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		      else if (x % 2 == 0 && y % 2 == 1)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		      else if (x % 2 == 1 && y % 2 == 1)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		    }
		  SDL_FillRect (screen, &destrect,
				SDL_MapRGB (screen->format,
					    skins[selected_skin].
					    fade_colour >> 16 & 0xff,
					    skins[selected_skin].
					    fade_colour >> 8 & 0xff,
					    skins[selected_skin].
					    fade_colour & 0xff));
		}
	    }
	  if (fadetype == 16)
	    {
		
	     fadesize--;
	    }
	  else
	    {
	     fadesize -= video.field_size / 16;
	    }
	  viewport_needs_redrawing ();
	   return 1;	
	}
    }
	return 0;
}

#else

/***************************************************************************
 * Show Game Area                                                          *
 ***************************************************************************/
/* Rewritten to use a viewport that can reposition itself at Robbo's
   destination in as many cycles as requested.

   game_area.redraw now specifies how much is redrawn */

int
show_game_area (void)
{
	int retval=0;
	int x, y, xoffset = viewport.xoffset, yoffset = viewport.yoffset;
	SDL_Rect srcrect, destrect, emptyrect;
	Uint32 level_colour;
	int vpepx, vpepy;

	if (game_area.redraw >= REDRAW_INITIALISE)
	{
		/* Maximum key/button repeat for all movement keys */
		user_controls[ACTION_UP].delay = 0;
		user_controls[ACTION_UP].interval = 0;
		user_controls[ACTION_UP_RIGHT].delay = 0;
		user_controls[ACTION_UP_RIGHT].interval = 0;
		user_controls[ACTION_RIGHT].delay = 0;
		user_controls[ACTION_RIGHT].interval = 0;
		user_controls[ACTION_DOWN_RIGHT].delay = 0;
		user_controls[ACTION_DOWN_RIGHT].interval = 0;
		user_controls[ACTION_DOWN].delay = 0;
		user_controls[ACTION_DOWN].interval = 0;
		user_controls[ACTION_DOWN_LEFT].delay = 0;
		user_controls[ACTION_DOWN_LEFT].interval = 0;
		user_controls[ACTION_LEFT].delay = 0;
		user_controls[ACTION_LEFT].interval = 0;
		user_controls[ACTION_UP_LEFT].delay = 0;
		user_controls[ACTION_UP_LEFT].interval = 0;
		user_controls[ACTION_PRIMARY_CLICK].delay = 0;
		user_controls[ACTION_PRIMARY_CLICK].interval = 0;

		/* Set a useable repeat for Robbo shoot keys */
		user_controls[ACTION_SHOOT_UP].delay = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
		user_controls[ACTION_SHOOT_UP].interval = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
		user_controls[ACTION_SHOOT_DOWN].delay = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
		user_controls[ACTION_SHOOT_DOWN].interval = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
		user_controls[ACTION_SHOOT_LEFT].delay = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
		user_controls[ACTION_SHOOT_LEFT].interval = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
		user_controls[ACTION_SHOOT_RIGHT].delay = ACTION_SHOOT_REPEAT_DELAY_PERIOD;
		user_controls[ACTION_SHOOT_RIGHT].interval = ACTION_SHOOT_REPEAT_DELAY_PERIOD;

		/* Free all existing hotspots including the pob */
		ROB_FreeAllDescendantObjects (ROB_GetObject (HOTSPOTS_ID), TRUE);
		/* Point the event processor to manage pointer controls events */
		gnurobbo_op_env.event_processor = &pointer_controls_event_processor;
		if (ROB_SetOpEnvEventProcessor (gnurobbo_op_env.event_processor))
			printf ("%s: %s\n", __func__, ROB_GetError ());
		/* Pointer controls are created the once only */
		if (ROB_GetObject (POINTER_CONTROLS_ID) == NULL) create_pointer_controls ();
	}

	#ifdef DEBUG_ROB_HOTSPOTS
		game_area.redraw |= REDRAW_EVERYTHING;
	#endif

	/* Manage the pointer controls */
	if (pointer_controls.state == POINTER_CONTROLS_MODE_ON)
	{
		/* Hide the pad for now until Robbo gets drawn */
		hide_pointer_controls_pad ();
		/* What needs to be redrawn? */
		if (pointer_controls.pad_type == POINTER_CONTROLS_PAD_TYPE_VIEWPORT)
		{
			/* The viewport will always need redrawing as alpha
			 * rectangles need to be combined with the tiles */
			viewport_needs_redrawing ();
		}
		else if (pointer_controls.pad_type == POINTER_CONTROLS_PAD_TYPE_SCREEN)
		{
			/* Everything will require redrawing as alpha 
			 * rectangles extend right up to the screen edges */
			game_area.redraw |= REDRAW_EVERYTHING;
		}
	}

	if (game_area.redraw >= REDRAW_EVERYTHING)
	{
		viewport_needs_redrawing ();
		clear_screen ();
		scoreline.redraw = 0xff;
		/* The Intro, Help and Options screens hide the pointer controls
		 * parent and so the parent's visibility is maintained here
		 * (it must always be visible in-game so that it can be activated
		 * with a click) */
		show_pointer_controls ();
		/* Maintain the toolbar button colours to match the level, image
		 * surface pointers (user may change skin) and visibility of the
		 * buttons (end screen hides some buttons) */
		update_pointer_controls_toolbar ();
	}

	/* Calculate the valid expected position of the viewport */
	vpepx = robbo.x - viewport.w / 2;
	if (vpepx < 0)
	{
		vpepx = 0;
	}
	else if (vpepx + viewport.w > level.w)
	{
		vpepx = level.w - viewport.w;
	}
	vpepy = robbo.y - viewport.h / 2;
	if (vpepy < 0)
	{
		vpepy = 0;
	}
	else if (vpepy + viewport.h > level.h)
	{
		vpepy = level.h - viewport.h;
	}
	/* If viewport.cycles_to_dest > 0 then move the viewport towards the
	 * expected position. viewport.cycles_to_dest tells us how many cycles
	 * we have to get the viewport over to the expected position and this
	 * is set to 1 on exit from this function. It is possible to lock the
	 * viewport at a set location by setting viewport.cycles_to_dest to 0
	 * before calling this function. It's also possible to instantly place
	 * the viewport at its expected position by setting
	 * viewport.cycles_to_dest to 1 */
	if ((viewport.x != vpepx || viewport.y != vpepy) && viewport.cycles_to_dest > 0)
	{
		if (viewport.x != vpepx)
		{
			if (viewport.x < vpepx)
			{
				viewport.x += ceil ((float) (vpepx - viewport.x) /
					viewport.cycles_to_dest);
			}
			else
			{
				viewport.x -= ceil ((float) (viewport.x - vpepx) /
					viewport.cycles_to_dest);
			}
		}
		if (viewport.y != vpepy)
		{
			if (viewport.y < vpepy)
			{
				viewport.y += ceil ((float) (vpepy - viewport.y) /
					viewport.cycles_to_dest);
			}
			else
			{
				viewport.y -= ceil ((float) (viewport.y - vpepy) /
					viewport.cycles_to_dest);
			}
		}
		viewport.cycles_to_dest--;
		viewport_needs_redrawing ();
	}
	else
	{
		viewport.cycles_to_dest = 1;
	}

	/* Iterate through the board locations in the viewport
	 * and draw the objects if required */
	for (y = viewport.y; y < viewport.y + viewport.h; y++)
	{
		for (x = viewport.x; x < viewport.x + viewport.w; x++)
		{
			destrect = set_rect (xoffset + (x - viewport.x) * video.field_size,
				yoffset + (y - viewport.y) * video.field_size,
				video.field_size, video.field_size);
			/* If the location is outside the dimensions of the level
			 * then draw a BACKGROUND_COLOUR tile instead */
			if (x < 0 || y < 0 || x >= level.w || y >= level.h)
			{
				SDL_FillRect (screen, &destrect, SDL_MapRGB (screen->format,
				skins[selected_skin].background_colour >> 16 & 0xff,
				skins[selected_skin].background_colour >> 8 & 0xff,
				skins[selected_skin].background_colour & 0xff));
			}
			else
			{
				/* The location is valid but only draw locations
				 * that have been marked for redrawing */
				if (board[x][y].redraw)
				{
					board[x][y].redraw = FALSE;
					if (level.now_is_blinking > 1)
					{
						/* If the level is blinking then draw a white tile */
						level_colour = 0xffffff;
					}
					else
					{
						if (level.colour_override != UNDEFINED)
						{
							level_colour = level.colour_override;
						}
						else if (level.colour != UNDEFINED)
						{
							level_colour = level.colour;
						}
						else
						{
							level_colour = DEFAULT_LEVEL_COLOUR;
						}
					}
					SDL_FillRect (screen, &destrect, SDL_MapRGB (screen->format,
						level_colour >> 16 & 0xff, level_colour >> 8 & 0xff,
						level_colour & 0xff));
					/* Get icon surface offsets for object's current
					 * animation frame */
					/* Now first, let's manage the background file, we
					 * cut it into 4 pieces, so we would change
					 * backgrounds in different levels */
					if (bgrnd != NULL)
					{
						int bgx = bgrnd->w / 2, bgy = bgrnd->h / 2;
						switch (level_packs[selected_pack].level_selected % 4)
						{
							case 0:
							emptyrect = set_rect ((video.field_size * x) % bgx,
								(video.field_size * y) % bgy,
								video.field_size, video.field_size);
								break;
							case 1:
							emptyrect = set_rect (((video.field_size * x) % bgx) + bgx,
								(video.field_size * y) % bgy,
								video.field_size, video.field_size);
								break;
							case 2:
							emptyrect = set_rect ((video.field_size * x) % bgx,
								((video.field_size * y) % bgy) + bgy,
								video.field_size, video.field_size);
								break;
							case 3:
							emptyrect = set_rect (((video.field_size * x) % bgx) + bgx,
								((video.field_size * y) % bgy) + bgy,
								video.field_size, video.field_size);
								break;
						}
						SDL_BlitSurface (bgrnd, &emptyrect, screen, &destrect);
					}
					/* now we draw empty field */
					if (board[x][y].type != EMPTY_FIELD)
					{
						srcrect = set_rect (board[x][y].icon[board[x][y].state].x,
							board[x][y].icon[board[x][y].state].y,
							video.field_size, video.field_size);
						if (SDL_BlitSurface (icons, &srcrect, screen, &destrect) < 0)
						{
							fprintf (stdout, "BlitSurface error: %s\n", SDL_GetError ());
						}
					}
					/* Are we drawing Robbo? */
					if (robbo.x == x && robbo.y == y && robbo.alive &&	/* Is this his location and is he alive? */
						(!robbo.teleporting || (robbo.teleporting && 
						robbo.moved <= DELAY_TELEPORTING)))
					{		/* Is he either not teleporting or just about to materialise from teleporting? */
						srcrect = set_rect (robbo_images_srcrect
							[robbo.direction + robbo.state].x, robbo_images_srcrect[robbo.direction +
							robbo.state].y, robbo_images_srcrect[robbo.direction +
							robbo.state].w, robbo_images_srcrect[robbo.direction +
							robbo.state].h);
						if (SDL_BlitSurface (icons, &srcrect, screen, &destrect) < 0)
						{
							fprintf (stdout, "BlitSurface error: %s\n", SDL_GetError ());
						}
						/* Reposition/resize the pointer controls pad over Robbo */
						if (pointer_controls.state == POINTER_CONTROLS_MODE_ON)
						{
							show_pointer_controls_pad ();
							update_pointer_controls_pad (destrect.x, destrect.y);
						}
					}
				}
			}
		}
	}

	/* If the level is blinking then mark the viewport for redrawing */
	if (level.now_is_blinking > 1)
	{
		level.now_is_blinking--;
		if (level.now_is_blinking == 1)
		{
			viewport_needs_redrawing ();
		}
	}

	/* Redraw the parts of the scoreline that have been requested */
	if (scoreline.redraw) show_scoreline ();

	game_area.redraw = 0;

	return retval;

}

/***************************************************************************
 * Show Game Area Fade                                                     *
 ***************************************************************************/
/* Simulates the screen fading in. I think fades can become annoying, so I
   don't fade when manually changing levels and packs.
   
   On entry: subfunction = FADE_SUB_INITIALISE to initialise with
				type = 1 to 17 for a specific fade type
			 subfunction = FADE_SUB_SHOW to show the fade
				(type is ignored)
			 subfunction = FADE_SUB_KILL with
				type = 0 to kill an existing fade */

int 
show_game_area_fade (int subfunction, int type)
{
  int xoffset = viewport.xoffset, yoffset = viewport.yoffset, x, y;
  static int fadesize = 0, fadetype;
  SDL_Rect destrect;

  if (subfunction == FADE_SUB_INITIALISE)
    {
      /* Initialise a fade */
      fadetype = type;
      if (fadetype == 0)
	{
	  fadesize = 0;
	}
      else if (fadetype == 16)
	{
	  fadesize = viewport.w;
	}
      else
	{
	  fadesize = video.field_size;
	}
    }
  else if (subfunction == FADE_SUB_SHOW)
    {
      if (fadesize > 0)
	{
	  for (y = 0; y < viewport.h; y++)
	    {
	      for (x = 0; x < viewport.w; x++)
		{
		  if (fadetype == 0)
		    {
		      /* No fade */
		    }
		  else if (fadetype == 1)
		    {
		      /* Fade middle dot */
		      destrect =
			set_rect (x * video.field_size + xoffset +
				  (video.field_size - fadesize) / 2,
				  y * video.field_size + yoffset +
				  (video.field_size - fadesize) / 2, fadesize,
				  fadesize);
		    }
		  else if (fadetype == 2)
		    {
		      /* Fade bottom right */
		      destrect =
			set_rect (x * video.field_size + xoffset +
				  video.field_size - fadesize,
				  y * video.field_size + yoffset +
				  video.field_size - fadesize, fadesize,
				  fadesize);
		    }
		  else if (fadetype == 3)
		    {
		      /* Fade top left and bottom right */
		      if (x % 2 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		      else if (x % 2 == 1)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		    }
		  else if (fadetype == 4)
		    {
		      /* Fade bottom left and top right */
		      if (x % 2 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		      else if (x % 2 == 1)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		    }
		  else if (fadetype == 5)
		    {
		      /* Fade bottom */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset +
				  video.field_size - fadesize,
				  video.field_size, fadesize);
		    }
		  else if (fadetype == 6)
		    {
		      /* Fade top left */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset, fadesize,
				  fadesize);
		    }
		  else if (fadetype == 7)
		    {
		      /* Fade top right */
		      destrect =
			set_rect (x * video.field_size + xoffset +
				  video.field_size - fadesize,
				  y * video.field_size + yoffset, fadesize,
				  fadesize);
		    }
		  else if (fadetype == 8)
		    {
		      /* Fade bottom left */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset +
				  video.field_size - fadesize, fadesize,
				  fadesize);
		    }
		  else if (fadetype == 9)
		    {
		      /* Intermittent fade top left and bottom right */
		      if (fadesize % 4 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		      else if (fadesize % 4 == 2)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		    }
		  else if (fadetype == 10)
		    {
		      /* Fade left */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset, fadesize,
				  video.field_size);
		    }
		  else if (fadetype == 11)
		    {
		      /* Fade middle tall */
		      destrect =
			set_rect (x * video.field_size + xoffset +
				  (video.field_size - fadesize) / 2,
				  y * video.field_size + yoffset, fadesize,
				  video.field_size);
		    }
		  else if (fadetype == 12)
		    {
		      /* Fade middle wide */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset +
				  (video.field_size - fadesize) / 2,
				  video.field_size, fadesize);
		    }
		  else if (fadetype == 13)
		    {
		      /* Fade top */
		      destrect =
			set_rect (x * video.field_size + xoffset,
				  y * video.field_size + yoffset,
				  video.field_size, fadesize);
		    }
		  else if (fadetype == 14)
		    {
		      /* Fade right */
		      destrect =
			set_rect (x * video.field_size + xoffset +
				  video.field_size - fadesize,
				  y * video.field_size + yoffset, fadesize,
				  video.field_size);
		    }
		  else if (fadetype == 15)
		    {
		      /* Intermittent fade bottom left and top right */
		      if (fadesize % 4 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		      else if (fadesize % 4 == 2)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		    }
		  else if (fadetype == 16)
		    {
		      /* Board fades from the right */
		      if (x < fadesize)
			destrect =
			  set_rect (x * video.field_size + xoffset,
				    y * video.field_size + yoffset,
				    video.field_size, video.field_size);
		    }
		  else if (fadetype == 17)
		    {
		      /* Fade top left, top right, bottom left and bottom right */
		      if (x % 2 == 0 && y % 2 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		      else if (x % 2 == 1 && y % 2 == 0)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset,
				      fadesize, fadesize);
			}
		      else if (x % 2 == 0 && y % 2 == 1)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		      else if (x % 2 == 1 && y % 2 == 1)
			{
			  destrect =
			    set_rect (x * video.field_size + xoffset +
				      video.field_size - fadesize,
				      y * video.field_size + yoffset +
				      video.field_size - fadesize, fadesize,
				      fadesize);
			}
		    }
		  SDL_FillRect (screen, &destrect,
				SDL_MapRGB (screen->format,
					    skins[selected_skin].
					    fade_colour >> 16 & 0xff,
					    skins[selected_skin].
					    fade_colour >> 8 & 0xff,
					    skins[selected_skin].
					    fade_colour & 0xff));
		}
	    }
	  if (fadetype == 16)
	    {
		
	     fadesize--;
	    }
	  else
	    {
	     fadesize -= video.field_size / 16;
	    }
	  viewport_needs_redrawing ();
	   return 1;	
	}
    }
	return 0;
}


#endif



/***************************************************************************
 * Show Scoreline                                                          *
 ***************************************************************************/
/* Now each item can be individually redrawn by the setting of bits in
   the global variable scoreline.redraw prior to calling this function */

void
show_scoreline (void)
{
  int xoffset, count, number;
  int yoffset = scoreline.yoffset;
  char tempstring[256];
  SDL_Rect destrect;

  if (scoreline.redraw & SCORELINE_ICONS)
    {
      xoffset = scoreline.xoffset;

      /* First erase the area occupied by the icons */
      destrect =
	set_rect (xoffset, yoffset, video.field_size * 10.5,
		  video.field_size);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				skins[selected_skin].
				background_colour >> 16 & 0xff,
				skins[selected_skin].
				background_colour >> 8 & 0xff,
				skins[selected_skin].
				background_colour & 0xff));

      /* Draw screw icon */
      destrect =
	set_rect (xoffset, yoffset, video.field_size, video.field_size);
      if (SDL_BlitSurface (icons, &score_screw_srcrect, screen, &destrect) <
	  0)
	{
	  fprintf (stdout, "BlitSurface error: %s\n", SDL_GetError ());
	}

      xoffset = xoffset + video.field_size * 2 + video.field_size / 4;

      /* Draw key icon */
      destrect =
	set_rect (xoffset, yoffset, video.field_size, video.field_size);
      if (SDL_BlitSurface (icons, &score_key_srcrect, screen, &destrect) < 0)
	{
	  fprintf (stdout, "BlitSurface error: %s\n", SDL_GetError ());
	}

      xoffset = xoffset + video.field_size * 2 + video.field_size / 2;

      /* Draw bullet icon */
      destrect =
	set_rect (xoffset, yoffset, video.field_size, video.field_size);
      if (SDL_BlitSurface (icons, &score_bullet_srcrect, screen, &destrect) <
	  0)
	{
	  fprintf (stdout, "BlitSurface error: %s\n", SDL_GetError ());
	}

      xoffset = xoffset + video.field_size * 3;

      /* Draw level icon */
      destrect =
	set_rect (xoffset, yoffset, video.field_size, video.field_size);
      if (SDL_BlitSurface (icons, &score_level_srcrect, screen, &destrect) <
	  0)
	{
	  fprintf (stdout, "BlitSurface error: %s\n", SDL_GetError ());
	}
    }

  if (scoreline.redraw & SCORELINE_SCREWS)
    {
      xoffset = scoreline.xoffset + video.field_size;

      /* Does the area occupied by the digits require erasing? */
      if (!(scoreline.redraw & SCORELINE_ICONS))
	{
	  destrect =
	    set_rect (xoffset, yoffset, video.field_size, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format,
				    skins[selected_skin].
				    background_colour >> 16 & 0xff,
				    skins[selected_skin].
				    background_colour >> 8 & 0xff,
				    skins[selected_skin].
				    background_colour & 0xff));
	}

      /* Draw screw count */
      number = 10;
      for (count = 0; count < 2; count++)
	{
	  sprintf (tempstring, "%i", (robbo.screws / number) % 10);
	  write_lcd_char (tempstring[0], xoffset, yoffset);
	  xoffset = xoffset + video.field_size / 2;
	  number = number / 10;
	}
    }

  if (scoreline.redraw & SCORELINE_KEYS)
    {
      xoffset =
	scoreline.xoffset + video.field_size * 3 + video.field_size / 2;

      /* Does the area occupied by the digits require erasing? */
      if (!(scoreline.redraw & SCORELINE_ICONS))
	{
	  destrect =
	    set_rect (xoffset, yoffset, video.field_size, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format,
				    skins[selected_skin].
				    background_colour >> 16 & 0xff,
				    skins[selected_skin].
				    background_colour >> 8 & 0xff,
				    skins[selected_skin].
				    background_colour & 0xff));
	}

      /* Draw key count */
      number = 10;
      for (count = 0; count < 2; count++)
	{
	  sprintf (tempstring, "%i", (robbo.keys / number) % 10);
	  write_lcd_char (tempstring[0], xoffset, yoffset);
	  xoffset = xoffset + video.field_size / 2;
	  number = number / 10;
	}
    }

  if (scoreline.redraw & SCORELINE_BULLETS)
    {
      xoffset = scoreline.xoffset + video.field_size * 6;

      /* Does the area occupied by the digits require erasing? */
      if (!(scoreline.redraw & SCORELINE_ICONS))
	{
	  destrect =
	    set_rect (xoffset, yoffset,
		      video.field_size + video.field_size / 2,
		      video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format,
				    skins[selected_skin].
				    background_colour >> 16 & 0xff,
				    skins[selected_skin].
				    background_colour >> 8 & 0xff,
				    skins[selected_skin].
				    background_colour & 0xff));
	}

      /* Draw bullet count */
      number = 100;
      for (count = 0; count < 3; count++)
	{
	  sprintf (tempstring, "%i", (robbo.bullets / number) % 10);
	  write_lcd_char (tempstring[0], xoffset, yoffset);
	  xoffset = xoffset + video.field_size / 2;
	  number = number / 10;
	}
    }

  if (scoreline.redraw & SCORELINE_LEVEL)
    {
      xoffset = scoreline.xoffset + video.field_size * 9;

      /* Does the area occupied by the digits require erasing? */
      if (!(scoreline.redraw & SCORELINE_ICONS))
	{
	  destrect =
	    set_rect (xoffset, yoffset,
		      video.field_size + video.field_size / 2,
		      video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format,
				    skins[selected_skin].
				    background_colour >> 16 & 0xff,
				    skins[selected_skin].
				    background_colour >> 8 & 0xff,
				    skins[selected_skin].
				    background_colour & 0xff));
	}

      /* Draw level count */
      number = 100;
      for (count = 0; count < 3; count++)
	{
	  sprintf (tempstring, "%i",
		   (level_packs[selected_pack].level_selected / number) % 10);
	  write_lcd_char (tempstring[0], xoffset, yoffset);
	  xoffset = xoffset + video.field_size / 2;
	  number = number / 10;
	}
    }

  if (scoreline.redraw & SCORELINE_PACK)
    {
      xoffset =
	scoreline.xoffset + video.field_size * 10 + video.field_size / 2;

      /* First erase the area occupied by the digits */
      destrect =
	set_rect (xoffset, yoffset,
		  video.field_size * 5 + video.field_size / 2,
		  video.field_size);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				skins[selected_skin].
				background_colour >> 16 & 0xff,
				skins[selected_skin].
				background_colour >> 8 & 0xff,
				skins[selected_skin].
				background_colour & 0xff));

      /* Draw "-" */
      tempstring[0] = '-';
      write_lcd_char (tempstring[0], xoffset, yoffset);

      /* Draw pack name */
      for (count = 0; count < strlen (level_packs[selected_pack].name);
	   count++)
	{
	  write_lcd_char (level_packs[selected_pack].name[count],
			  xoffset + (count + 1) * video.field_size / 2,
			  yoffset);
	}
    }

  if (scoreline.redraw & SCORELINE_AUTHOR)
    {
      fgcolor.r = skins[selected_skin].author_text_colour >> 16 & 0xff;
      fgcolor.g = skins[selected_skin].author_text_colour >> 8 & 0xff;
      fgcolor.b = skins[selected_skin].author_text_colour & 0xff;
      bgcolor.r = skins[selected_skin].background_colour >> 16 & 0xff;
      bgcolor.g = skins[selected_skin].background_colour >> 8 & 0xff;
      bgcolor.b = skins[selected_skin].background_colour & 0xff;

      /* Draw level author */
      if (level.author[0] == '_')
	{
	  sprintf (tempstring, "%s", &level.author[1]);
	}
      else if (strlen (level.author) > 0)
	{
	  sprintf (tempstring, "%s: %s", txt_Level_Author, level.author);
	}
      else
	{
	  strcpy (tempstring, "");
	}
      if (strlen (tempstring) > 0)
	{
	  image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
	  destrect =
	    set_rect (authorline.xoffset, authorline.yoffset, image->w,
		      image->h);
	  SDL_BlitSurface (image, NULL, screen, &destrect);
	  SDL_FreeSurface (image);
	}
    }

  scoreline.redraw = 0;
}

/*************************************************************/
/* Set Rect **************************************************/
/*************************************************************/

SDL_Rect
set_rect (int x, int y, int w, int h)
{
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
  return rect;
}

/*************************************************************/
/* Get Pixel *************************************************/
/*************************************************************/
/* Not used in this project anymore */

Uint32
getpixel (SDL_Surface * surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp)
    {
    case 1:
      return *p;
    case 2:
      return *(Uint16 *) p;
    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	return p[0] << 16 | p[1] << 8 | p[2];
      else
	return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
      return *(Uint32 *) p;
    default:
      return 0;			/* shouldn't happen, but avoids warnings */
    }
}

/***************************************************************************
 * Clear Screen                                                            *
 ***************************************************************************/
/* To save CPU usage -- which varies greatly between platforms/devices and
   video resolution -- not all parts of the screen need erasing and so this
   function now erases only what is required. The main reason for this
   optimisation is to accommodate the new ROB engine which will introduce
   draggable objects and possibly a simulated pointer over a game that was
   never designed for it. I'm simply preventing unnecessary layering */

void
clear_screen (void)
{
  SDL_Rect destrect;
  SDL_Color colour;
#ifdef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
  extern txt_x;
  extern txt_y;
#endif

  colour.r = skins[selected_skin].background_colour >> 16 & 0xff;
  colour.g = skins[selected_skin].background_colour >> 8 & 0xff;
  colour.b = skins[selected_skin].background_colour & 0xff;

  switch (game_mode)
    {
    case GAME_ON:
      /* Clear the left-hand area */
      destrect = set_rect (0, 0, viewport.xoffset, screen->h);
#ifdef DEBUG_CLEAR_SCREEN
      colour.r = 0x80;
      colour.g = 0;
      colour.b = 0;
#endif
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format, colour.r, colour.g,
				colour.b));
      /* Clear the right-hand area */
      destrect =
	set_rect (viewport.xoffset + viewport.w * video.field_size, 0,
		  screen->w - viewport.xoffset + viewport.w * video.field_size, screen->h);
#ifdef DEBUG_CLEAR_SCREEN
      colour.r = 0;
      colour.g = 0x80;
      colour.b = 0;
#endif
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format, colour.r, colour.g,
				colour.b));
      /* Clear the area occupied by the authorline */
      destrect =
	set_rect (viewport.xoffset, 0, viewport.w * video.field_size,
		  viewport.yoffset);
#ifdef DEBUG_CLEAR_SCREEN
      colour.r = 0;
      colour.g = 0;
      colour.b = 0x80;
#endif
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format, colour.r, colour.g,
				colour.b));
      /* Clear the area occupied between the bottom of the viewport and the scoreline */
      destrect =
	set_rect (viewport.xoffset,
		  viewport.yoffset + viewport.h * video.field_size,
		  viewport.w * video.field_size, video.field_size / 4);
#ifdef DEBUG_CLEAR_SCREEN
      colour.r = 0x80;
      colour.g = 0;
      colour.b = 0x80;
#endif
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format, colour.r, colour.g,
				colour.b));
      /* Clear the area occupied below the bottom of the scoreline */
      destrect =
	set_rect (viewport.xoffset, scoreline.yoffset + video.field_size,
		  viewport.w * video.field_size,
		  screen->h - scoreline.yoffset - video.field_size);
#ifdef DEBUG_CLEAR_SCREEN
      colour.r = 0;
      colour.g = 0x80;
      colour.b = 0x80;
#endif
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format, colour.r, colour.g,
				colour.b));
      break;
    case INTRO_SCREEN:
    case END_SCREEN:
    case HELP_SCREEN:
    case OPTIONS_SCREEN:
    default:
#ifdef DEBUG_CLEAR_SCREEN
      colour.r = 0x80;
      colour.g = 0x80;
      colour.b = 0;
#endif

#ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
      SDL_FillRect (screen, NULL,
		    SDL_MapRGB (screen->format, colour.r, colour.g,
				colour.b));
#else
      if (game_mode != DESIGNER_ON)
      {
        SDL_FillRect (screen, NULL,
		      SDL_MapRGB (screen->format, colour.r, colour.g,
		          colour.b));
      }
      else
      {
        // game area
        destrect.x = 0;
        destrect.y = 0;
        destrect.w = screen->w;
        destrect.h = (txt_y) * video.field_size + k_view.offsety + (video.field_size - 24) / 2;
        SDL_FillRect(screen, &destrect, SDL_MapRGB(screen->format,
          colour.r, colour.g, colour.b));

        // left-hand side of infostring
        destrect.x = 0;
        destrect.y = (txt_y) * video.field_size + k_view.offsety + (video.field_size - 24) / 2;
        destrect.w = (txt_x) * video.field_size + k_view.offsetx + video.field_size / 8;
        destrect.h = 24;
        SDL_FillRect(screen, &destrect, SDL_MapRGB(screen->format,
          colour.r, colour.g, colour.b));

        // right-hand side of infostring
        destrect.x = screen->w * 2 / 3 + 32;
        destrect.y = (txt_y) * video.field_size + k_view.offsety + (video.field_size - 24) / 2;
        destrect.w = screen->w;
        destrect.h = 24;
        SDL_FillRect(screen, &destrect, SDL_MapRGB(screen->format,
          colour.r, colour.g, colour.b));

        // below infostring
        destrect.x = 0;
        destrect.y = (txt_y) * video.field_size + k_view.offsety + (video.field_size - 24) / 2 + 24;
        destrect.w = screen->w;
        destrect.h = screen->h - ((txt_y) * video.field_size + k_view.offsety + (video.field_size - 24) / 2 + 24);
        SDL_FillRect(screen, &destrect, SDL_MapRGB(screen->format,
          colour.r, colour.g, colour.b));
      }
#endif
      break;
    }
}

/***************************************************************************
 * Set Video Mode                                                          *
 ***************************************************************************/
/* Now allows the setting of any resolution with a choice of field_sizes.

   This function works thus :-
   * There is a user requested mode, a hi-res mode (640x480) and a lo-res mode (320x240)
   * ROB needs software surfaces for alpha blits.
   * The GP2X using the GPH SDK is forced to 320x240 software surface fullscreen
   * The Zaurus is forced fullscreen
   * An attempt is made to set the user requested mode
   * If that fails then an attempt is made to set the hi-res mode (640x480)
   * If that fails then an attempt is made to set the lo-res mode (320x240)
   * If that fails then the program quits */

/*	On exit: returns 1 on error */

int
set_video_mode (void)
{
#ifdef PLATFORM_PSVITA
  screen = SDL_SetVideoMode (SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE | SDL_ANYFORMAT);
  if (screen == NULL)
  {
      printf ("SDL_SetVideoMode error\n");
      return(1);
  }
#else
  SDL_Rect **modes;
  int hwsurface[3], count, flags;
#ifdef DEBUG_VIDEO
  const SDL_VideoInfo *videoinfo;
#endif
  char description[256];

  /* Initialise all 3 modes to use software surfaces by default */
  for (count = 0; count < 3; count++)
    hwsurface[count] = SDL_SWSURFACE;

  /* Validate the screen resolution and field_size that the user may have requested */
  if (video.xres == UNDEFINED || video.yres == UNDEFINED)
    {
      video.xres = 640;
      video.yres = 480;
      if (video.field_size == UNDEFINED)
	video.field_size = 32;
    }
  else
    {
      if (video.field_size == UNDEFINED)
	video.field_size = 32;
      if (video.xres < 480 || video.yres < 480)
	video.field_size = 16;
    }

#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
  /* Force GP2X screen resolution as there is no option. */
  video.xres = 320; video.yres = 240; video.field_size = 16;
#elif defined(PLATFORM_ZAURUS)
#elif defined(PLATFORM_PSP)
#endif

  /* Validate fullscreen that the user may have requested */
  if (video.fullscreen == UNDEFINED)
    video.fullscreen = 0;

#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
  video.fullscreen = SDL_FULLSCREEN;
#elif defined(PLATFORM_ZAURUS)
  video.fullscreen = SDL_FULLSCREEN;
#elif defined(PLATFORM_FREMANTLE)
  video.fullscreen = SDL_FULLSCREEN;
#elif defined(PLATFORM_PSP)
  video.fullscreen = SDL_FULLSCREEN;
#endif

  /* Validate the viewport dimensions */
  if (viewport.max_w == UNDEFINED || viewport.max_h == UNDEFINED)
    {
      viewport.max_w = DEFAULT_VIEWPORT_WIDTH;
      viewport.max_h = DEFAULT_VIEWPORT_HEIGHT;
    }
  if (viewport.maximise == UNDEFINED)
    viewport.maximise = FALSE;

  /* Query available fullscreen hwsurface modes */
#ifdef DEBUG_VIDEO
  printf ("*** Start %s ***\n", __func__);
  printf ("Fullscreen hwsurfaces :-\n");
#endif
  modes = SDL_ListModes (NULL, SDL_FULLSCREEN | SDL_HWSURFACE);
  if (modes == NULL)
    {
#ifdef DEBUG_VIDEO
      printf ("None found\n");
#endif
    }
  else if (modes == (SDL_Rect **) - 1)
    {
#ifdef DEBUG_VIDEO
      printf ("Any dimension is okay\n");
#endif
      for (count = 0; count < 3; count++)
	hwsurface[count] = SDL_HWSURFACE;
    }
  else
    {
      for (count = 0; modes[count]; ++count)
	{
#ifdef DEBUG_VIDEO
	  printf ("%dx%d\n", modes[count]->w, modes[count]->h);
#endif
	  if (modes[count]->w == video.xres && modes[count]->h == video.yres)
	    {
	      hwsurface[0] = SDL_HWSURFACE;
	    }
	  else if (modes[count]->w == 640 && modes[count]->h == 480)
	    {
	      hwsurface[1] = SDL_HWSURFACE;
	    }
	  else if (modes[count]->w == 320 && modes[count]->h == 240)
	    {
	      hwsurface[2] = SDL_HWSURFACE;
	    }
	}
    }

  /* Query available fullscreen swsurface modes */
#ifdef DEBUG_VIDEO
  printf ("Fullscreen swsurfaces :-\n");
#endif
  modes = SDL_ListModes (NULL, SDL_FULLSCREEN | SDL_SWSURFACE);
  if (modes == NULL)
    {
#ifdef DEBUG_VIDEO
      printf ("None found\n");
#endif
    }
  else if (modes == (SDL_Rect **) - 1)
    {
#ifdef DEBUG_VIDEO
      printf ("Any dimension is okay\n");
#endif
    }
  else
    {
      for (count = 0; modes[count]; ++count)
	{
#ifdef DEBUG_VIDEO
	  printf ("%dx%d\n", modes[count]->w, modes[count]->h);
#endif
	}
    }
#ifdef DEBUG_VIDEO
  videoinfo = SDL_GetVideoInfo ();
  printf ("Best video mode info :-\n");
  printf ("hw_available: %u\n", videoinfo->hw_available);
  printf ("wm_available: %u\n", videoinfo->wm_available);
  printf ("blit_hw: %u\n", videoinfo->blit_hw);
  printf ("blit_hw_CC: %u\n", videoinfo->blit_hw_CC);
  printf ("blit_hw_A: %u\n", videoinfo->blit_hw_A);
  printf ("blit_sw: %u\n", videoinfo->blit_sw);
  printf ("blit_sw_CC: %u\n", videoinfo->blit_sw_CC);
  printf ("blit_sw_A: %u\n", videoinfo->blit_sw_A);
  printf ("blit_fill: %u\n", videoinfo->blit_fill);
  printf ("video_mem: %uKB\n", videoinfo->video_mem);
  printf ("BitsPerPixel: %u\n", videoinfo->vfmt->BitsPerPixel);
  printf ("BytesPerPixel: %u\n", videoinfo->vfmt->BytesPerPixel);
#endif

#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
  /* Well after all that clever video mode detection code the GPH GP2X
   * SDL states it has hwsurface support and then seg faults ;) */
  for (count = 0; count < 3; count++)
    hwsurface[count] = SDL_SWSURFACE;
#elif defined(PLATFORM_ZAURUS)
#elif defined(PLATFORM_PSP)
#endif

  /* First attempt to set the user requested mode, then hi-res, then lo-res, then quit */
  for (count = 0; count < 3; count++)
    {

      /* Set the hi-res or lo-res modes if the user requested mode failed */
      if (count == 1)
	{
	  video.xres = 640;
	  video.yres = 480;
	  video.field_size = 32;
	}
      else if (count == 2)
	{
	  video.xres = 320;
	  video.yres = 240;
	  video.field_size = 16;
	}

      /* Set the SDL flags and prepare the description string */
      flags = SDL_SWSURFACE | SDL_ANYFORMAT | video.fullscreen;
      strcpy (description, "software");

      /* Tell the user what we're about to do */
      if (count == 0)
	{
	  fprintf (stdout, "Setting video mode %ix%i 16bpp %s surface.\n",
		   video.xres, video.yres, description);
	}
      else if (count == 1)
	{
	  fprintf (stdout,
		   "Trying default hi-res video mode %ix%i 16bpp %s surface.\n",
		   video.xres, video.yres, description);
	}
      else if (count == 2)
	{
	  fprintf (stdout,
		   "Trying default lo-res video mode %ix%i 16bpp %s surface.\n",
		   video.xres, video.yres, description);
	}

      /* We have enough info to set the correct mode now */
      screen = SDL_SetVideoMode (video.xres, video.yres, 16, flags);
      if (screen != NULL)
	{
	  break;
	}
      else if (count < 2)
	{
	  fprintf (stdout, "Failed: %s\n", SDL_GetError ());
	}
      else
	{
	  return 1;
	}
    }

  /* It's safe to initialise these now as video.field_size has been decided */
  video.xshift = 0;
  video.yshift = 0;

#ifdef DEBUG_VIDEO
  videoinfo = SDL_GetVideoInfo ();
  printf ("Current video mode info :-\n");
  printf ("hw_available: %u\n", videoinfo->hw_available);
  printf ("wm_available: %u\n", videoinfo->wm_available);
  printf ("blit_hw: %u\n", videoinfo->blit_hw);
  printf ("blit_hw_CC: %u\n", videoinfo->blit_hw_CC);
  printf ("blit_hw_A: %u\n", videoinfo->blit_hw_A);
  printf ("blit_sw: %u\n", videoinfo->blit_sw);
  printf ("blit_sw_CC: %u\n", videoinfo->blit_sw_CC);
  printf ("blit_sw_A: %u\n", videoinfo->blit_sw_A);
  printf ("blit_fill: %u\n", videoinfo->blit_fill);
  printf ("video_mem: %uKB\n", videoinfo->video_mem);
  printf ("BitsPerPixel: %u\n", videoinfo->vfmt->BitsPerPixel);
  printf ("BytesPerPixel: %u\n", videoinfo->vfmt->BytesPerPixel);
  printf ("*** Stop %s ***\n", __func__);
#endif

#endif
  return 0;
}

/***************************************************************************
 * Toggle Fullscreen                                                       *
 ***************************************************************************/
/* Toggles between a window and fullscreen on supported platforms */

void
toggle_fullscreen (int *fullscreen)
{
  if (!(SDL_WM_ToggleFullScreen (screen)))
    {
      fprintf (stdout, "Unable to toggle fullscreen: %s\n", SDL_GetError ());
    }

  /* Be careful to record the current fullscreen state */
  if (*fullscreen != SDL_FULLSCREEN)
    {
      *fullscreen = SDL_FULLSCREEN;
    }
  else
    {
      *fullscreen = 0;
    }

}

/***************************************************************************
 * Show Level Colour                                                       *
 ***************************************************************************/
/* Used for debugging */

void
show_level_colour (int debug_colour_select_r, int debug_colour_select_g,
		   int debug_colour_select_b,
		   int debug_colour_select_component)
{
  char tempstring[60];
  int xoffset, yoffset;
  SDL_Rect destrect;

  xoffset = authorline.xoffset + video.field_size * 13.5;
  yoffset = authorline.yoffset;

  /* Display red component */
  if (debug_colour_select_component == 0)
    {
      bgcolor.r = 0xff;
      bgcolor.g = 0;
      bgcolor.b = 0;
      fgcolor.r = 0;
      fgcolor.g = 0;
      fgcolor.b = 0;
    }
  else
    {
      bgcolor.r = 0;
      bgcolor.g = 0;
      bgcolor.b = 0;
      fgcolor.r = 0xff;
      fgcolor.g = 0xff;
      fgcolor.b = 0xff;
    }
  sprintf (tempstring, "%02X", debug_colour_select_r);
  image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
  destrect = set_rect (xoffset, yoffset, image->w, image->h);
  SDL_BlitSurface (image, NULL, screen, &destrect);
  xoffset += image->w;
  SDL_FreeSurface (image);
  /* Display green component */
  if (debug_colour_select_component == 1)
    {
      bgcolor.r = 0;
      bgcolor.g = 0xff;
      bgcolor.b = 0;
      fgcolor.r = 0;
      fgcolor.g = 0;
      fgcolor.b = 0;
    }
  else
    {
      bgcolor.r = 0;
      bgcolor.g = 0;
      bgcolor.b = 0;
      fgcolor.r = 0xff;
      fgcolor.g = 0xff;
      fgcolor.b = 0xff;
    }
  sprintf (tempstring, "%02X", debug_colour_select_g);
  image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
  destrect = set_rect (xoffset, yoffset, image->w, image->h);
  SDL_BlitSurface (image, NULL, screen, &destrect);
  xoffset += image->w;
  SDL_FreeSurface (image);
  /* Display blue component */
  if (debug_colour_select_component == 2)
    {
      bgcolor.r = 0;
      bgcolor.g = 0;
      bgcolor.b = 0xff;
      fgcolor.r = 0;
      fgcolor.g = 0;
      fgcolor.b = 0;
    }
  else
    {
      bgcolor.r = 0;
      bgcolor.g = 0;
      bgcolor.b = 0;
      fgcolor.r = 0xff;
      fgcolor.g = 0xff;
      fgcolor.b = 0xff;
    }
  sprintf (tempstring, "%02X", debug_colour_select_b);
  image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
  destrect = set_rect (xoffset, yoffset, image->w, image->h);
  SDL_BlitSurface (image, NULL, screen, &destrect);
  SDL_FreeSurface (image);
}

/***************************************************************************
 * Inc Colour Component                                                    *
 ***************************************************************************/
/* Used for debugging */

void
inc_colour_component (int *colour_component)
{
  if (*colour_component == 0xf8)
    {
      *colour_component = 0xff;
    }
  else if (*colour_component == 0xff)
    {
      *colour_component = 0x0;
    }
  else
    {
      *colour_component += 0x8;
    }
}

/***************************************************************************
 * Dec Colour Component                                                    *
 ***************************************************************************/
/* Used for debugging */

void
dec_colour_component (int *colour_component)
{
  if (*colour_component == 0x0)
    {
      *colour_component = 0xff;
    }
  else if (*colour_component == 0xff)
    {
      *colour_component = 0xf8;
    }
  else
    {
      *colour_component -= 0x8;
    }
}

/***************************************************************************
 * Show Intro Screen                                                       *
 ***************************************************************************/
/* intro_screen.redraw now specifies how much is redrawn */

void
show_introscreen (void)
{
  int count, xoffset, yoffset;
  ROB_Object rob_object;
  SDL_Rect destrect;
  char tempstring[256];
  static int txtversionw = 0;

  xoffset = (screen->w - video.field_size * 15) / 2 + video.xshift;
  yoffset = (screen->h - video.field_size * 15) / 2 + video.yshift;

  bgcolor.r = skins[selected_skin].background_colour >> 16 & 0xff;
  bgcolor.g = skins[selected_skin].background_colour >> 8 & 0xff;
  bgcolor.b = skins[selected_skin].background_colour & 0xff;

  if (intro_screen.redraw >= REDRAW_INITIALISE)
    {
      /* Reinitialise some variables */

      /* Restore normal key/button repeat values for these keys */
      user_controls[ACTION_UP].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_UP].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_UP_RIGHT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_UP_RIGHT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_RIGHT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_RIGHT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_DOWN_RIGHT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_DOWN_RIGHT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_DOWN].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_DOWN].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_DOWN_LEFT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_DOWN_LEFT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_LEFT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_LEFT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_UP_LEFT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_UP_LEFT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_SHOOT_UP].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_SHOOT_UP].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_SHOOT_DOWN].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_SHOOT_DOWN].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_SHOOT_LEFT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_SHOOT_LEFT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_SHOOT_RIGHT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_SHOOT_RIGHT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_PRIMARY_CLICK].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_PRIMARY_CLICK].interval = KEY_REPEAT_INTERVAL;

      /* Free all existing hotspots including the pob */
      ROB_FreeAllDescendantObjects (ROB_GetObject (HOTSPOTS_ID), TRUE);
      /* Hide the pointer controls */
      hide_pointer_controls ();
      /* Point the event processor to manage Intro Screen events */
      gnurobbo_op_env.event_processor = &intro_screen_event_processor;
      if (ROB_SetOpEnvEventProcessor (gnurobbo_op_env.event_processor))
	printf ("%s: %s\n", __func__, ROB_GetError ());
      /* Create the hotspot parent object which will be used to identify the group */
      if (ROB_SetObjectDefaults (&rob_object))
	printf ("%s: %s\n", __func__, ROB_GetError ());
      rob_object.id = HOTSPOTS_ID;
      if (ROB_CreateObject (&rob_object))
	printf ("%s: %s\n", __func__, ROB_GetError ());

    }

#ifdef DEBUG_ROB_HOTSPOTS
  intro_screen.redraw |= REDRAW_EVERYTHING;
#endif

  if (intro_screen.redraw >= REDRAW_EVERYTHING)
    {
      /* Reinitialise some variables */

      /* Set entire screen to a colour */
      clear_screen ();

      /* Draw the version */
      fgcolor.r = skins[selected_skin].version_text_colour >> 16 & 0xff;
      fgcolor.g = skins[selected_skin].version_text_colour >> 8 & 0xff;
      fgcolor.b = skins[selected_skin].version_text_colour & 0xff;
      image = TTF_RenderUTF8_Shaded (font, VERSION, fgcolor, bgcolor);
      txtversionw = image->w;
      if (video.field_size == 16)
	{
	  destrect =
	    set_rect (xoffset + video.field_size * 15 - image->w,
		      yoffset + 37, image->w, image->h);
	}
      else if (video.field_size == 32)
	{
	  destrect =
	    set_rect (xoffset + video.field_size * 15 - image->w,
		      yoffset + 74, image->w, image->h);
	}
      SDL_BlitSurface (image, NULL, screen, &destrect);
      SDL_FreeSurface (image);

      /* Draw the intro story */
      fgcolor.r = skins[selected_skin].general_text_colour >> 16 & 0xff;
      fgcolor.g = skins[selected_skin].general_text_colour >> 8 & 0xff;
      fgcolor.b = skins[selected_skin].general_text_colour & 0xff;
      for (count = 0; count < TXT_INTRO_STORY_ROWS; count++)
	{
	  image =
	    TTF_RenderUTF8_Shaded (font,
				   &txt_intro_story[count * TXT_MAX_COLS],
				   fgcolor, bgcolor);
	  SDL_SetColorKey (image, SDL_SRCCOLORKEY,
			   SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				       bgcolor.b));
	  if (video.field_size == 16)
	    {
	      destrect =
		set_rect (xoffset + 2,
			  yoffset + 48 + count * (FONTSIZE16 +
						  video.field_size / 8),
			  image->w, image->h);
	    }
	  else if (video.field_size == 32)
	    {
	      destrect =
		set_rect (xoffset + 4,
			  yoffset + 96 + count * (FONTSIZE32 +
						  video.field_size / 8),
			  image->w, image->h);
	    }
	  SDL_BlitSurface (image, NULL, screen, &destrect);
	  SDL_FreeSurface (image);
	}

      /* Draw the intro keys */
      fgcolor.r =
	skins[selected_skin].default_controls_text_colour >> 16 & 0xff;
      fgcolor.g =
	skins[selected_skin].default_controls_text_colour >> 8 & 0xff;
      fgcolor.b = skins[selected_skin].default_controls_text_colour & 0xff;
      for (count = 0; count < TXT_INTRO_KEYS_ROWS; count++)
	{
	  image =
	    TTF_RenderUTF8_Shaded (font,
				   &txt_intro_keys[count * TXT_MAX_COLS],
				   fgcolor, bgcolor);
	  SDL_SetColorKey (image, SDL_SRCCOLORKEY,
			   SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				       bgcolor.b));
	  if (video.field_size == 16)
	    {
	      destrect =
		set_rect (xoffset + 6,
			  yoffset + 105 + count * (FONTSIZE16 +
						   video.field_size / 8),
			  image->w, image->h);
	    }
	  else if (video.field_size == 32)
	    {
	      destrect =
		set_rect (xoffset + 12,
			  yoffset + 210 + count * (FONTSIZE32 +
						   video.field_size / 8),
			  image->w, image->h);
	    }
	  SDL_BlitSurface (image, NULL, screen, &destrect);
	  SDL_FreeSurface (image);
	}

      /* Draw the intro keys description */
      fgcolor.r =
	skins[selected_skin].default_controls_text_colour >> 16 & 0xff;
      fgcolor.g =
	skins[selected_skin].default_controls_text_colour >> 8 & 0xff;
      fgcolor.b = skins[selected_skin].default_controls_text_colour & 0xff;
      for (count = 0; count < TXT_INTRO_KEYS_DESCRIPTION_ROWS; count++)
	{
	  image =
	    TTF_RenderUTF8_Shaded (font,
				   &txt_intro_keys_description[count *
							       TXT_MAX_COLS],
				   fgcolor, bgcolor);
	  SDL_SetColorKey (image, SDL_SRCCOLORKEY,
			   SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				       bgcolor.b));
	  if (video.field_size == 16)
	    {
	      destrect =
		set_rect (xoffset + 79,
			  yoffset + 105 + count * (FONTSIZE16 +
						   video.field_size / 8),
			  image->w, image->h);
	    }
	  else if (video.field_size == 32)
	    {
	      destrect =
		set_rect (xoffset + 158,
			  yoffset + 210 + count * (FONTSIZE32 +
						   video.field_size / 8),
			  image->w, image->h);
	    }
	  SDL_BlitSurface (image, NULL, screen, &destrect);
	  SDL_FreeSurface (image);
	}

      /* Draw the intro credits */
      fgcolor.r = skins[selected_skin].credits_text_colour >> 16 & 0xff;
      fgcolor.g = skins[selected_skin].credits_text_colour >> 8 & 0xff;
      fgcolor.b = skins[selected_skin].credits_text_colour & 0xff;
      for (count = 0; count < TXT_INTRO_CREDITS_ROWS; count++)
	{
	  image =
	    TTF_RenderUTF8_Shaded (font,
				   &txt_intro_credits[count * TXT_MAX_COLS],
				   fgcolor, bgcolor);
	  SDL_SetColorKey (image, SDL_SRCCOLORKEY,
			   SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				       bgcolor.b));
	  if (video.field_size == 16)
	    {
	      destrect =
		set_rect (xoffset + video.field_size * 15 / 2 - image->w / 2,
			  yoffset + 199 + count * (FONTSIZE16 +
						   video.field_size / 8),
			  image->w, image->h);
	    }
	  else if (video.field_size == 32)
	    {
	      destrect =
		set_rect (xoffset + video.field_size * 15 / 2 - image->w / 2,
			  yoffset + 398 + count * (FONTSIZE32 +
						   video.field_size / 8),
			  image->w, image->h);
	    }
	  SDL_BlitSurface (image, NULL, screen, &destrect);
	  SDL_FreeSurface (image);
	}
    }

  if (intro_screen.redraw >= REDRAW_INTERMEDIATE)
    {
      /* Does the menu text area only require erasing? */
      if (intro_screen.redraw <= REDRAW_INTERMEDIATE)
	{
	  destrect =
	    set_rect (xoffset + 125 * video.field_size / 16,
		      yoffset + 104 * video.field_size / 16,
		      113 * video.field_size / 16,
		      90 * video.field_size / 16);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format,
				    skins[selected_skin].
				    background_colour >> 16 & 0xff,
				    skins[selected_skin].
				    background_colour >> 8 & 0xff,
				    skins[selected_skin].
				    background_colour & 0xff));
	}

      /* Draw the intro menu text */
      for (count = 0; count < TXT_INTRO_MENU_ROWS; count++)
	{

/*#ifndef HAVE_DESIGNER*/
	  if (count == 5) continue;	/* Skip it */
/*#endif*/

	  if (count == introscreenselecteditem)
	    {
	      fgcolor.r =
		skins[selected_skin].menu_selected_text_colour >> 16 & 0xff;
	      fgcolor.g =
		skins[selected_skin].menu_selected_text_colour >> 8 & 0xff;
	      fgcolor.b =
		skins[selected_skin].menu_selected_text_colour & 0xff;
	    }
	  else
	    {
	      fgcolor.r = skins[selected_skin].menu_text_colour >> 16 & 0xff;
	      fgcolor.g = skins[selected_skin].menu_text_colour >> 8 & 0xff;
	      fgcolor.b = skins[selected_skin].menu_text_colour & 0xff;
	    }
	  image =
	    TTF_RenderUTF8_Shaded (font,
				   &txt_intro_menu[count * TXT_MAX_COLS],
				   fgcolor, bgcolor);
	  SDL_SetColorKey (image, SDL_SRCCOLORKEY,
			   SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				       bgcolor.b));
	  if (video.field_size == 16)
	    {
	      destrect =
		set_rect (xoffset + 129,
			  yoffset + 105 + count * (FONTSIZE16 +
						   video.field_size / 8),
			  image->w, image->h);
	    }
	  else if (video.field_size == 32)
	    {
	      destrect =
		set_rect (xoffset + 258,
			  yoffset + 210 + count * (FONTSIZE32 +
						   video.field_size / 8),
			  image->w, image->h);
	    }
	  SDL_BlitSurface (image, NULL, screen, &destrect);
	  register_hotspot (HOTSPOTS_ID, HOTSPOTS_ID + 1 + count, destrect);
	  SDL_FreeSurface (image);
	}

      /* Draw the current level */
      /* < */
      strcpy (tempstring, "< ");
      if (level_packs[selected_pack].level_selected > 1)
	{
	  fgcolor.r =
	    skins[selected_skin].menu_selected_text_colour >> 16 & 0xff;
	  fgcolor.g =
	    skins[selected_skin].menu_selected_text_colour >> 8 & 0xff;
	  fgcolor.b = skins[selected_skin].menu_selected_text_colour & 0xff;
	}
      else
	{
	  fgcolor.r =
	    skins[selected_skin].menu_selected_limit_text_colour >> 16 & 0xff;
	  fgcolor.g =
	    skins[selected_skin].menu_selected_limit_text_colour >> 8 & 0xff;
	  fgcolor.b =
	    skins[selected_skin].menu_selected_limit_text_colour & 0xff;
	}
      image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
      SDL_SetColorKey (image, SDL_SRCCOLORKEY,
		       SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				   bgcolor.b));
      if (video.field_size == 16)
	{
	  destrect =
	    set_rect (xoffset + 164, yoffset + 117, image->w, image->h);
	}
      else if (video.field_size == 32)
	{
	  destrect =
	    set_rect (xoffset + 330, yoffset + 234, image->w, image->h);
	}
      SDL_BlitSurface (image, NULL, screen, &destrect);
      register_hotspot (HOTSPOTS_ID,
			HOTSPOTS_ID + 1 + INTRO_SCREEN_MENU_ITEM_LEVEL +
			INTRO_SCREEN_MENU_ITEM_COUNT, destrect);
      destrect.x += image->w;
      SDL_FreeSurface (image);
      /* 999 */
      sprintf (tempstring, "%i %s %i",
	       level_packs[selected_pack].level_selected, txt_of,
	       level_packs[selected_pack].last_level);
      fgcolor.r = skins[selected_skin].general_text_colour >> 16 & 0xff;
      fgcolor.g = skins[selected_skin].general_text_colour >> 8 & 0xff;
      fgcolor.b = skins[selected_skin].general_text_colour & 0xff;
      image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
      SDL_SetColorKey (image, SDL_SRCCOLORKEY,
		       SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				   bgcolor.b));
      destrect.w = image->w;
      destrect.h = image->h;
      SDL_BlitSurface (image, NULL, screen, &destrect);
      destrect.x += image->w;
      SDL_FreeSurface (image);
      /* > */
      strcpy (tempstring, " >");
      if (level_packs[selected_pack].level_selected <
	  level_packs[selected_pack].level_reached)
	{
	  fgcolor.r =
	    skins[selected_skin].menu_selected_text_colour >> 16 & 0xff;
	  fgcolor.g =
	    skins[selected_skin].menu_selected_text_colour >> 8 & 0xff;
	  fgcolor.b = skins[selected_skin].menu_selected_text_colour & 0xff;
	}
      else
	{
	  fgcolor.r =
	    skins[selected_skin].menu_selected_limit_text_colour >> 16 & 0xff;
	  fgcolor.g =
	    skins[selected_skin].menu_selected_limit_text_colour >> 8 & 0xff;
	  fgcolor.b =
	    skins[selected_skin].menu_selected_limit_text_colour & 0xff;
	}
      image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
      SDL_SetColorKey (image, SDL_SRCCOLORKEY,
		       SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				   bgcolor.b));
      destrect.w = image->w;
      destrect.h = image->h;
      SDL_BlitSurface (image, NULL, screen, &destrect);
      register_hotspot (HOTSPOTS_ID,
			HOTSPOTS_ID + 1 + INTRO_SCREEN_MENU_ITEM_LEVEL +
			INTRO_SCREEN_MENU_ITEM_COUNT * 2, destrect);
      SDL_FreeSurface (image);

      /* Draw the current pack */
      /* < */
      strcpy (tempstring, "< ");
      if (selected_pack > 0)
	{
	  fgcolor.r =
	    skins[selected_skin].menu_selected_text_colour >> 16 & 0xff;
	  fgcolor.g =
	    skins[selected_skin].menu_selected_text_colour >> 8 & 0xff;
	  fgcolor.b = skins[selected_skin].menu_selected_text_colour & 0xff;
	}
      else
	{
	  fgcolor.r =
	    skins[selected_skin].menu_selected_limit_text_colour >> 16 & 0xff;
	  fgcolor.g =
	    skins[selected_skin].menu_selected_limit_text_colour >> 8 & 0xff;
	  fgcolor.b =
	    skins[selected_skin].menu_selected_limit_text_colour & 0xff;
	}
      image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
      SDL_SetColorKey (image, SDL_SRCCOLORKEY,
		       SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				   bgcolor.b));
      if (video.field_size == 16)
	{
	  destrect =
	    set_rect (xoffset + 165, yoffset + 129, image->w, image->h);
	}
      else if (video.field_size == 32)
	{
	  destrect =
	    set_rect (xoffset + 330, yoffset + 258, image->w, image->h);
	}
      SDL_BlitSurface (image, NULL, screen, &destrect);
      register_hotspot (HOTSPOTS_ID,
			HOTSPOTS_ID + 1 + INTRO_SCREEN_MENU_ITEM_PACK +
			INTRO_SCREEN_MENU_ITEM_COUNT, destrect);
      destrect.x += image->w;
      SDL_FreeSurface (image);
      /* "Original" */
      fgcolor.r = skins[selected_skin].general_text_colour >> 16 & 0xff;
      fgcolor.g = skins[selected_skin].general_text_colour >> 8 & 0xff;
      fgcolor.b = skins[selected_skin].general_text_colour & 0xff;
      image =
	TTF_RenderUTF8_Shaded (font, level_packs[selected_pack].name, fgcolor,
			       bgcolor);
      SDL_SetColorKey (image, SDL_SRCCOLORKEY,
		       SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				   bgcolor.b));
      destrect.w = image->w;
      destrect.h = image->h;
      SDL_BlitSurface (image, NULL, screen, &destrect);
      destrect.x += image->w;
      SDL_FreeSurface (image);
      /* > */
      strcpy (tempstring, " >");
      if (selected_pack < found_pack_count - 1)
	{
	  fgcolor.r =
	    skins[selected_skin].menu_selected_text_colour >> 16 & 0xff;
	  fgcolor.g =
	    skins[selected_skin].menu_selected_text_colour >> 8 & 0xff;
	  fgcolor.b = skins[selected_skin].menu_selected_text_colour & 0xff;
	}
      else
	{
	  fgcolor.r =
	    skins[selected_skin].menu_selected_limit_text_colour >> 16 & 0xff;
	  fgcolor.g =
	    skins[selected_skin].menu_selected_limit_text_colour >> 8 & 0xff;
	  fgcolor.b =
	    skins[selected_skin].menu_selected_limit_text_colour & 0xff;
	}
      image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
      SDL_SetColorKey (image, SDL_SRCCOLORKEY,
		       SDL_MapRGB (image->format, bgcolor.r, bgcolor.g,
				   bgcolor.b));
      destrect.w = image->w;
      destrect.h = image->h;
      SDL_BlitSurface (image, NULL, screen, &destrect);
      register_hotspot (HOTSPOTS_ID,
			HOTSPOTS_ID + 1 + INTRO_SCREEN_MENU_ITEM_PACK +
			INTRO_SCREEN_MENU_ITEM_COUNT * 2, destrect);
      SDL_FreeSurface (image);
    }

  if (intro_screen.redraw >= REDRAW_ANIMATED)
    {
      /* Draw the horizontal line under the GNU Robbo banner */
      if (video.field_size == 16)
	{
	  destrect =
	    set_rect (xoffset + 0, yoffset + 43, 240 - txtversionw, video.field_size / 8);
	}
      else if (video.field_size == 32)
	{
	  destrect =
	    set_rect (xoffset + 0, yoffset + 86, 480 - txtversionw, video.field_size / 8);
	}
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Draw the horizontal line above the controls area */
      destrect =
	set_rect (xoffset + 0, yoffset + 102 * video.field_size / 16,
		  240 * video.field_size / 16, video.field_size / 8);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Draw the horizontal line below the controls area */
      destrect =
	set_rect (xoffset + 0, yoffset + 194 * video.field_size / 16,
		  240 * video.field_size / 16, video.field_size / 8);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Draw the vertical line to the left of the controls area */
      destrect =
	set_rect (xoffset + 0, yoffset + 102 * video.field_size / 16,
		  video.field_size / 8, 92 * video.field_size / 16);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Draw the vertical line in the middle of the controls area */
      destrect =
	set_rect (xoffset + 123 * video.field_size / 16,
		  yoffset + 102 * video.field_size / 16, video.field_size / 8,
		  92 * video.field_size / 16);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Draw the vertical line to the right of the controls area */
      destrect =
	set_rect (xoffset + 238 * video.field_size / 16,
		  yoffset + 102 * video.field_size / 16, video.field_size / 8,
		  92 * video.field_size / 16);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Draw the GNU Robbo banner */
      if (video.field_size == 16)
	{
	  show_banner (banner, xoffset + 0, yoffset + 6, 80, 8);
	}
      else if (video.field_size == 32)
	{
	  show_banner (banner, xoffset + 40, yoffset + 19, 80, 8);
	}
      /* Update the banner colour frame */
      if (--bannerstate <= 0)
	{
	  bannerstate = DELAY_BANNER;
	  if (++bannerframe >= BANNER_COLOURS)
	    bannerframe = 0;
	}
    }

  intro_screen.redraw = 0;
}

/***************************************************************************
 * Show Help Screen                                                        *
 ***************************************************************************/
/* help_screen.redraw now specifies how much is redrawn */

void
show_helpscreen (void)
{
  int xoffset, yoffset, yincrement = video.field_size + video.field_size / 4;
  int iconoffsets1[7 * 2] =
    { 52, 1, 103, 1, 137, 18, 86, 103, 120, 1, 137, 1, 1, 18 };
  int iconoffsets2[7 * 2] = { 86, 1, 1, 1, 69, 1, 0, 0, 0, 0, 0, 0, 0, 0 };
  int iconoffsets3[7 * 2] =
    { 0, 0, 0, 0, 0, 0, 86, 69, 86, 69, 86, 69, 0, 0 };
  Uint32 tilecolour = skins[selected_skin].help_tile_colour;
  SDL_Rect srcrect, destrect;
  static int lastpage = 0;
  ROB_Object rob_object;
  char tempstring[256];
  int count, tempframe;

  xoffset = (screen->w - video.field_size * 15) / 2 + video.xshift;
  yoffset = (screen->h - video.field_size * 15) / 2 + video.yshift;

  bgcolor.r = skins[selected_skin].background_colour >> 16 & 0xff;
  bgcolor.g = skins[selected_skin].background_colour >> 8 & 0xff;
  bgcolor.b = skins[selected_skin].background_colour & 0xff;
  fgcolor.r = skins[selected_skin].general_text_colour >> 16 & 0xff;
  fgcolor.g = skins[selected_skin].general_text_colour >> 8 & 0xff;
  fgcolor.b = skins[selected_skin].general_text_colour & 0xff;

  if (help_screen.redraw >= REDRAW_INITIALISE)
    {
      /* Reinitialise some variables */
      /* Restore normal key/button repeat values for these keys */
      user_controls[ACTION_UP].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_UP].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_UP_RIGHT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_UP_RIGHT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_RIGHT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_RIGHT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_DOWN_RIGHT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_DOWN_RIGHT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_DOWN].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_DOWN].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_DOWN_LEFT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_DOWN_LEFT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_LEFT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_LEFT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_UP_LEFT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_UP_LEFT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_SHOOT_UP].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_SHOOT_UP].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_SHOOT_DOWN].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_SHOOT_DOWN].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_SHOOT_LEFT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_SHOOT_LEFT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_SHOOT_RIGHT].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_SHOOT_RIGHT].interval = KEY_REPEAT_INTERVAL;
      user_controls[ACTION_PRIMARY_CLICK].delay = KEY_REPEAT_DELAY;
      user_controls[ACTION_PRIMARY_CLICK].interval = KEY_REPEAT_INTERVAL;

      /* Free all existing hotspots including the pob */
      ROB_FreeAllDescendantObjects (ROB_GetObject (HOTSPOTS_ID), TRUE);
      /* Hide the pointer controls */
      hide_pointer_controls ();
      /* Point the event processor to manage Help Screen events */
      gnurobbo_op_env.event_processor = &help_screen_event_processor;
      if (ROB_SetOpEnvEventProcessor (gnurobbo_op_env.event_processor))
	printf ("%s: %s\n", __func__, ROB_GetError ());
      /* Create the hotspot parent object which will be used to identify the group */
      if (ROB_SetObjectDefaults (&rob_object))
	printf ("%s: %s\n", __func__, ROB_GetError ());
      rob_object.id = HOTSPOTS_ID;
      if (ROB_CreateObject (&rob_object))
	printf ("%s: %s\n", __func__, ROB_GetError ());

    }

#ifdef DEBUG_ROB_HOTSPOTS
  help_screen.redraw |= REDRAW_EVERYTHING;
#endif

  if (help_screen.redraw >= REDRAW_EVERYTHING)
    {
      /* Monitor page changes and free any existing hotspots */
      if (lastpage != helppage)
	{
	  /* Free the existing hotspots from the last page */
	  ROB_FreeAllDescendantObjects (ROB_GetObject (HOTSPOTS_ID), FALSE);
	}
      lastpage = helppage;

      /* Set entire screen to a colour */
      clear_screen ();

      if (helppage == 0)
	{
	  /* Draw the help text */
	  for (count = 0; count < TXT_HELP_PAGE0_ROWS; count++)
	    {
	      image =
		TTF_RenderUTF8_Shaded (font,
				       &txt_help_page0[count * TXT_MAX_COLS],
				       fgcolor, bgcolor);
	      SDL_SetColorKey (image, SDL_SRCCOLORKEY,
			       SDL_MapRGB (image->format, bgcolor.r,
					   bgcolor.g, bgcolor.b));
	      if (video.field_size == 16)
		{
		  destrect =
		    set_rect (xoffset + 2,
			      yoffset + 57 + count * (FONTSIZE16 +
						      video.field_size / 8),
			      image->w, image->h);
		}
	      else if (video.field_size == 32)
		{
		  destrect =
		    set_rect (xoffset + 4,
			      yoffset + 104 + count * (FONTSIZE32 +
						       video.field_size / 8),
			      image->w, image->h);
		}
	      SDL_BlitSurface (image, NULL, screen, &destrect);
	      SDL_FreeSurface (image);
	    }
	}
      else if (helppage == 1)
	{
	  for (count = 0; count < TXT_HELP_PAGE1_ROWS; count++)
	    {
	      /* Draw the background tiles for the icons */
	      destrect =
		set_rect (xoffset + 2 * video.field_size / 16,
			  yoffset + 57 * video.field_size / 16 +
			  count * yincrement, video.field_size,
			  video.field_size);
	      SDL_FillRect (screen, &destrect,
			    SDL_MapRGB (screen->format,
					tilecolour >> 16 & 0xff,
					tilecolour >> 8 & 0xff,
					tilecolour & 0xff));
	      /* Draw the icons */
	      srcrect =
		set_rect (iconoffsets1[count * 2] * video.field_size / 16,
			  iconoffsets1[count * 2 + 1] * video.field_size / 16,
			  video.field_size, video.field_size);
	      destrect =
		set_rect (xoffset + 2 * video.field_size / 16,
			  yoffset + 57 * video.field_size / 16 +
			  count * yincrement, video.field_size,
			  video.field_size);
	      SDL_BlitSurface (icons, &srcrect, screen, &destrect);
	      if (count == 4)
		{
		  /* Draw the background tile for the door icon */
		  destrect =
		    set_rect (xoffset + 2 * video.field_size / 16 +
			      video.field_size * 1.5,
			      yoffset + 57 * video.field_size / 16 +
			      count * yincrement, video.field_size,
			      video.field_size);
		  SDL_FillRect (screen, &destrect,
				SDL_MapRGB (screen->format,
					    tilecolour >> 16 & 0xff,
					    tilecolour >> 8 & 0xff,
					    tilecolour & 0xff));
		  /* Draw the door icon */
		  srcrect =
		    set_rect (154 * video.field_size / 16,
			      1 * video.field_size / 16, video.field_size,
			      video.field_size);
		  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
		}

		/* Draw the text for the icons */
		image = TTF_RenderUTF8_Shaded (font,
			&txt_help_page1[count * TXT_MAX_COLS], fgcolor, bgcolor);
		if (count == 4) {
			destrect = set_rect (xoffset + 2 * video.field_size / 16 +
				video.field_size * 3,
				yoffset + 57 * video.field_size / 16 +
				count * yincrement, image->w, image->h);
		} else {
			destrect = set_rect (xoffset + 2 * video.field_size / 16 +
				video.field_size * 1.5,
				yoffset + 57 * video.field_size / 16 +
				count * yincrement, image->w, image->h);
		}
		SDL_BlitSurface (image, NULL, screen, &destrect);
		SDL_FreeSurface (image);

		}
	}
      else if (helppage == 2)
	{
	  for (count = 0; count < TXT_HELP_PAGE2_ROWS; count++)
	    {
	      if (count < 3)
		{
		  /* Draw the background tiles for the icons */
		  destrect =
		    set_rect (xoffset + 2 * video.field_size / 16,
			      yoffset + 57 * video.field_size / 16 +
			      count * yincrement, video.field_size,
			      video.field_size);
		  SDL_FillRect (screen, &destrect,
				SDL_MapRGB (screen->format,
					    tilecolour >> 16 & 0xff,
					    tilecolour >> 8 & 0xff,
					    tilecolour & 0xff));
		  /* Draw the icons */
		  srcrect =
		    set_rect (iconoffsets2[count * 2] * video.field_size / 16,
			      iconoffsets2[count * 2 +
					   1] * video.field_size / 16,
			      video.field_size, video.field_size);
		  destrect =
		    set_rect (xoffset + 2 * video.field_size / 16,
			      yoffset + 57 * video.field_size / 16 +
			      count * yincrement, video.field_size,
			      video.field_size);
		  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
		}
	      /* Draw the text for the icons */
	      image =
		TTF_RenderUTF8_Shaded (font,
				       &txt_help_page2[count * TXT_MAX_COLS],
				       fgcolor, bgcolor);
	      destrect =
		set_rect (xoffset + 2 * video.field_size / 16 +
			  video.field_size * 1.5,
			  yoffset + 57 * video.field_size / 16 +
			  count * yincrement, image->w, image->h);
	      SDL_BlitSurface (image, NULL, screen, &destrect);
	      SDL_FreeSurface (image);
	    }
	}
      else if (helppage == 3)
	{
	  for (count = 0; count < TXT_HELP_PAGE3_ROWS; count++)
	    {
	      if (count > 2 && count < 6)
		{
		  /* Draw the background tiles for the icons */
		  destrect =
		    set_rect (xoffset + 2 * video.field_size / 16,
			      yoffset + 57 * video.field_size / 16 +
			      count * yincrement, video.field_size,
			      video.field_size);
		  SDL_FillRect (screen, &destrect,
				SDL_MapRGB (screen->format,
					    tilecolour >> 16 & 0xff,
					    tilecolour >> 8 & 0xff,
					    tilecolour & 0xff));
		  /* Draw the icons */
		  srcrect =
		    set_rect (iconoffsets3[count * 2] * video.field_size / 16,
			      iconoffsets3[count * 2 +
					   1] * video.field_size / 16,
			      video.field_size, video.field_size);
		  destrect =
		    set_rect (xoffset + 2 * video.field_size / 16,
			      yoffset + 57 * video.field_size / 16 +
			      count * yincrement, video.field_size,
			      video.field_size);
		  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
		}
	      /* Draw the text for the icons */
	      image =
		TTF_RenderUTF8_Shaded (font,
				       &txt_help_page3[count * TXT_MAX_COLS],
				       fgcolor, bgcolor);
	      if (count >= 3 && count <= 5)
		{
		  destrect =
		    set_rect (xoffset + 2 * video.field_size / 16 +
			      video.field_size * 6.5,
			      yoffset + 57 * video.field_size / 16 +
			      count * yincrement, image->w, image->h);
		}
	      else
		{
		  destrect =
		    set_rect (xoffset + 2 * video.field_size / 16 +
			      video.field_size * 1.5,
			      yoffset + 57 * video.field_size / 16 +
			      count * yincrement, image->w, image->h);
		}
	      SDL_BlitSurface (image, NULL, screen, &destrect);
	      SDL_FreeSurface (image);
	    }
	}
    }

  if (help_screen.redraw >= REDRAW_INTERMEDIATE)
    {
      /* Exit */
      if (helppageselecteditem == 1)
	{
	  fgcolor.r =
	    skins[selected_skin].menu_selected_text_colour >> 16 & 0xff;
	  fgcolor.g =
	    skins[selected_skin].menu_selected_text_colour >> 8 & 0xff;
	  fgcolor.b = skins[selected_skin].menu_selected_text_colour & 0xff;
	}
      else
	{
	  fgcolor.r = skins[selected_skin].menu_text_colour >> 16 & 0xff;
	  fgcolor.g = skins[selected_skin].menu_text_colour >> 8 & 0xff;
	  fgcolor.b = skins[selected_skin].menu_text_colour & 0xff;
	}
      sprintf (tempstring, "%s", txt_Exit);
      image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
      destrect =
	set_rect (xoffset + 240 * video.field_size / 16 / 2 - image->w / 2,
		  yoffset + 220 * video.field_size / 16, image->w, image->h);
      SDL_BlitSurface (image, NULL, screen, &destrect);
      register_hotspot (HOTSPOTS_ID, HOTSPOTS_ID + 1 + 1, destrect);
      SDL_FreeSurface (image);

      if (helppage < HELP_SCREEN_PAGES - 1)
	{
	  /* Next > */
	  if (helppageselecteditem == 2)
	    {
	      fgcolor.r =
		skins[selected_skin].menu_selected_text_colour >> 16 & 0xff;
	      fgcolor.g =
		skins[selected_skin].menu_selected_text_colour >> 8 & 0xff;
	      fgcolor.b =
		skins[selected_skin].menu_selected_text_colour & 0xff;
	    }
	  else
	    {
	      fgcolor.r = skins[selected_skin].menu_text_colour >> 16 & 0xff;
	      fgcolor.g = skins[selected_skin].menu_text_colour >> 8 & 0xff;
	      fgcolor.b = skins[selected_skin].menu_text_colour & 0xff;
	    }
	  sprintf (tempstring, "%s >", txt_Next);
	  image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
	  destrect =
	    set_rect (xoffset + 238 * video.field_size / 16 - image->w,
		      yoffset + 220 * video.field_size / 16, image->w,
		      image->h);
	  SDL_BlitSurface (image, NULL, screen, &destrect);
	  register_hotspot (HOTSPOTS_ID, HOTSPOTS_ID + 1 + 2, destrect);
	  SDL_FreeSurface (image);
	}

      if (helppage > 0)
	{
	  /* < Back */
	  if (helppageselecteditem == 0)
	    {
	      fgcolor.r =
		skins[selected_skin].menu_selected_text_colour >> 16 & 0xff;
	      fgcolor.g =
		skins[selected_skin].menu_selected_text_colour >> 8 & 0xff;
	      fgcolor.b =
		skins[selected_skin].menu_selected_text_colour & 0xff;
	    }
	  else
	    {
	      fgcolor.r = skins[selected_skin].menu_text_colour >> 16 & 0xff;
	      fgcolor.g = skins[selected_skin].menu_text_colour >> 8 & 0xff;
	      fgcolor.b = skins[selected_skin].menu_text_colour & 0xff;
	    }
	  sprintf (tempstring, "< %s", txt_Back);
	  image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolor, bgcolor);
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16,
		      yoffset + 220 * video.field_size / 16, image->w,
		      image->h);
	  SDL_BlitSurface (image, NULL, screen, &destrect);
	  register_hotspot (HOTSPOTS_ID, HOTSPOTS_ID + 1 + 0, destrect);
	  SDL_FreeSurface (image);
	}
    }

  if (help_screen.redraw >= REDRAW_ANIMATED)
    {
      if (helppage == 2)
	{
	  /* Animate the capsule */
	  srcrect =
	    set_rect (capsuleoffsets[capsuleframe * 2] * video.field_size /
		      16,
		      capsuleoffsets[capsuleframe * 2 +
				     1] * video.field_size / 16,
		      video.field_size, video.field_size);
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16,
		      yoffset + 57 * video.field_size / 16 + 3 * yincrement,
		      video.field_size, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
	  /* Update the capsule frame */
	  if (--capsulestate <= 0)
	    {
	      capsulestate = DELAY_CAPSULE;
	      if (++capsuleframe > 1)
		capsuleframe = 0;
	    }
	  /* Animate the teleport */
	  srcrect =
	    set_rect (teleportoffsets[teleportframe * 2] * video.field_size /
		      16,
		      teleportoffsets[teleportframe * 2 +
				      1] * video.field_size / 16,
		      video.field_size, video.field_size);
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16,
		      yoffset + 57 * video.field_size / 16 + 4 * yincrement,
		      video.field_size, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
	  /* Update the teleport frame */
	  if (--teleportstate <= 0)
	    {
	      teleportstate = DELAY_TELEPORT;
	      if (++teleportframe > 1)
		teleportframe = 0;
	    }
	  /* Animate the barrier */
	  srcrect =
	    set_rect (barrieroffsets[barrierframe * 2] * video.field_size /
		      16,
		      barrieroffsets[barrierframe * 2 +
				     1] * video.field_size / 16,
		      video.field_size, video.field_size);
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16,
		      yoffset + 57 * video.field_size / 16 + 5 * yincrement,
		      video.field_size, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
	  /* Update the barrier frame */
	  if (--barrierstate <= 0)
	    {
	      barrierstate = DELAY_BARRIER;
	      if (++barrierframe > 1)
		barrierframe = 0;
	    }
	  /* Animate the butterfly */
	  srcrect =
	    set_rect (butterflyoffsets[butterflyframe * 2] *
		      video.field_size / 16,
		      butterflyoffsets[butterflyframe * 2 +
				       1] * video.field_size / 16,
		      video.field_size, video.field_size);
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16,
		      yoffset + 57 * video.field_size / 16 + 6 * yincrement,
		      video.field_size, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
	  /* Update the butterfly frame */
	  if (--butterflystate <= 0)
	    {
	      butterflystate = DELAY_BUTTERFLY;
	      if (++butterflyframe > 1)
		butterflyframe = 0;
	    }
	}
      else if (helppage == 3)
	{
	  /* Animate the bear */
	  srcrect =
	    set_rect (bearoffsets[bearframe * 2] * video.field_size / 16,
		      bearoffsets[bearframe * 2 + 1] * video.field_size / 16,
		      video.field_size, video.field_size);
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16,
		      yoffset + 57 * video.field_size / 16, video.field_size,
		      video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
	  /* Update the bear frame */
	  if (--bearstate <= 0)
	    {
	      bearstate = DELAY_BEAR;
	      if (++bearframe > 1)
		bearframe = 0;
	    }
	  /* Animate the blackbear */
	  srcrect =
	    set_rect (blackbearoffsets[blackbearframe * 2] *
		      video.field_size / 16,
		      blackbearoffsets[blackbearframe * 2 +
				       1] * video.field_size / 16,
		      video.field_size, video.field_size);
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16,
		      yoffset + 57 * video.field_size / 16 + 1 * yincrement,
		      video.field_size, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
	  /* Update the blackbear frame */
	  if (--blackbearstate <= 0)
	    {
	      blackbearstate = DELAY_BEAR;
	      if (++blackbearframe > 1)
		blackbearframe = 0;
	    }
	  /* Animate the bird */
	  srcrect =
	    set_rect (birdoffsets[birdframe * 2] * video.field_size / 16,
		      birdoffsets[birdframe * 2 + 1] * video.field_size / 16,
		      video.field_size, video.field_size);
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16,
		      yoffset + 57 * video.field_size / 16 + 2 * yincrement,
		      video.field_size, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
	  /* Update the bird frame */
	  if (--birdstate <= 0)
	    {
	      birdstate = DELAY_BIRD;
	      if (++birdframe > 1)
		birdframe = 0;
	    }
	  /* Animate the gun */
	  tempframe = gunframe;
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16 + video.field_size,
		      yoffset + 57 * video.field_size / 16 + 3 * yincrement,
		      video.field_size * 5, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  for (count = 0; count < 5; count++)
	    {
	      if (gunoffsets[tempframe * 2] != 0
		  && gunoffsets[tempframe * 2 + 1] != 0)
		{
		  srcrect =
		    set_rect (gunoffsets[tempframe * 2] * video.field_size /
			      16,
			      gunoffsets[tempframe * 2 +
					 1] * video.field_size / 16,
			      video.field_size, video.field_size);
		  destrect =
		    set_rect (xoffset + 2 * video.field_size / 16 +
			      (count + 1) * video.field_size,
			      yoffset + 57 * video.field_size / 16 +
			      3 * yincrement, video.field_size,
			      video.field_size);
		  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
		}
	      tempframe++;
	      if (tempframe >= 25)
		tempframe = 0;
	    }
	  /* Update the gun frame */
	  if (--gunstate <= 0)
	    {
	      gunstate = DELAY_LASER;
	      gunframe += 5;
	      if (gunframe >= 25)
		gunframe = 0;
	    }
	  /* Animate the laser */
	  tempframe = laserframe;
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16 + video.field_size,
		      yoffset + 57 * video.field_size / 16 + 4 * yincrement,
		      video.field_size * 5, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  for (count = 0; count < 5; count++)
	    {
	      if (laseroffsets[tempframe * 2] != 0
		  && laseroffsets[tempframe * 2 + 1] != 0)
		{
		  srcrect =
		    set_rect (laseroffsets[tempframe * 2] * video.field_size /
			      16,
			      laseroffsets[tempframe * 2 +
					   1] * video.field_size / 16,
			      video.field_size, video.field_size);
		  destrect =
		    set_rect (xoffset + 2 * video.field_size / 16 +
			      (count + 1) * video.field_size,
			      yoffset + 57 * video.field_size / 16 +
			      4 * yincrement, video.field_size,
			      video.field_size);
		  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
		}
	      tempframe++;
	      if (tempframe >= 70)
		tempframe = 0;
	    }
	  /* Update the laser frame */
	  if (--laserstate <= 0)
	    {
	      laserframe += 5;
	      if (laserframe >= 70)
		laserframe = 0;
	      if (laserframe >= 45)
		laserstate = DELAY_LITTLE_BOOM;
	      if (laserframe < 45)
		laserstate = DELAY_LASER;
	    }
	  /* Animate the blaster */
	  tempframe = blasterframe;
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16 + video.field_size,
		      yoffset + 57 * video.field_size / 16 + 5 * yincrement,
		      video.field_size * 5, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  for (count = 0; count < 5; count++)
	    {
	      if (blasteroffsets[tempframe * 2] != 0
		  && blasteroffsets[tempframe * 2 + 1] != 0)
		{
		  srcrect =
		    set_rect (blasteroffsets[tempframe * 2] *
			      video.field_size / 16,
			      blasteroffsets[tempframe * 2 +
					     1] * video.field_size / 16,
			      video.field_size, video.field_size);
		  destrect =
		    set_rect (xoffset + 2 * video.field_size / 16 +
			      (count + 1) * video.field_size,
			      yoffset + 57 * video.field_size / 16 +
			      5 * yincrement, video.field_size,
			      video.field_size);
		  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
		}
	      tempframe++;
	      if (tempframe >= 45)
		tempframe = 0;
	    }
	  /* Update the blaster frame */
	  if (--blasterstate <= 0)
	    {
	      blasterstate = DELAY_LASER;
	      blasterframe += 5;
	      if (blasterframe >= 45)
		blasterframe = 0;
	    }
	  /* Animate Robbo */
	  srcrect =
	    set_rect (robbooffsets[robboframe * 2] * video.field_size / 16,
		      robbooffsets[robboframe * 2 +
				   1] * video.field_size / 16,
		      video.field_size, video.field_size);
	  destrect =
	    set_rect (xoffset + 2 * video.field_size / 16,
		      yoffset + 57 * video.field_size / 16 + 6 * yincrement,
		      video.field_size, video.field_size);
	  SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, tilecolour >> 16 & 0xff,
				    tilecolour >> 8 & 0xff,
				    tilecolour & 0xff));
	  SDL_BlitSurface (icons, &srcrect, screen, &destrect);
	  /* Update the Robbo frame */
	  if (--robbostate <= 0)
	    {
	      robbostate = DELAY_ROBBO;
	      if (++robboframe > 1)
		robboframe = 0;
	    }
	}

      /* Animate the Help banner */
      /* Draw the horizontal line under the banner */
      destrect =
	set_rect (xoffset + 0, yoffset + 43 * video.field_size / 16,
		  240 * video.field_size / 16, video.field_size / 8);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Draw the Help banner */
      if (video.field_size == 16)
	{
	  show_banner (helpscreenbanner,
		       xoffset + video.field_size * 15 / 2 - 33 * 3 / 2,
		       yoffset + 6, 33, 10);
	}
      else if (video.field_size == 32)
	{
	  show_banner (helpscreenbanner,
		       xoffset + video.field_size * 15 / 2 - 33 * 5 / 2,
		       yoffset + 19, 33, 10);
	}
      /* Update the banner colour frame */
      if (--bannerstate <= 0)
	{
	  bannerstate = DELAY_BANNER;
	  if (++bannerframe >= BANNER_COLOURS)
	    bannerframe = 0;
	}
    }

  help_screen.redraw = 0;
}

/***************************************************************************
 * Show Options Screen                                                     *
 ***************************************************************************/
/* Note that this function was becoming very large and so I've dispensed
 * with many of the if-then-else statements and simply started with a default
 * and then overridden it. Also I've placed certain related things together on
 * the same line and standardised everything to attempt to find patterns that
 * can be procedurised
 * 
 * options_screen.redraw now specifies how much is redrawn */

void show_optionsscreen (void)
{
	SDL_Color menu_text_colour, menu_greyed_text_colour;
	SDL_Color menu_selected_text_colour, menu_selected_limit_text_colour;
	SDL_Color fgcolour, bgcolour, general_text_colour;
	int xoffset, yoffset, count, optionid, action;
	SDL_Surface *icons_preview;
	static int lastpage = 0;
	ROB_Object rob_object;
	char tempstring[256];
	SDL_Rect destrect;

	xoffset = (screen->w - video.field_size * 15) / 2 + video.xshift;
	yoffset = (screen->h - video.field_size * 15) / 2 + video.yshift;

	bgcolour.r = skins[selected_skin].background_colour >> 16 & 0xff;
	bgcolour.g = skins[selected_skin].background_colour >> 8 & 0xff;
	bgcolour.b = skins[selected_skin].background_colour & 0xff;
	general_text_colour.r = skins[selected_skin].general_text_colour >> 16 & 0xff;
	general_text_colour.g =	skins[selected_skin].general_text_colour >> 8 & 0xff;
	general_text_colour.b = skins[selected_skin].general_text_colour & 0xff;
	menu_text_colour.r = skins[selected_skin].menu_text_colour >> 16 & 0xff;
	menu_text_colour.g = skins[selected_skin].menu_text_colour >> 8 & 0xff;
	menu_text_colour.b = skins[selected_skin].menu_text_colour & 0xff;
	menu_greyed_text_colour.r =
		skins[selected_skin].menu_greyed_text_colour >> 16 & 0xff;
	menu_greyed_text_colour.g =
		skins[selected_skin].menu_greyed_text_colour >> 8 & 0xff;
	menu_greyed_text_colour.b =
		skins[selected_skin].menu_greyed_text_colour & 0xff;
	menu_selected_text_colour.r =
		skins[selected_skin].menu_selected_text_colour >> 16 & 0xff;
	menu_selected_text_colour.g =
		skins[selected_skin].menu_selected_text_colour >> 8 & 0xff;
	menu_selected_text_colour.b =
		skins[selected_skin].menu_selected_text_colour & 0xff;
	menu_selected_limit_text_colour.r =
		skins[selected_skin].menu_selected_limit_text_colour >> 16 & 0xff;
	menu_selected_limit_text_colour.g =
		skins[selected_skin].menu_selected_limit_text_colour >> 8 & 0xff;
	menu_selected_limit_text_colour.b =
		skins[selected_skin].menu_selected_limit_text_colour & 0xff;

	if (options_screen.redraw >= REDRAW_INITIALISE)
	{
		/* Reinitialise some variables */
		temp_game_cycle_limit = game_cycle_limit;
		temp_key_repeat_delay = key_repeat_delay;
		temp_key_repeat_interval = key_repeat_interval;
		temp_default_joystick = default_joystick;
		temp_joystick_dead_zone = joystick_dead_zone;
		temp_selected_locale = selected_locale;
		temp_rcfile_save_frequency = rcfile.save_frequency;
		temp_gnurobbo_op_env = gnurobbo_op_env;
		temp_pointer_controls = pointer_controls;
		temp_game_sound = sound;
		temp_sfx_vol = sfx_vol;
		temp_selected_skin = selected_skin;
		for (count = 0; count < USER_CONTROLS; count++)
		{
			temp_user_controls[count].device = user_controls[count].device;
			temp_user_controls[count].id = user_controls[count].id;
			temp_user_controls[count].mod = user_controls[count].mod;
		}

		/* Restore normal key/button repeat values for these keys */
		user_controls[ACTION_UP].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_UP].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_UP_RIGHT].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_UP_RIGHT].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_RIGHT].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_RIGHT].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_DOWN_RIGHT].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_DOWN_RIGHT].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_DOWN].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_DOWN].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_DOWN_LEFT].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_DOWN_LEFT].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_LEFT].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_LEFT].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_UP_LEFT].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_UP_LEFT].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_SHOOT_UP].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_SHOOT_UP].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_SHOOT_DOWN].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_SHOOT_DOWN].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_SHOOT_LEFT].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_SHOOT_LEFT].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_SHOOT_RIGHT].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_SHOOT_RIGHT].interval = KEY_REPEAT_INTERVAL;
		user_controls[ACTION_PRIMARY_CLICK].delay = KEY_REPEAT_DELAY;
		user_controls[ACTION_PRIMARY_CLICK].interval = KEY_REPEAT_INTERVAL;

		/* Initialise the options array; items can be enabled/disabled
		 * here at runtime. Set everything to TRUE and then disable the few */
		for (count = 0; count < OPTIONS_COUNT; count++)
		{
			options[count] = TRUE;
		}
		options[0 * 13 + OPTIONS_MENUPOS_LEFT] = FALSE;
		options[OPTIONS_LANGUAGE + 1] = FALSE;
		options[0 * 13 + 2] = FALSE;	/* Not currently used */
		options[0 * 13 + 5] = FALSE;	/* Not currently used */
		options[0 * 13 + 8] = FALSE;	/* Not currently used */
		for (count = 1; count < OPTIONS_MENUPOS_LEFT; count++)
		{
			options[OPTIONS_SKIN + count] = FALSE;
		}
		#ifndef PLATFORM_PSP
		#ifndef PLATFORM_PSVITA
			if (joystick_count == 0)
		#endif
		#endif
		{
			options[OPTIONS_DEFAULT_JOYSTICK] = FALSE;
			options[OPTIONS_JOYSTICK_AXES_DEAD_ZONE] = FALSE;
		}
		options[OPTIONS_KEY_REPEAT_DELAY - 1] = FALSE;
		options[2 * 13 + 7] = FALSE;	/* Not currently used */
		for (count = 0; count < OPTIONS_MENUPOS_LEFT; count++)
		{
			options[OPTIONS_RECONFIGURATION_HELP + count] = FALSE;
		}
		options[(OPTIONS_SCREEN_PAGES - 1) * 13 + OPTIONS_MENUPOS_RIGHT] = FALSE;

		/* Disable Toggle Fullscreen on platforms that don't support this */
		#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)
		#elif defined(PLATFORM_GP2X)  || defined(PLATFORM_CAANOO)
			options[OPTIONS_ACTION_TOGGLE_FULLSCREEN] = FALSE;
		#elif defined(PLATFORM_ZAURUS)
			options[OPTIONS_ACTION_TOGGLE_FULLSCREEN] = FALSE;
		#elif defined(PLATFORM_PSP)
			options[OPTIONS_ACTION_TOGGLE_FULLSCREEN] = FALSE;
		#elif defined(PLATFORM_PSVITA)
			options[OPTIONS_ACTION_TOGGLE_FULLSCREEN] = FALSE;
		#endif

		/* Disable pointer options on platforms that don't support the mouse */
		#if defined(PLATFORM_WIN32) || defined(PLATFORM_PC)  || defined(PLATFORM_CAANOO)
		#elif defined(PLATFORM_GP2X) 
			#ifndef HAVE_DESIGNER
				/* F100 doesn't support the mouse, F200 does (or can) so
				* what happens here depends on HAVE_DESIGNER in the Makefile */
				options[OPTIONS_ACTION_SCROLL_UP] = FALSE;
				options[OPTIONS_ACTION_SCROLL_DOWN] = FALSE;
				options[OPTIONS_ACTION_PRIMARY_CLICK] = FALSE;
				options[OPTIONS_SYSTEM_POINTER] = FALSE;
				options[OPTIONS_POINTER_CONTROLS_PAD_TYPE] = FALSE;
			#endif
		#elif defined(PLATFORM_ZAURUS)
		#elif defined(PLATFORM_PSP)
			options[OPTIONS_ACTION_SCROLL_UP] = FALSE;
			options[OPTIONS_ACTION_SCROLL_DOWN] = FALSE;
			options[OPTIONS_ACTION_PRIMARY_CLICK] = FALSE;
			options[OPTIONS_SYSTEM_POINTER] = FALSE;
			options[OPTIONS_POINTER_CONTROLS_PAD_TYPE] = FALSE;
		#elif defined(PLATFORM_PSVITA)
			options[OPTIONS_ACTION_SCROLL_UP] = FALSE;
			options[OPTIONS_ACTION_SCROLL_DOWN] = FALSE;
			options[OPTIONS_ACTION_PRIMARY_CLICK] = FALSE;
			options[OPTIONS_SYSTEM_POINTER] = FALSE;
			options[OPTIONS_POINTER_CONTROLS_PAD_TYPE] = FALSE;
		#endif

		/* Disable Toggle Designer if not enabled in the Makefile */
		#ifndef HAVE_DESIGNER
			options[OPTIONS_ACTION_TOGGLE_DESIGNER] = FALSE;
		#endif

		/* Disable the diagonal controls for now as they're not currently
		* implemented and possibly they never will be (they were going
		* to be used by the ROB engine in the designer) */
		options[4 * 13 + 1] = FALSE;
		options[4 * 13 + 3] = FALSE;
		options[4 * 13 + 5] = FALSE;
		options[4 * 13 + 7] = FALSE;

		/* If the currently selected item on any page is disabled
		 * then advance it until an enabled one is found. This will be
		 * the case only from the outset when each page's selected
		 * item is set to 0 and this code helps to make the Options
		 * behemoth more self configuring */
		for (count = 0; count < OPTIONS_SCREEN_PAGES; count++)
		{
			while (!options[count * 13 + optionspageselecteditem[count]])
			{
				optionspageselecteditem[count]++;
				if (optionspageselecteditem[count] >= OPTIONS_MENUPOS_LEFT)
					optionspageselecteditem[count] = OPTIONS_MENUPOS_EXIT;
			}
		}

		/* Free all existing hotspots including the pob */
		ROB_FreeAllDescendantObjects (ROB_GetObject (HOTSPOTS_ID), TRUE);
		/* Hide the pointer controls */
		hide_pointer_controls ();
		/* Point the event processor to manage Options Screen events */
		gnurobbo_op_env.event_processor = &options_screen_event_processor;
		if (ROB_SetOpEnvEventProcessor (gnurobbo_op_env.event_processor))
			printf ("%s: %s\n", __func__, ROB_GetError ());
		/* Create the hotspot parent object which will be used to identify the group */
		if (ROB_SetObjectDefaults (&rob_object))
			printf ("%s: %s\n", __func__, ROB_GetError ());
		rob_object.id = HOTSPOTS_ID;
		if (ROB_CreateObject (&rob_object))
			printf ("%s: %s\n", __func__, ROB_GetError ());
	}

	#ifdef DEBUG_ROB_HOTSPOTS
		options_screen.redraw |= REDRAW_EVERYTHING;
	#endif

	if (options_screen.redraw >= REDRAW_EVERYTHING)
	{
		/* Monitor page changes and free any existing hotspots */
		if (lastpage != optionspage)
		{
			/* Free the existing hotspots from the last page */
			ROB_FreeAllDescendantObjects (ROB_GetObject (HOTSPOTS_ID), FALSE);
		}
		lastpage = optionspage;
		/* Set entire screen to a colour */
		clear_screen ();
	}

	if (options_screen.redraw >= REDRAW_INTERMEDIATE)
	{
		for (optionid = optionspage * 13; optionid < (optionspage + 1)
			* 13; optionid++)
		{
			if (optionid % 13 == OPTIONS_MENUPOS_LEFT)
			{
			if (options[optionid])
			{
			/* < Back */
			fgcolour = menu_text_colour;
			if (optionid % 13 ==
			optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			sprintf (tempstring, "< %s", txt_Back);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 220 * video.field_size / 16,
			image->w, image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			}
			else if (optionid % 13 == OPTIONS_MENUPOS_SAVE)
			{
			/* Save */
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Save, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 90 * video.field_size / 16 -
			image->w / 2,
			yoffset + 220 * video.field_size / 16, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid % 13 == OPTIONS_MENUPOS_EXIT)
			{
			/* Exit */
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Exit, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 150 * video.field_size / 16 -
			image->w / 2,
			yoffset + 220 * video.field_size / 16, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid % 13 == OPTIONS_MENUPOS_RIGHT)
			{
			if (options[optionid])
			{
			/* Page > */
			fgcolour = menu_text_colour;
			if (optionid % 13 ==
			optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			sprintf (tempstring, "%s >", txt_Next);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 238 * video.field_size / 16 -
			image->w,
			yoffset + 220 * video.field_size / 16,
			image->w, image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			}
			else if (optionid == OPTIONS_GAME_SPEED)
			{
			/***************************/
			/* Game Speed              */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Game_Speed, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			/* As this is the first hotspot being registered here, and in
			 * fact the first one I coded I will explain how they work :-
			 * The optionid (as listed in game.h) is added to the parentid
			 * + 1, and so hotspotid - 1 - parentid returns the optionid.
			 * Because the option text is just one part of the option, we
			 * need to create ids for the "<" and ">" parts. I decided on
			 * using "<" = optionid + OPTIONS_COUNT and ">" = optionid +
			 * OPTIONS_COUNT * 2 because the three parts can be easily
			 * identified with "n / OPTIONS_COUNT" and "n % OPTIONS_COUNT".
			 * 
			 * When the user clicks these hotspots, the ROB engine will
			 * send events to the event processor which in this case is
			 * options_screen_event_processor() where they will be
			 * processed and the relevant code executed. I recommend
			 * enabling DEBUG_ROB_HOTSPOTS to see this in action :) */
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* +< */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_game_cycle_limit < GAME_CYCLE_LIMIT_MAX)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " +< ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* n */
			fgcolour = general_text_colour;
			sprintf (tempstring, "%iHz", temp_game_cycle_limit);
			if (temp_game_cycle_limit == 33)
			sprintf (tempstring, "%s", txt_Fast);
			if (temp_game_cycle_limit == 25)
			sprintf (tempstring, "%s", txt_Normal);
			if (temp_game_cycle_limit == 20)
			sprintf (tempstring, "%s", txt_Slow);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* >- */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_game_cycle_limit > GAME_CYCLE_LIMIT_MIN)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " >-       ", fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid == OPTIONS_DEFAULT_JOYSTICK)
			{
			/***************************/
			/* Default Joystick        */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Default_Joystick,
			fgcolour, bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* < */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_default_joystick > 0)
			fgcolour = menu_selected_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " < ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* Joystick name */
			fgcolour = general_text_colour;
			if (joystick_count == 0)
			{
			strcpy (tempstring, txt_No_joystick_found);
			}
			else if (strlen (joystick_list[temp_default_joystick]) <=
			28)
			{
			sprintf (tempstring, "%i:", temp_default_joystick);
			strcat (tempstring,
			joystick_list[temp_default_joystick]);
			}
			else
			{
			sprintf (tempstring, "%i:", temp_default_joystick);
			strncat (tempstring,
			joystick_list[temp_default_joystick], 28 - 3);
			/* Force string null termination in case max chars were copied */
			tempstring[28 + 2 - 4] = 0;
			strcat (tempstring, "...");
			}
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			/* Does the area require erasing first? */
			if (options_screen.redraw <= REDRAW_INTERMEDIATE)
			{
			destrect.w = video.field_size * 10;
			destrect.h = video.field_size;
			SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, bgcolour.r,
			bgcolour.g, bgcolour.b));
			}
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* > */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_default_joystick < joystick_count - 1)
			fgcolour = menu_selected_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " > ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid == OPTIONS_JOYSTICK_AXES_DEAD_ZONE)
			{
			/***************************/
			/* Joystick Axes Dead Zone */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Joystick_Axes_Dead_Zone,
			fgcolour, bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* < */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_joystick_dead_zone > 1)
			fgcolour = menu_selected_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " < ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* n% */
			fgcolour = general_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			sprintf (tempstring, "%i%%", temp_joystick_dead_zone);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* > */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_joystick_dead_zone < 99)
			fgcolour = menu_selected_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " >  ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid == OPTIONS_KEY_REPEAT_DELAY)
			{
			/***************************/
			/* Key Repeat:             */
			/*   Delay                 */
			/***************************/
			fgcolour = menu_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Key_Repeat, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13 - 1) * video.field_size,
			image->w, image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* Delay */
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Delay, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 12 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* < */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_key_repeat_delay > KEY_REPEAT_MIN)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " +< ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* n */
			fgcolour = general_text_colour;
			sprintf (tempstring, "%ims", temp_key_repeat_delay);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* > */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_key_repeat_delay < KEY_REPEAT_MAX)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " >-      ", fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid == OPTIONS_KEY_REPEAT_INTERVAL)
			{
			/***************************/
			/* Key Repeat:             */
			/*   Interval              */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Interval, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 12 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* < */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_key_repeat_interval > KEY_REPEAT_MIN)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " +< ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* n */
			fgcolour = general_text_colour;
			sprintf (tempstring, "%ims", temp_key_repeat_interval);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* > */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_key_repeat_interval < KEY_REPEAT_MAX)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " >-      ", fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid == OPTIONS_LANGUAGE)
			{
			/***************************/
			/* Language                */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Language, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* < */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_selected_locale > 0)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " < ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* Locale name */
			fgcolour = general_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font,
			locales[temp_selected_locale].name,
			fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* > */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_selected_locale < locale_count - 1)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " >        ", fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* By */
			fgcolour = menu_text_colour;
			sprintf (tempstring, "%s ", txt_Translation_by);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13 + 1) * video.field_size,
			image->w, image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* Locale author */
			fgcolour = general_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font,
			locales[temp_selected_locale].
			author, fgcolour, bgcolour);
			/* Does the area require erasing first? */
			if (options_screen.redraw <= REDRAW_INTERMEDIATE)
			{
			destrect.w = video.field_size * 12;
			destrect.h = video.field_size;
			SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, bgcolour.r,
			bgcolour.g, bgcolour.b));
			}
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid == OPTIONS_SAVE_FREQUENCY)
			{
			/***************************/
			/* Save Frequency          */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Save_Frequency, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* < */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_rcfile_save_frequency > 0)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " < ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* On Exit / On Change */
			fgcolour = general_text_colour;
			if (temp_rcfile_save_frequency == RCFILE_SAVE_ON_EXIT)
			sprintf (tempstring, "%s", txt_On_Exit);
			if (temp_rcfile_save_frequency == RCFILE_SAVE_ON_CHANGE)
			sprintf (tempstring, "%s", txt_On_Change);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* > */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_rcfile_save_frequency < 1)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " >        ", fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid == OPTIONS_RECONFIGURATION_HELP)
			{
			/********************************/
			/* Control Reconfiguration Help */
			/********************************/
			fgcolour = general_text_colour;
			for (count = 0; count < TXT_OPTIONS_PAGE1_ROWS; count++)
			{
			image =
			TTF_RenderUTF8_Shaded (font,
			&txt_options_page1[count *
			TXT_MAX_COLS],
			fgcolour, bgcolour);
			SDL_SetColorKey (image, SDL_SRCCOLORKEY,
			SDL_MapRGB (image->format, bgcolour.r,
			bgcolour.g, bgcolour.b));
			destrect.x = xoffset + 2 * video.field_size / 16;
			destrect.y =
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size;
			if (video.field_size == 16)
			destrect.y +=
			count * (FONTSIZE16 + video.field_size / 8);
			if (video.field_size == 32)
			destrect.y +=
			count * (FONTSIZE32 + video.field_size / 8);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			}
			else if (optionid == OPTIONS_RESTORE_DEFAULT_CONTROLS)
			{
			/****************************/
			/* Restore Default Controls */
			/****************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Restore_Default_Controls,
			fgcolour, bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid == OPTIONS_SYSTEM_POINTER)
			{
			/***************************/
			/* System Pointer          */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_System_Pointer, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* < */
			fgcolour = menu_selected_limit_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			if (temp_gnurobbo_op_env.systempointer > 0)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " < ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* Disabled / Enabled */
			fgcolour = general_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			if (temp_gnurobbo_op_env.systempointer == FALSE)
			sprintf (tempstring, "%s", txt_Disabled);
			if (temp_gnurobbo_op_env.systempointer == TRUE)
			sprintf (tempstring, "%s", txt_Enabled);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* > */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_gnurobbo_op_env.systempointer < TRUE)
			fgcolour = menu_selected_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " >   ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid == OPTIONS_POINTER_CONTROLS_PAD_TYPE)
			{
			/*****************************/
			/* Pointer Controls Pad Type */
			/*****************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			if (!options[optionid]) fgcolour = menu_greyed_text_colour;
			image = TTF_RenderUTF8_Shaded (font, txt_Pointer_Controls_Pad_Type,
			fgcolour, bgcolour);
			destrect = set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 + (optionid % 13) *
			video.field_size, image->w, image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* < */
			fgcolour = menu_selected_limit_text_colour;
			if (!options[optionid]) fgcolour = menu_greyed_text_colour;
			if (temp_pointer_controls.pad_type >
			POINTER_CONTROLS_PAD_TYPE_VIEWPORT)
			fgcolour = menu_selected_text_colour;
			image = TTF_RenderUTF8_Shaded (font, " < ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* Viewport / Screen */
			fgcolour = general_text_colour;
			if (!options[optionid]) fgcolour = menu_greyed_text_colour;
			if (temp_pointer_controls.pad_type ==
			POINTER_CONTROLS_PAD_TYPE_VIEWPORT)
			sprintf (tempstring, "%s", txt_Viewport);
			if (temp_pointer_controls.pad_type ==
			POINTER_CONTROLS_PAD_TYPE_SCREEN)
			sprintf (tempstring, "%s", txt_Screen);
			image = TTF_RenderUTF8_Shaded (font, tempstring, fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* > */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_pointer_controls.pad_type <
			POINTER_CONTROLS_PAD_TYPE_SCREEN)
			fgcolour = menu_selected_text_colour;
			if (!options[optionid]) fgcolour = menu_greyed_text_colour;
			image = TTF_RenderUTF8_Shaded (font, " >      ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			else if (optionid == OPTIONS_SOUND)
			{
			/***************************/
			/* Enable SOUND option */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			sprintf (tempstring, "%s", txt_Sound);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			fgcolour = menu_selected_limit_text_colour;
			if (temp_game_sound > 0)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " < ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			fgcolour = general_text_colour;
			if (temp_game_sound == 0)
			sprintf (tempstring, "%s",  txt_Disabled);
			if (temp_game_sound == 1)
			sprintf (tempstring, "%s",  txt_Enabled);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* >- */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_game_sound < 1)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " >   ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);

			}
			else if (optionid == OPTIONS_SFX_VOL)
			{
			/***************************/
			/* Sound FX volume option */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			sprintf (tempstring, "%s", txt_Sfx_Volume);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			fgcolour = menu_selected_limit_text_colour;
			if (temp_sfx_vol > 0)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " < ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			fgcolour = general_text_colour;
			sprintf (tempstring, "%d", temp_sfx_vol);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			fgcolour = menu_selected_limit_text_colour;
			if (temp_sfx_vol < 128)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " >    ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);

			}
			else if (optionid >= OPTIONS_ACTION_UP
			&& optionid <= OPTIONS_ACTION_PRIMARY_CLICK)
			{
			/***************************/
			/* Action                  */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			
			action = optionid - OPTIONS_ACTION_UP - ((optionid - OPTIONS_ACTION_UP) / 13) * 4;
			if (action == ACTION_UP)
			{
			sprintf (tempstring, "%s ", txt_Move_Up);
			}
			else if (action == ACTION_UP_RIGHT)
			{
			sprintf (tempstring, "%s ", txt_Move_Up_Right);
			}
			else if (action == ACTION_RIGHT)
			{
			sprintf (tempstring, "%s ", txt_Move_Right);
			}
			else if (action == ACTION_DOWN_RIGHT)
			{
			sprintf (tempstring, "%s ", txt_Move_Down_Right);
			}
			else if (action == ACTION_DOWN)
			{
			sprintf (tempstring, "%s ", txt_Move_Down);
			}
			else if (action == ACTION_DOWN_LEFT)
			{
			sprintf (tempstring, "%s ", txt_Move_Down_Left);
			}
			else if (action == ACTION_LEFT)
			{
			sprintf (tempstring, "%s ", txt_Move_Left);
			}
			else if (action == ACTION_UP_LEFT)
			{
			sprintf (tempstring, "%s ", txt_Move_Up_Left);
			}
			else if (action == ACTION_RESTART)
			{
			sprintf (tempstring, "%s ", txt_Restart);
			}
			else if (action == ACTION_SHOOT_UP)
			{
			sprintf (tempstring, "%s ", txt_Shoot_Up);
			}
			else if (action == ACTION_SHOOT_DOWN)
			{
			sprintf (tempstring, "%s ", txt_Shoot_Down);
			}
			else if (action == ACTION_SHOOT_LEFT)
			{
			sprintf (tempstring, "%s ", txt_Shoot_Left);
			}
			else if (action == ACTION_SHOOT_RIGHT)
			{
			sprintf (tempstring, "%s ", txt_Shoot_Right);
			}
			else if (action == ACTION_SELECT)
			{
			sprintf (tempstring, "%s ", txt_Confirm);
			}
			else if (action == ACTION_EXIT)
			{
			sprintf (tempstring, "%s ", txt_Exit);
			}
			else if (action == ACTION_HELP)
			{
			sprintf (tempstring, "%s ", txt_Help);
			}
			else if (action == ACTION_OPTIONS)
			{
			sprintf (tempstring, "%s ", txt_Options);
			}
			else if (action == ACTION_TOGGLE_FULLSCREEN)
			{
			sprintf (tempstring, "%s ", txt_Toggle_Fullscreen);
			}
			else if (action == ACTION_PREVIOUS_LEVEL)
			{
			sprintf (tempstring, "%s ", txt_Previous_Level);
			}
			else if (action == ACTION_NEXT_LEVEL)
			{
			sprintf (tempstring, "%s ", txt_Next_Level);
			}
			else if (action == ACTION_PREVIOUS_PACK)
			{
			sprintf (tempstring, "%s ", txt_Previous_Pack);
			}
			else if (action == ACTION_NEXT_PACK)
			{
			sprintf (tempstring, "%s ", txt_Next_Pack);
			}
			else if (action == ACTION_HOME)
			{
			sprintf (tempstring, "%s ", txt_Home);
			}
			else if (action == ACTION_END)
			{
			sprintf (tempstring, "%s ", txt_End);
			}
			else if (action == ACTION_PAGEUP)
			{
			sprintf (tempstring, "%s ", txt_Page_Up);
			}
			else if (action == ACTION_PAGEDOWN)
			{
			sprintf (tempstring, "%s ", txt_Page_Down);
			}
			else if (action == ACTION_TOGGLE_DESIGNER)
			{
			sprintf (tempstring, "%s ", txt_Toggle_Designer);
			}
			else if (action == ACTION_VOLUP)
			{
			sprintf (tempstring, "%s ", txt_Volume_Up);
			}
			else if (action == ACTION_VOLDOWN)
			{
			sprintf (tempstring, "%s ", txt_Volume_Down);
			}
			else if (action >= ACTION_MODIFIER1
			&& action <= ACTION_MODIFIER4)
			{
			sprintf (tempstring, "%s%i ", txt_Modifier,
			action - ACTION_MODIFIER1 + 1);
			}
			else if (action == ACTION_SCROLL_UP)
			{
			sprintf (tempstring, "%s ", txt_Scroll_Up);
			}
			else if (action == ACTION_SCROLL_DOWN)
			{
			sprintf (tempstring, "%s ", txt_Scroll_Down);
			}
			else if (action == ACTION_PRIMARY_CLICK)
			{
			sprintf (tempstring, "%s ", txt_Primary_Click);
			}
			else
			{
			sprintf (tempstring, "%s %i", "Unknown", action);
			}
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* device */
			fgcolour = general_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			destrect.x = xoffset + 110 * video.field_size / 16;
			if (temp_user_controls[action].device == UNDEFINED)
			sprintf (tempstring, "%s", "-");
			if (temp_user_controls[action].device == DEVICE_KEYBOARD)
			sprintf (tempstring, "%s: ", txt_Key);
			if (temp_user_controls[action].device == DEVICE_JOYSTICK)
			sprintf (tempstring, "%s: ", txt_Joy);
			if (temp_user_controls[action].device == DEVICE_MOUSE)
			sprintf (tempstring, "%s: ", txt_Ptr);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			/* Does the area require erasing first? */
			if (options_screen.redraw <= REDRAW_INTERMEDIATE)
			{
			destrect.w = 130 * video.field_size / 16;
			destrect.h = image->h;
			SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, bgcolour.r,
			bgcolour.g, bgcolour.b));
			}
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* id */
			if (temp_user_controls[action].id == UNDEFINED)
			{
			sprintf (tempstring, "%s ", "-");
			}
			else
			{
			if (joystick_count > 0)
			{
			get_input_device_text (temp_user_controls[action].device,
			temp_user_controls[action].id,
			joystick_list
			[temp_default_joystick],
			tempstring);
			}
			else
			{
			get_input_device_text (temp_user_controls[action].device,
			temp_user_controls[action].id, "",
			tempstring);
			}
			sprintf (tempstring, "%s ", tempstring);
			}
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* < */
			if (action < ACTION_MODIFIER1 || action > ACTION_MODIFIER4)
			{
			destrect.x = xoffset + 190 * video.field_size / 16;
			fgcolour = menu_selected_limit_text_colour;
			if (temp_user_controls[action].mod != UNDEFINED)
			fgcolour = menu_selected_text_colour;
			if (temp_user_controls[action].device == UNDEFINED)
			fgcolour = menu_greyed_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, "< ", fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* mod */
			fgcolour = general_text_colour;
			if (temp_user_controls[action].device == UNDEFINED)
			fgcolour = menu_greyed_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			if (temp_user_controls[action].mod == UNDEFINED)
			{
			sprintf (tempstring, "%s", "--");
			}
			else
			{
			sprintf (tempstring, "%s%i", txt_Mod,
			temp_user_controls[action].mod -
			ACTION_MODIFIER1 + 1);
			}
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* > */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_user_controls[action].mod < ACTION_MODIFIER4)
			fgcolour = menu_selected_text_colour;
			if (temp_user_controls[action].device == UNDEFINED)
			fgcolour = menu_greyed_text_colour;
			if (!options[optionid])
			fgcolour = menu_greyed_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " > ", fgcolour,
			bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			}
			}
			else if (optionid == OPTIONS_SKIN)
			{
			/***************************/
			/* Skin                    */
			/***************************/
			fgcolour = menu_text_colour;
			if (optionid % 13 == optionspageselecteditem[optionspage])
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, txt_Skin, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13) * video.field_size, image->w,
			image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_OPT;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* < */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_selected_skin > 0)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " < ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_DEC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* Skin name */
			fgcolour = general_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font,
			skins[temp_selected_skin].name,
			fgcolour, bgcolour);
			/* Does the area require erasing first? */
			if (options_screen.redraw <= REDRAW_INTERMEDIATE)
			{
			destrect.w = video.field_size * 13;
			destrect.h = video.field_size;
			SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, bgcolour.r,
			bgcolour.g, bgcolour.b));
			}
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* > */
			fgcolour = menu_selected_limit_text_colour;
			if (temp_selected_skin < skin_count - 1)
			fgcolour = menu_selected_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font, " > ", fgcolour, bgcolour);
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			REGISTER_HOTSPOT_OPTIONS_SCREEN_INC;
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* By */
			fgcolour = menu_text_colour;
			sprintf (tempstring, "%s ", txt_By);
			image =
			TTF_RenderUTF8_Shaded (font, tempstring, fgcolour,
			bgcolour);
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13 + 1) * video.field_size,
			image->w, image->h);
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* Skin author */
			fgcolour = general_text_colour;
			image =
			TTF_RenderUTF8_Shaded (font,
			skins[temp_selected_skin].author,
			fgcolour, bgcolour);
			/* Does the area require erasing first? */
			if (options_screen.redraw <= REDRAW_INTERMEDIATE)
			{
			destrect.w = video.field_size * 14;
			destrect.h = video.field_size;
			SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, bgcolour.r,
			bgcolour.g, bgcolour.b));
			}
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			destrect.x += image->w;
			SDL_FreeSurface (image);
			/* Skin about */
			fgcolour = general_text_colour;
			destrect =
			set_rect (xoffset + 2 * video.field_size / 16,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13 + 2) * video.field_size,
			240 * video.field_size / 16,
			3 * video.field_size);
			/* Does the area require erasing first? */
			if (options_screen.redraw <= REDRAW_INTERMEDIATE)
			{
			SDL_FillRect (screen, &destrect,
			SDL_MapRGB (screen->format, bgcolour.r,
			bgcolour.g, bgcolour.b));
			}
			for (count = 0; count < 3; count++)
			{
			image =
			TTF_RenderUTF8_Shaded (font,
			skins[temp_selected_skin].
			about[count], fgcolour,
			bgcolour);
			SDL_SetColorKey (image, SDL_SRCCOLORKEY,
			SDL_MapRGB (image->format, bgcolour.r,
			bgcolour.g, bgcolour.b));
			destrect.w = image->w;
			destrect.h = image->h;
			SDL_BlitSurface (image, NULL, screen, &destrect);
			SDL_FreeSurface (image);
			if (video.field_size == 16)
			destrect.y += FONTSIZE16 + video.field_size / 8;
			if (video.field_size == 32)
			destrect.y += FONTSIZE32 + video.field_size / 8;
			}
			/* Draw the icons preview image */
			icons_preview = create_skin_preview ();
			destrect =
			set_rect (xoffset + video.field_size * 15 / 2 -
			icons_preview->w / 2,
			yoffset + 57 * video.field_size / 16 +
			(optionid % 13 + 5) * video.field_size,
			icons_preview->w, icons_preview->h);
			SDL_BlitSurface (icons_preview, NULL, screen, &destrect);
			SDL_FreeSurface (icons_preview);
			}
		}
	}

	if (options_screen.redraw >= REDRAW_ANIMATED)
	{
		/* Animate the Options banner */
		/* Draw the horizontal line under the banner */
		destrect = set_rect (xoffset + 0, yoffset + 43 *
			video.field_size / 16, 240 * video.field_size / 16,
			video.field_size / 8);
		SDL_FillRect (screen, &destrect, SDL_MapRGB (screen->format,
			bannercolour[bannerframe] >> 16 & 0xff,
			bannercolour[bannerframe] >> 8 & 0xff,
			bannercolour[bannerframe] & 0xff));
		/* Draw the Options banner */
		if (video.field_size == 16)
		{
			show_banner (optionsscreenbanner,
				xoffset + video.field_size * 15 / 2 - 60 * 3 / 2,
				yoffset + 6, 60, 10);
		}
		else if (video.field_size == 32)
		{
			show_banner (optionsscreenbanner,
				xoffset + video.field_size * 15 / 2 - 60 * 5 / 2,
				yoffset + 19, 60, 10);
		}
		/* Update the banner colour frame */
		if (--bannerstate <= 0)
		{
			bannerstate = DELAY_BANNER;
			if (++bannerframe >= BANNER_COLOURS) bannerframe = 0;
		}
	}

	options_screen.redraw = 0;
}

/***************************************************************************
 * Show Message Box                                                        *
 ***************************************************************************/
 /* This displays one very simple msgbox over the screen at once.
    Drawing a new msgbox will automatically kill any existing one.

    Examples
    --------
    To initialise :-
    strcpy(temp_msg_box.name, "id101");
    strcpy(temp_msg_box.message, "my message");
    temp_msg_box.timeout = 33;
    temp_msg_box.dynamic = FALSE;
    temp_msg_box.w = 6 * video.field_size;
    temp_msg_box.h = 2 * video.field_size;
    show_message_box(MESSAGE_BOX_SUB_INITIALISE, &temp_msg_box);
    To redraw after you've rebuilt your screen :-
    show_message_box(MESSAGE_BOX_SUB_SHOW, NULL);
    To kill an existing box :-
    show_message_box(MESSAGE_BOX_SUB_KILL, NULL); */

void
show_message_box (int subfunction, MSG_Box * msg_box)
{
  static int bannerstate = 0, bannerframe = 0;
  static MSG_Box the_box;
  SDL_Rect destrect;

  if (subfunction == MESSAGE_BOX_SUB_INITIALISE && msg_box != NULL)
    {

      /* If there's currently an existing msgbox active then kill it first */
      if (the_box.timeout > 0)
	show_message_box (MESSAGE_BOX_SUB_KILL, NULL);

      /* Create a local copy of the passed msgbox */
      the_box = *msg_box;
      if (strlen (the_box.message) == 0)
	strcpy (the_box.message, " ");	/* This stops SDL_ttf from seg faulting on NULL strings >( */
      /* Render the text so we can get its size in image->w and image->h */
      bgcolor.r = skins[selected_skin].background_colour >> 16 & 0xff;
      bgcolor.g = skins[selected_skin].background_colour >> 8 & 0xff;
      bgcolor.b = skins[selected_skin].background_colour & 0xff;
      fgcolor.r = skins[selected_skin].general_text_colour >> 16 & 0xff;
      fgcolor.g = skins[selected_skin].general_text_colour >> 8 & 0xff;
      fgcolor.b = skins[selected_skin].general_text_colour & 0xff;
      image = TTF_RenderUTF8_Shaded (font, the_box.message, fgcolor, bgcolor);
      /* Calculate some dimensions */
      if (the_box.dynamic)
	{
	  the_box.w = image->w + video.field_size;
	  the_box.h = image->h + video.field_size;
	}
      SDL_FreeSurface (image);
      the_box.x = (screen->w - the_box.w) / 2;
      the_box.y = (screen->h - the_box.h) / 2;
      the_box.bartopleftx = the_box.x + video.field_size / 8;
      the_box.bartoplefty = the_box.y + video.field_size / 8;
      the_box.barbottomrightx = the_box.x + the_box.w - video.field_size / 4;
      the_box.barbottomrighty = the_box.y + the_box.h - video.field_size / 4;
#ifdef DEBUG_MESSAGE_BOX
      printf ("*** Start %s ***\n", __func__);
      printf ("the_box.w=%i the_box.h=%i\n", the_box.w, the_box.h);
      printf ("*** Stop %s ***\n", __func__);
#endif

    }
  else
    if ((subfunction == MESSAGE_BOX_SUB_SHOW
	 || subfunction == MESSAGE_BOX_SUB_KILL) && the_box.timeout > 0)
    {

      /* Kill the box if requested by showing one last time and rebuilding the screen */
      if (subfunction == MESSAGE_BOX_SUB_KILL)
	the_box.timeout = 1;

      /* Draw box background */
      bgcolor.r = skins[selected_skin].background_colour >> 16 & 0xff;
      bgcolor.g = skins[selected_skin].background_colour >> 8 & 0xff;
      bgcolor.b = skins[selected_skin].background_colour & 0xff;
      destrect = set_rect (the_box.x, the_box.y, the_box.w, the_box.h);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format, bgcolor.r, bgcolor.g,
				bgcolor.b));
      /* Draw top bar */
      destrect =
	set_rect (the_box.bartopleftx, the_box.bartoplefty,
		  the_box.barbottomrightx - the_box.bartopleftx,
		  video.field_size / 8);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Draw bottom bar */
      destrect =
	set_rect (the_box.bartopleftx, the_box.barbottomrighty,
		  the_box.barbottomrightx - the_box.bartopleftx,
		  video.field_size / 8);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Draw left bar */
      destrect =
	set_rect (the_box.bartopleftx, the_box.bartoplefty,
		  video.field_size / 8,
		  the_box.barbottomrighty - the_box.bartoplefty);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Draw right bar */
      destrect =
	set_rect (the_box.barbottomrightx, the_box.bartoplefty,
		  video.field_size / 8,
		  the_box.barbottomrighty - the_box.bartoplefty +
		  video.field_size / 8);
      SDL_FillRect (screen, &destrect,
		    SDL_MapRGB (screen->format,
				bannercolour[bannerframe] >> 16 & 0xff,
				bannercolour[bannerframe] >> 8 & 0xff,
				bannercolour[bannerframe] & 0xff));
      /* Update the banner colour frame */
      if (--bannerstate <= 0)
	{
	  bannerstate = DELAY_BANNER;
	  if (++bannerframe >= BANNER_COLOURS)
	    bannerframe = 0;
	}
      /* Write message */
      fgcolor.r = skins[selected_skin].general_text_colour >> 16 & 0xff;
      fgcolor.g = skins[selected_skin].general_text_colour >> 8 & 0xff;
      fgcolor.b = skins[selected_skin].general_text_colour & 0xff;
      image = TTF_RenderUTF8_Shaded (font, the_box.message, fgcolor, bgcolor);
      destrect =
	set_rect (the_box.x + (the_box.w - image->w) / 2,
		  the_box.y + (the_box.h - image->h) / 2, image->w, image->h);
      SDL_BlitSurface (image, NULL, screen, &destrect);
      SDL_FreeSurface (image);

      /* Decrement the timeout and rebuild the screen if it times out */
      if (--the_box.timeout == 0)
	{
	  the_box.timeout--;
	  /* Restore/rebuild the screen */
	  if (game_mode == DESIGNER_ON)
	    {
	      game_area.redraw |= REDRAW_EVERYTHING;
	    }
	  else if (game_mode == INTRO_SCREEN)
	    {
	      intro_screen.redraw |= REDRAW_EVERYTHING;
	    }
	  else if (game_mode == GAME_ON)
	    {
	      game_area.redraw |= REDRAW_EVERYTHING;
	    }
	  else if (game_mode == END_SCREEN)
	    {
	      /* show_endscreen(); Redundant */
	    }
	  else if (game_mode == HELP_SCREEN)
	    {
	      help_screen.redraw |= REDRAW_EVERYTHING;
	    }
	  else if (game_mode == OPTIONS_SCREEN)
	    {
	      options_screen.redraw |= REDRAW_EVERYTHING;
	    }
	}
    }
}

/***************************************************************************
 * Show Banner                                                             *
 ***************************************************************************/
/* On entry: banner points to the banner array
			 xpos and ypos are the screen offsets to draw it
			 width and height are the dimensions of the banner array */

void
show_banner (char *banner[], int xpos, int ypos, int width, int height)
{
  SDL_Rect destrect;
  int x, y, frame = bannerframe;

  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
	{
	  if (banner[y][x] != '.')
	    {
	      if (video.field_size == 16)
		{
		  destrect = set_rect (x * 3 + xpos, y * 4 + ypos, 2, 3);
		}
	      else
		{
		  destrect = set_rect (x * 5 + xpos, y * 7 + ypos, 4, 6);
		}
	      SDL_FillRect (screen, &destrect,
			    SDL_MapRGB (screen->format,
					bannercolour[frame] >> 16 & 0xff,
					bannercolour[frame] >> 8 & 0xff,
					bannercolour[frame] & 0xff));
	    }
	}
      frame++;
      if (frame >= BANNER_COLOURS)
	frame = 0;
    }
}

/***************************************************************************
 * Write LCD Char                                                          *
 ***************************************************************************/
/* As we only have uppercase letters, numbers and a hyphon spread across
   multiple surfaces, this function will manage them.
   On entry: lcdchar = char to write
			 xpos and ypos are the screen offsets to draw it */

void
write_lcd_char (char lcdchar, int xpos, int ypos)
{
  SDL_Rect srcrect, destrect;

  destrect = set_rect (xpos, ypos, video.field_size / 2, video.field_size);

  /* Force lowercase uppercase */
  if (lcdchar >= 'a' && lcdchar <= 'z')
    lcdchar -= 0x20;

  if (lcdchar >= 'A' && lcdchar <= 'Z')
    {
      srcrect =
	set_rect ((lcdchar - 'A') * (video.field_size / 2 +
				     video.field_size / 16), 0,
		  video.field_size / 2, video.field_size);
      SDL_BlitSurface (alpha, &srcrect, screen, &destrect);
    }
  else if (lcdchar >= '0' && lcdchar <= '9')
    {
      srcrect =
	set_rect ((lcdchar - '0') * (video.field_size / 2 +
				     video.field_size / 16), 0,
		  video.field_size / 2, video.field_size);
      SDL_BlitSurface (ciphers, &srcrect, screen, &destrect);
    }
  else if (lcdchar == '-')
    {
      srcrect =
	set_rect (26 * (video.field_size / 2 + video.field_size / 16), 0,
		  video.field_size / 2, video.field_size);
      SDL_BlitSurface (alpha, &srcrect, screen, &destrect);
    }
  else
    {
      /* Drop everything else */
    }
}

/***************************************************************************
 * Show End Screen                                                         *
 ***************************************************************************/
/* Currently this only shows a congratulatory message although I may add
   some funny animations such as Robbo being chased across the screen by bears
   and the capsule zooming up screen etc. */

void
show_endscreen (void)
{
  int count, xoffset, yoffset;
  SDL_Rect destrect;

  xoffset = (screen->w - video.field_size * 15) / 2 + video.xshift;
  yoffset =
    (screen->h - video.field_size * 15) / 2 + video.field_size * 15 / 2 -
    video.field_size * 3 / 2 + video.yshift;

  /* Manage the pointer controls which will have been created by now */
  show_pointer_controls ();
  if (pointer_controls.state == POINTER_CONTROLS_MODE_ON)
    {
      /* Hide the pad as it won't be needed here */
      hide_pointer_controls_pad ();
      /* Hide most of the toolbar buttons as they won't be needed here */
      ROB_GetObject (POINTER_CONTROLS_TOOLBAR_OPTIONS_ID)->visible = FALSE;
      ROB_GetObject (POINTER_CONTROLS_TOOLBAR_DESIGNER_ID)->visible = FALSE;
      ROB_GetObject (POINTER_CONTROLS_TOOLBAR_RESTART_ID)->visible = FALSE;
      ROB_GetObject (POINTER_CONTROLS_TOOLBAR_HIDE_ID)->visible = FALSE;
    }

  clear_screen ();

  bgcolor.r = skins[selected_skin].background_colour >> 16 & 0xff;
  bgcolor.g = skins[selected_skin].background_colour >> 8 & 0xff;
  bgcolor.b = skins[selected_skin].background_colour & 0xff;
  fgcolor.r = skins[selected_skin].general_text_colour >> 16 & 0xff;
  fgcolor.g = skins[selected_skin].general_text_colour >> 8 & 0xff;
  fgcolor.b = skins[selected_skin].general_text_colour & 0xff;

  for (count = 0; count < TXT_ENDSCREEN_CONGRATS_ROWS; count++)
    {
      image =
	TTF_RenderUTF8_Shaded (font,
			       &txt_endscreen_congrats[count * TXT_MAX_COLS],
			       fgcolor, bgcolor);
      destrect =
	set_rect (xoffset + video.field_size * 15 / 2 - image->w / 2,
		  yoffset + count * video.field_size, image->w, image->h);
      SDL_BlitSurface (image, NULL, screen, &destrect);
      SDL_FreeSurface (image);
    }

}

/***************************************************************************
 * Register Hotspot                                                        *
 ***************************************************************************/
/* This function registers transparent pointer hotspots that are placed over
   menu items. Technically it is creating ROB objects and attaching them to
   an already existing parent object.
   
   Note that this will be called again to register the same objects as the
   calling function will be rebuilding the screen, therefore the existence
   of the object is checked for first before being updated with the latest
   coords and dimensions.
   
   If you enable DEBUG_ROB_HOTSPOTS you will be able to see the semi-opaque
   hotspots over the menu items and you can drag them around the screen */

/* On entry: pid = the parent object id
			 id = the id for this new object
			 destrect = the object dimensions */

void register_hotspot (int pid, int id, SDL_Rect destrect)
{
	ROB_Object rob_object, *hotspot;

	/* Create the object if it doesn't already exist */
	if ((hotspot = ROB_GetObject (id)) == NULL)
	{
		if (ROB_SetObjectDefaults (&rob_object))
			printf ("%s: %s\n", __func__, ROB_GetError ());
		rob_object.id = id;
		rob_object.pid = pid;
		if (ROB_CreateObject (&rob_object))
			printf ("%s: %s\n", __func__, ROB_GetError ());

		#ifdef DEBUG_ROB_HOTSPOTS
			if ((hotspot = ROB_GetObject (id)) != NULL) {
				hotspot->x = destrect.x;
				hotspot->y = destrect.y;
				hotspot->w = destrect.w;
				hotspot->h = destrect.h;
				hotspot->alpha = 128;
				hotspot->bgcolour = 0x808000;
				hotspot->bgcolour_over = 0xff8000;
				hotspot->draggable = ROB_DRAG_ANYWHERE;
			}
		#endif
	}
	/* Update the object properties */
	if ((hotspot = ROB_GetObject (id)) != NULL)
	{
		#ifndef DEBUG_ROB_HOTSPOTS
			hotspot->x = destrect.x;
			hotspot->y = destrect.y;
			hotspot->w = destrect.w;
			hotspot->h = destrect.h;
		#endif
	}
}


int efxcoord(int effect,int tile){
    return (video.field_size*3*effect)+((effect+1)*((video.field_size==16)?1:2))+(video.field_size*(tile+1));

}

