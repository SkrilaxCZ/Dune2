#ifndef AUDIO_AUDIO_H
#define AUDIO_AUDIO_H

#include "../table/sound.h"

extern bool g_enable_audio;
extern bool g_enable_music;
extern bool g_enable_sound;
extern bool g_enable_subtitles;

extern float music_volume;
extern float sound_volume;
extern float voice_volume;

extern bool g_opl_mame;
extern char sound_font_path[1024];
extern char music_message[128];

void Audio_DisplayMusicName();
void Audio_PlayMusic(MusicID musicID);
void Audio_PlayMusicFile(MusicInfo* m);
void Audio_PlayMusicIfSilent(MusicID musicID);
void Audio_StopMusicUnlessMenu();
void Audio_ReplayMusic();
void Audio_AdjustMusicVolume(float delta, bool adjust_current_track_only);
void Audio_PlayEffect(SoundID effectID);
void Audio_LoadSampleSet(SampleSet setID);
void Audio_PlaySample(SampleID sampleID, int volume, float pan);
void Audio_PlaySoundAtTile(SoundID soundID, tile32 position);
void Audio_PlaySound(SoundID soundID);
void Audio_PlaySoundCutscene(SoundID soundID);
void Audio_PlayVoice(VoiceID voiceID);
bool Audio_Poll();

#include "audio_a5.h"

#define Audio_PollMusic         AudioA5_PollMusic
#define Audio_MusicIsPlaying    AudioA5_MusicIsPlaying

#endif
