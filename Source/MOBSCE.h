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
#define MOBSCE_VERSION_FEATURE 27
#define MOBSCE_VERSION_PATCH 0
#define MOBSCE_VERSION "0.27.0"

#define EVENT_QUEUE_SIZE 512

#define ERROR_SHOW_ALL 2
#define ERROR_SHOW_MESSAGE 1
#define ERROR_SHOW 0
#define ERROR_DISABLE -1
#define WARNING_SHOW_ALL 2
#define WARNING_SHOW_MESSAGE 1
#define WARNING_DISABLE 0

#define ERROR_INVALID_ENGINE -1
#define ERROR_SDL_FAILURE 11
#define ERROR_MEMORY 12
#define ERROR_INVALID_PARAMETER 13
#define ERROR_TIME_FAILURE 14
#define WARNING_SDL_FAILURE 21
#define WARNING_INVALID_PARAMETER 22
#define WARNING_INIH_FAILURE 23
#define WARNING_IGNORABLE_FAILURE 24
#define WARNING_NULL NULL
#define RETURN_SUCCESS 0
#define SENTINEL_OBJECT (uint32)(0-1)

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
	SDL_Event Events[EVENT_QUEUE_SIZE];
	uint64 IDCounter;
	uint8 SpriteZResortNeeded;
	int8 ERROR_LEVEL;
	int8 WARNING_LEVEL;
	bool RandSeeded;
	bool Running;
} Engine;

bool IsZero(void* Pointer, int32 Size);
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
int8 RunEngine(Engine* Engine);
int8 CleanupEngine(Engine* Engine);

#endif