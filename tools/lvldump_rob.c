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
/* This program requires an Atari Robbo RMB file that has been created with Poklik's Robbo Kreator.
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define VERSION "060"

/* Global variables */
char wall='q';
char author_string[256];
int level = 1;
int absolute_offset = 0;

int t0count = 0, t1count = 0, t2count = 0, t3count = 0, t4count = 0, t5count = 0, t6count = 0, t7count = 0, t8count = 0, t9count = 0;
char additional[4096];
int additional_count = 0;
int x = 0, y = 0;



int paleta[257][3]={
{0,0,0},{28,28,28},{57,57,57},{89,89,89},{121,121,121},{146,146,146},{171,171,171},{188,188,188},{205,205,205},{217,217,217},{230,230,230},{236,236,236},{242,242,242},{248,248,248},{255,255,255},{255,255,255},
{57,23,1},{94,35,4},{131,48,8},{165,71,22},{200,95,36},{227,120,32},{255,145,29},{255,171,29},{255,197,29},{255,206,52},{255,216,76},{255,230,81},{255,244,86},{255,249,119},{255,255,152},{255,255,152},
{69,25,4},{114,30,17},{159,36,30},{179,58,32},{200,81,34},{227,105,32},{255,129,30},{255,140,37},{255,152,44},{255,174,56},{255,197,69},{255,197,89},{255,198,109},{255,213,135},{255,228,161},{255,228,161},
{74,23,4},{126,26,13},{178,29,23},{200,33,25},{223,37,28},{236,59,56},{250,82,85},{252,97,97},{255,112,110},{255,127,126},{255,143,143},{255,157,158},{255,171,173},{255,185,189},{255,199,206},{255,199,206},
{5,5,104},{59,19,109},{113,34,114},{139,42,140},{165,50,166},{185,56,186},{205,62,207},{219,71,221},{234,81,235},{244,95,245},{254,109,255},{254,122,253},{255,135,251},{255,149,253},{255,164,255},{255,164,255},
{40,4,121},{64,9,132},{89,15,144},{112,36,157},{136,57,170},{164,65,195},{192,74,220},{208,84,237},{224,94,255},{233,109,255},{242,124,255},{248,138,255},{255,152,255},{254,161,255},{254,171,255},{254,171,255},
{53,8,138},{66,10,173},{80,12,208},{100,40,208},{121,69,208},{141,75,212},{162,81,217},{176,88,236},{190,96,255},{197,107,255},{204,119,255},{209,131,255},{215,144,255},{219,157,255},{223,170,255},{223,170,255},
{5,30,129},{6,38,165},{8,47,202},{38,61,212},{68,76,222},{79,90,238},{90,104,255},{101,117,255},{113,131,255},{128,145,255},{144,160,255},{151,169,255},{159,178,255},{175,190,255},{192,203,255},{192,203,255},
{12,4,139},{34,24,160},{56,45,181},{72,62,199},{88,79,218},{97,89,236},{107,100,255},{122,116,255},{138,132,255},{145,142,255},{153,152,255},{165,163,255},{177,174,255},{184,184,255},{192,194,255},{192,194,255},
{29,41,90},{29,56,118},{29,72,146},{28,92,172},{28,113,198},{50,134,207},{72,155,217},{78,168,236},{85,182,255},{112,199,255},{140,216,255},{147,219,255},{155,223,255},{175,228,255},{195,233,255},{195,233,255},
{47,67,2},{57,82,2},{68,97,3},{65,122,18},{62,148,33},{74,159,46},{87,171,59},{92,189,85},{97,208,112},{105,226,122},{114,245,132},{124,250,141},{135,255,151},{154,255,166},{173,255,182},{173,255,182},
{10,65,8},{13,84,10},{16,104,13},{19,125,15},{22,146,18},{25,165,20},{28,185,23},{30,201,25},{33,217,27},{71,228,45},{110,240,64},{120,247,77},{131,255,91},{154,255,122},{178,255,154},{178,255,154},
{4,65,11},{5,83,14},{6,102,17},{7,119,20},{8,136,23},{9,155,26},{11,175,29},{72,196,31},{134,217,34},{143,233,36},{153,249,39},{168,252,65},{183,255,91},{201,255,110},{220,255,129},{220,255,129},
{2,53,15},{7,63,21},{12,74,28},{45,95,30},{79,116,32},{89,131,36},{100,146,40},{130,161,46},{161,176,52},{169,193,58},{178,210,65},{196,217,69},{214,225,73},{228,240,78},{242,255,83},{242,255,83},
{38,48,1},{36,56,3},{35,64,5},{81,84,27},{128,105,49},{151,129,53},{175,153,58},{194,167,62},{213,181,67},{219,192,61},{225,203,56},{226,216,54},{227,229,52},{239,242,88},{251,255,125},{251,255,125},
{64,26,2},{88,31,5},{112,36,8},{141,58,19},{171,81,31},{181,100,39},{191,119,48},{208,133,58},{225,147,68},{237,160,78},{249,173,88},{252,183,92},{255,193,96},{255,198,113},{255,203,131},{255,203,131},
{255,203,131}
};





