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

/* ÄÖßäöü <- German -> äöüÄÖÜß */
/* óąćęłńŚśźż <- Polish ->  */
/* ÚáäéíôúýČčĎďľňŠšťž <- Slovak ->  */
/* ÄÅÖäåö <- Swedish ->  */
/* АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ <- Russian -> абвгдеёжзийклмнопрстуфхцчшщъыьэюя */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define VERSION "0.2"

char file1[50];
char file2[50];

int main(int argc, char *argv[]) {
	FILE *fp1;
	unsigned char ch1;
	unsigned int offset = 0; // 0x00000000.
	int sequence_found = FALSE;
		
	//Process any command line arguments.
	if (argc < 2 || !strcmp(argv[1], "--help")) {
		fprintf(stdout,"cat non-ASCII UTF-8 byte sequences from a UTF-8 text file to the console.\nUsage: catutf8 FILE\nExample: catutf8 myutf8file.txt | sort -u\n");
		return 0;
	}
	
	strcpy(file1, argv[1]);
	
	if((fp1 = fopen (file1, "rb")) == NULL) {
		fprintf(stdout, "Cannot open file %s\n", file1);
		exit(1);	
	}

	while (!feof(fp1)) {
		fread(&ch1, sizeof(char), 1, fp1);

		if ((ch1 >= 0xc2 && ch1 <= 0xf4) && !sequence_found) {
			sequence_found = TRUE;
			fprintf(stdout, "\n%02X", ch1);
		} else if ((ch1 >= 0xc2 && ch1 <= 0xf4) && sequence_found) {
			fprintf(stdout, "\n%02X", ch1);
		} else if (ch1 >= 0x80) {
			fprintf(stdout, " %02X", ch1);
		} else {
			sequence_found = FALSE;
		}

		offset++;
	}
	fprintf(stdout, "\n");
	
	fclose(fp1);
	
	return 0;
}

