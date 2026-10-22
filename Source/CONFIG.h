#ifndef CONFIG_H
#define CONFIG_H

#include "INCLUDE.h"
#include "TYPES.h"

typedef struct Config {
	uint32 Samplerate;
	uint16 Channels;
	uint16 Chunksize;
	uint32 Voices;
	uint8 MusicVolume;
	uint8 SoundVolume;
	bool Muted;
	uint16 LogicalX;
	uint16 LogicalY;
	uint16 WindowFlags;
	uint16 RendererFlags;
	uint32 Codecs;
	uint32 Formats;
	bool Render;
	bool Audiate;
	float WindowScale;
} Config;

int handler(void* user, const char* section, const char* name, const char* value);
int8 UpdateConfig(char* File, Config* Config, Engine* Engine);
int8 LoadEngineConfig(Engine* Engine);

#endif