/*
 * GNU Robbo Copyright (C) 2002-2010 The GNU Robbo Team (see AUTHORS).
 * GNU Robbo is free software - you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.  GNU Robbo is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the impled warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with GNU CC; see the file
 * COPYING. If not, write to the Free Software Foundation, 59 Temple Place 
 * - Suite 330, Boston, MA 02111-1307, USA. 
 */

#include "game.h"

int             viewh;
/*
 * first we declare all the objects on the objects bar 
 */
struct k_icon_init K_icons_list[] = {
    {12, 8, KGAMEOBJ, EMPTY_FIELD, -1},
    {3, 6, KGAMEOBJ, ROBBO, 0},
    {1, 1, KGAMEOBJ, MAGNET, 0},
    {2, 1, KGAMEOBJ, MAGNET, 2},
    {1, 7, KGAMEOBJ, MAGNET, 1},
    {2, 7, KGAMEOBJ, MAGNET, 3},
    {6, 5, KGAMEOBJ, GUN, 0,},
    {8, 5, KGAMEOBJ, GUN, 2},
    {7, 5, KGAMEOBJ, GUN, 1},
    {9, 5, KGAMEOBJ, GUN, 3},
    {2, 2, KGAMEOBJ, BEAR, -1},
    {7, 3, KGAMEOBJ, BEAR_B, -1},
    {9, 3, KGAMEOBJ, BUTTERFLY, -1},
    {4, 2, KGAMEOBJ, BIRD, -1},
    {1, 2, KGAMEOBJ, QUESTIONMARK, -1},
    {5, 1, KGAMEOBJ, SCREW, -1},
    {6, 1, KGAMEOBJ, BULLET, -1},
    {7, 1, KGAMEOBJ, BOX, -1},
    {8, 1, KGAMEOBJ, KEY, -1},
    {9, 1, KGAMEOBJ, BOMB, -1},
    {10, 1, KGAMEOBJ, DOOR, -1},
    {6, 2, KGAMEOBJ, CAPSULE, -1},
    {7, 7, KGAMEOBJ, PUSH_BOX, -1},
    {3, 7, KGAMEOBJ, RADIOACTIVE_FIELD, -1},
    {5, 7, KGAMEOBJ, STOP, -1},
    {10, 4, KGAMEOBJ, BARRIER, -1},
    {1, 5, KGAMEOBJ, TELEPORT, -1},
    {6, 7, KGAMEOBJ, GROUND, -1},
    {3, 1, KGAMEOBJ, WALL, -1},
    {1, 4, KGAMEOBJ, K_LASER, -1},
    {0, 0, KOBJ_S, K_LTYPE, -1},
    {0, 0, KOBJ_S, K_RMSTATE, -1},

    {0, 0, KOBJ_S, K_DIRECTION, -1},
    {6, 2, KOBJ_S, K_ERASER, -1},
    {6, 4, KOBJ_S, K_INSERT, -1},
    {6, 3, KOBJ_S, K_SAVE, -1},
    {8, 7, KOBJ_S, K_RELOAD, -1},
    {6, 5, KOBJ_S, K_SIZE, -1},
    {12,1, KOBJ_S, K_SCREWDOWN,-1},
    {12,2, KOBJ_S, K_SCREWUP,-1},
    {8, 5, KOBJ_V, K_EXIT, 0},
    {1, 1, KOBJ_V, -1, 0},
    {8, 3, KOBJ_V, K_SCROLL2, 0},
    {8, 1, KOBJ_V, K_SCROLL, 0},
    {8, 2, KOBJ_V, K_SCROLL, 2},
    {8, 4, KOBJ_V, K_SCROLL2, 2},
    /*
     * horizontal scrolls should at the end 
     */
    {9, 4, KOBJ_V, K_SCROLL2, 1},
    {9, 2, KOBJ_V, K_SCROLL, 1},
    {9, 1, KOBJ_V, K_SCROLL, 3},
    {9, 3, KOBJ_V, K_SCROLL2, 3},


    {-1, -1, -1, -1, -1,}
};

struct k_icons  K_icons[K_MAXICONS];

/*
 * designer state variables 
 */
/*
 * I decided to use global config for all objects added to the board, this 
 * way writting of the designer is easier 
 */


int             teleportnr = 1;	/* teleport number */
int             direction = 0;	/* direction, it is the movement direction 
				 */
int             ltype = 0;	/* laser type */
int             rotable = 0;	/* gun rotable */
int             shooting = 0;	/* bird shooting flag */
int             movable = 0;	/* movable flag */
int             rmstate = 0;
int             eraser = 0;	/* unused */
int             insert = 0;	/* flag that shows if we would insert
				 * level, during save, or replace */
int             gsize = 0;
int 		bombtype=0;  /* regular bomb */
int             wallnumber = 0;
int             selected = -1;	/* selected icon */
int             txt_x,
                txt_y;
int             lastselected = 0;


void
k_init_icons()
{
    int             x1 = 0,
	x2 = k_view.w,
	y1 = k_view.h,
	y2 = 0;

    int             cnt = 0;

    for (cnt = 0; K_icons_list[cnt].x != -1; cnt++) {
	/*
	 * if horizontal scroll icons not needed, not draw them 
	 */
	if (K_icons_list[cnt].type == KOBJ_V
	    && (K_icons_list[cnt].game_obj == K_SCROLL
		|| K_icons_list[cnt].game_obj == K_SCROLL2)
	    && (K_icons_list[cnt].state % 2 == 1)) {
	    if (level.w <= k_view.w) {
		K_icons[cnt].x = -1;
		continue;
	    }
	}

	if (K_icons_list[cnt].type == KBREAKLINE) {
	    y1++;
	    x1 = 0;
	    continue;
	}
	if (K_icons_list[cnt].game_obj == EMPTY_FIELD)
	    selected = cnt;
	K_icons[cnt].x = K_icons_list[cnt].x;
	K_icons[cnt].y = K_icons_list[cnt].y;
	K_icons[cnt].game_obj = K_icons_list[cnt].game_obj;
	K_icons[cnt].redraw = 1;
	K_icons[cnt].type = K_icons_list[cnt].type;
	K_icons[cnt].blink = 0;
	K_icons[cnt].direction = K_icons_list[cnt].state;
	switch (K_icons_list[cnt].type) {
	case KGAMEOBJ:
	    K_icons[cnt].blink2 = 0;
	case KOBJ_S:
	    if (x1 >= k_view.w) {
		x1 = 0;
		y1++;
	    }
	    K_icons[cnt].scrx = x1++;
	    K_icons[cnt].scry = y1;
	    break;
	case KOBJ_V:
	    K_icons[cnt].scrx = x2;
	    K_icons[cnt].scry = y2++;
	    break;
	}
    }
    K_icons[cnt].x = -1;
    K_icons[cnt].y = -1;
}

/*
 * this will initialize konstruktor viewport 
 * a bit copied from level_init from level.h.
 */
