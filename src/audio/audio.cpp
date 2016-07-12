/* audio.c */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "../os/common.h"
#include "../os/math.h"

#include "audio.h"

#include "../config.h"
#include "../file.h"
#include "../gui/gui.h"
#include "../opendune.h"
#include "../sprites.h"
#include "../string.h"
#include "../table/locale.h"
#include "../table/widgetinfo.h"
#include "../tile.h"
#include "../timer/timer.h"
#include "../tools/coord.h"
#include "../tools/random_general.h"
#include "../tools/random_lcg.h"

bool g_enable_audio;

bool g_enable_music = true;
bool g_enable_sound = true;
bool g_enable_subtitles = false;

float music_volume = 0.85f;
float sound_volume = 0.65f;
float voice_volume = 1.0f;

bool g_opl_mame = true;

static MusicInfo* curr_music;
char music_message[128];

static SampleSet s_curr_sample_set = SAMPLESET_INVALID;

static int64_t s_sample_last_played[SAMPLEID_MAX];
static SampleID s_voice_queue[256];
static int s_voice_head = 0;
static int s_voice_tail = 0;

void Audio_DisplayMusicName()
{
	GUI_DisplayText(music_message, 5);
}

static void Audio_PlayMusicGroup(MusicID musicID, bool respect_want_setting)
{
	AudioA5_StopMusic();

	if (musicID == MUSIC_STOP)
	{
		curr_music = NULL;
		music_message[0] = '\0';
		return;
	}

	if ((!g_enable_audio) || (!g_enable_music) || (musicID == MUSIC_INVALID))
		return;

	int start = musicID;
	int end = musicID;

	if (musicID == MUSIC_RANDOM_IDLE)
	{
		start = MUSIC_IDLE1;
		end = MUSIC_BONUS;
	}
	else if (musicID == MUSIC_RANDOM_ATTACK)
	{
		start = MUSIC_ATTACK1;
		end = MUSIC_ATTACK6;
	}

	int num_songs = end - start;
	if (num_songs > 0)
	{
		int r = Tools_RandomLCG_Range(0, num_songs - 1);
		Audio_PlayMusicFile(&g_table_music[start + r]);
	}
	else
		Audio_PlayMusicFile(&g_table_music[musicID]);
}

void Audio_PlayMusicFile(MusicInfo* m)
{
	if (!g_enable_audio)
		return;

	AudioA5_InitMusic(m);
	curr_music = m;

	if (m->songname != NULL)
		snprintf(music_message, sizeof(music_message), "Playing %s", m->songname);
	else
		snprintf(music_message, sizeof(music_message), "Playing %s", m->filename);
}

void Audio_PlayMusic(MusicID musicID)
{
	Audio_PlayMusicGroup(musicID, true);
}

void Audio_PlayMusicIfSilent(MusicID musicID)
{
	if (!Audio_MusicIsPlaying())
		Audio_PlayMusic(musicID);
}

void Audio_ReplayMusic()
{
	if (curr_music == NULL)
		return;

	Audio_PlayMusicFile(curr_music);
}

void Audio_StopMusicUnlessMenu()
{
	if (curr_music == NULL)
		return;

	if (curr_music == &g_table_music[MUSIC_MAIN_MENU])
		return;

	AudioA5_StopMusic();
}

void Audio_AdjustMusicVolume(float delta, bool adjust_current_track_only)
{
	if (curr_music == NULL)
		return;

	MusicInfo* m = curr_music;
	float volume;

	/* Adjust single track. */
	if (adjust_current_track_only)
		volume = music_volume;
	else
	{
		music_volume += delta;
		music_volume = clamp(0.0f, music_volume, 1.0f);
		volume = music_volume;
	}

	char* str = strstr(music_message, ", vol");
	if (str == NULL)
		str = music_message + strlen(music_message);

	snprintf(str, sizeof(music_message) - (str - music_message), ", vol %.2f", music_volume);
	AudioA5_SetMusicVolume(volume);
}

void Audio_PlayEffect(SoundID effectID)
{
	if (!g_enable_audio || !g_enable_sound)
		return;

	AudioA5_PlaySoundEffect(effectID);
}

static char Audio_GetSamplePrefix(SampleSet setID)
{
	switch (setID)
	{
	case SAMPLESET_HARKONNEN:
		return 'H';

	case SAMPLESET_ATREIDES:
		return 'A';

	case SAMPLESET_ORDOS:
		return 'O';

	case SAMPLESET_BENE_GESSERIT:
		return 'M';

	default:
		break;
	}

	return 'Z';
}

static void Audio_LoadSample(const char* filename, SampleID sampleID)
{
	if (filename == NULL || !File_Exists(filename))
		return;

	const uint8 file_index = File_Open(filename, FILE_MODE_READ);
	const uint32 file_size = File_GetSize(file_index);

	AudioA5_StoreSample(sampleID, file_index, file_size);
	File_Close(file_index);
}

