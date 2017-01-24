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


/*
 * Defines 
 */
#define MESSAGE_BOX_VOLUME_ID "volume"
#define DELAY_MESSAGE_BOX_VOLUME (1 * game_cycle_limit)	/* ms */

/*
 * files and stuff 
 */
#define SOUNDS_DIR "sounds"
#define MUSIC_DIR "music"

/*
 * tags for the soundskin 
 */

#define AMMO_TAG "[ammo]"
#define SCREW_TAG "[screw]"
#define KEY_TAG "[key]"
#define DOOR_TAG "[door]"
#define BOMB_TAG "[bomb]"
#define GUN_TAG "[gun]"
#define BOX_TAG "[box]"
#define BIRD_TAG "[bird]"
#define TELEPORT_TAG "[teleport]"
#define WALK_TAG "[walk]"
#define CAPSULE_TAG "[capsule]"
#define KILL_TAG "[kill]"
#define MAGNET_TAG "[magnet]"
#define EXIT_OPEN_TAG "[exit-open]"
#define SHOOT_TAG "[robbo-shoots]"

#define SND_READING 128
#define SND_MISSING 129
#define SND_INIT 100
#define SND_NOINIT 200

#define SND_FILEPRESENT 64
#define SND_SAMPLELOADED 63
#define SND_ABSENT 60
#define SND_PLAYING 1
#define SND_PAUSED 2
#define SND_NOTPLAYING 0

#define MAXFNAMELEN  1024


/*
 * Variables 
 */
char            ammo_file[MAXFNAMELEN];
char            screw_file[MAXFNAMELEN];
char            bomb_file[MAXFNAMELEN];
char            box_file[MAXFNAMELEN];
char            door_file[MAXFNAMELEN];
char            gun_file[MAXFNAMELEN];
char            key_file[MAXFNAMELEN];
char            shoot_file[MAXFNAMELEN];
char            bird_file[MAXFNAMELEN];
char            teleport_file[MAXFNAMELEN];
char            walk_file[MAXFNAMELEN];
char            capsule_file[MAXFNAMELEN];
char            kill_file[MAXFNAMELEN];
char            magnet_file[MAXFNAMELEN];
char            exit_open_file[MAXFNAMELEN];


int allocated_channels=-1;
int             destroyed_playlist = 0;
int             next_song = 0;
int             absent = 1;
int             tempvol = 0;
 /*******************/
 /*
  * table of sounds 
  */
 /*******************/
/*
 * format is simple: initialized, event_type,sample_hook, file_name 
 */
struct snd_sample sounds[] = {
    {0, SFX_BULLET, NULL, ammo_file, AMMO_TAG},
    {0, SFX_SCREW, NULL, screw_file, SCREW_TAG},
    {0, SFX_BOMB, NULL, bomb_file, BOMB_TAG},
    {0, SFX_BOX, NULL, box_file, BOX_TAG},
    {0, SFX_DOOR, NULL, door_file, DOOR_TAG},
    {0, SFX_GUN, NULL, gun_file, GUN_TAG},
    {0, SFX_KEY, NULL, key_file, KEY_TAG},
    {0, SFX_SHOOT, NULL, shoot_file, SHOOT_TAG},
    {0, SFX_BIRD, NULL, bird_file, BIRD_TAG},
    {0, SFX_TELEPORT, NULL, teleport_file, TELEPORT_TAG},
    {0, SFX_ROBBO, NULL, walk_file, WALK_TAG},
    {0, SFX_CAPSULE, NULL, capsule_file, CAPSULE_TAG},
    {0, SFX_KILL, NULL, kill_file, KILL_TAG},
    {0, SFX_MAGNET, NULL, magnet_file, MAGNET_TAG},
    {0, SFX_EXIT_OPEN, NULL, exit_open_file, EXIT_OPEN_TAG},
    {0, 0, NULL, NULL, NULL}	/* this always should be at the end */
};

struct snd_music playlist[MUS_MAXSONGS];

/*
 * Function prototypes 
 */


/***************************************************************************
 * Volume Up                                                               *
 ***************************************************************************/

