/* audio_a5.cpp */

#include <cassert>
#include <cstdio>
#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_memfile.h>

#include "audio_a5.h"
#include "sound_adlib.h"

#include "audio.h"
#include "../common_a5.h"
#include "../file.h"
#include "../table/sound.h"

/* Sample instance 0 for narrator voices.
 * Sample instance 1 for acknowledgements.
 * Other instances for general battle sounds.
 */
#define MAX_SAMPLE_INSTANCES 12

/* Music configuration. */
static const int SRATE = 44100;
static const int NUMFRAGS = 4;
static const int FRAGLEN = 2048;

static ALLEGRO_AUDIO_STREAM* s_music_stream;
static ALLEGRO_AUDIO_STREAM* s_effect_stream;
static SoundAdLibPC* s_adlib;
static ALLEGRO_THREAD* s_music_thread;

static ALLEGRO_SAMPLE* s_sample[SAMPLEID_MAX];
static ALLEGRO_SAMPLE_INSTANCE* s_instance[MAX_SAMPLE_INSTANCES];
static ALLEGRO_VOICE* al_voice;
static ALLEGRO_MIXER* al_mixer;

static char *AudioA5_LoadMusic(const MusicInfo *mid, uint32 *ret_length);
static void AudioA5_FreeMusicStream();
static ALLEGRO_AUDIO_STREAM *AudioA5_InitAdlib(const MusicInfo *mid);

/*--------------------------------------------------------------*/

