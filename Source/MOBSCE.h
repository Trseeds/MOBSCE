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

#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_image.h"
#include "ini.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "BUTTONS.h"

#define MOBSCE_VERSION_RELEASE 0
#define MOBSCE_VERSION_FEATURE 24
#define MOBSCE_VERSION_PATCH 0
#define MOBSCE_VERSION "0.24.0"


#define STRING_BUFFER_SIZE 5120
#define OBJECT_NAME_SIZE 128
#define MIN_ALLOCATE 1000
#define EVENT_QUEUE_SIZE 512
#define TINT_NOCHANGE 255

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
#define WARNING_SDL_FAILURE 21
#define WARNING_INVALID_PARAMETER 22
#define WARNING_INIH_FAILURE 23
#define WARNING_IGNORABLE_FAILURE 24
#define WARNING_NULL NULL
#define RETURN_SUCCESS 0
#define SENTINEL_OBJECT (uint32)(0-1)

//basic data types
typedef unsigned char* bytepointer; //use for raw byte logic
typedef unsigned char ubyte;
typedef signed char sbyte;
typedef unsigned char bool; //use for true/false
#define false 0
#define true 1
typedef uint8_t uint8; //use for data that is not covered by the above
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;

enum Flip {
	FLIP_H = SDL_FLIP_HORIZONTAL,
	FLIP_V = SDL_FLIP_VERTICAL,
	FLIP_NONE = SDL_FLIP_NONE
};

typedef struct Vector2 {
	int32 X;
	int32 Y;
} Vector2;

typedef struct Vector3 {
	int32 X;
	int32 Y;
	int32 Z;
} Vector3;

typedef struct Vector4 {
	int32 X;
	int32 Y;
	int32 Z;
	int32 W;
} Vector4;

typedef struct FVector2 {
	double X;
	double Y;
} FVector2;

typedef struct FVector3 {
	double X;
	double Y;
	double Z;
} FVector3;

typedef struct FVector4 {
	double X;
	double Y;
	double Z;
	double W;
} FVector4;

typedef struct CustomSpriteData CustomSpriteData;
typedef struct CustomActorData CustomActorData;

typedef struct Engine Engine;

typedef struct Error {
	int8 Code;
	char Thrower[STRING_BUFFER_SIZE];
	char Message[STRING_BUFFER_SIZE];
	char Description[STRING_BUFFER_SIZE];
	char Parameters[STRING_BUFFER_SIZE];
} Error;

//config struct used for parsing the config file
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
	uint16 Codecs;
	bool Render;
	bool Audiate;
} Config;

typedef struct SpriteRenderParameters {
	SDL_Texture* Texture;
	Vector3 Position;
	Vector4 Origin;
	Vector2 Dimensions;
	uint8 Transparency;
	double Angle;
	uint8 Flip;
	Vector3 Tint;
	bool Visible;
} SpriteRenderParameters;

//objects
typedef struct Actor {
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
	char Name[OBJECT_NAME_SIZE];
	FVector2 Position;
	Vector2 Dimensions;
	int32 Voice;
	void (*Routine)(struct Actor*, Engine*);
	CustomActorData* CustomData;
} Actor;

typedef struct Sprite {
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
	char Name[OBJECT_NAME_SIZE];
	uint32 TextureID;
	SpriteRenderParameters RenderParameters;
	uint32 ActorReferenceIndex;
	uint64 ExpectedActorID;
	void (*Routine)(struct Sprite*, Engine*);
	CustomSpriteData* CustomData;
} Sprite;

//engine
typedef struct Audio {
	uint32 Samplerate;
	uint16 Channels;
	uint16 Chunksize;
	uint32 Voices;
	uint8 MusicVolume;
	uint8 SoundVolume;
	bool Muted;
	uint16 Codecs;
} Audio;

typedef struct Video {
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	Vector2 LogicalDimensions;
	Vector4 WindowBounds;
	uint16 WindowFlags;
	uint16 RendererFlags;
	char WindowTitle[STRING_BUFFER_SIZE];
	char WindowIconPath[STRING_BUFFER_SIZE];
} Video;

typedef struct Input {
	uint8* SDL_Keystate;
	uint8 SDL_PreviousKeystate[SDL_NUM_SCANCODES];
	uint32 SDL_MouseState;
	uint32 SDL_PreviousMouseState;
	bool KeysDown[SDL_NUM_SCANCODES];
	bool KeysUp[SDL_NUM_SCANCODES];
	Vector2 MousePosition;
	bool MouseDown[5];
	bool MouseUp[5];
	int8 VerticalMouseScroll;
	int8 HorizontalMouseScroll;
	//controller stuff
	SDL_GameController* Gamepad;
	bool GamepadIsConnected;
	uint8 GamepadPreviousState[14];
	double GamepadPreviousTriggersState[2];
	bool GamepadButtonsUp[14];
	bool GamepadButtonsDown[14];
	double GamepadTriggers[2];
	bool GamepadTriggersUp[2];
	double GamepadSticks[4];
} Input;

typedef struct Clock {
	double DeltaTime;
	uint64 CurrentTime;
	uint64 PreviousTime;
	uint64 TotalFrames;
	uint64 RealTime;
	double FrameRate;
} Clock;