/* Function prototypes */
void convert_char(unsigned char ch);
void write_additional(void);
int read_level(FILE *fdesc,int level);

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
    

//        printf("\n%d\n",argc);
	/* Process command line arguments */
	if (argc != 7) {
		fprintf(stdout, "lvldump_rob %s\n", VERSION);
		fprintf(stdout, "Usage: lvldump -f file -a \"author\" -o offset -l n\n");
		fprintf(stdout, "Example: lvldump -f robboix -a \"XE-TEAM/JANUSOFT\" -o 0x50B5 -l 21\n");
		fprintf(stdout, "\nWhere:\n");
		fprintf(stdout, "  -f file      is the filename of a gunzipped atari800 save state file\n");
		fprintf(stdout, "  -a \"author\"  is the level author string\n");
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
			} else {
				fprintf(stdout, "Unrecognised option %s\n", argv[count]);
				return 1;
			}
		} 
	}
	
	/* Initialise some variables */
	
	/* Attempt to open the save state file */
	if((fp = fopen (filename, "rb")) == NULL) {
		fprintf(stdout, "Cannot open file %s\n", filename);
		return 1;	
	}
	read_level(fp,level);
	fclose(fp);
	return 0;
}

int read_level(FILE *fp,int level) 
{
        int seek_base=0x0241;
	int size=505;
	int begin=8;	
        int screws=0;
        int a,b,c;
	int red,green,blue,ataricol;
	int level_start=seek_base+(size*(level-1));
	int level_end=level_start+begin+(16*31);
	int offset=0;
	unsigned char ch;
	char header=0;
//	printf("%d\n%x\n%x\n",level,level_start,level_end);
	while (!feof(fp) && offset <= level_end) {
	//	printf("%x ",offset);
		fread(&ch, sizeof(char), 1, fp);
	        if(offset==level_start+begin) {
		    ataricol=(int)ch;
		    red=paleta[ataricol][0];
		    green=paleta[ataricol][1];
		    blue=paleta[ataricol][2];
		//    printf("%d\n",ataricol);
		}		        
		if(offset==level_start+2){
		    a=(int)ch;
		    offset++;
		    fread(&ch, sizeof(char), 1, fp);
                    b=(int)ch;
                    c=a+256*b;
//	printf("%d %d %d\n%x\n\n",a,b,c,seek_base+(size*(level-1)));

		}
		if (offset > level_start+begin) {
			/* Start dumping */
			if (!header) {
				fprintf(stdout, "[offset]\n%X\n[level]\n%i\n[colour]\n%x%x%x\n[screws]\n%x\n[size]\n16.31\n[author]\n%s\n[data]\n", offset, level,red,green,blue,c, author_string);
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

}



/***************************************************************************
 * Convert Char                                                            *
 ***************************************************************************/

void convert_char(unsigned char ch) {
	char temp[50];

	/* Convert the hex digit into something GNU Robbo can use */
	switch (ch) {
		case 0x06:
		case 0x08:
			printf("~");
			break;
		case 0x05:	//TEMP: 0x11 and 0x05 are barrier entry/exit points and appear as wall.
		case 0x00:	//Wall in RobboVII and RobboVIII
		case 0x11:
			printf("%c",wall);
			break;
		case 0x0F:
			printf("=");
			break;
		case 0x13:
			printf("-");
			break;
		case 0x15: /* 0x15 is actually active capsule, but we do not supprt these in gnu robbo */
		case 0x14:
			printf("!");
			break;
		case 0x20:
		case 0xe4:
			printf(".");
			break;
		case 0x02:
			printf("Q"); /* bomb type 2 in robbo alex */
			break;
		case 0xa0:	//Wall in RobboVI (Original.dat)
		case 0x5c:		//Bright wall in RobboVI (Original.dat)
		/* Should be Q */	
			printf("%c",wall);
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
		case 0x0d:
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
		case 0x17:	//Gun blaster firing down.
			sprintf(temp, "%i.%i.}.1.0.2.0.0.0\n", x, y);
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
		
		case 0x2f:
		case 0x2c:
		case 0x2e:	//Gun random rotatable.
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
//		case 0x0D:
//		case 0x17:
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
			printf(".");
			break;
	}
}

/***************************************************************************
 * Write Additional                                                        *
 ***************************************************************************/

void write_additional(void) {
	printf("[additional]\n%i\n%s[end]\n\n", additional_count, additional);
}