void AudioA5_Init()
{
	if (!al_install_audio())
	{
		fprintf(stderr, "al_install_audio() failed.\n");
		goto audio_init_failed;
	}
	else
	{
		g_enable_audio = true;
	}

	al_voice = al_create_voice(SRATE, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
	if (al_voice == NULL)
	{
		fprintf(stderr, "al_create_voice() failed.\n");
		goto audio_init_failed;
	}

	al_mixer = al_create_mixer(SRATE, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
	if (al_mixer == NULL)
	{
		fprintf(stderr, "al_create_mixer() failed.\n");
		goto audio_init_failed;
	}

	al_attach_mixer_to_voice(al_mixer, al_voice);

	for (int i = 0; i < MAX_SAMPLE_INSTANCES; i++)
	{
		s_instance[i] = al_create_sample_instance(NULL);
		al_attach_sample_instance_to_mixer(s_instance[i], al_mixer);
	}

	al_init_acodec_addon();

	if (s_effect_stream == NULL)
		s_effect_stream = AudioA5_InitAdlib(&g_table_music[MUSIC_IDLE1]);
	return;

audio_init_failed:

	al_destroy_mixer(al_mixer);
	al_mixer = NULL;

	al_destroy_voice(al_voice);
	al_voice = NULL;

	g_enable_audio = false;
	g_enable_subtitles = true;
}

void AudioA5_Uninit()
{
	for (int sampleID = 0; sampleID < SAMPLEID_MAX; sampleID++)
	{
		al_destroy_sample(s_sample[sampleID]);
		s_sample[sampleID] = NULL;
	}

	for (int i = 0; i < MAX_SAMPLE_INSTANCES; i++)
	{
		al_destroy_sample_instance(s_instance[i]);
		s_instance[i] = NULL;
	}

	AudioA5_FreeMusicStream();

	if (s_effect_stream != NULL)
	{
		al_destroy_audio_stream(s_effect_stream);
		s_effect_stream = NULL;

		delete s_adlib;
		s_adlib = NULL;
	}

	al_destroy_mixer(al_mixer);
	al_destroy_voice(al_voice);
	al_uninstall_audio();
}

/*--------------------------------------------------------------*/

static char* AudioA5_LoadMusic(const MusicInfo* mid, uint32* ret_length)
{
	const char* filename = mid->filename;
	uint16 file_index = File_Open(filename, FILE_MODE_READ);

	if (file_index == FILE_INVALID)
	{
		return NULL;
	}

	uint32 length = File_GetSize(file_index);
	assert(length != 0);

	char* buf = new char[length];
	if (buf == NULL)
	{
		File_Close(file_index);
		return NULL;
	}

	File_Read(file_index, buf, length);
	File_Close(file_index);
	*ret_length = length;
	return buf;
}

/* Note: We can only have one instance of SoundAdLibPC. */
static ALLEGRO_AUDIO_STREAM* AudioA5_InitAdlib(const MusicInfo* mid)
{
	ALLEGRO_AUDIO_STREAM* stream;

	stream = al_create_audio_stream(NUMFRAGS, FRAGLEN, SRATE, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1);
	if (stream == NULL)
		return NULL;

	uint32 length;
	char* buf = AudioA5_LoadMusic(mid, &length);
	if (buf == NULL)
	{
		al_destroy_audio_stream(stream);
		return NULL;
	}

	ALLEGRO_FILE* f = al_open_memfile(buf, length, "r");
	delete s_adlib;
	s_adlib = new SoundAdLibPC(f, SRATE, g_opl_mame);
	s_adlib->init();
	al_fclose(f);
	delete[] buf;

	al_set_audio_stream_gain(stream, music_volume);
	al_set_audio_stream_pan(stream, ALLEGRO_AUDIO_PAN_NONE);
	al_attach_audio_stream_to_mixer(stream, al_mixer);
	al_register_event_source(g_a5_input_queue, al_get_audio_stream_event_source(stream));
	return stream;
}
static void AudioA5_FreeMusicStream()
{
	if (s_adlib != NULL)
	{
		delete s_adlib;
		s_adlib = NULL;

		if (s_music_stream == s_effect_stream)
			s_effect_stream = NULL;
	}
}

void AudioA5_InitMusic(const MusicInfo* m)
{
	const int track = m->track;

	AudioA5_FreeMusicStream();

	ALLEGRO_AUDIO_STREAM* stream = AudioA5_InitAdlib(m);
	if (stream == NULL)
		return;

	if (s_effect_stream != NULL)
		al_destroy_audio_stream(s_effect_stream);

	s_adlib->playTrack(track);
	s_music_stream = stream;
	s_effect_stream = stream;
}

void AudioA5_SetMusicVolume(float volume)
{
	if (s_music_stream != NULL)
		al_set_audio_stream_gain(s_music_stream, volume);
}

void AudioA5_StopMusic()
{
	/* We retain Adlib for sound effects.
	 */
	if (s_adlib != NULL)
		s_adlib->haltTrack();
}

void AudioA5_PollMusic()
{
	ALLEGRO_AUDIO_STREAM* stream = s_effect_stream;


	if (s_adlib == NULL || stream == NULL)
		return;

	void* frag = al_get_audio_stream_fragment(stream);
	if (frag == NULL)
		return;

	s_adlib->callback(s_adlib, (uint8 *)frag, FRAGLEN * al_get_audio_depth_size(ALLEGRO_AUDIO_DEPTH_INT16));
	al_set_audio_stream_fragment(stream, frag);
}

bool AudioA5_MusicIsPlaying()
{
	return s_adlib->isPlaying();
}

void AudioA5_PlaySoundEffect(SoundID effectID)
{
	if (s_adlib != NULL)
		s_adlib->playSoundEffect(effectID);
}

void AudioA5_StoreSample(SampleID sampleID, uint8 file_index, uint32 file_size)
{
	char header[0x1A];
	char ignore1;
	unsigned int size = 0;
	unsigned char rate;
	char ignore2;
	uint8* data = (uint8 *)al_malloc(file_size - 32);

	File_Read(file_index, header, 0x1A); /* "Creative Voice File..." */
	File_Read(file_index, &ignore1, 1); /* block type */
	File_Read(file_index, &size, 3);
	File_Read(file_index, &rate, 1);
	File_Read(file_index, &ignore2, 1);
	File_Read(file_index, data, file_size - 32);

	unsigned int freq = 1000000 / (256 - rate);
	ALLEGRO_AUDIO_DEPTH depth = ALLEGRO_AUDIO_DEPTH_UINT8;
	ALLEGRO_CHANNEL_CONF chan_conf = ALLEGRO_CHANNEL_CONF_1;

	al_destroy_sample(s_sample[sampleID]);
	s_sample[sampleID] = al_create_sample(data, size - 2, freq, depth, chan_conf, true);
}

bool AudioA5_PlaySample(SampleID sampleID, float volume, float pan)
{
	int idx_start, idx_end;
	float gain;

	if (s_sample[sampleID] == NULL)
		return true;

	if ((SAMPLE_VOICE_FRAGMENT_ENEMY <= sampleID && sampleID <= SAMPLE_VOICE_FRAGMENT_YOUR_NEXT_CONQUEST) ||
		(SAMPLE_INTRO_THREE_HOUSES_FIGHT <= sampleID && sampleID <= SAMPLE_INTRO_YOUR) ||
		(sampleID == SAMPLE_RADAR_STATIC))
	{
		idx_start = 0;
		idx_end = 0;
		gain = voice_volume * volume;
		pan = ALLEGRO_AUDIO_PAN_NONE;
	}
	else if (SAMPLE_AFFIRMATIVE <= sampleID && sampleID <= SAMPLE_MOVING_OUT)
	{
		idx_start = 1;
		idx_end = 1;
		gain = sound_volume * volume;
	}
	else
	{
		idx_start = 2;
		idx_end = MAX_SAMPLE_INSTANCES - 1;
		gain = sound_volume * volume;
	}

	return AudioA5_PlaySampleRaw(sampleID, gain, pan, idx_start, idx_end);
}

bool AudioA5_PlaySampleRaw(SampleID sampleID, float volume, float pan, int idx_start, int idx_end)
{
	if (s_sample[sampleID] == NULL)
		return true;

	if (pan < -100.0f)
		pan = ALLEGRO_AUDIO_PAN_NONE;

	for (int i = idx_start; i <= idx_end; i++)
	{
		ALLEGRO_SAMPLE_INSTANCE* si = s_instance[i];

		if (al_get_sample_instance_playing(si))
			continue;

		if (!al_set_sample(si, s_sample[sampleID]))
			continue;

		al_set_sample_instance_gain(si, volume);
		al_set_sample_instance_pan(si, pan);
		al_play_sample_instance(si);
		return true;
	}

	return false;
}

bool AudioA5_PollNarrator()
{
	return (al_get_sample_instance_playing(s_instance[0]));
}
