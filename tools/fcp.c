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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define VERSION "0.2"

char file1[50];
char file2[50];

int main(int argc, char *argv[]) {
	FILE *fp1, *fp2;
	char ch1, ch2;
	unsigned int offset = 0; // 0x00000000.
	short int difference = -1;	//-1 means any difference.
		
	//Process any command line arguments.
	if (argc < 3 || argc > 4 || !strcmp(argv[1], "--help")) {
		fprintf(stdout,"fcp (file compare) dumps the differences between two binary files.\nYou can optionally pass a value for difference: 0 means find all that are equal.\nThis is sort of a rudimentary Trainer.\nUsage: fcp FILE1 FILE2 [0-9]\n");
		return 0;
	}
	
	strcpy(file1, argv[1]);
	strcpy(file2, argv[2]);
	
	if (argc > 3) {
		if (isdigit(*argv[3])) {
			difference = *argv[3] - 0x30;
		} else {
			fprintf(stdout,"Difference must be between 0 and 9.\n");
			return 1;
		}
	} else {
		difference = -1;
	}
	
	if((fp1 = fopen (file1, "rb")) == NULL) {
		fprintf(stdout, "Cannot open file %s\n", file1);
		exit(1);	
	}
	if((fp2 = fopen (file2, "rb")) == NULL) {
		fprintf(stdout, "Cannot open file %s\n", file2);
		exit(1);	
	}

	while (!feof(fp1) && !feof(fp2)) {
		fread(&ch1, sizeof(char), 1, fp1);
		fread(&ch2, sizeof(char), 1, fp2);
		if (difference == -1) {
			if (ch2 != ch1) fprintf(stdout, "%08X: %02hhX %02hhX\n", offset, ch1, ch2);
		} else {
			if (ch2 == ch1 + difference) fprintf(stdout, "%08X: %02hhX %02hhX\n", offset, ch1, ch2);
		}
		offset++;
	}
	
	fclose(fp1);
	fclose(fp2);
	
	return 0;
}

