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
#if defined(PLATFORM_WIN32)
	#define RESOURCE_FILE "gnurobborc"
#elif defined(PLATFORM_PC)
	#define RESOURCE_FILE ".gnurobborc"
#elif defined(PLATFORM_GP2X) || defined(PLATFORM_CAANOO)
	#define RESOURCE_FILE "gnurobborc"
#elif defined(PLATFORM_ZAURUS)
	#define RESOURCE_FILE ".gnurobborc"
#elif defined(PLATFORM_FREMANTLE)
	#define RESOURCE_FILE "gnurobborc"
#elif defined(PLATFORM_PSP)
	#define RESOURCE_FILE ".gnurobborc"
#elif defined(PLATFORM_PSVITA)
	#define RESOURCE_FILE "gnurobborc"
#endif

#define RCFILE_SAVE_ON_EXIT 0
#define RCFILE_SAVE_ON_CHANGE 1

/* Variables */
char path_resource_file[100];

struct {
	int save_frequency;
} rcfile;

int temp_rcfile_save_frequency;

/* Function prototypes */
int read_resource_file(char *filename);
int save_resource_file(char *filename, int inform);




