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



/*
 * Defines 
 */
#define SND_FULL    3
#define SND_NORM    2
#define SND_HALF    1
#define SND_MUTE    0
#define SND_QUIET   4
#define SND_CHANNELS 16

#define SND_RATE 44100

/*
 * sound effects types 
 */

#define SFX_BULLET       1
#define SFX_SCREW        2
#define SFX_BOMB         3
#define SFX_BOX          4
#define SFX_DOOR         5
#define SFX_GUN          6
#define SFX_KEY          7
#define SFX_SHOOT        8
#define SFX_BIRD         9
#define SFX_TELEPORT    10
#define SFX_ROBBO       11
#define SFX_CAPSULE     12
#define SFX_KILL        13
#define SFX_MAGNET      14
#define SFX_EXIT_OPEN	15

#define MUS_MAXSONGS 40

#define SND_SFX_DEFAULT_VOLUME 30
#define SND_MUS_DEFAULT_VOLUME SND_SFX_DEFAULT_VOLUME



struct snd_sample {
    int             initialized;
    int             type;
    Mix_Chunk      *s_sample;
    char           *fname;
    char           *rctag;
};

struct snd_music {
    int             present;
    Mix_Music      *music;
    int             playing;
    char            fname[128];
};


/*
 * Variables 
 */
int sound;
int temp_game_sound;
int             sfx_vol;
int temp_sfx_vol;
/*
 * if we do not support music, we assume, that volume is sfx_volume 
 */
#ifdef HAVE_MUSIC
int             volume;
#else
#define volume sfx_vol
#endif


/*
 * Function prototypes 
 */


void            volume_up(void);
void            volume_down(void);

void            load_soundskin(char *fname);

void            audio_init(void);	/* this one initializes sound
					 * system - shuld be run only once 
					 */

void            audio_destroy(void);	/* this one destroys all audio
					 * content from the memory, and
					 * sets sound variable to 0 */
					/*
					 * if you want to use audio again, set sound
					 * variable to 1 and call audio_init 
					 */
void            load_samples(void);	/* loads samples into memory */


void            play_sound(int event, int vol);	/* this one plays sounds */
				    /*
				     * vol can take 3 values: SND_NORM -
				     * sound is played with volume set in
				     * sfx_vol 
				     */
				    /*
				     * SND_HALF which is half of sfx_vol,
				     * and SND_MUTE - which means mute
				     * sound or no sound at all 
				     */
int             audio_opened();	/* returns 0 if mixer is not initialized */
void            audio_open();	/* initializes mixer should be the first
				 * thing before playing any sound */
void            audio_close();	/* closes mixer - done befere exit */


#ifndef HAVE_MUSIC

#define music_stop()
#define make_playlist()
#define music_finished()
#define destroy_playlist()
#define play_music()

#else
void            make_playlist(void);
void            play_music(void);
void            music_finished(void);
void            destroy_playlist(void);
void            music_stop(void);

#endif
