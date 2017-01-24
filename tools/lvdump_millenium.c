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


/***************************************************************************
 * How to use this program                                                 *
 ***************************************************************************/
/* This program requires an Robbo Millenium datafiles */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h> 
#define VERSION "063"

/* Global variables */

char wall='Q';
char author_string[256];
int level = 1;
int absolute_offset = 0;

int t0count = 0, t1count = 0, t2count = 0, t3count = 0, t4count = 0, t5count = 0, t6count = 0, t7count = 0, t8count = 0, t9count = 0;
char additional[2048];
int additional_count = 0;
int x = 0, y = 0;

/* Function prototypes */
void convert_char(unsigned char ch);
void write_additional(void);


/***************************************************************************
 * Main                                                                    *
 ***************************************************************************/

int main(int argc, char *argv[]) {
	char filename[256];
	FILE *fp;
	unsigned int offset = 0;
	int header = 0, count, level_start, level_end;
	char ch;
	int rr;
	struct timeval tv;

	gettimeofday(&tv,NULL);
	
	srand((unsigned int)(tv.tv_usec));
	rr=rand();

	 rr=(int) (rr/(RAND_MAX/6))+1;
	switch (rr) {
	    case 1:
		wall='Q';
		break;
	    case 2:
		wall='O';
		break;
	    case 3:
		wall='o';
		break;
	    case 4:
		wall='p';
		break;
	    case 5:
		wall='P';
		break;
	    case 6:
		wall='s';
		break;
	    }
    

	/* Process command line arguments */
	if (argc != 9) {
		fprintf(stdout, "lvldump %s\n", VERSION);
		fprintf(stdout, "Usage: lvldump -f file -a \"author\" -o offset -l n\n");
		fprintf(stdout, "Example: lvldump -f robboix -a \"XE-TEAM/JANUSOFT\" -o 0x50B5 -l 21\n");
		fprintf(stdout, "\nWhere:\n");
		fprintf(stdout, "  -f file      is the filename of a gunzipped atari800 save state file\n");
		fprintf(stdout, "  -a \"author\"  is the level author string\n");
		fprintf(stdout, "  -o offset    is the absolute offset of the level data (0x50A3 for\n");
		fprintf(stdout, "                 Robbo VI, VII and VIII, 0x50B5 for Robbo IX), not the\n");
		fprintf(stdout, "                 offset of this level\n");
		fprintf(stdout, "                 offset 0 for processingRobbo Millenium dta files\n");
		fprintf(stdout, "  -l n         is the level to dump to the console\n");
		fprintf(stdout, "  --help       displays this help\n");
		fprintf(stdout, "\nReport bugs to :-\n");
		fprintf(stdout, "http://sourceforge.net/projects/gnurobbo\n");
		return 1;
	} else {
		for (count = 1; count < argc; count++) {
			if (!strcmp(argv[count], "--help")) {
			} else if (!strcmp(argv[count], "-f")) {
				strcpy(filename, argv[++count]);
			} else if (!strcmp(argv[count], "-a")) {
				strcpy(author_string, argv[++count]);
			} else if (!strcmp(argv[count], "-l")) {
				level = atoi(argv[++count]);
			} else if (!strcmp(argv[count], "-o")) {
				 sscanf(argv[++count], "%x", &absolute_offset);
			} else {
				fprintf(stdout, "Unrecognised option %s\n", argv[count]);
				return 1;
			}
		} 
	}
	
	/* Initialise some variables */
	level_start = absolute_offset + (level - 1) * 496;
	level_end = level_start + 16 * 31;	/* 0x1F0 blocks */
	
	/* Attempt to open the save state file */
	if((fp = fopen (filename, "rb")) == NULL) {
		fprintf(stdout, "Cannot open file %s\n", filename);
		return 1;	
	}

	while (!feof(fp) && offset < level_end) {
		fread(&ch, sizeof(char), 1, fp);
		if (offset >= level_start) {
			/* Start dumping */
			if (!header) {
				fprintf(stdout, "[offset]\n%X\n[level]\n%i\n[colour]\n[size]\n16.31\n[author]\n%s\n[data]\n", offset, level, author_string);
				header++;
			}
			convert_char(ch);
			x++;
			if (x % 16 == 0) {
				x = 0;
				y++;
				printf("\n");
			}
		}
		offset++;
	}
	
	write_additional();

	fclose(fp);
	return 0;
}