typedef struct Resource {
	SDL_Texture** Textures;
	Mix_Chunk** Sounds;
	Mix_Music** Music;
	uint32 NumberOfTextures;
	uint32 NumberOfSounds;
	uint32 NumberOfMusics;
	uint32 NumberOfSprites;
	uint32 NumberOfSpriteReferences;
	uint32 NumberOfActors;
	uint32 NumberOfActorReferences;
	uint32 AllocatedTextureMemory;
	uint32 AllocatedSoundMemory;
	uint32 AllocatedMusicMemory;
	uint32 AllocatedSpriteMemory;
	uint32 AllocatedSpriteReferenceMemory;
	uint32 AllocatedActorMemory;
	uint32 AllocatedActorReferenceMemory;
} Resource;

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
	SDL_Event Events[EVENT_QUEUE_SIZE];
	uint64 IDCounter;
	bool Running;
	uint8 SpriteZResortNeeded;
	int8 ERROR_LEVEL;
	int8 WARNING_LEVEL;
} Engine;

typedef struct ResourceInfo {
	void* Pointer;
	uint16 SizeOfResource;
	void (*FreeFunction)(void*);
	uint32* AllocatedResourceMemory;
	uint32* NumberOfResources;
	bool IsPointerArray;
} ResourceInfo;

//engine stuff
bool IsZero(void* Pointer, int32 Size);
void ThrowError(Error* Error, Engine* Engine);
void ThrowWarning(Error* Warning, Engine* Engine);
uint64 GetNewObjectID(Engine* Engine);
int QSCompactPointerPool(const void* X, const void* Y);
int QSCompactObjectPool(const void* X, const void* Y);
int QSSortSpritesByZ(const void* X, const void* Y);
bool PoolCanBeShrunk(void* Pool, uint64 AllocatedElements, uint64 AllocatedSize);
int64 LinearMap(int64 Number, int64 DomainMin, int64 DomainMax, int64 RangeMin, int64 RangeMax);
void SeedRNG();
int64 GetRandomNumber(int64 Min, int64 Max);
int handler(void* user, const char* section, const char* name, const char* value);
int8 UpdateConfig(char* File, Config* Config, Engine* Engine);
int8 LoadEngineConfig(Engine* Engine);
int8 InitSDL(Engine* Engine);
void CleanupSDL();
int8 GetSDLEvents(Engine* Engine);
int8 GetBasePath(Engine* Engine);
char* GetAssetPath(char* Asset, char* Output, Engine* Engine);
Engine* InitEngine(char* ConfigFile, char* WindowTitle, char* WindowIconPath, int8 ERROR_LEVEL, int8 WARNING_LEVEL, bool Render, bool Audiate, bool ConfigProvided, Config* Config);
int8 RunEngine(Engine* Engine);
int8 CleanupEngine(Engine* Engine);

//audio
int8 InitAudio(Engine* Engine);
uint8 ScreenPan(int32 X, uint32 EdgeDistance, Engine* Engine);
uint8 ScreenVolume(int32 X, uint32 EdgeDistance, Engine* Engine);
int8 PlaySound(uint32 SoundID, int32 Voice, uint8 Volume, uint8 Pan, Engine* Engine); //change panning logic to be 0-100 with 0 being hard left and 100 being hard right.
int MixMusicVolume(Engine* Engine);
int PlayMusic(int32 MusicID, Engine* Engine);

//video
int InitVideo(Engine* Engine);
int RestartVideo(Engine* Engine);
int CleanupVideo(Engine* Engine);
int DrawSprite(Sprite* Sprite, Engine* Engine);
int Render(Engine* Engine);

//input
int GetKeyboardInput(Engine* Engine);
int GetMouseInput(Engine* Engine);
int GetGamepadInput(Engine* Engine);
int GetInput(Engine* Engine); 
int RumbleGamepad(int Strength, int Duration, Engine* Engine);

//clock
int KeepTime(Engine* Engine);

//resource
void* OSMemoryAllocate(size_t Size);
void OSMemoryFree(void* Pointer, size_t Size);
int InitResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
int ExtendResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
int ShrinkResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
int CleanupResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
void* FindOpenResourceSpace(void* Pool, int PoolSize, int Size);
Uint32 FindOpenReferenceSpace(void* Pool, int AllocatedReferenceMemory);
void SpriteFreeFunction(void* SpritePtr);
void ActorFreeFunction(void* ActorPtr);
//audio
int CacheSound(char* File, Engine* Engine);
int CacheMusic(char* File, Engine* Engine);
//video
int CacheTexture(char* File, Engine* Engine);
//objects
uint32 CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, int TextureID, CustomSpriteData* CustomData, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine);
uint32 CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, int Voice, CustomActorData* CustomData, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine);
int DestroySprite(Sprite* DSprite, void (*FreeFunction)(void*), Engine* Engine);
int DestroyActor(Actor* DActor, void (*FreeFunction)(void*), Engine* Engine);
uint32 GetSpriteByName(char* Name, Engine* Engine);
uint32 GetSpriteByID(Uint64 ID, Engine* Engine);
uint32 GetSpriteByProperty(void* Property, uint32 Offset, uint32 Size, Engine* Engine);
uint32 GetActorByName(char* Name, Engine* Engine);
uint32 GetActorByID(Uint64 ID, Engine* Engine);
uint32 GetActorByProperty(void* Property, uint32 Offset, uint32 Size, Engine* Engine);

#endif