void
volume_up(void)
{
    char            tempstring[256];
    MSG_Box         temp_msg_box;

    if (volume < 128)
    {
	volume++;
	sprintf(tempstring, "%s: %i", txt_Volume, volume);
	strcpy(temp_msg_box.name, MESSAGE_BOX_VOLUME_ID);
	strcpy(temp_msg_box.message, tempstring);
	temp_msg_box.timeout = DELAY_MESSAGE_BOX_VOLUME;
	temp_msg_box.dynamic = FALSE;
	temp_msg_box.w = 6 * video.field_size;
	temp_msg_box.h = 2 * video.field_size;
	show_message_box(MESSAGE_BOX_SUB_INITIALISE, &temp_msg_box);
#ifndef HAVE_MUSIC
	/* If the Options screen is currently showing the Sfx Volume
	 * then this will update temp_sfx_vol and the screen text */
	if (game_mode == OPTIONS_SCREEN && optionspage == OPTIONS_SFX_VOL / 13)
	{
		temp_sfx_vol = volume;
		options_screen.redraw |= REDRAW_INTERMEDIATE;
    }
#endif
    }
}

/***************************************************************************
 * Volume Down                                                             *
 ***************************************************************************/

void
volume_down(void)
{
    char            tempstring[256];
    MSG_Box         temp_msg_box;

    if (volume > 0)
    {
	volume--;
	sprintf(tempstring, "%s: %i", txt_Volume, volume);
	strcpy(temp_msg_box.name, MESSAGE_BOX_VOLUME_ID);
	strcpy(temp_msg_box.message, tempstring);
	temp_msg_box.timeout = DELAY_MESSAGE_BOX_VOLUME;
	temp_msg_box.dynamic = FALSE;
	temp_msg_box.w = 6 * video.field_size;
	temp_msg_box.h = 2 * video.field_size;
	show_message_box(MESSAGE_BOX_SUB_INITIALISE, &temp_msg_box);
#ifndef HAVE_MUSIC
	/* If the Options screen is currently showing the Sfx Volume
	 * then this will update temp_sfx_vol and the screen text */
	if (game_mode == OPTIONS_SCREEN && optionspage == OPTIONS_SFX_VOL / 13)
	{
		temp_sfx_vol = volume;
		options_screen.redraw |= REDRAW_INTERMEDIATE;
    }
#endif
    }
}


#ifdef HAVE_MUSIC


/*
 * destroy existing playlist 
 */


void
destroy_playlist(void)
{
    int             c = 0;
    destroyed_playlist = 1;
    Mix_HaltMusic();
    /*
     * printf("destroy playlist [");
     */
    absent = 1;
    for (c = 0; c < MUS_MAXSONGS; c++) {
	if (playlist[c].present == SND_SAMPLELOADED) {
	    Mix_FreeMusic(playlist[c].music);
	}
	playlist[c].playing = SND_NOTPLAYING;
	playlist[c].music = NULL;
	playlist[c].present = SND_ABSENT;
	/*
	 * printf(".");
	 */
    }
    next_song = 0;
    /*
     * printf("]\n");
     */
}

/* clean up the song after it stopped playing */
void
music_finished(void)
{
    int             c;
#ifdef DEBUG_SOUND
    printf("music finished\n");
#endif
    for (c = 0; c < MUS_MAXSONGS; c++) {
	if (playlist[c].music != NULL) {
		playlist[c].playing = SND_NOTPLAYING;
		playlist[c].present = SND_FILEPRESENT;
		Mix_FreeMusic(playlist[c].music);
		playlist[c].music = NULL;
	    break;
	}
    }
}



void
music_stop(void)
{
    int c = 0;
    int f=0;
    for (c = 0; c < MUS_MAXSONGS; c++) {
       if((f==0) && ((playlist[c].playing==SND_PLAYING) || (playlist[c].playing==SND_PAUSED))) { 
	    f=1; /* make sure, we call that only once */
	    Mix_HaltMusic();
	}
      playlist[c].playing = SND_NOTPLAYING;
	if(playlist[c].present == SND_SAMPLELOADED)  {
		playlist[c].present = SND_FILEPRESENT;
		Mix_FreeMusic(playlist[c].music);
		playlist[c].music = NULL;
		}
	}
    

}

/*
 * this is music playing engine, it is supposed to be called onec a while, 
 * for eg. with screen refresh 
 */