/***************************************************************************
 * Convert Char                                                            *
 ***************************************************************************/

void convert_char(unsigned char ch) {
	char temp[50];

	/* Convert the hex digit into something GNU Robbo can use */
	switch (ch) {
		case 0x06:
			printf("~"); // pushbox (ruchoma kratka)
			break;
		case 0x05:	//TEMP: 0x11 and 0x05 are barrier entry/exit points and appear as wall.
		case 0x00:	//Wall in RobboVII and RobboVIII
		case 0x11:
			printf("q"); // fat wall
			break;
		case 0x0F:
			printf("="); // barrier
			break;
		case 0x13:
			printf("-");
			break;
		case 0x14:
			printf("!");
			break;
		case 0x20:
			printf(".");
			break;
//		case '\xa0':	//Wall in RobboVI (Original.dat)
//		case 0x5c:		//Bright wall in RobboVI (Original.dat)
		case 0xDB:		
	    		printf("%c",wall);
			break;
		case 0x54: // this is the "stop" sign in original robbo millenium, but now it is converted to ammo
			printf("X");
			break;
		case 0x21:
			printf("'");
			break;
		case 0x23:
			printf("#");
			break;
		case 0x24:
			printf("T");
			break;
		case 0x25:
			printf("H");
			break;
		case 0x26:
			printf("V");
			break;
		case 0x2a:
			printf("R");
			break;
		case 0x2b:
			printf("+");
			break;
		case 0x3d:
			printf("%%");
			break;
		case 0x3f:
			printf("?");
			break;
		case 0x40:
			printf("b");
			break;
		case 0x7c:	//Door horizontal.
		case 0x12:	//Door vertical!
			printf("D");
			break;
		case 0x58:
			printf("k");  // this should be implemented brand new obstacle which is killing, when robbo tries to walk on it
			break;
		case 0x7b: // magnet down
			sprintf(temp, "%i.%i.M.1\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("M");
			break;
		    
		case 0x7d: // Magnet Up - there is possible magnet down too
			sprintf(temp, "%i.%i.M.3\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("M");
			break;
		case 0x28:	//Magnet right.
			sprintf(temp, "%i.%i.M.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("M");
			break;
		case 0x29:	//Magnet left.
			sprintf(temp, "%i.%i.M.2\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("M");
			break;
		case 0x30:	//Teleport 0
			sprintf(temp, "%i.%i.&.10.%i\n", x, y, t0count);
			strcat(additional, temp);
			additional_count++;
			t0count++;
			printf("&");
			break;
		case 0x31:	//Teleport 1
			sprintf(temp, "%i.%i.&.1.%i\n", x, y, t1count);
			strcat(additional, temp);
			additional_count++;
			t1count++;
			printf("&");
			break;
		case 0x32:	//Teleport 2
			sprintf(temp, "%i.%i.&.2.%i\n", x, y, t2count);
			strcat(additional, temp);
			additional_count++;
			t2count++;
			printf("&");
			break;
		case 0x33:	//Teleport 3
			sprintf(temp, "%i.%i.&.3.%i\n", x, y, t3count);
			strcat(additional, temp);
			additional_count++;
			t3count++;
			printf("&");
			break;
		case 0x34:	//Teleport 4
			sprintf(temp, "%i.%i.&.4.%i\n", x, y, t4count);
			strcat(additional, temp);
			additional_count++;
			t4count++;
			printf("&");
			break;
		case 0x35:	//Teleport 5
			sprintf(temp, "%i.%i.&.5.%i\n", x, y, t5count);
			strcat(additional, temp);
			additional_count++;
			t5count++;
			printf("&");
			break;
		case 0x36:	//Teleport 6
			sprintf(temp, "%i.%i.&.6.%i\n", x, y, t6count);
			strcat(additional, temp);
			additional_count++;
			t6count++;
			printf("&");
			break;
		case 0x37:	//Teleport 7
			sprintf(temp, "%i.%i.&.7.%i\n", x, y, t7count);
			strcat(additional, temp);
			additional_count++;
			t7count++;
			printf("&");
			break;
		case 0x38:	//Teleport 8
			sprintf(temp, "%i.%i.&.8.%i\n", x, y, t8count);
			strcat(additional, temp);
			additional_count++;
			t8count++;
			printf("&");
			break;
		case 0x39:	//Teleport 9
			sprintf(temp, "%i.%i.&.9.%i\n", x, y, t9count);
			strcat(additional, temp);
			additional_count++;
			t9count++;
			printf("&");
			break;
		case 0x0E:	//Gun moving left-right firing up.
			sprintf(temp, "%i.%i.}.3.0.0.1.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x18:	//Gun blaster firing up.
			sprintf(temp, "%i.%i.}.3.0.2.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x5e:	//Gun solid firing up.
			sprintf(temp, "%i.%i.}.3.0.1.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x2e:	//Gun random rotatable.
			sprintf(temp, "%i.%i.}.0.0.0.0.1.1\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x2c:	//Gun random rotatable.
			sprintf(temp, "%i.%i.}.0.0.0.0.1.1\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;

		case 0x04:	//Gun blaster firing left.
			sprintf(temp, "%i.%i.}.2.0.2.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x3e:	//Gun solid firing right.
			sprintf(temp, "%i.%i.}.0.0.1.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x22:	//Gun solid firing down.
			sprintf(temp, "%i.%i.}.1.0.1.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x1c:	//Gun solid firing up.
			sprintf(temp, "%i.%i.}.3.0.1.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x1f:	//Gun firing right.
			sprintf(temp, "%i.%i.}.0.0.0.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x3c:	//Gun solid firing left.
			sprintf(temp, "%i.%i.}.2.0.1.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x1d:	//Gun firing down.
			sprintf(temp, "%i.%i.}.1.0.0.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x01:	//Gun blaster firing right.
			sprintf(temp, "%i.%i.}.0.0.2.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x27:	//Gun solid firing down.
			sprintf(temp, "%i.%i.}.1.0.1.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x2d:	//Gun random rotatable definitely not solid.
			sprintf(temp, "%i.%i.}.0.0.0.0.1.1\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x1e:	//Gun firing left.
			sprintf(temp, "%i.%i.}.2.0.0.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("}");
			break;
		case 0x0D:
		case 0x17:
			printf("?");
			break;
		case 0x44: //Bear left
			sprintf(temp, "%i.%i.@.2\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("@");
			break;
		case 0x43:	//Bear right
			sprintf(temp, "%i.%i.@.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("@");
			break;
		case 0x41: //Bear up.
			sprintf(temp, "%i.%i.@.3\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("@");
			break;
		case 0x42:	//Bear down.
			sprintf(temp, "%i.%i.@.1\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("@");
			break;
		case 0x45:	//Black Bear down
			sprintf(temp, "%i.%i.*.1\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("*");
			break;
		case 0x46:	//Black Bear up
			sprintf(temp, "%i.%i.*.3\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("*");
			break;
		case 0x47:	//Black Bear left.
			sprintf(temp, "%i.%i.*.2\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("*");
			break;
		case 0x48:	//Black Bear right.
			sprintf(temp, "%i.%i.*.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("*");
			break;
		case 0x4d:	//Bird moving left-right firing down probably direction 2.
			sprintf(temp, "%i.%i.^.2.1.1\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("^");
			break;
		case 0x4e:	//Bird moving left-right firing down direction 0.
			sprintf(temp, "%i.%i.^.0.1.1\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("^");
			break;
		case 0x49:	//Bird moving left-right direction 2
			sprintf(temp, "%i.%i.^.2.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("^");
			break;
		case 0x4a:	//Bird moving left-right direction 0
			sprintf(temp, "%i.%i.^.0.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("^");
			break;
		case 0x4b:	//Bird moving up-down probably direction 3.
			sprintf(temp, "%i.%i.^.3.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("^");
			break;

		case 0x4c:	//Bird moving up-down.
			sprintf(temp, "%i.%i.^.1.0.0\n", x, y);
			strcat(additional, temp);
			additional_count++;
			printf("^");
			break;
		default:
			printf("%02hhx ", ch);
			break;
	}
}

/***************************************************************************
 * Write Additional                                                        *
 ***************************************************************************/

void write_additional(void) {
	printf("[additional]\n%i\n%s[end]\n\n", additional_count, additional);
}











