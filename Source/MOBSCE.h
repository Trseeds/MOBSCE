/*
MOBSCE
Modular
Object
Based
SDL
C
Engine
*/

#ifndef MOBSCE_H
#define MOBSCE_H

#include "INCLUDE.h"

typedef struct Engine Engine;

#include "TYPES.h"
#include "OBJECTS.h"
#include "AUDIO.h"
#include "VIDEO.h"
#include "INPUT.h"
#include "CONFIG.h"
#include "RESOURCE.h"
#include "CLOCK.h"

#define MOBSCE_VERSION_RELEASE 0
#define MOBSCE_VERSION_FEATURE 30
#define MOBSCE_VERSION_PATCH 0
#define MOBSCE_VERSION "0.30.0"

#define EVENT_QUEUE_SIZE 512

enum Warnings {
	WARNING_SDL_FAILURE = 21,
	WARNING_INVALID_PARAMETER = 22,
	WARNING_INIH_FAILURE = 23,
	WARNING_IGNORABLE_FAILURE = 24,
};

enum Errors {
	ERROR_INVALID_ENGINE = -1,
	ERROR_SDL_FAILURE = 11,
	ERROR_MEMORY = 12,
	ERROR_INVALID_PARAMETER = 13,
	ERROR_TIME_FAILURE = 14,
};

#define ERROR_SHOW_ALL 2
#define ERROR_SHOW_MESSAGE 1
#define ERROR_SHOW 0
#define ERROR_DISABLE -1
#define WARNING_SHOW_ALL 2
#define WARNING_SHOW_MESSAGE 1
#define WARNING_DISABLE 0

#define RETURN_SUCCESS 0
#define SENTINEL_OBJECT (uint32)(0-1)
#define SENTINEL_ID 0

typedef struct Engine Engine;

typedef struct Error {
	int8 Code;
	char Thrower[STRING_BUFFER_SIZE];
	char Message[STRING_BUFFER_SIZE];
	char Description[STRING_BUFFER_SIZE];
	char Parameters[STRING_BUFFER_SIZE];
} Error;

typedef struct Engine {
	char BasePath[STRING_BUFFER_SIZE];
	char ConfigPath[STRING_BUFFER_SIZE];
	Config Config;
	Audio Audio;
	Video Video;
	Input Input;
	Clock Clock;
	Resource Resource;
	Actor* Actors;
	Actor** ActorReferences;
	Sprite* Sprites;
	Sprite** SpriteReferences;
	Timer* Timers;
	Timer** TimerReferences;
	Sound* Sounds;
	Sound** SoundReferences;
	Music* Musics;
	Music** MusicReferences;
	Texture* Textures;
	Texture** TextureReferences;
	SDL_Event Events[EVENT_QUEUE_SIZE];
	uint64 IDCounter;
	uint8 SpriteZResortNeeded;
	int8 ERROR_LEVEL;
	int8 WARNING_LEVEL;
	bool Running;
} Engine;

void ThrowError(Error* Error, Engine* Engine);
void ThrowWarning(Error* Warning, Engine* Engine);
uint64 GetNewObjectID(Engine* Engine);
int64 LinearMap(int64 Number, int64 DomainMin, int64 DomainMax, int64 RangeMin, int64 RangeMax);
void SeedRNG(Engine* Engine);
int64 GetRandomNumber(int64 Min, int64 Max);
int8 InitSDL(Engine* Engine);
void CleanupSDL();
int8 GetSDLEvents(Engine* Engine);
int8 GetBasePath(Engine* Engine);
char* GetAssetPath(char* Asset, char* Output, Engine* Engine);
Engine* InitEngine(char* ConfigFile, char* WindowTitle, char* WindowIconPath, int8 ERROR_LEVEL, int8 WARNING_LEVEL, uint32 AllocationThreshold, bool Render, bool Audiate, Config* Config);
void TickObjects(Engine* Engine);
int8 RunEngine(Engine* Engine);
int8 CleanupEngine(Engine* Engine);

#endif