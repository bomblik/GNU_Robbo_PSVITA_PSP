#ifdef USE_PIXMAP_FONT

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
#define DEBUG_RECORD_FONT_DIMENSIONS
*/

#define FONT_WIDTH_MARKER_COLOUR 0x081828	/* 16 bit colours have a common granularity of 8 */
#define FONT_CHAR_COLOUR 0xf8e8d8

#define FONT_CHAR_COUNT 223		/* There's currently an extra slot at pixmap end */

#define FONT_UNSUPPORTED_CODE_POINT 95
#define FONT_CODE_POINT_C3_84 96			/* Ä */
#define FONT_CODE_POINT_C3_96 97			/* Ö */
#define FONT_CODE_POINT_C3_9C 98			/* Ü */
#define FONT_CODE_POINT_C3_9F 99			/* ß */
#define FONT_CODE_POINT_C3_A4 100			/* ä */
#define FONT_CODE_POINT_C3_B6 101			/* ö */
#define FONT_CODE_POINT_C3_BC 102			/* ü German <^ */
#define FONT_CODE_POINT_C4_84 103			/* Ą */
#define FONT_CODE_POINT_C4_86 104			/* Ć */
#define FONT_CODE_POINT_C4_98 105			/* Ę */
#define FONT_CODE_POINT_C5_81 106			/* Ł */
#define FONT_CODE_POINT_C5_83 107			/* Ń */
#define FONT_CODE_POINT_C3_93 108			/* Ó */
#define FONT_CODE_POINT_C5_9A 109			/* Ś */
#define FONT_CODE_POINT_C5_B9 110			/* Ź */
#define FONT_CODE_POINT_C5_BB 111			/* Ż */
#define FONT_CODE_POINT_C4_85 112			/* ą */
#define FONT_CODE_POINT_C4_87 113			/* ć */
#define FONT_CODE_POINT_C4_99 114			/* ę */
#define FONT_CODE_POINT_C5_82 115			/* ł */
#define FONT_CODE_POINT_C5_84 116			/* ń */
#define FONT_CODE_POINT_C3_B3 117			/* ó */
#define FONT_CODE_POINT_C5_9B 118			/* ś */
#define FONT_CODE_POINT_C5_BA 119			/* ź */
#define FONT_CODE_POINT_C5_BC 120			/* ż Polish <^	*/
#define FONT_CODE_POINT_C3_81 121			/* Á */
#define FONT_CODE_POINT_C4_8C 122			/* Č */
#define FONT_CODE_POINT_C4_8E 123			/* Ď */
#define FONT_CODE_POINT_C3_89 124			/* É */
#define FONT_CODE_POINT_C3_8D 125			/* Í */
#define FONT_CODE_POINT_C4_B9 126			/* Ĺ */
#define FONT_CODE_POINT_C4_BD 127			/* Ľ */
#define FONT_CODE_POINT_C5_87 128			/* Ň */
#define FONT_CODE_POINT_C3_94 129			/* Ô */
#define FONT_CODE_POINT_C5_94 130			/* Ŕ */
#define FONT_CODE_POINT_C5_A0 131			/* Š */
#define FONT_CODE_POINT_C5_A4 132			/* Ť */
#define FONT_CODE_POINT_C3_9A 133			/* Ú */
#define FONT_CODE_POINT_C3_9D 134			/* Ý */
#define FONT_CODE_POINT_C5_BD 135			/* Ž */
#define FONT_CODE_POINT_C3_A1 136			/* á */
#define FONT_CODE_POINT_C4_8D 137			/* č */
#define FONT_CODE_POINT_C4_8F 138			/* ď */
#define FONT_CODE_POINT_C3_A9 139			/* é */
#define FONT_CODE_POINT_C3_AD 140			/* í */
#define FONT_CODE_POINT_C4_BA 141			/* ĺ */
#define FONT_CODE_POINT_C4_BE 142			/* ľ */
#define FONT_CODE_POINT_C5_88 143			/* ň */
#define FONT_CODE_POINT_C3_B4 144			/* ô */
#define FONT_CODE_POINT_C5_95 145			/* ŕ */
#define FONT_CODE_POINT_C5_A1 146			/* š */
#define FONT_CODE_POINT_C5_A5 147			/* ť */
#define FONT_CODE_POINT_C3_BA 148			/* ú */
#define FONT_CODE_POINT_C3_BD 149			/* ý */
#define FONT_CODE_POINT_C5_BE 150			/* ž Slovak <^ */
#define FONT_CODE_POINT_C3_85 151			/* Å */
#define FONT_CODE_POINT_C3_A5 152			/* å Swedish <^ */
#define FONT_CODE_POINT_D0_90 153			/* A */
#define FONT_CODE_POINT_D0_91 154			/* Б */
#define FONT_CODE_POINT_D0_92 155			/* В */
#define FONT_CODE_POINT_D0_93 156			/* Г */
#define FONT_CODE_POINT_D0_94 157			/* Д */
#define FONT_CODE_POINT_D0_95 158			/* Е */
#define FONT_CODE_POINT_D0_96 159			/* Ж */
#define FONT_CODE_POINT_D0_97 160			/* З */
#define FONT_CODE_POINT_D0_98 161			/* И */
#define FONT_CODE_POINT_D0_99 162			/* Й */
#define FONT_CODE_POINT_D0_9A 163			/* К */
#define FONT_CODE_POINT_D0_9B 164			/* Л */
#define FONT_CODE_POINT_D0_9C 165			/* М */
#define FONT_CODE_POINT_D0_9D 166			/* Н */
#define FONT_CODE_POINT_D0_9E 167			/* О */
#define FONT_CODE_POINT_D0_9F 168			/* П */
#define FONT_CODE_POINT_D0_A0 169			/* Р */
#define FONT_CODE_POINT_D0_A1 170			/* С */
#define FONT_CODE_POINT_D0_A2 171			/* Т */
#define FONT_CODE_POINT_D0_A3 172			/* У */
#define FONT_CODE_POINT_D0_A4 173			/* Ф */
#define FONT_CODE_POINT_D0_A5 174			/* Х */
#define FONT_CODE_POINT_D0_A6 175			/* Ц */
#define FONT_CODE_POINT_D0_A7 176			/* Ч */
#define FONT_CODE_POINT_D0_A8 177			/* Ш */
#define FONT_CODE_POINT_D0_A9 178			/* Щ */
#define FONT_CODE_POINT_D0_AA 179			/* Ъ */
#define FONT_CODE_POINT_D0_AB 180			/* Ы */
#define FONT_CODE_POINT_D0_AC 181			/* Ь */
#define FONT_CODE_POINT_D0_AD 182			/* Э */
#define FONT_CODE_POINT_D0_AE 183			/* Ю */
#define FONT_CODE_POINT_D0_AF 184			/* Я */
#define FONT_CODE_POINT_D0_B0 185			/* а */
#define FONT_CODE_POINT_D0_B1 186			/* б */
#define FONT_CODE_POINT_D0_B2 187			/* в */
#define FONT_CODE_POINT_D0_B3 188			/* г */
#define FONT_CODE_POINT_D0_B4 189			/* д */
#define FONT_CODE_POINT_D0_B5 190			/* е */
#define FONT_CODE_POINT_D0_B6 191			/* ж */
#define FONT_CODE_POINT_D0_B7 192			/* з */
#define FONT_CODE_POINT_D0_B8 193			/* и */
#define FONT_CODE_POINT_D0_B9 194			/* й */
#define FONT_CODE_POINT_D0_BA 195			/* к */
#define FONT_CODE_POINT_D0_BB 196			/* л */
#define FONT_CODE_POINT_D0_BC 197			/* м */
#define FONT_CODE_POINT_D0_BD 198			/* н */
#define FONT_CODE_POINT_D0_BE 199			/* о */
#define FONT_CODE_POINT_D0_BF 200			/* п */
#define FONT_CODE_POINT_D1_80 201			/* р */
#define FONT_CODE_POINT_D1_81 202			/* с */
#define FONT_CODE_POINT_D1_82 203			/* т */
#define FONT_CODE_POINT_D1_83 204			/* у */
#define FONT_CODE_POINT_D1_84 205			/* ф */
#define FONT_CODE_POINT_D1_85 206			/* х */
#define FONT_CODE_POINT_D1_86 207			/* ц */
#define FONT_CODE_POINT_D1_87 208			/* ч */
#define FONT_CODE_POINT_D1_88 209			/* ш */
#define FONT_CODE_POINT_D1_89 210			/* щ */
#define FONT_CODE_POINT_D1_8A 211			/* ъ */
#define FONT_CODE_POINT_D1_8B 212			/* ы */
#define FONT_CODE_POINT_D1_8C 213			/* ь */
#define FONT_CODE_POINT_D1_8D 214			/* э */
#define FONT_CODE_POINT_D1_8E 215			/* ю */
#define FONT_CODE_POINT_D1_8F 216			/* я */
#define FONT_CODE_POINT_D0_81 217			/* Ё */
#define FONT_CODE_POINT_D1_91 218			/* ё Russian <^ */
#define FONT_CODE_POINT_C4_9B 219			/* ě */
#define FONT_CODE_POINT_C5_99 220			/* ř */
#define FONT_CODE_POINT_C5_AF 221			/* ů Czech <^ */


