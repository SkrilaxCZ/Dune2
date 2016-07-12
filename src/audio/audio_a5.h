#ifndef AUDIO_AUDIOA5_H
#define AUDIO_AUDIOA5_H

#include "types.h"
#include "../table/sound.h"

void AudioA5_Init();
void AudioA5_Uninit();

void AudioA5_InitMusic(const MusicInfo* m);
void AudioA5_SetMusicVolume(float volume);
void AudioA5_StopMusic();
void AudioA5_PollMusic();
bool AudioA5_MusicIsPlaying();
void AudioA5_PlaySoundEffect(SoundID effectID);

void AudioA5_StoreSample(SampleID sampleID, uint8 file_index, uint32 file_size);
bool AudioA5_PlaySample(SampleID sampleID, float volume, float pan);
bool AudioA5_PlaySampleRaw(SampleID sampleID, float volume, float pan, int idx_start, int idx_end);
bool AudioA5_PollNarrator();

#endif