void
play_music(void)
{
    int             c = 0;
    int             current_song = -1;
    char            fname[1024];
    int             absents = 0;

    if ((sound == 0) || ((absent == 1) && (destroyed_playlist == 0)))	/* check if we play sound at all, and if 
									 * there are any music files present  */
	return;								/* or if not, did we just destroyed playlist? */
    if (destroyed_playlist) {	/* we destroyed playlist recently, so recreate it */
    	make_playlist();
	destroyed_playlist = SND_NOTPLAYING;
    }
    /*
     * ok, now search which song is playing (if any) 
     */
    for (c = 0; c < MUS_MAXSONGS; c++) {
	if (playlist[c].playing == SND_PAUSED) {	/* music is paused 
							 */
	    if (volume > 0) {	/* should we resume music */
		playlist[c].playing = SND_PLAYING;
		Mix_ResumeMusic();
	    };
	    return;
	}
	if (playlist[c].playing == SND_PLAYING) {
	    if (volume == 0) {
		playlist[c].playing = SND_PAUSED;
		Mix_PauseMusic();
	    } else {
		if (tempvol != volume) {
		    Mix_VolumeMusic(volume);
		    tempvol = volume;
		}
	    }
	    return;		/* music is still playing, no point of
				 * doing anything */
	}
	if (playlist[c].present == SND_ABSENT)
	    absents++;
	if (playlist[c].music != NULL)
	    current_song = c;	/* this way we can determine the current
				 * song */
    }
    if (absents == MUS_MAXSONGS) {
	absent = 1;		/* no hope, no music */
	return;			/* no song present */
    }

    /*
     * we are still here, so, there is a need of song change 
     */

    /* search for available music */
    while (playlist[next_song].present != SND_FILEPRESENT) 
	    next_song = ((next_song+1) % MUS_MAXSONGS);
	    
    sprintf(fname, "%s/%s/%s", PACKAGE_DATA_DIR, MUSIC_DIR,playlist[next_song].fname);
    playlist[next_song].music = Mix_LoadMUS(fname);
    if (playlist[next_song].music != NULL) {       
	playlist[next_song].present = SND_SAMPLELOADED;
	playlist[next_song].playing = SND_PLAYING;
	Mix_VolumeMusic(volume);
	Mix_PlayMusic(playlist[next_song].music, 0);
	Mix_HookMusicFinished(music_finished); /* hook up a handler that will clean up after a song play*/
    } else {
	playlist[next_song].present = SND_ABSENT;  /* oops, meanwhile, somebody deleted the song file? */
    }
        next_song = ((next_song+1) % MUS_MAXSONGS);
}


void
make_playlist(void)
{
    DIR            *dir = NULL;
    struct dirent  *dentry;
    char            musicpath[1024];
    int             song_count = 0;

    destroy_playlist();
    sprintf(musicpath, "%s/%s/", PACKAGE_DATA_DIR, MUSIC_DIR);
    next_song = 0;

    if ((dir = opendir(musicpath)) == NULL) {	/* no music, means we will 
						 * not play any */
	return;
    }
    while (((dentry = readdir(dir)) != NULL)
	   && (song_count < MUS_MAXSONGS)) {
	if ((strstr(dentry->d_name, ".ogg")) != NULL) {	/* we have got an
							 * ogg file */
	    strncpy(playlist[song_count].fname, dentry->d_name, 128);
	    playlist[song_count].fname[127] = 0;
	    playlist[song_count].present = SND_FILEPRESENT;
	    playlist[song_count].playing = SND_NOTPLAYING;
	    song_count++;
	    absent = 0;
	}
    }
    closedir(dir);
}
#endif


/**********************
 * Load the soundskin *
 **********************/
