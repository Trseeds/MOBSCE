#ifndef AUDIO_H
#define AUDIO_H

#include "TYPES.h"

typedef struct Audio {
	uint32 Samplerate;
	uint16 Channels;
	uint16 Chunksize;
	uint32 Voices;
	uint8 MusicVolume;
	uint8 SoundVolume;
	bool Muted;
	uint32 Codecs;
} Audio;

int8 InitAudio(Engine* Engine);
uint8 ScreenPan(int32 X, uint32 EdgeDistance, Engine* Engine);
uint8 ScreenVolume(int32 X, uint32 EdgeDistance, Engine* Engine);
int8 PlaySound(Mix_Chunk* Sound, int32 Voice, uint8 Volume, uint8 Pan, Engine* Engine);
int8 MixMusicVolume(Engine* Engine);
int8 PlayMusic(Mix_Music* Music, int8 Loops, Engine* Engine);

#endif