void
konstruktor_viewport_init()
{
	int icons;
	int designersize;

	/* Count the number of icons used at the bottom of the screen */
	for (icons = 0; K_icons_list[icons].x != -1 &&
		K_icons_list[icons].type != KOBJ_V; icons++);

	/* Set viewport as wide as possible */
	k_view.w = level.w;
	/* Enforce a minimum width */
	if (k_view.w < DEFAULT_VIEWPORT_WIDTH - 2)
		k_view.w = DEFAULT_VIEWPORT_WIDTH - 2;
	/* Now adjust it to fit within the dimensions of the
	 * screen allowing for the scroll controls column */
	if (k_view.w >= screen->w / video.field_size - 1)
		k_view.w = screen->w / video.field_size - 1;
	/* Set the xoffset and centre the viewport */
	k_view.offsetx =
		(screen->w - (k_view.w + 1) * video.field_size) / 2 + video.xshift;

	/* Calculate the depth of the icons and text area */
	designersize = icons / k_view.w + 1;
	if ((icons % k_view.w) &&
		(k_view.w - icons % k_view.w < DEFAULT_VIEWPORT_WIDTH - 1))
		designersize++;

	/* Set viewport as high as possible */
	k_view.h = level.h;
	/* Enforce a minimum height */
	if (k_view.h < DEFAULT_VIEWPORT_HEIGHT - 1 - designersize)
		k_view.h = DEFAULT_VIEWPORT_HEIGHT - 1 - designersize;
	/* Now adjust it to fit within the dimensions of the screen
	 * allowing for the authorline, scoreline, icons and text area */
	if (k_view.h >= screen->h / video.field_size - 3 - designersize)
		k_view.h = screen->h / video.field_size - 3 - designersize;
	/* Set the yoffset and centre the viewport */
	k_view.offsety =
		(screen->h - (k_view.h + designersize) * video.field_size) / 2 + video.yshift;

	/* Calculate the position of the text area */
	txt_x = icons % k_view.w;
	if (k_view.w - icons % k_view.w < DEFAULT_VIEWPORT_WIDTH - 1) txt_x = 0;
	txt_y = k_view.h + designersize - 1;

	/* Set the authorline yoffset */
	authorline.yoffset = k_view.offsety - video.field_size * 1.25;

	/* Set the scoreline yoffset */
	scoreline.yoffset = k_view.offsety + (k_view.h + designersize + 0.25) * video.field_size;
}

/*
 * 
 * initialize Designer
 * 
 * 
 */
int
konstruktor_init()
{
    hide_pointer_controls();	/* hide pointer controls, so it will not
				 * go in the way */
    gsize = -1;			/* -1 means custom size, this can happen */
    inforedraw = 1;
    lastclick = 0;
    clear_screen();
    level_init();
    if (level.w == 16 && level.h == 31)
	gsize = 0;
    else if (level.w == 32 && level.h == 31)
	gsize = 1;
    inforedraw = 1;
    sprintf(infostring, txt_konstruktor_Welcome_to_GNU_Robbo_Designer);
    robbo.keys = k_count_obj(KEY);	/* count keys on a map */
    robbo.bullets = k_count_obj(BULLET) * 9;	/* count bullets */
    game_area.redraw = REDRAW_EVERYTHING;
    k_view.x = 0;
    k_view.y = 0;
    konstruktor_viewport_init();
    k_init_icons();
    k_redraw_all();
    konstruktor_show_game_area();
    return 0;
}

/*
 * prepare everything for finishing designer 
 */
int
konstruktor_end()
{
    clear_screen();
    level_init();
    game_area.redraw |= REDRAW_EVERYTHING;
    insert = 0;			/* we will forget if we wanted to insert a 
				 * new level */
    selected = -1;
    lastselected = 0;
    return 0;
}


/***************************************************************************
 * konstruktor animate                                                     *
 ***************************************************************************/
/*
 * this is the animate function for the designer, it should be run in
 * every cycle this function is based on update_game function form
 * board.c, the difference is that most of its functionality is being
 * cut, as we do not want the game to go on here, we just want that nice
 * feeling of game paused. 
 */

void
konstruktor_animate(void)
{
    int        temp_direction,     x,
                    y;
	
    /*
     * Now iterate through every board location updating objects 
     */
    for (y = 0; y < level.h; y++) {
	for (x = 0; x < level.w; x++) {
	    /*
	     * decrement rotation delay counter if it is a gun, and needs
	     * it 
	     */
	    if ((board[x][y].type == GUN || board[x][y].type==MAGNET) && (board[x][y].rotated > 0))
		board[x][y].rotated--;	/* Decrement rotation delay */
	    /*
	     * Decrement the object's delay and then check if it needs
	     * processing 
	     */
	    if (board[x][y].moved > 0)
		board[x][y].moved--;
	    if (board[x][y].moved <= 0) {
		/*
		 * animate all the necessary objects 
		 */
		switch (board[x][y].type) {
		case BEAR:
		case BEAR_B:
		case BARRIER:
		case BIRD:
		case BUTTERFLY:
		case CAPSULE:
		case RADIOACTIVE_FIELD:
		    board[x][y].moved = DELAY_KONSTRUKTOR;
		    negate_state(x, y);
		    break;
		    /*
		     * teleport has different blinking frequency, than
		     * others, so we leave it as it is here 
		     */
		case TELEPORT:
		    negate_state(x, y);
		    board[x][y].moved = DELAY_TELEPORT;
		    break;
		    /*
		     * GUN logic 
		     */
		case GUN:
		    if (board[x][y].rotable)
			if (board[x][y].rotated == 0) {
			    if (board[x][y].randomrotated) {
				temp_direction=rand() & 0x03;
					if(board[x][y].movable==1) {
						board[x][y].direction2=abs(board[x][y].direction2+(temp_direction-board[x][y].direction)) % 4;
						} 
					board[x][y].direction = temp_direction;
				}
			    else
				board[x][y].direction =
				    (board[x][y].direction + 1) & 0x03;
			    board[x][y].rotated = DELAY_ROTATION;
			    redraw_field(x, y);
			}
		    board[x][y].state = board[x][y].direction;
		    break;
		case MAGNET:
		    if (board[x][y].rotable)
			if (board[x][y].rotated == 0) {
			    temp_direction=rand() & 0x03;
			    board[x][y].direction = temp_direction;
			    board[x][y].rotated = DELAY_ROTATION;
			    redraw_field(x, y);
			}
		    board[x][y].state = board[x][y].direction;
		    break;

		}
	    }
	}
    }
}


/*
 * change left upper corner's coordinates 
 */
void
konstruktor_scroll(int direction)
{
    game_area.redraw = REDRAW_EVERYTHING;

    k_redraw_all();
    switch (direction) {
    case 0:
	if (k_view.y > 0)
	    k_view.y--;
	break;
    case 3:
	if (k_view.x < level.w - k_view.w)
	    k_view.x++;
	break;
    case 2:
	if (k_view.y < level.h - k_view.h)
	    k_view.y++;
	break;
    case 1:
	if (k_view.x > 0)
	    k_view.x--;
    }
};

/*
 * this one is almost all taken from screen file 
 */