void
load_soundskin(char *fname)
{
    char            name[1024];
    char            line[1024];
    int             c = 0,
	c1 = 0;
    int             notag = 0;
    FILE           *fd;

    // if (sound == 0) /* there is nothing to do here, why
    // * bother? */
    // return;
    /*
     * unload all samples 
     */
    for (c = 0; sounds[c].type != 0; c++) {
	if (sounds[c].initialized == SND_INIT) {
	    if (sounds[c].s_sample != NULL)
		Mix_FreeChunk(sounds[c].s_sample);	/* the ones that
							 * are initialized 
							 * should be
							 * removed */
	}
	sounds[c].s_sample = NULL;	/* set null on the sample hook, so 
					 * it will not be removed twice */
	sounds[c].initialized = SND_MISSING;
    }

    sprintf(name, "%s/%s/skins/%s", PACKAGE_DATA_DIR, SOUNDS_DIR, fname);
    if ((fd = fopen(name, "r")) == NULL) {	/* no skin file, well,
						 * what a pity, let's try
						 * the default one */
	sprintf(name, "%s/%s/skins/default.dat", PACKAGE_DATA_DIR,
		SOUNDS_DIR);
	if ((fd = fopen(name, "r")) == NULL)	/* hmmm no default sound * 
						 * skin, how strange */
	    return;
    }
    /*
     * ok, so let's read the soundskin file 
     */
    while ((fgets(line, 1024, fd)) != NULL) {
	for (c = strlen(line) - 1; c >= 0; c--)	/* fgets will get [CR]LFs
						 * * too so they should be
						 * * removed, the spaces at
						 * * the end also */
	    if (line[c] == 13 || line[c] == 10 || line[c] == ' ')
		line[c] = 0;
	    else
		break;		/* we don't read many lines, just one */
	/*
	 * checking if we have a tag 
	 */
	notag = 1;
	for (c = 0; sounds[c].type != 0; c++)
	    if (!strncmp(sounds[c].rctag, line, strlen(sounds[c].rctag))) {	/* we 
										 * got 
										 * a 
										 * tag 
										 * check, 
										 * if 
										 * we 
										 * processed 
										 * anything 
										 */
		notag = 0;
		for (c1 = 0; sounds[c1].type != 0; c1++)
		    if ((sounds[c1].initialized == SND_READING) && (c1 != c))	/* data 
										 * missing 
										 */
			sounds[c1].initialized = SND_MISSING;
		sounds[c].initialized = SND_READING;
		break;
	    }
	if (notag == 1) {	/* ok, so we have data */
	    for (c = 0; sounds[c].type != 0; c++)
		if (sounds[c].initialized == SND_READING) {
		    strncpy(sounds[c].fname, line, MAXFNAMELEN);
		    sounds[c].fname[MAXFNAMELEN - 1] = 0;
		    sounds[c].initialized = SND_NOINIT;
#ifdef DEBUG_SOUND
		    printf("     %s reading as <%s>\n", sounds[c].rctag,
			   sounds[c].fname);
#endif
		    break;
		}
	}
    }
    fclose(fd);
    for (c = 0; sounds[c].type != 0; c++)
	if (sounds[c].initialized == SND_READING)
	    sounds[c].initialized = SND_MISSING;	/* a bit of
							 * cleaning */
    load_samples();

}





/*
 * remove audio from the memory 
 */
void
audio_destroy()
{
    int             c = 0;
    /*
     * we don't want to destroy already destroyed 
     */
#ifdef DEBUG_SOUND
    printf("*** Audio destroy\n");
    printf("*** sound=%i\n", sound);
#endif
    for (c = 0; sounds[c].type != 0; c++) {
	if (sounds[c].initialized == SND_INIT) {
	    if (sounds[c].s_sample != NULL)
		Mix_FreeChunk(sounds[c].s_sample);	/* the ones that
							 * are initialized 
							 * should be
							 * removed */
	}
	sounds[c].s_sample = NULL;	/* set null on the sample hook, so 
					 * it will not be removed twice */
	sounds[c].initialized = SND_MISSING;
    }
    /*
     * if we support music, we have to handle it 
     */
#ifdef HAVE_MUSIC
    destroy_playlist();
#endif
    sound = 0;
#ifdef DEBUG_SOUND
    printf("*** sound=%i audio_destroy end\n", sound);
#endif
	audio_close();
}

/*
 * this will load the samples to the memory 
 */

void
load_samples(void)
{
    int             c = 0;
    char            fullpath[1024];
#ifdef DEBUG_SOUND
    printf("sound setting %d\n", sound);
#endif
    /*
     * Samples init 
     */
    
    for (c = 0; sounds[c].type != 0; c++) {   /** If any of the samples is missing, we will not try to use it **/
	if (sounds[c].initialized == SND_MISSING)	/* skip missing *
							 * samples */
	    continue;
	sprintf(fullpath, "%s/%s/%s", PACKAGE_DATA_DIR, SOUNDS_DIR,
		sounds[c].fname);
#ifdef DEBUG_SOUND
	printf("*** load samples from %s\n", fullpath);
#endif
	sounds[c].s_sample = Mix_LoadWAV(fullpath);
	if (sounds[c].s_sample == NULL)	/* it seems the sample isn't *
					 * present, so mark it as not *
					 * usable */
	    sounds[c].initialized = SND_NOINIT;
	else
	    sounds[c].initialized = SND_INIT;
    }
}