static void Audio_LoadSampleFromSet(SampleSet setID, SampleID sampleID)
{
	const SoundData* s = &g_table_voices[sampleID];
	const char* filename;
	char buf[16];

	/* [+-/?]FILENAME. */
	filename = s->string + 1;
	switch (s->string[0])
	{
	case '+':
		/* +: common to all houses. */
		if (s_curr_sample_set != SAMPLESET_INVALID)
			return;

		/* +%c: common to all houses, substitute with language prefix. */
		if (s->string[1] == '%')
		{
			char prefix = Audio_GetSamplePrefix(SAMPLESET_INVALID);
			snprintf(buf, sizeof(buf), s->string + 1, prefix);
			filename = buf;
		}
		break;

	case '-':
		/* -: common to all houses. */
		if (s_curr_sample_set != SAMPLESET_INVALID)
			return;
		break;

	case '/':
		/* /: Bene Gesserit only (called mercenary in Dune II). */
		/* if (setID != SAMPLESET_BENE_GESSERIT) return; */
		if (s_curr_sample_set != SAMPLESET_INVALID)
			return;
		break;

	case '?':
		/* ?%c: load as required, substitute with house or language prefix. */
		if (s->string[1] == '%')
		{
			char prefix = Audio_GetSamplePrefix(setID);
			snprintf(buf, sizeof(buf), s->string + 1, prefix);
			filename = buf;
		}
		break;

	case '%':
		/* %c: substitute with house or language prefix. */
		{
			char prefix = Audio_GetSamplePrefix(setID);
			snprintf(buf, sizeof(buf), s->string, prefix);
			filename = buf;
		}
		break;

	default:
		return;
	}

	Audio_LoadSample(filename, sampleID);
}

void Audio_LoadSampleSet(SampleSet setID)
{
	if (!g_enable_audio)
		return;

	/* Initialisation. */
	if (setID == SAMPLESET_INVALID)
	{
		Audio_LoadSample(g_table_voices[SAMPLE_RADAR_STATIC].string + 1, SAMPLE_RADAR_STATIC);
	}

	if (s_curr_sample_set == setID)
		return;

	for (int sampleID = 0; sampleID < SAMPLEID_MAX; sampleID++)
	{
		Audio_LoadSampleFromSet(setID, (SampleID)sampleID);
	}

	s_curr_sample_set = setID;
}

void Audio_PlaySample(SampleID sampleID, int volume, float pan)
{
	if (!g_enable_audio || sampleID == SAMPLE_INVALID || !g_enable_sound)
		return;

	const int64_t curr_ticks = Timer_GetTicks();
	assert(sampleID < SAMPLEID_MAX);

	if (curr_ticks - s_sample_last_played[sampleID] > 8)
	{
		s_sample_last_played[sampleID] = curr_ticks;
		AudioA5_PlaySample(sampleID, (float)volume / 255.0f, pan);
	}
}

void Audio_PlaySoundAtTile(SoundID soundID, tile32 position)
{
	if (soundID == SOUND_INVALID)
		return;

	assert(soundID < SOUNDID_MAX);

	const SampleID sampleID = g_table_voiceMapping[soundID];
	if (sampleID == SAMPLE_INVALID)
		return;

	int volume = 255;
	float pan = 0.0f;

	if (position.x != 0 && position.y != 0)
	{
		const WidgetInfo* wi = &g_table_gameWidgetInfo[GAME_WIDGET_VIEWPORT];
		const int cx = Tile_GetPackedX(g_viewportPosition) + wi->width / (2 * TILE_SIZE);
		const int cy = Tile_GetPackedY(g_viewportPosition) + wi->height / (2 * TILE_SIZE);
		const uint16 packed = Tile_PackXY(cx, cy);

		const int ux = Tile_GetPosX(position);

		volume = Tile_GetDistancePacked(packed, Tile_PackTile(position));
		if (volume > 64)
			volume = 64;

		volume = 255 - (volume * 255 / 80);
		pan = clamp(-0.5f, 0.05f * (ux - cx), 0.5f);
	}

	Audio_PlaySample(sampleID, volume, pan);
}

void Audio_PlaySound(SoundID soundID)
{
	tile32 tile;

	tile.x = 0;
	tile.y = 0;
	Audio_PlaySoundAtTile(soundID, tile);
}

void Audio_PlaySoundCutscene(SoundID soundID)
{
	if ((soundID == SOUND_INVALID) || !g_enable_sound)
		return;

	const SampleID sampleID = g_table_voiceMapping[soundID];
	if (sampleID != SAMPLE_INVALID)
		AudioA5_PlaySampleRaw(sampleID, voice_volume, -1000.0f, 1, 11);
}

static bool Audio_QueueVoice(SampleID sampleID)
{
	const int index = (s_voice_tail + 1) & 0xFF;

	if (index == s_voice_head)
		return false;

	s_voice_queue[s_voice_tail] = sampleID;
	s_voice_tail = index;
	return true;
}

void Audio_PlayVoice(VoiceID voiceID)
{
	if (voiceID == VOICE_INVALID)
		return;

	if (voiceID == VOICE_STOP)
	{
		s_voice_head = s_voice_tail;
		return;
	}

	assert(voiceID < VOICEID_MAX);
	const Feedback* s = &g_feedback[voiceID];

	if (g_enable_sound)
	{
		for (int i = 0; i < NUM_SPEECH_PARTS; i++)
		{
			const SampleID sampleID	= s->voiceId[i];

			if (sampleID == SAMPLE_INVALID)
				break;

			if (!Audio_QueueVoice(sampleID))
				break;
		}

		Audio_Poll();
	}

	if (g_enable_subtitles)
	{
		g_viewportMessageText = String_Get_ByIndex(s->messageId);
		g_viewportMessageCounter = 4;
	}
}

bool Audio_Poll()
{
	if (!g_enable_audio || !g_enable_sound)
		return false;

	bool playing = AudioA5_PollNarrator();
	if (playing)
		return true;

	if (s_voice_head != s_voice_tail)
	{
		playing = AudioA5_PlaySample(s_voice_queue[s_voice_head], 1.0f, 0.0f);

		if (playing)
			s_voice_head = (s_voice_head + 1) & 0xFF;
	}

	return playing;
}