void
konstruktor_show_game_area(void)
{
    int             x,
                    y,
                    xoffset = k_view.offsetx,
	yoffset = k_view.offsety;
    SDL_Rect        srcrect,
                    destrect,
                    emptyrect;
    Uint32          level_colour;
    int             nx,
                    ny;
    if (lastclick > 0)
	lastclick--;
    play_music();
    if (game_area.redraw >= REDRAW_EVERYTHING) {
	k_redraw_all();
	clear_screen();
	scoreline.redraw = 0xff;
    }
    /*
     * the viewport and draw the objects if required 
     */
    for (y = k_view.y; y < k_view.y + k_view.h; y++) {
	for (x = k_view.x; x < k_view.x + k_view.w; x++) {
	    destrect =
		set_rect(xoffset + (x - k_view.x) * video.field_size,
			 yoffset + (y - k_view.y) * video.field_size,
			 video.field_size, video.field_size);
	    /*
	     * If the location is outside the dimensions of the level then 
	     * draw a BACKGROUND_COLOUR tile instead 
	     */
	    if (x < 0 || y < 0 || x >= level.w || y >= level.h) {
		if (game_area.redraw & REDRAW_EVERYTHING)
		    SDL_FillRect(screen, &destrect,
				 SDL_MapRGB(screen->format,
					    skins
					    [selected_skin].
					    background_colour >> 16 & 0xff,
					    skins[selected_skin].
					    background_colour >> 8 & 0xff,
					    skins[selected_skin].
					    background_colour & 0xff));
	    } else {
		/*
		 * we will not redraw everything all the time 
		 */
		if (board[x][y].redraw == FALSE
		    && game_area.redraw != REDRAW_EVERYTHING)
		    continue;
		board[x][y].redraw = FALSE;

		/*
		 * The location is valid but only draw locations that have 
		 * been marked for redrawing 
		 */
		if (level.colour_override != UNDEFINED) {
		    level_colour = level.colour_override;
		} else if (level.colour != UNDEFINED) {
		    level_colour = level.colour;
		} else {
		    level_colour = DEFAULT_LEVEL_COLOUR;
		}

		SDL_FillRect(screen, &destrect,
			     SDL_MapRGB(screen->format,
					level_colour >> 16 & 0xff,
					level_colour >> 8 & 0xff,
					level_colour & 0xff));
		/*
		 * Get icon surface offsets for object's current animation 
		 * frame 
		 */
		/*
		 * now first, let's manage the background file, we cut it
		 * into 4 pieces, so we would change backgrounds in
		 * different levels 
		 */
		if (bgrnd != NULL) {
		    int             bgx = bgrnd->w / 2,
			bgy = bgrnd->h / 2;
		    switch (level_packs[selected_pack].level_selected % 4) {
		    case 0:
			emptyrect =
			    set_rect((video.field_size * x) % bgx,
				     (video.field_size * y) % bgy,
				     video.field_size, video.field_size);
			break;
		    case 1:
			emptyrect =
			    set_rect(((video.field_size * x) % bgx) + bgx,
				     (video.field_size * y) % bgy,
				     video.field_size, video.field_size);
			break;
		    case 2:
			emptyrect =
			    set_rect((video.field_size * x) % bgx,
				     ((video.field_size * y) % bgy) + bgy,
				     video.field_size, video.field_size);
			break;
		    case 3:
			emptyrect =
			    set_rect(((video.field_size * x) % bgx) + bgx,
				     ((video.field_size * y) % bgy) + bgy,
				     video.field_size, video.field_size);
			break;
		    }
		    SDL_BlitSurface(bgrnd, &emptyrect, screen, &destrect);

		};
		k_geticoncoords(6, 7, &emptyrect);
		SDL_BlitSurface(k_icons, &emptyrect, screen, &destrect);
		/*
		 * now we draw empty field 
		 */
		if (board[x][y].type != EMPTY_FIELD) {
		    if ((board[x][y].shooting != 0)
			&& (board[x][y].type == BIRD)) {
			k_geticoncoords(3, 4, &srcrect);
			SDL_BlitSurface(k_icons, &srcrect, screen,
					&destrect);
		    }
		 if(board[x][y].type==GUN && board[x][y].movable==1) board[x][y].state=board[x][y].direction+4;
		    srcrect =
			set_rect(board[x][y].icon[board[x][y].state].x,
				 board[x][y].icon[board[x][y].state].y,
				 video.field_size, video.field_size);

		    if (SDL_BlitSurface(icons, &srcrect, screen, &destrect)
			< 0) {
			fprintf(stdout, "BlitSurface error: %s\n",
				SDL_GetError());
		    }
		    switch (board[x][y].type) {
		    case LASER_L:
		    case LASER_D:
			k_geticoncoords(5, (board[x][y].direction) + 5,
					&srcrect);
			SDL_BlitSurface(k_icons, &srcrect, screen,
					&destrect);
			break;
		    case BARRIER:
			k_geticoncoords(5, (board[x][y].direction) + 5,
					&srcrect);
			SDL_BlitSurface(k_icons, &srcrect, screen,
					&destrect);
			break;
		    case TELEPORT:
			nx = 4 + (board[x][y].teleportnumber / 9);
			ny = (board[x][y].teleportnumber % 9) +
			    ((board[x][y].teleportnumber > 8) ? 1 : 0);
			k_geticoncoords(nx, ny, &srcrect);
			SDL_BlitSurface(k_icons, &srcrect, screen,
					&destrect);
			break;
		    case MAGNET:
			if (board[x][y].rotable) {
			    k_geticoncoords(6, 1, &srcrect);
			    SDL_BlitSurface(k_icons, &srcrect, screen, &destrect);
			}
			break;			
		    case GUN:
			nx = 7;
			ny = ((board[x][y].solidlaser + 2) % 3) + 1;
			k_geticoncoords(nx, ny, &srcrect);
			SDL_BlitSurface(k_icons, &srcrect, screen, &destrect);
			if (board[x][y].movable) {
			    k_geticoncoords(5, board[x][y].direction2 + 5, &srcrect);
			    SDL_BlitSurface(k_icons, &srcrect, screen, &destrect);
			}
			if (board[x][y].rotable) {
			    k_geticoncoords(6, 1, &srcrect);
			    SDL_BlitSurface(k_icons, &srcrect, screen, &destrect);
			}

			break;
		    case BEAR:
		    case BEAR_B:
			k_geticoncoords(5, board[x][y].direction + 5,
					&srcrect);
			SDL_BlitSurface(k_icons, &srcrect, screen,
					&destrect);
			break;
		    case BIRD:
			k_geticoncoords(5, board[x][y].direction + 5,
					&srcrect);
			SDL_BlitSurface(k_icons, &srcrect, screen,
					&destrect);


			break;

		    }
		}
		/*
		 * Are we drawing Robbo? 
		 */
		if (robbo.x == x && robbo.y == y) {	/* Is this his
							 * location and is 
							 * * * * * * * * * 
							 * * * * he
							 * alive? */
		    k_geticoncoords(3, 6, &srcrect);
		    if (SDL_BlitSurface(icons, &srcrect, screen, &destrect)
			< 0) {
			fprintf(stdout, "BlitSurface error: %s\n",
				SDL_GetError());
		    }
		}
	    }
	}
    }
    konstruktor_draw_icons();
    /*
     * Redraw the parts of the scoreline that have been requested 
     */
    if (scoreline.redraw)
	show_scoreline();
    game_area.redraw = 0;
}

/*
 * manage designer clicks 
 */