/*
 * check if audio was open, returns 0 if not opened, or returns the number 
 * of times it was opened 
 */
int
audio_opened()
{
    int             audio_rate = 0;
    Uint16          audio_format = 0;
    int             channels = 0;
    int             retval = 0;
    retval = Mix_QuerySpec(&audio_rate, &audio_format, &channels);
#ifdef DEBUG_SOUND
    printf("num audio open %d\n", retval);
    printf("*** sound=%i\n", sound);
#endif
    return retval;
}

/*
 * make the sound system to be ready for loading samples and music 
 */

void
audio_init(void)
{

    int             c = 0;
#ifdef DEBUG_SOUND
    printf("** audio init %d\n", sound);
    printf("*** sound=%i\n", sound);
#endif
   audio_open();
   for (c = 0; sounds[c].type != 0; c++) {	/* this will mark all of the samples as not initialized, but will not do anything else */
	sounds[c].s_sample = NULL;
	sounds[c].initialized = SND_MISSING;
	strcpy(sounds[c].fname, SINGLE_SPACE);
    }
    /*
     * if we want to support music 
     */
#ifdef HAVE_MUSIC
    /*
     * initialize the music table 
     */
    for (c = 0; c < MUS_MAXSONGS; c++) {
	playlist[c].present = SND_ABSENT;

	playlist[c].playing = SND_NOTPLAYING;
	playlist[c].music = NULL;
    }
    destroyed_playlist=1;
#endif
}


/**************
 * Play Sound *
 **************/
/*
 * the argument is type of an object, which is defined in board.h * and
 * volume definition. there are some prefedined values (check sound.h) *
 * if we want to set the volume of a sample manually we do it this way:
 * play_sound(SCREW,SND_FULL+new_volume) * and now the volume would be
 * sfx_vol/new_volume 
 */


void
play_sound(int event, int vol)
{
    int             chan = 0;
    int             c = 0;
    int channels=Mix_Playing(-1);
    if (sound == 0)		/* no sound, means no sound */
	return;
    if (sfx_vol == 0)
	return;

    /*
     * just in case, if play_sound would be called with an invalid value 
     */
    switch (vol) {
    case SND_FULL:
	vol = 128;
	break;
    case SND_HALF:
	vol = sfx_vol / 2;
	if(channels>allocated_channels-5)  /* We want at least 5 channels to be available, in order to play quiet sounds */
		return;
	break;
  case SND_QUIET:
	vol = sfx_vol / 8;
	if(channels>allocated_channels-5) 
		return;
	break;
    case SND_MUTE:
	vol = 0;
	break;
    case SND_NORM:

	vol = sfx_vol;
	break;
    default:
	/*
	 * OK, so we have other value set, so we assume someone wanted to
	 * divide sfx_vol by other value 
	 */
	vol = sfx_vol / (vol - SND_FULL);
	break;
    }
	
    for (c = 0; sounds[c].type != 0; c++) {
	if (sounds[c].type == event) {
	    if (sounds[c].initialized == SND_INIT) {
		//Mix_VolumeChunk(sounds[c].s_sample, vol);
		chan = Mix_PlayChannel(-1, sounds[c].s_sample, 0);
		Mix_Volume(chan,vol);
	    }
	    break;
	}
    }
    return;
}



void
audio_open()
{
    int             audio_rate = SND_RATE;
    Uint16          audio_format = AUDIO_S16SYS;
    int             audio_channels = 2;
#if defined(PLATFORM_GP2X) && defined(TOOLCHAIN_OPEN2X)  || defined(PLATFORM_CAANOO)
    int             audio_buffers = 256;
#else
    int             audio_buffers = 1024;
#endif

    if (audio_opened() == 0) {	/* check if we already initialized sound
				 * subsystem */
#ifdef DEBUG_SOUND
	printf
	    ("audio_rate=%i audio_format=%i audio_channels=%i audio_buffers=%i\n",
	     audio_rate, audio_format, audio_channels, audio_buffers);
#endif
	if (Mix_OpenAudio (audio_rate, audio_format, audio_channels, audio_buffers) != 0) {		/* it seems we will not have the sound
															 * after all */
	    fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
	    sound = 0;
	} else {
		allocated_channels=Mix_AllocateChannels(SND_CHANNELS);
		printf("Managed to allocate %d sound channels \n",allocated_channels);

	}
    }

}

void
audio_close()
{
    if (audio_opened() > 0)
	Mix_CloseAudio();

}
