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
#define MAX_LEVEL_PACKS 84
#define LOCAL_DATA_DIR ".gnurobbo"
#if defined(PLATFORM_FREMANTLE)
	#define LOCAL_DATA_DIR "MyDocs/.gnurobbo/"
#endif
#define LEVELS_DIR "levels"
#define DEFAULT_USER_LEVEL_PACK "mylevels.dat"
#define DEFAULT_LEVEL_PACK "original.dat"
#define DEFAULT_LEVEL_START 1	/* Start level for Robbo */
#define DEFAULT_LEVEL_COLOUR 0x608050

/* Variables */
struct {
	int w;
	int h;
	char author[60];		/* The text displayed on the authorline above the viewport */
	int now_is_blinking;	/* When TRUE this will change the level background colour to white */
	Uint32 colour;			/* The colour if found or the default_level_colour if found in the level pack */
	Uint32 colour_override;	/* The colour if found or the default_level_colour if found in the skinrc */
	char notes[1024];		/* This is displayed nowhere now, but contains information about notes */
} level;

struct pack {
	char filename[256];		/* e.g. /usr/local/share/gnurobbo/levels/original.dat or ~/.gnurobbo/levels/mylevels.dat */
	char name[20];			/* e.g. Original */
	int last_level;			/* e.g. 53 */
	int level_reached;		/* e.g. 14 */
	int level_selected;		/* e.g. 3 is currently selected by the user */
	int selected;			/* e.g. This pack is currently selected by the user */
};
struct pack level_packs[MAX_LEVEL_PACKS];

int found_pack_count;		/* How many packs were physically found */
int level_pack_count;		/* How many packs are in the list (historical entries from the rc are added to the end) */
int selected_pack;			/* Which pack in the list is currently selected */

/* Function prototypes */
int level_init(void);
void create_userpack(void);
int find_all_dat_files(void);
void read_level_packs(void);
void sort_level_packs(void);
int load_level_data(int level_number);