void
konstruktor_click()
{
    /*
     * precalculate where we acually clicked 
     */
    int             nx = (kmx - k_view.offsetx) / video.field_size;
    int             ny = ((kmy - k_view.offsety) / video.field_size);
    int             cnt;
    /*
     * refresh all the icons 
     */
    if (kmx - k_view.offsetx < 0 || kmy - k_view.offsety < 0)
	return;
    k_redraw_all();
    if (nx >= 0 && ny >= 0 && nx < k_view.w && ny < k_view.h) {
	/*
	 * ok, we have to handle map click. now we have to find out the
	 * position 
	 */
	lastselected = -1;
	nx = nx + k_view.x;
	ny = ny + k_view.y;
	if (selected != -1) {
	    sprintf(infostring,
		    "                                            ");
	    inforedraw = 1;
	    if (K_icons[selected].game_obj == ROBBO) {
		redraw_field(robbo.x, robbo.y);
		sprintf(infostring, "Robbo");
	    }
	    /*
	     * do we try to delete one of the counted objects? 
	     */
	    switch (board[nx][ny].type) {
	    case BULLET:
		game_area.redraw = REDRAW_EVERYTHING;
		robbo.bullets = robbo.bullets - 9;	/* bullets */
		break;
	    case KEY:
		game_area.redraw = REDRAW_EVERYTHING;
		robbo.keys--;	/* keys */
		break;

	    case SCREW:
		game_area.redraw = REDRAW_EVERYTHING;
		robbo.screws--;	/* screws */
		if(robbo.screws<0) robbo.screws=0;
	    }
	    
	    if (K_icons[selected].game_obj != K_LASER)
		create_object(nx, ny, K_icons[selected].game_obj);
		if(K_icons[selected].game_obj == BOMB && bombtype==1)						
		create_object(nx, ny, BOMB2);
	    switch (K_icons[selected].game_obj) {
	    case GROUND:
		sprintf(infostring, txt_konstruktor_Ground);
		break;
	    case STOP:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Stop,
			k_count_obj(STOP),
			txt_konstruktor_on_map);
		break;
	    case RADIOACTIVE_FIELD:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Radioactive_Field,
			k_count_obj(RADIOACTIVE_FIELD),
			txt_konstruktor_on_map);
		break;
	    case PUSH_BOX:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Push_Box,
			k_count_obj(PUSH_BOX),
			txt_konstruktor_on_map);
		break;
	    case CAPSULE:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Capsule,
			k_count_obj(CAPSULE),
			txt_konstruktor_on_map);
		break;
	    case BOMB:
		if (bombtype==0) {
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Bomb,
			k_count_obj(BOMB),
			txt_konstruktor_on_map);
		} else { 
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Bomb2,
			k_count_obj(BOMB2),
			txt_konstruktor_on_map);
		}
		break;
	    case DOOR:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Door,
			k_count_obj(DOOR),
			txt_konstruktor_on_map);
		break;
	    case BOX:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Box,
			k_count_obj(BOX),
			txt_konstruktor_on_map);
		break;
	    case QUESTIONMARK:
		sprintf(infostring, "? (%d %s)",
			k_count_obj(QUESTIONMARK),
			txt_konstruktor_on_map);
		break;
	    case EMPTY_FIELD:
		sprintf(infostring, txt_konstruktor_Empty_Field);
		break;
	    case BUTTERFLY:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Butterfly,
			k_count_obj(BUTTERFLY),
			txt_konstruktor_on_map);
		break;
	    case K_LASER:
		if (direction == 0 || direction == 2) {
		    create_object(nx, ny, LASER_L);
		} else {
		    create_object(nx, ny, LASER_D);
		}
		board[nx][ny].direction = direction;
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Gun_Fire,
			k_count_obj(LASER_L) + k_count_obj(LASER_D),
			txt_konstruktor_on_map);
		break;
	    case BARRIER:
		board[nx][ny].direction = (direction >= 2) ? 2 : 0;
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Force_Field,
			k_count_obj(BARRIER),
			txt_konstruktor_on_map);
		break;
	    case SCREW:
		robbo.screws++;
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Screw,
			k_count_obj(SCREW),
			txt_konstruktor_on_map);
		game_area.redraw = REDRAW_EVERYTHING;
		break;
	    case WALL:
		board[nx][ny].state = wallnumber;
		if (wallnumber != 3)
		    sprintf(infostring, txt_konstruktor_Wall);
		else
		    sprintf(infostring, txt_konstruktor_Black_Wall);
