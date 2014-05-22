// Filename    : OMUSIC.CPP
// Description : music

#include <OINFO.H>
#include <GAMEDEF.H>
#include <OAUDIO.H>
#include <OMUSIC.H>
#include <OSYS.H>
#include <OCONFIG.H>
#include <OVGABUF.H>

// -------- define constant --------//
// random select 1 - 17 for background music

#define LOW_RANDOM_SONG   1
#define HIGH_RANDOM_SONG  17

// -------- begin of function Music::Music ---------//
Music::Music() {
    init_flag = 0;
    song_id = 0;
    music_channel = 0;
}

// -------- end of function Music::Music ---------//

// -------- begin of function Music::~Music ---------//
Music::~Music() {
    deinit();
}

// -------- end of function Music::~Music ---------//

// -------- begin of function Music::init ---------//
// note : call Music::init after audio.init
void Music::init() {
    deinit();
    init_flag = audio.init_flag;

#ifdef DEBUG
    //if ( init_flag )
    //DEBUG_LOG("Music:: set init_flag");
#endif
}

// -------- end of function Music::init ---------//

// -------- begin of function Music::deinit ---------//
// call deinit before audio.deinit
void Music::deinit() {
    if( init_flag ) {
	stop();
    }
}

// -------- end of function Music::deinit ---------//

// -------- begin of function Music::stop ---------//
int Music::stop() {
    vga_front.temp_unlock();
    //DEBUG_LOG("Music:: stop called");
    vga_front.temp_restore_lock();

    if( init_flag ) {
	if(music_channel) {
	    audio.stop_cd();
	    music_channel = 0;
	    song_id = 0;
	}
	return 1;
    }
    else {
	return -1;
    }
}

// -------- end of function Music::stop ---------//

// -------- begin of function Music::play ---------//
// <int> songId
// <int> playType   0 = non-looped, 1 = looped
int Music::play(int songId, int playType) {
    if( !init_flag ) {
	return 0;
    }

    vga_front.temp_unlock();
    stop();
    vga_front.temp_restore_lock();

    // in version 2, check cd, (equals to check  playType & MUSIC_CD_THEN_WAV or MUSIC_PLAY_CD)

    if( playType & MUSIC_CD_THEN_WAV ) {
	return play(songId, playType & ~MUSIC_CD_THEN_WAV | MUSIC_PLAY_CD)
	    || play(songId, playType & ~MUSIC_CD_THEN_WAV & ~MUSIC_PLAY_CD);
    }
    else if( playType & MUSIC_PLAY_CD ) {
	// +1 to skip the program track
	if( audio.cd_init_flag && config.cd_music_volume > 0 && audio.play_cd(songId+1, config.cd_music_volume) ) {
	    play_type = playType;
	    song_id = songId;
	    music_channel = 1;

	    return 1;
	}
    }
    else {
	// no wav to play
	//		if( audio.wav_init_flag )
	//		{
	//			String waveFileStr(DIR_MUSIC);
	//			waveFileStr += music_file[songId-1];
	//			if( !DIR_MUSIC[0] || !m.is_file_exist(waveFileStr) || !audio.wav_init_flag )
	//				return 0;
	//			if( playType & MUSIC_PLAY_LOOPED )
	//				music_channel = audio.play_loop_wav(waveFileStr, 0, AbsVolume(config.wav_music_volume,0) );
	//			else
	//				music_channel = audio.play_long_wav(waveFileStr, AbsVolume(config.wav_music_volume,0) );
	//			play_type = playType;
	//			song_id = songId;
	//			return music_channel != 0;
	//		}
	return 0;
    }


    return 0;
}

// -------- end of function Music::play ---------//

// -------- begin of function Music::is_playing ---------//
// [int] songId        id of the song (default=0, don't care)
int Music::is_playing(int songId) {
    if( !init_flag )
	return 0;

    if( !music_channel )
	return 0;

    return audio.is_cd_playing() && (!songId || songId == song_id);
}

// -------- end of function Music::is_playing ---------//

// -------- begin of function Music::random_bgm_track --------//
int Music::random_bgm_track(int excludeSong) {
    int s = LOW_RANDOM_SONG + m.get_time() % (HIGH_RANDOM_SONG - LOW_RANDOM_SONG + 1);
    if( s == excludeSong ) {
	// avoid selecting excludeSong if possible
	if( ++s > HIGH_RANDOM_SONG )
	    s = LOW_RANDOM_SONG;
    }
    err_when(s < LOW_RANDOM_SONG || s > HIGH_RANDOM_SONG );
    return s;
}

// -------- end of function Music::random_bgm_track --------//

// -------- begin of function Music::change_volume --------//
void Music::change_volume(int vol) {
    if( !init_flag )
	return;

    if( is_playing() ) {
	audio.set_cd_volume(vol);
    }
}

//-------- end of function Music::change_volume --------//

//-------- begin of function Music::yield --------//

void Music::yield() {
    if( !init_flag )
	return;

    int oldSongId = song_id;

    if( config.cd_music_volume ) {
	if( !is_playing() ) {
	    vga_front.temp_unlock();
	    //DEBUG_LOG("Music::yield(): play random");	//##
	    vga_front.temp_restore_lock();

	    int songId;

	    while(1) {
		songId = random_bgm_track(oldSongId);

		//--- only play Christmas music in December ---//

		if( songId==9 && info.game_month != 12 )
		    continue;

		//--- only play Graduation March music in May or June ---//

		if( songId==10 && info.game_month != 5 && info.game_month != 6 )
		    continue;

		play( songId, sys.cdrom_drive ? MUSIC_CD_THEN_WAV : 0 );
		return;
	    }
	}
    }
}

//-------- end of function Music::yield --------//