/* Variables */
int char_dimensions[FONT_CHAR_COUNT][2];	/* Character in font, width marker x1 and width marker x2 */

/* Function prototypes */
SDL_Rect get_code_point_srcrect(SDL_Surface *font, const unsigned char *utf8);
Uint32 get_pixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);


/***************************************************************************
 * TTF Init                                                                *
 ***************************************************************************/

int TTF_Init(void) {
	return FALSE;	/* Hello, this is SDL_ttf and everything is fine, really ;) */
}

/***************************************************************************
 * TTF Quit                                                                *
 ***************************************************************************/

void TTF_Quit(void) {
}

/***************************************************************************
 * TTF OpenFont                                                            *
 ***************************************************************************/
/* This function :-
   * Loads a pixmap font
   * Sets the transparent colour 0xff00ff
   * Converts the surface to the same format as the screen for fast blitting
   * Sets a default maximum width for each character
   * Records the variable widths of each character if width markers exist in row 0 */

SDL_Surface *TTF_OpenFont(const char *file, int ptsize) {
	SDL_Surface *unconverted, *converted;
	int count, cx, x12;

	/* Load the bitmap */
	if ((unconverted = SDL_LoadBMP(file)) == NULL) {
		fprintf(stdout,"Cannot load bitmap: %s\n", file);
		return NULL;
	}
	/* Set the transparent colour */
	if (SDL_SetColorKey(unconverted, SDL_SRCCOLORKEY, SDL_MapRGB(unconverted->format, 0xff, 0x00, 0xff)) < 0) {
		fprintf(stdout,"Cannot set surface colour key: %s\n", SDL_GetError());
		return NULL;
	}
	/* Convert the loaded surface to the same pixel format as the screen for fast blitting */
	converted = SDL_DisplayFormat(unconverted);
	SDL_FreeSurface(unconverted);
	if (converted == NULL) {
		fprintf(stdout, "Cannot create surface: %s\n", SDL_GetError());
		return NULL;
	}

	/* Record the dimensions of each individual character within the font */
	if (SDL_MUSTLOCK(converted)) SDL_LockSurface(converted);
	for (count = 0; count < FONT_CHAR_COUNT; count++) {
		/* Set default fixed width dimensions */
		char_dimensions[count][0] = count * converted->w / FONT_CHAR_COUNT;
		char_dimensions[count][1] = (count + 1) * converted->w / FONT_CHAR_COUNT - 1;
		x12 = 0;
		/* If width markers exist then use those */
		for (cx = char_dimensions[count][0]; cx <= char_dimensions[count][1]; cx++) {
			if (get_pixel(converted, cx, 0) == SDL_MapRGB(converted->format, 
				FONT_WIDTH_MARKER_COLOUR >> 16 & 0xff, FONT_WIDTH_MARKER_COLOUR >> 8 & 0xff, 
				FONT_WIDTH_MARKER_COLOUR & 0xff)) {
				if (x12 == 0) {
					char_dimensions[count][x12] = cx;
					/* Adjust to compensate for the x2 scaled-up font with double width markers */
					if (video.field_size == 32) cx++;
				} else if (x12 == 1) {
					/* Adjust to compensate for the x2 scaled-up font with double width markers */
					if (video.field_size == 32) cx++;
					char_dimensions[count][x12] = cx;
				}
				if (++x12 > 1) break;
			}
		}
		#ifdef DEBUG_RECORD_FONT_DIMENSIONS
			printf("char %i: %i -> %i\n", count, char_dimensions[count][0], char_dimensions[count][1]);
		#endif
	}
	if (SDL_MUSTLOCK(converted)) SDL_UnlockSurface(converted);

	return converted;
}