//		    game_area.redraw=REDRAW_EVERYTHING;
		break;
	    case TELEPORT:
		board[nx][ny].teleportnumber = teleportnr;
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Teleport,
			k_count_obj(TELEPORT),
			txt_konstruktor_on_map);
		break;
	    case GUN:
		board[nx][ny].solidlaser = (ltype + 1) % 3;
		board[nx][ny].movable = movable;
		board[nx][ny].direction2 = direction;
		board[nx][ny].direction = K_icons[selected].direction;
		board[nx][ny].state = K_icons[selected].direction;
		board[nx][ny].rotated = 0;
		board[nx][ny].rotable = rotable;
		board[nx][ny].randomrotated = 1;
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Gun,
			k_count_obj(GUN),
			txt_konstruktor_on_map);
		break;
	    case MAGNET:
		board[nx][ny].direction = K_icons[selected].direction;
		board[nx][ny].state = K_icons[selected].direction;
		board[nx][ny].rotable=rotable;
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Magnet,
			k_count_obj(MAGNET),
			txt_konstruktor_on_map);
		break;
	    case BEAR:
	    case BEAR_B:
		board[nx][ny].direction = direction;
		board[nx][ny].state = 0;
		if (board[nx][ny].type == BEAR)
		    sprintf(infostring, "%s (%d %s)",
		        txt_konstruktor_Bear,
		        k_count_obj(BEAR),
				txt_konstruktor_on_map);
		else
		    sprintf(infostring, "%s (%d %s)",
		        txt_konstruktor_Black_Bear,
		        k_count_obj(BEAR_B),
				txt_konstruktor_on_map);
		break;
	    case BIRD:
		board[nx][ny].direction = direction;
		board[nx][ny].direction2 = 1;
		board[nx][ny].state = 0;
		board[nx][ny].shooting = shooting;
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Bird,
			k_count_obj(BIRD),
			txt_konstruktor_on_map);
		break;
	    case KEY:
		robbo.keys++;
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Key,
			k_count_obj(KEY),
			txt_konstruktor_on_map);
		game_area.redraw = REDRAW_EVERYTHING;
		break;
	    case BULLET:
		robbo.bullets = robbo.bullets + 9;
		sprintf(infostring, "%s (%dx9 %s)",
			txt_konstruktor_Bullets,
			k_count_obj(BULLET),
			txt_konstruktor_on_map);
		game_area.redraw = REDRAW_EVERYTHING;
		break;
	    };
	rearange_walls();
	}
	return;
    }
    if (lastclick > 0) {
	return;
    }
    lastclick = 5;
    /*
     * toolbar clicks 
     */
    for (cnt = 0; K_icons[cnt].x != -1; cnt++) {
	if (nx == K_icons[cnt].scrx && ny == K_icons[cnt].scry) {
	 
   	    K_icons[cnt].blink = DELAY_K_BLINK2; /* here we should blink clicked object, so it would not get negative index */
	    switch (K_icons[cnt].game_obj) {
	    case -1:
		return;
	    case K_SCREWUP:
		robbo.screws++;
		sprintf(infostring,"%s (%d - %d)",txt_konstruktor_Screws,robbo.screws,k_count_obj(SCREW));
		game_area.redraw = REDRAW_EVERYTHING;

		inforedraw=1;
		break;

	    case K_SCREWDOWN:
		robbo.screws--;
		if(robbo.screws<0) robbo.screws=0;
		sprintf(infostring,"%s (%d - %d)",txt_konstruktor_Screws,robbo.screws,k_count_obj(SCREW));
		game_area.redraw = REDRAW_EVERYTHING;

		inforedraw=1;
		break;


	    case RADIOACTIVE_FIELD:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Radioactive_Field,
			k_count_obj(RADIOACTIVE_FIELD),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case K_RELOAD:
		if (cnt == lastselected) {
		    sprintf(infostring, txt_konstruktor_Level_reloaded);
		    clear_screen();
		    level_init();
		    konstruktor_viewport_init();
		    if (level.w == 16 && level.h == 31)
			gsize = 0;
		    else if (level.w == 32 && level.h == 31)
			gsize = 1;
		    robbo.keys = k_count_obj(KEY);	/* count keys on a map */
		    robbo.bullets = k_count_obj(BULLET) * 9;	/* count bullets  */
		    game_area.redraw = REDRAW_EVERYTHING;
		    k_view.x = 0;
		    k_view.y = 0;

		    k_init_icons();
		    k_redraw_all();
		    cnt = -1;
		} else {
		    sprintf(infostring,
		    	txt_konstruktor_Reload_level_select_again_to_reload);
		    #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
		    lastclick = 50;
		    #else
		    lastclick = 10;
		    #endif
		};
		inforedraw = 1;
		break;
	    case STOP:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Stop,
			k_count_obj(STOP),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case BARRIER:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Force_Field,
			k_count_obj(BARRIER),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case MAGNET:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Magnet,
			k_count_obj(MAGNET),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case ROBBO:
		sprintf(infostring, "Robbo");
		inforedraw = 1;
		break;
	    case GUN:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Gun,
			k_count_obj(GUN),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case BEAR:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Bear,
			k_count_obj(BEAR),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case BEAR_B:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Black_Bear,
			k_count_obj(BEAR_B),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case BUTTERFLY:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Butterfly,
			k_count_obj(BUTTERFLY),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case BOMB:
		if (cnt== selected) 
			bombtype=(bombtype+1) %2;
		if(bombtype==0) {
			sprintf(infostring, "%s (%d %s)",
				txt_konstruktor_Bomb,
				k_count_obj(BOMB),
				txt_konstruktor_on_map);
		} else {
			sprintf(infostring, "%s (%d %s)",
				txt_konstruktor_Bomb2,
				k_count_obj(BOMB2),
				txt_konstruktor_on_map);
		}
		inforedraw = 1;
		break;
	    case DOOR:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Door,
			k_count_obj(DOOR),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case BULLET:
		sprintf(infostring, "%s (%dx9 %s)",
			txt_konstruktor_Bullets,
			k_count_obj(BULLET),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case BOX:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Box,
			k_count_obj(BOX),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case KEY:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Key,
			k_count_obj(KEY),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case CAPSULE:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Capsule,
			k_count_obj(CAPSULE),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case PUSH_BOX:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Push_Box,
			k_count_obj(PUSH_BOX),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case SCREW:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Screw,
			k_count_obj(SCREW),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case EMPTY_FIELD:
		sprintf(infostring, txt_konstruktor_Empty_Field);
		inforedraw = 1;
		break;
	    case WALL:
		if (cnt == selected)
		    wallnumber = (wallnumber + 1) % 10;
		if (wallnumber != 3)
		    sprintf(infostring, txt_konstruktor_Wall);
		else
		    sprintf(infostring, txt_konstruktor_Black_Wall);
		inforedraw = 1;
		break;
	    case K_LASER:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Gun_Fire,
			k_count_obj(LASER_L) + k_count_obj(LASER_D),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case TELEPORT:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Teleport,
			k_count_obj(TELEPORT),
			txt_konstruktor_on_map);
		inforedraw = 1;

		if (cnt == selected)
		    teleportnr = (teleportnr % 11) + 1;
		break;
	    case GROUND:
		sprintf(infostring, txt_konstruktor_Ground);
		inforedraw = 1;
		break;
	    case BIRD:
		sprintf(infostring, "%s (%d %s)",
			txt_konstruktor_Bird,
			k_count_obj(BIRD),
			txt_konstruktor_on_map);
		inforedraw = 1;

		if (cnt == selected)
		    shooting = (shooting + 1) % 2;
		break;
	    case QUESTIONMARK:
		sprintf(infostring, "? (%d %s)",
			k_count_obj(QUESTIONMARK),
			txt_konstruktor_on_map);
		inforedraw = 1;
		break;
	    case K_DIRECTION:
	    direction = (direction + 1) % 4;
	    switch (direction) {
	        case 0:
	        sprintf(infostring, txt_konstruktor_Direction_right);
	        break;
	        case 1:
	        sprintf(infostring, txt_konstruktor_Direction_down);
	        break;
	        case 2:
	        sprintf(infostring, txt_konstruktor_Direction_left);
	        break;
	        case 3:
	        sprintf(infostring, txt_konstruktor_Direction_up);
	        break;
		}
		inforedraw = 1;
		break;
	    case K_LTYPE:
		ltype = (ltype + 1) % 3;
		switch (ltype) {
		case 0:
		    sprintf(infostring, txt_konstruktor_Laser_Gun);
		    break;
		case 1:
		    sprintf(infostring, txt_konstruktor_Blaster_Gun);
		    break;
		case 2:
		    sprintf(infostring, txt_konstruktor_Regular_Gun);
		    break;
		}
		inforedraw = 1;
		break;
	    case K_RMSTATE:
		rmstate = (rmstate + 1) % 4;
		rotable = (rmstate & 2) / 2;
		movable = (rmstate & 1);
	    switch (rmstate) {
	        case 0:
	        sprintf(infostring, txt_konstruktor_Fixed);
	        break;
	        case 1:
	        sprintf(infostring, txt_konstruktor_Moveable);
	        break;
	        case 2:
	        sprintf(infostring, txt_konstruktor_Rotating);
	        break;
	        case 3:
	        sprintf(infostring, txt_konstruktor_Moveable_Rotating);
	        break;
		}
		inforedraw = 1;
		break;
	    case K_INSERT:
		if (lastselected == cnt) {
		    sprintf(infostring, txt_konstruktor_Level_created);
		    insert = 1;
		    /*
		     * Clear board 
		     */
		    robbo.screws = 0;
		    robbo.keys = 0;
		    robbo.bullets = 0;	/* Thunor: I added this as it was missing */
		    scoreline.redraw = 0xff;	/* Thunor: I added this as it was missing */
		    switch (gsize) {	/* if gsize is different than 0,1, 
					 * we leave the map size as it is */
		    case 0:
			level.w = 16;
			level.h = 31;
			break;
		    case 1:
			level.w = 32;
			level.h = 31;
			break;
		    }
		    clear_entire_board();
		    cnt = -1;

		} else {
		    sprintf(infostring,
		    	txt_konstruktor_New_level_select_again_to_create);
		    #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
		    lastclick = 50;
		    #else
		    lastclick = 10;
		    #endif
		}
		inforedraw = 1;
		break;
	    case K_ERASER:
		/*
		 * Clear board 
		 */
		if (lastselected == cnt) {
		    sprintf(infostring, txt_konstruktor_Level_cleared);
		    robbo.screws = 0;
		    robbo.keys = 0;
		    robbo.bullets = 0;	/* Thunor: I added this as it was missing */
		    scoreline.redraw = 0xff;	/* Thunor: I added this as it was missing */
		    switch (gsize) {	/* if gsize is different than 0,1, 
					 * we leave the map size as it is */
		    case 0:
			level.w = 16;
			level.h = 31;
			break;
		    case 1:
			level.w = 32;
			level.h = 31;
			break;
		    }
		    clear_entire_board();
		    cnt = -1;
		} else {
		    sprintf(infostring,
		    	txt_konstruktor_Clear_level_select_again_to_clear);
		    #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
		    lastclick = 50;
		    #else
		    lastclick = 10;
		    #endif
		}
		inforedraw = 1;
		break;
	    case K_SAVE:
		if (lastselected == cnt) {
		    if (insert == 1)
			sprintf(infostring, txt_konstruktor_New_level_appended_to_pack);
		    else
			sprintf(infostring, txt_konstruktor_Changes_saved);
		    cnt = -1;

		    k_save_map();

		    insert = 0;	/* since we saved the level, now we will
				 * not try to insert another one */
		} else {
		    #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
		    lastclick = 50;
		    #else
		    lastclick = 10;
		    #endif

		    if (insert == 1)
			sprintf(infostring,
				txt_konstruktor_Save_select_again_to_save_new_level);
		    else
			sprintf(infostring,
				txt_konstruktor_Save_select_again_to_save_changes);
		}
		inforedraw = 1;
		/*
		 * unless user wants differently 
		 */
		break;
	    case K_SIZE:	/* this would set up map size, after
				 * cleaning it */
		gsize = (gsize + 1) % 2;
		switch (gsize) {
		case 0:
		    level.w = 16;
		    level.h = 31;
		    k_view.x = 0;
		    sprintf(infostring, "%s 16x31",
		    	txt_konstruktor_Level_resized_to);
		    #ifdef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
		    rob_lyr_pointer->y = (txt_y) * video.field_size + k_view.offsety + (video.field_size - 24) / 2 - rob_lyr_pointer->h;
		    #endif
		    konstruktor_viewport_init();
		    break;
		case 1:
		    level.w = 32;
		    level.h = 31;
		    sprintf(infostring, "%s 32x31",
		    	txt_konstruktor_Level_resized_to);
		    konstruktor_viewport_init();
		    break;
		}
	    	robbo.screws = k_count_obj(SCREW);	/* Thunor: I added this as it was missing */
	    	robbo.keys = k_count_obj(KEY);	/* Thunor: I added this as it was missing */
	    	robbo.bullets = k_count_obj(BULLET) * 9;	/* Thunor: I added this as it was missing */
		k_init_icons();
		game_area.redraw = REDRAW_EVERYTHING;
		inforedraw = 1;

		break;
	    case K_SCROLL:
		lastclick = 3;
		konstruktor_scroll(K_icons[cnt].direction);
		break;
	    case K_SCROLL2:
		lastclick = 3;
		game_area.redraw = REDRAW_EVERYTHING;

		switch (K_icons[cnt].direction) {
		case 0:
		    k_view.y = 0;
		    break;
		case 1:
		    k_view.x = 0;
		    break;
		case 2:
		    k_view.y = level.h - k_view.h;
		    break;
		case 3:
		    k_view.x = level.w - k_view.w;
		    break;
		}
		break;
	    case K_EXIT:
		if (lastselected == cnt) {

		    game_mode = GAME_ON;
		    #if defined(PLATFORM_PSVITA)
		    ROB_SetOpEnvPointer(FALSE, ROB_POINTER_MODE_OFF);
		    #endif
		    konstruktor_end();
		    cnt = -1;
		} else {
		    sprintf(infostring,
		    	txt_konstruktor_Exit_designer_select_again_to_exit);
		    #ifndef SIMULATE_DESIGNER_CONTROLS_AS_BUTTONS
		    lastclick = 50;	/* very long click is used as another click */
		    #else
		    lastclick = 10;
		    #endif
		    inforedraw = 1;
		};
		break;
	    }
	    
	    if (cnt >= 0)	    /* that if added by Thunor should not hurt, and it should help with negative index thing */
	    {
	        if (K_icons[cnt].type == KGAMEOBJ) {
	            selected = cnt;
	        }
/*	        K_icons[cnt].blink = DELAY_K_BLINK2; */
	    }

	    lastselected = cnt;
	    break;
	}
    }
}


