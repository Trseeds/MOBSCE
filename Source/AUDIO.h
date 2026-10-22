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
int8 PlaySound(uint32 SoundID, int32 Voice, uint8 Volume, uint8 Pan, Engine* Engine); //change panning logic to be 0-100 with 0 being hard left and 100 being hard right.
int8 MixMusicVolume(Engine* Engine);
int8 PlayMusic(int32 MusicID, Engine* Engine);

#endif