/***************************************************************************
 * TTF CloseFont                                                           *
 ***************************************************************************/

void TTF_CloseFont(SDL_Surface *font) {
	if (font) SDL_FreeSurface(font);	
}

/***************************************************************************
 * TTF RenderUTF8 Shaded                                                   *
 ***************************************************************************/
/* This function renders UTF-8 text using a pixmap font and then fills the
 * text with the foreground colour. To achieve this it requires that the
 * pixmap font is drawn using FONT_CHAR_COLOUR and so this colour should not
 * be used as the background colour. I have chosen a unique enough colour not
 * to cause a problem */

SDL_Surface *TTF_RenderUTF8_Shaded(SDL_Surface *font, const char *text, SDL_Color fg, SDL_Color bg) {
	int code_point_size, text_offset, character_spacing = -video.field_size / 16;
	int rendered_text_width, rendered_text_height;
	SDL_Surface *rendered_text = NULL;
	const unsigned char *utf8 = text;
	SDL_Rect srcrect, destrect;
	int fillx, filly;

	/* First calculate dimensions of rendered text */
	rendered_text_height = font->h - video.field_size / 16;
	rendered_text_width = text_offset = 0;
	while (*(utf8 + text_offset) != 0) {
		srcrect = get_code_point_srcrect(font, utf8 + text_offset);
		rendered_text_width += srcrect.w;
		rendered_text_width += character_spacing;
		/* Point to next code point being careful not to go past the terminating zero */
		if (*(utf8 + text_offset) >= 0xc2 && *(utf8 + text_offset) <= 0xdf) {
			code_point_size = 2;
		} else if (*(utf8 + text_offset) >= 0xe0 && *(utf8 + text_offset) <= 0xef) {
			code_point_size = 3;
		} else if (*(utf8 + text_offset) >= 0xf0 && *(utf8 + text_offset) <= 0xf4) {
			code_point_size = 4;
		} else {
			code_point_size = 1;
		}
		while (code_point_size--) {
			text_offset++;
			if (*(utf8 + text_offset) == 0) {
				rendered_text_width -= character_spacing;	/* We don't want character spacing adjustments after the last char */
				break;
			}
		}
	}

	/* Create a surface for the rendered text */
	if ((rendered_text = SDL_CreateRGBSurface(screen->flags, rendered_text_width, 
		rendered_text_height, screen->format->BitsPerPixel, screen->format->Rmask, 
		screen->format->Gmask, screen->format->Bmask, screen->format->Amask)) != NULL) {

		/* Fill it with the background colour */
		SDL_FillRect(rendered_text, NULL, SDL_MapRGB(rendered_text->format, bg.r, bg.g, bg.b));

		/* Render the text */
		destrect.x = destrect.y = text_offset = 0;
		while (*(utf8 + text_offset) != 0) {
			srcrect = get_code_point_srcrect(font, utf8 + text_offset);
			destrect.w = srcrect.w;
			destrect.h = srcrect.h;
			SDL_BlitSurface(font, &srcrect, rendered_text, &destrect);
			destrect.x += destrect.w + character_spacing;
			/* Point to next code point being careful not to go past the terminating zero */
			if (*(utf8 + text_offset) >= 0xc2 && *(utf8 + text_offset) <= 0xdf) {
				code_point_size = 2;
			} else if (*(utf8 + text_offset) >= 0xe0 && *(utf8 + text_offset) <= 0xef) {
				code_point_size = 3;
			} else if (*(utf8 + text_offset) >= 0xf0 && *(utf8 + text_offset) <= 0xf4) {
				code_point_size = 4;
			} else {
				code_point_size = 1;
			}
			while (code_point_size--) {
				text_offset++;
				if (*(utf8 + text_offset) == 0) break;
			}
		}

		/* Fill the text with the foreground colour */
		if (SDL_MUSTLOCK(rendered_text)) SDL_LockSurface(rendered_text);
		for (filly = 0; filly < rendered_text->h; filly++) {
			for (fillx = 0; fillx < rendered_text->w; fillx++) {
				if (get_pixel(rendered_text, fillx, filly) == SDL_MapRGB(rendered_text->format, 
					FONT_CHAR_COLOUR >> 16 & 0xff, FONT_CHAR_COLOUR >> 8 & 0xff, FONT_CHAR_COLOUR & 0xff)) {
					set_pixel(rendered_text, fillx, filly, 
						SDL_MapRGB(rendered_text->format, fg.r, fg.g, fg.b));
				}
			}
		}
		if (SDL_MUSTLOCK(rendered_text)) SDL_UnlockSurface(rendered_text);

	} else {
		fprintf(stdout,"Cannot create RGB surface: %s\n", SDL_GetError());
	}

	return rendered_text;	
}