/*
 * simple translator betweem icon coordinates it is easier to manage icons 
 * this way 
 */
void
k_geticoncoords(int x, int y, SDL_Rect * rect)
{
    int             nx,
                    ny;
    nx = ((video.field_size / 16) * x) + ((x - 1) * video.field_size);
    ny = ((video.field_size / 16) * y) + ((y - 1) * video.field_size);
    rect->x = nx;
    rect->y = ny;
    rect->w = video.field_size;
    rect->h = video.field_size;
}


/*
 * draw designer icons 
 */
void
konstruktor_draw_icons()
{
    int             nx,
                    ny;
    unsigned int    color = 0;
    int             cnt;
    SDL_Surface    *image;
    SDL_Color       bgcolor,
                    fgcolor;
    SDL_Rect        dst,
                    src;
    if (K_icons[selected].blink == 0)
	K_icons[selected].blink = 1;
    for (cnt = 0; K_icons[cnt].x != -1; cnt++) {
	if (K_icons[cnt].game_obj == -1)
	    continue;

	if (K_icons[cnt].blink > 0) {
	    K_icons[cnt].blink--;
	    if (K_icons[cnt].blink <= 0)
		if (cnt == selected) {
		    K_icons[cnt].blink = DELAY_K_BLINK;
		    K_icons[selected].blink2 =
			(K_icons[selected].blink2 + 1) % 2;
		}
	    K_icons[cnt].redraw = 1;
	}
	if (K_icons[cnt].redraw <= 0)
	    continue;		/* no redraw? what a pity */
	K_icons[cnt].redraw = 0;
	dst.h = video.field_size;
	dst.w = video.field_size;
	switch (K_icons[cnt].type) {
	case KGAMEOBJ:
	    dst.x = K_icons[cnt].scrx * video.field_size + k_view.offsetx;
	    dst.y = K_icons[cnt].scry * video.field_size + k_view.offsety;
		color = skins[selected_skin].designer_GAMEOBJ_tile_colour;
	    break;
	case KOBJ_S:
	    dst.y = K_icons[cnt].scry * video.field_size + k_view.offsety;
	    dst.x = K_icons[cnt].scrx * video.field_size + k_view.offsetx;
		color = skins[selected_skin].designer_OBJS_tile_colour;
	    break;
	case KOBJ_V:
	    dst.x = K_icons[cnt].scrx * video.field_size + k_view.offsetx;
	    dst.y = K_icons[cnt].scry * video.field_size + k_view.offsety;
		color = skins[selected_skin].designer_OBJV_tile_colour;
	    break;
	}

	SDL_FillRect(screen, &dst, SDL_MapRGB(screen->format,
		color >> 16 & 0xff, color >> 8 & 0xff, color & 0xff));

	switch (K_icons[cnt].game_obj) {
	case BOMB:
		if(bombtype==1) {
		 	k_geticoncoords(8,7,&src);
		} else k_geticoncoords(9,1,&src);
	    SDL_BlitSurface(icons, &src, screen, &dst);
	    break;
	case WALL:
	    switch (wallnumber) {
	    case 0:
		k_geticoncoords(3, 1, &src);
		break;
	    case 1:
		k_geticoncoords(4, 1, &src);
		break;
	    case 2:
		k_geticoncoords(6, 3, &src);
		break;
	    case 3:
		k_geticoncoords(8, 2, &src);
		break;
	    case 4:
		k_geticoncoords(10, 2, &src);
		break;
	    case 5:
		k_geticoncoords(9, 6, &src);
		break;
	    case 6:
		k_geticoncoords(10, 6, &src);
		break;
	    case 7:
		k_geticoncoords(11, 1, &src);
		break;
	    case 8:
		k_geticoncoords(11, 2, &src);
		break;
	    case 9:
		k_geticoncoords(1,9,&src);
		break;
	    }
	    SDL_BlitSurface(icons, &src, screen, &dst);
	    break;
	case K_LTYPE:
	    k_geticoncoords(6, 8, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    k_geticoncoords(3, ltype + 1, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    break;
	case K_DIRECTION:
	    k_geticoncoords(2, direction + 4, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    break;
	case K_RMSTATE:
	    k_geticoncoords(6, 8, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    if (movable == 1) {
		if (rotable == 1)
		    k_geticoncoords(7, 7, &src);
		else
		    k_geticoncoords(3, 7, &src);
	    } else if (rotable == 1) {
		k_geticoncoords(2, 8, &src);
	    } else {
		k_geticoncoords(7, 8, &src);
	    }
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    break;
	case K_ERASER:
	case K_SAVE:
	case K_INSERT:
	    nx = K_icons[cnt].x;
	    ny = K_icons[cnt].y;
	    k_geticoncoords(nx, ny, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    break;
	case K_SIZE:
	    if (gsize >= 0 && gsize < 2) {
		nx = 6;
		ny = gsize + 5;
	    } else {
		nx = 7;
		ny = 5;
	    }
	    k_geticoncoords(nx, ny, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    break;
	case K_LASER:
	    if (direction == 0 || direction == 2) {
		/*
		 * vertical laser 
		 */
		k_geticoncoords(1, 4, &src);
		SDL_BlitSurface(icons, &src, screen, &dst);
	    } else {
		k_geticoncoords(3, 4, &src);
		SDL_BlitSurface(icons, &src, screen, &dst);
	    }
	    k_geticoncoords(5, direction + 5, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    break;
	case GUN:
	    k_geticoncoords(K_icons[cnt].x, K_icons[cnt].y, &src);
	    SDL_BlitSurface(icons, &src, screen, &dst);
	    nx = 7;
	    ny = ltype + 1;
	    k_geticoncoords(nx, ny, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    if (movable) {
		k_geticoncoords(5, direction + 5, &src);
		SDL_BlitSurface(k_icons, &src, screen, &dst);
	    }
	    if (rotable) {
		k_geticoncoords(6, 1, &src);
		SDL_BlitSurface(k_icons, &src, screen, &dst);
	    }
	    break;

	case MAGNET:
	    k_geticoncoords(K_icons[cnt].x, K_icons[cnt].y, &src);
	    SDL_BlitSurface(icons, &src, screen, &dst);
	    if (rotable) {
		k_geticoncoords(6, 1, &src);
		SDL_BlitSurface(k_icons, &src, screen, &dst);
	    }
	    break;
	case TELEPORT:
	    k_geticoncoords(K_icons[cnt].x, K_icons[cnt].y, &src);
	    SDL_BlitSurface(icons, &src, screen, &dst);
	    nx = 1 + (teleportnr / 9);
	    ny = (teleportnr % 9) + ((teleportnr > 8) ? 1 : 0);
	    k_geticoncoords(nx, ny, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    break;
	case BEAR:
	case BEAR_B:
	    k_geticoncoords(K_icons[cnt].x, K_icons[cnt].y, &src);
	    SDL_BlitSurface(icons, &src, screen, &dst);
	    k_geticoncoords(5, direction + 5, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    break;
	case BIRD:
	    k_geticoncoords(K_icons[cnt].x, K_icons[cnt].y, &src);
	    SDL_BlitSurface(icons, &src, screen, &dst);
	    k_geticoncoords(5, direction + 5, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    if (shooting) {
		k_geticoncoords(3, 4, &src);
		SDL_BlitSurface(k_icons, &src, screen, &dst);
	    }
	    break;
	case BARRIER:
	    k_geticoncoords(K_icons[cnt].x, K_icons[cnt].y, &src);
	    SDL_BlitSurface(icons, &src, screen, &dst);
	    k_geticoncoords(5, ((direction >= 2) ? 2 : 0) + 5, &src);
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	    break;
	default:
	    k_geticoncoords(K_icons[cnt].x, K_icons[cnt].y, &src);
	    if (K_icons[cnt].type == KGAMEOBJ)
		SDL_BlitSurface(icons, &src, screen, &dst);
	    else
		SDL_BlitSurface(k_icons, &src, screen, &dst);
	}
	/*
	 * manage blinks 
	 */
	if (K_icons[cnt].blink > 0) {	/* draw * blink */
	    switch (K_icons[cnt].blink2) {
	    case 0:
		k_geticoncoords(3, 6, &src);
		break;
	    case 1:
		k_geticoncoords(3, 5, &src);
		break;
	    };
	    SDL_BlitSurface(k_icons, &src, screen, &dst);
	}
    }
    if (inforedraw) {
	inforedraw = 0;

	fgcolor.r = skins[selected_skin].designer_text_colour >> 16 & 0xff;
	fgcolor.g = skins[selected_skin].designer_text_colour >> 8 & 0xff;
	fgcolor.b = skins[selected_skin].designer_text_colour & 0xff;
	bgcolor.r = skins[selected_skin].background_colour >> 16 & 0xff;
	bgcolor.g = skins[selected_skin].background_colour >> 8 & 0xff;
	bgcolor.b = skins[selected_skin].background_colour & 0xff;

	image = TTF_RenderUTF8_Shaded(font, infostring, fgcolor, bgcolor);
	dst.h = image->h;
	dst.x = (txt_x) * video.field_size + k_view.offsetx + video.field_size / 8;
	dst.y = (txt_y) * video.field_size + k_view.offsety;
	/* I've centred the text vertically within field_size */
	if (dst.h < video.field_size) dst.y += (video.field_size - dst.h) / 2;
	dst.w = (DEFAULT_VIEWPORT_WIDTH - 1) * video.field_size - video.field_size / 8;

	SDL_FillRect(screen, &dst, SDL_MapRGB(screen->format,
		bgcolor.r, bgcolor.g, bgcolor.b));

	dst.w = image->w;
	dst.h = image->h;
	SDL_BlitSurface(image, NULL, screen, &dst);
	SDL_FreeSurface(image);
    }
}

/*
 * helper function, used for save 
 */
char
k_transform_char(int c, int state)
{
    char            walls[] = "OQo-qpPsSa";
    switch (c) {
    case LASER_L:
	return 'L';
    case LASER_D:
	return 'l';
    case STOP:
	return 'X';
    case RADIOACTIVE_FIELD:
	return 'k';
    case EMPTY_FIELD:
	return '.';
    case ROBBO:
	return 'R';
    case WALL:
	if(state>9) state=9;
	return walls[state];
    case SCREW:
	return 'T';
    case BULLET:
	return '\'';
    case BOX:
	return '#';
    case KEY:
	return '%';
    case BOMB2:
	return 'B';
    case BOMB:
	return 'b';
    case DOOR:
	return 'D';
    case QUESTIONMARK:
	return '?';
    case BEAR:
	return '@';
    case BIRD:
	return '^';
    case CAPSULE:
	return '!';
    case GROUND:
	return 'H';
    case BEAR_B:
	return '*';
    case BUTTERFLY:
	return 'V';
    case TELEPORT:
	return '&';
    case GUN:
	return '}';
    case MAGNET:
	return 'M';
    case PUSH_BOX:
	return '~';
    case BARRIER:
	return '=';
    }
    return '.';
}



/*
 * Saves level the way it does it depends on variable insert, if it is
 * zero it would replace current level, if it is 1 it will append edited
 * level at the end of the set it needs handler to opne file and level
 * number 
 */

void
k_save_level(FILE * fdesc, int lvl)
{
    int             x,
                    y;
    char            line[1024];
    int             adds = 0;
    int             tn = 0;
    fprintf(fdesc, "%d\n", lvl);
    fprintf(fdesc, "[colour]\n");
    if (level.colour != UNDEFINED)	/* This means not set */
    	fprintf(fdesc, "%06x\n", level.colour);
    fprintf(fdesc, "[size]\n%d.%d\n", level.w, level.h);
    fprintf(fdesc, "[screws]\n%d\n",robbo.screws);
    fprintf(fdesc, "[author]\n");
    if (level.author[0] != 0) fprintf(fdesc, "%s\n", level.author);
    fprintf(fdesc, "[level_notes]\n");
    /* level.notes is terminated by an enforced newline */
    if (level.notes[0] != 0) fprintf(fdesc, "%s", level.notes);
    fprintf(fdesc, "[data]\n");
    for (y = 0; y < level.h; y++) {
	for (x = 0; x < level.w; x++) {
	    if (robbo.x == x && robbo.y == y) {
		line[x] = k_transform_char(ROBBO, 0);
	    } else {
		line[x] =
		    k_transform_char(board[x][y].type, board[x][y].state);
		switch (board[x][y].type) {
		case BARRIER:
		case LASER_L:
		case LASER_D:
		case GUN:
		case BIRD:
		case TELEPORT:
		case MAGNET:
		case BEAR:
		case BEAR_B:
		    adds++;
		};
	    }
	}
	line[x] = '\0';
	fprintf(fdesc, "%s\n", line);
    }
    fprintf(fdesc, "[additional]\n%d\n", adds);
    for (y = 0; y < level.h; y++) {
	for (x = 0; x < level.w; x++) {
	    switch (board[x][y].type) {
	    case LASER_L:
		fprintf(fdesc, "%d.%d.L.%d\n", x, y,
			board[x][y].direction);
		break;
	    case LASER_D:
		fprintf(fdesc, "%d.%d.l.%d\n", x, y,
			board[x][y].direction);
		break;
	    case BARRIER:
		fprintf(fdesc, "%d.%d.=.%d\n", x, y,
			board[x][y].direction);
		break;
	    case GUN:
		fprintf(fdesc, "%d.%d.}.%d.%d.%d.%d.%d.%d\n", x, y,
			board[x][y].direction, board[x][y].direction2,
			board[x][y].solidlaser, board[x][y].movable,
			board[x][y].rotable, board[x][y].randomrotated);
		break;
	    case BIRD:
		fprintf(fdesc, "%d.%d.^.%d.%d.%d\n", x, y,
			board[x][y].direction, board[x][y].direction2,
			board[x][y].shooting);
		break;
	    case BEAR:
		fprintf(fdesc, "%d.%d.@.%d\n", x, y,
			board[x][y].direction);
		break;
	    case BEAR_B:
		fprintf(fdesc, "%d.%d.*.%d\n", x, y,
			board[x][y].direction);
		break;
	    case MAGNET:
		if(board[x][y].rotable==0) /* neurocyp: non rotated magnets are saved as usual, rotated ones we save extended */
			fprintf(fdesc, "%d.%d.M.%d\n", x, y, board[x][y].state);
		else
			fprintf(fdesc, "%d.%d.M.%d.%d\n", x, y, board[x][y].state,board[x][y].rotable);
		
		break;
	    case TELEPORT:
		fprintf(fdesc, "%d.%d.&.%d.%d\n", x, y,
			board[x][y].teleportnumber, tn++);
		break;
	    }
	}
    }
    fprintf(fdesc, "[end]\n\n");
}


/*
 * this is kind of a functional stub 
 */
void
k_save_map()
{
    FILE           *fdesc,
                   *fdescr;
    int             empty = 0;
    int             maxlevel = K_UNDEFINED;
    int             lvl = K_UNDEFINED;
    int             skip_level = 0;
    char            line[1024];
    char            fname[1024];
    char            fname2[1024];

    sprintf(fname, "%s", level_packs[selected_pack].filename);

#ifndef PLATFORM_PSVITA
    sprintf(fname2, "%s%s", level_packs[selected_pack].filename, ".tmp");
#else
    char            tmp[1024];
    char            *str;
    char            *filename;
    char            dest_fname[1024];

    sprintf(tmp, "%s", level_packs[selected_pack].filename);
    str = strtok(tmp, "/");
    while (str != NULL)
    {
      filename = str;
      str = strtok (NULL, "/");
    }

    sprintf(dest_fname, "%s", CONFIG_DATA_DIR "/" LEVELS_DIR "/");
    strcat(dest_fname, filename);

    sprintf(fname2, "%s", CONFIG_DATA_DIR "/" LEVELS_DIR "/");
    strcat(fname2, filename);
    strcat(fname2, ".tmp");

#endif

    fdescr = fopen(fname, "r");
    fdesc = fopen(fname2, "w+");
    if ((fdesc == NULL) || (fdescr == NULL)) {
	/*
	 * oops it seems we cannot write the file 
	 */
	if (fdesc)
	    fclose(fdesc);
	if (fdescr)
	    fclose(fdescr);
	return;
    };
    while (!feof(fdescr)) {
	fgets(line, 1023, fdescr);
	if (line[0] == '\n' || line[0] == '\r') {
	    empty++;
	} else
	    empty = 0;
	/* Thunor: This is discarding a series of newlines which will
	 * remove them from level_notes. This isn't a problem now but
	 * it might be in the future so I'll mark it temp temp */
	if (empty > 2)
	    continue;
	if (skip_level == 1) {
	    if (!strncmp(line, "[end]", 5))
		skip_level = 0;
	    continue;
	}
	if ((lvl == K_FOUND) && (insert == 0)) {
	    sscanf(line, "%d\n", &lvl);
	    if (lvl == level_packs[selected_pack].level_selected) {
		k_save_level(fdesc, lvl);
		skip_level = 1;
		lvl = K_WRITTEN;
		continue;
	    }
	    lvl = K_UNDEFINED;
	}
	if (maxlevel == K_FOUND) {
	    sscanf(line, "%d\n", &maxlevel);
	    if (insert == 1)
		maxlevel++;
	    sprintf(line, "%d\n", maxlevel);
	}
	if (!strncmp(line, "[last_level]", 12))
	    maxlevel = K_FOUND;
	if (!strncmp(line, "[level]", 7) && lvl != K_WRITTEN)
	    lvl = K_FOUND;
	fprintf(fdesc, "%s", line);
    }
    if (insert == 1) {
	/*
	 * ok, now we have to add our level 
	 */
	fprintf(fdesc, "[level]\n");
	k_save_level(fdesc, maxlevel);
	level_packs[selected_pack].last_level++;
    }
    fclose(fdesc);
    fclose(fdescr);

#if defined(PLATFORM_PSVITA)
    sceIoRemove(dest_fname);
    sceIoRename(fname2, dest_fname);

    strcpy(level_packs[selected_pack].filename, dest_fname);
#elif defined(PLATFORM_PSP)
    sceIoRemove(fname);
    sceIoRename(fname2, fname);

    strcpy(level_packs[selected_pack].filename, fname);
#else
    /*
     * now all that is left to do is exchange the files 
     */
    unlink(fname);
    rename(fname2, fname);
#endif
}

/*
 * count objects on the map - comes in handy, when counting keys, or
 * bullets 
 */
int
k_count_obj(int obj)
{
    int             x,
                    y,
                    res = 0;
    for (y = 0; y < level.h; y++)
	for (x = 0; x < level.w; x++)
	    if (board[x][y].type == obj)
		res++;
    return res;
};

		    /*
		     * mark all icons to be redrawn 
		     */
void
k_redraw_all()
{
    int             cnt;
    int             x,
                    y;
    for (y = k_view.x; y < k_view.x + k_view.h; y++)
	for (x = k_view.y; x < k_view.y + k_view.w; x++) {
	    redraw_field(x, y);
	}
    for (cnt = 0; K_icons[cnt].x != -1; cnt++)
	K_icons[cnt].redraw = 1;
};