/***************************************************************************
 * Get Code Point SrcRect                                                  *
 ***************************************************************************/
/* This translates a UTF-8 code point into an SDL_Rect within the font */

SDL_Rect get_code_point_srcrect(SDL_Surface *font, const unsigned char *utf8) {
	SDL_Rect srcrect;
	int index = FONT_UNSUPPORTED_CODE_POINT;
	
	srcrect.y = video.field_size / 16;
	srcrect.h = font->h - video.field_size / 16;

	/* Translate the code point to a rectangle within the font surface */
	if (*utf8 < 0x80) {
		/* < 0x80 is ASCII */
		index = *utf8 - 0x20;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x84) {		/* Ä */
		index = FONT_CODE_POINT_C3_84;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x96) {		/* Ö */
		index = FONT_CODE_POINT_C3_96;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x9c) {		/* Ü */
		index = FONT_CODE_POINT_C3_9C;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x9f) {		/* ß */
		index = FONT_CODE_POINT_C3_9F;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xa4) {		/* ä */
		index = FONT_CODE_POINT_C3_A4;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xb6) {		/* ö */
		index = FONT_CODE_POINT_C3_B6;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xbc) {		/* ü */
		index = FONT_CODE_POINT_C3_BC;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x84) {		/* Ą */
		index = FONT_CODE_POINT_C4_84;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x86) {		/* Ć */
		index = FONT_CODE_POINT_C4_86;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x98) {		/* Ę */
		index = FONT_CODE_POINT_C4_98;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x81) {		/* Ł */
		index = FONT_CODE_POINT_C5_81;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x83) {		/* Ń */
		index = FONT_CODE_POINT_C5_83;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x93) {		/* Ó */
		index = FONT_CODE_POINT_C3_93;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x9a) {		/* Ś */
		index = FONT_CODE_POINT_C5_9A;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xb9) {		/* Ź */
		index = FONT_CODE_POINT_C5_B9;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xbb) {		/* Ż */
		index = FONT_CODE_POINT_C5_BB;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x85) {		/* ą */
		index = FONT_CODE_POINT_C4_85;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x87) {		/* ć */
		index = FONT_CODE_POINT_C4_87;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x99) {		/* ę */
		index = FONT_CODE_POINT_C4_99;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x82) {		/* ł */
		index = FONT_CODE_POINT_C5_82;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x84) {		/* ń */
		index = FONT_CODE_POINT_C5_84;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xb3) {		/* ó */
		index = FONT_CODE_POINT_C3_B3;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x9b) {		/* ś */
		index = FONT_CODE_POINT_C5_9B;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xba) {		/* ź */
		index = FONT_CODE_POINT_C5_BA;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xbc) {		/* ż */
		index = FONT_CODE_POINT_C5_BC;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x81) {		/* Á */
		index = FONT_CODE_POINT_C3_81;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x8c) {		/* Č */
		index = FONT_CODE_POINT_C4_8C;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x8e) {		/* Ď */
		index = FONT_CODE_POINT_C4_8E;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x89) {		/* É */
		index = FONT_CODE_POINT_C3_89;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x8d) {		/* Í */
		index = FONT_CODE_POINT_C3_8D;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0xb9) {		/* Ĺ */
		index = FONT_CODE_POINT_C4_B9;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0xbd) {		/* Ľ */
		index = FONT_CODE_POINT_C4_BD;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x87) {		/* Ň */
		index = FONT_CODE_POINT_C5_87;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x94) {		/* Ô */
		index = FONT_CODE_POINT_C3_94;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x94) {		/* Ŕ */
		index = FONT_CODE_POINT_C5_94;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xa0) {		/* Š */
		index = FONT_CODE_POINT_C5_A0;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xa4) {		/* Ť */
		index = FONT_CODE_POINT_C5_A4;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x9a) {		/* Ú */
		index = FONT_CODE_POINT_C3_9A;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x9d) {		/* Ý */
		index = FONT_CODE_POINT_C3_9D;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xbd) {		/* Ž */
		index = FONT_CODE_POINT_C5_BD;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xa1) {		/* á */
		index = FONT_CODE_POINT_C3_A1;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x8d) {		/* č */
		index = FONT_CODE_POINT_C4_8D;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x8f) {		/* ď */
		index = FONT_CODE_POINT_C4_8F;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xa9) {		/* é */
		index = FONT_CODE_POINT_C3_A9;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xad) {		/* í */
		index = FONT_CODE_POINT_C3_AD;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0xba) {		/* ĺ */
		index = FONT_CODE_POINT_C4_BA;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0xbe) {		/* ľ */
		index = FONT_CODE_POINT_C4_BE;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x88) {		/* ň */
		index = FONT_CODE_POINT_C5_88;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xb4) {		/* ô */
		index = FONT_CODE_POINT_C3_B4;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x95) {		/* ŕ */
		index = FONT_CODE_POINT_C5_95;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xa1) {		/* š */
		index = FONT_CODE_POINT_C5_A1;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xa5) {		/* ť */
		index = FONT_CODE_POINT_C5_A5;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xba) {		/* ú */
		index = FONT_CODE_POINT_C3_BA;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xbd) {		/* ý */
		index = FONT_CODE_POINT_C3_BD;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xbe) {		/* ž */
		index = FONT_CODE_POINT_C5_BE;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0x85) {		/* Å */
		index = FONT_CODE_POINT_C3_85;
	} else if (*utf8 == 0xc3 && *(utf8 + 1) == 0xa5) {		/* å */
		index = FONT_CODE_POINT_C3_A5;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x90) {		/* A */
		index = FONT_CODE_POINT_D0_90;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x91) {		/* Б */
		index = FONT_CODE_POINT_D0_91;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x92) {		/* В */
		index = FONT_CODE_POINT_D0_92;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x93) {		/* Г */
		index = FONT_CODE_POINT_D0_93;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x94) {		/* Д */
		index = FONT_CODE_POINT_D0_94;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x95) {		/* Е */
		index = FONT_CODE_POINT_D0_95;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x96) {		/* Ж */
		index = FONT_CODE_POINT_D0_96;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x97) {		/* З */
		index = FONT_CODE_POINT_D0_97;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x98) {		/* И */
		index = FONT_CODE_POINT_D0_98;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x99) {		/* Й */
		index = FONT_CODE_POINT_D0_99;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x9a) {		/* К */
		index = FONT_CODE_POINT_D0_9A;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x9b) {		/* Л */
		index = FONT_CODE_POINT_D0_9B;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x9c) {		/* М */
		index = FONT_CODE_POINT_D0_9C;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x9d) {		/* Н */
		index = FONT_CODE_POINT_D0_9D;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x9e) {		/* О */
		index = FONT_CODE_POINT_D0_9E;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x9f) {		/* П */
		index = FONT_CODE_POINT_D0_9F;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xa0) {		/* Р */
		index = FONT_CODE_POINT_D0_A0;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xa1) {		/* С */
		index = FONT_CODE_POINT_D0_A1;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xa2) {		/* Т */
		index = FONT_CODE_POINT_D0_A2;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xa3) {		/* У */
		index = FONT_CODE_POINT_D0_A3;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xa4) {		/* Ф */
		index = FONT_CODE_POINT_D0_A4;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xa5) {		/* Х */
		index = FONT_CODE_POINT_D0_A5;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xa6) {		/* Ц */
		index = FONT_CODE_POINT_D0_A6;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xa7) {		/* Ч */
		index = FONT_CODE_POINT_D0_A7;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xa8) {		/* Ш */
		index = FONT_CODE_POINT_D0_A8;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xa9) {		/* Щ */
		index = FONT_CODE_POINT_D0_A9;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xaa) {		/* Ъ */
		index = FONT_CODE_POINT_D0_AA;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xab) {		/* Ы */
		index = FONT_CODE_POINT_D0_AB;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xac) {		/* Ь */
		index = FONT_CODE_POINT_D0_AC;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xad) {		/* Э */
		index = FONT_CODE_POINT_D0_AD;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xae) {		/* Ю */
		index = FONT_CODE_POINT_D0_AE;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xaf) {		/* Я */
		index = FONT_CODE_POINT_D0_AF;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xb0) {		/* а */
		index = FONT_CODE_POINT_D0_B0;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xb1) {		/* б */
		index = FONT_CODE_POINT_D0_B1;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xb2) {		/* в */
		index = FONT_CODE_POINT_D0_B2;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xb3) {		/* г */
		index = FONT_CODE_POINT_D0_B3;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xb4) {		/* д */
		index = FONT_CODE_POINT_D0_B4;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xb5) {		/* е */
		index = FONT_CODE_POINT_D0_B5;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xb6) {		/* ж */
		index = FONT_CODE_POINT_D0_B6;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xb7) {		/* з */
		index = FONT_CODE_POINT_D0_B7;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xb8) {		/* и */
		index = FONT_CODE_POINT_D0_B8;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xb9) {		/* й */
		index = FONT_CODE_POINT_D0_B9;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xba) {		/* к */
		index = FONT_CODE_POINT_D0_BA;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xbb) {		/* л */
		index = FONT_CODE_POINT_D0_BB;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xbc) {		/* м */
		index = FONT_CODE_POINT_D0_BC;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xbd) {		/* н */
		index = FONT_CODE_POINT_D0_BD;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xbe) {		/* о */
		index = FONT_CODE_POINT_D0_BE;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0xbf) {		/* п */
		index = FONT_CODE_POINT_D0_BF;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x80) {		/* р */
		index = FONT_CODE_POINT_D1_80;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x81) {		/* с */
		index = FONT_CODE_POINT_D1_81;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x82) {		/* т */
		index = FONT_CODE_POINT_D1_82;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x83) {		/* у */
		index = FONT_CODE_POINT_D1_83;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x84) {		/* ф */
		index = FONT_CODE_POINT_D1_84;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x85) {		/* х */
		index = FONT_CODE_POINT_D1_85;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x86) {		/* ц */
		index = FONT_CODE_POINT_D1_86;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x87) {		/* ч */
		index = FONT_CODE_POINT_D1_87;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x88) {		/* ш */
		index = FONT_CODE_POINT_D1_88;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x89) {		/* щ */
		index = FONT_CODE_POINT_D1_89;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x8a) {		/* ъ */
		index = FONT_CODE_POINT_D1_8A;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x8b) {		/* ы */
		index = FONT_CODE_POINT_D1_8B;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x8c) {		/* ь */
		index = FONT_CODE_POINT_D1_8C;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x8d) {		/* э */
		index = FONT_CODE_POINT_D1_8D;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x8e) {		/* ю */
		index = FONT_CODE_POINT_D1_8E;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x8f) {		/* я */
		index = FONT_CODE_POINT_D1_8F;
	} else if (*utf8 == 0xd0 && *(utf8 + 1) == 0x81) {		/* Ё */
		index = FONT_CODE_POINT_D0_81;
	} else if (*utf8 == 0xd1 && *(utf8 + 1) == 0x91) {		/* ё */
		index = FONT_CODE_POINT_D1_91;
	} else if (*utf8 == 0xc4 && *(utf8 + 1) == 0x9b) {		/* ě */
		index = FONT_CODE_POINT_C4_9B;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0x99) {		/* ř */
		index = FONT_CODE_POINT_C5_99;
	} else if (*utf8 == 0xc5 && *(utf8 + 1) == 0xaf) {		/* ů */
		index = FONT_CODE_POINT_C5_AF;
	} else {
		/* Unsupported code point */
		index = FONT_UNSUPPORTED_CODE_POINT;
	}

	srcrect.x = char_dimensions[index][0];
	srcrect.w = char_dimensions[index][1] - srcrect.x + 1;

	return srcrect;
}

/***************************************************************************
 * Get Pixel                                                               *
 ***************************************************************************/
/* This function is based upon getpixel() from the SDL documentation.
 * If surfaces must be locked/unlocked then the caller is responsible for this.
 * 
 * On exit: returns a value which can be converted to RGB using SDL_GetRGB() */

Uint32 get_pixel(SDL_Surface *surface, int x, int y) {
	Uint32 retpixel;
	int bpp;
	Uint8 *offset;

	bpp = surface->format->BytesPerPixel;
	offset = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	if (bpp == 1) {
		retpixel = *offset;
	} else if (bpp == 2) {
		retpixel = *(Uint16*)offset;
	} else if (bpp == 3) {
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			retpixel = offset[0] << 16 | offset[1] << 8 | offset[2];
		} else {
			retpixel = offset[0] | offset[1] << 8 | offset[2] << 16;
		}
	} else if (bpp == 4) {
		retpixel = *(Uint32*)offset;
	} else {
		retpixel = 0;
	}

	return retpixel;
}

/***************************************************************************
 * Set Pixel                                                               *
 ***************************************************************************/
/* This function is based upon putpixel() from the SDL documentation.
 * If surfaces must be locked/unlocked then the caller is responsible for this.
 * 
 * The pixel being put should be mapped to the surface format first using SDL_MapRGB() */

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
	int bpp;
	Uint8 *offset;

	bpp = surface->format->BytesPerPixel;
	offset = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp) {
		case 1:
			*offset = pixel;
			break;
		case 2:
			*(Uint16 *)offset = pixel;
			break;
		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				offset[0] = (pixel >> 16) & 0xff;
				offset[1] = (pixel >> 8) & 0xff;
				offset[2] = pixel & 0xff;
			} else {
				offset[0] = pixel & 0xff;
				offset[1] = (pixel >> 8) & 0xff;
				offset[2] = (pixel >> 16) & 0xff;
			}
			break;
		case 4:
			*(Uint32 *)offset = pixel;
			break;
	}
}

#endif	/* USE_PIXMAP_FONT */
