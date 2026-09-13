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
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "BUTTONS.h"

#define MOBSCE_VERSION_RELEASE 0
#define MOBSCE_VERSION_FEATURE 23
#define MOBSCE_VERSION_PATCH 0
#define MOBSCE_VERSION "0.23.0"

#define OBJECT_USED 1

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

enum Flip {
	FLIP_H = SDL_FLIP_HORIZONTAL,
	FLIP_V = SDL_FLIP_VERTICAL,
	FLIP_NONE = SDL_FLIP_NONE
};

//basic data types
typedef unsigned char ubyte;
typedef signed char sbyte;

typedef struct Vector2 {
	int X;
	int Y;
} Vector2;

typedef struct Vector3 {
	int X;
	int Y;
	int Z;
} Vector3;

typedef struct Vector4 {
	int X;
	int Y;
	int Z;
	int W;
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

//config struct used for parsing the config file
typedef struct Config {
	int Samplerate;
	int Channels;
	int Chunksize;
	int Voices;
	int MusicVolume;
	int SoundVolume;
	int Muted;
	int LogicalX;
	int LogicalY;
	int WindowFlags;
	int RendererFlags;
	int Codecs;
} Config;

typedef struct SpriteRenderParameters {
	SDL_Texture* Texture;
	Vector3 Position;
	Vector4 Origin;
	Vector2 Dimensions;
	int Transparency;
	double Angle;
	int Flip;
	Vector3 Tint;
	int Visible;
} SpriteRenderParameters;

//objects
typedef struct Actor {
	ubyte IsUsed;
	Uint64 ID;
	Uint32 ReferenceIndex;
	char Name[OBJECT_NAME_SIZE];
	FVector2 Position;
	Vector2 Dimensions;
	int Voice;
	void (*Routine)(struct Actor*, Engine*);
	CustomActorData* CustomData;
} Actor;

typedef struct Sprite {
	ubyte IsUsed;
	Uint64 ID;
	Uint32 ReferenceIndex;
	char Name[OBJECT_NAME_SIZE];
	int TextureID;
	SpriteRenderParameters RenderParameters;
	Uint32 ActorReferenceIndex;
	Uint64 ExpectedActorID;
	void (*Routine)(struct Sprite*, Engine*);
	CustomSpriteData* CustomData;
} Sprite;

//engine
typedef struct Audio {
	int Samplerate;
	int Channels;
	int Chunksize;
	int Voices;
	int MusicVolume;
	int SoundVolume;
	int Muted;
	int Codecs;
} Audio;

typedef struct Video {
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	Vector2 LogicalDimensions;
	Vector4 WindowBounds;
	int WindowFocused;
	int WindowFlags;
	int RendererFlags;
	char WindowTitle[STRING_BUFFER_SIZE];
	char WindowIconPath[STRING_BUFFER_SIZE];
} Video;

typedef struct Input {
	Uint8* SDL_Keystate;
	Uint8 SDL_PreviousKeystate[SDL_NUM_SCANCODES];
	Uint32 SDL_MouseState;
	Uint32 SDL_PreviousMouseState;
	ubyte KeysDown[SDL_NUM_SCANCODES];
	ubyte KeysUp[SDL_NUM_SCANCODES];
	Vector2 MousePosition;
	ubyte MouseDown[5];
	ubyte MouseUp[5];
	sbyte VerticalMouseScroll;
	sbyte HorizontalMouseScroll;
	//controller stuff
	SDL_GameController* Gamepad;
	ubyte GamepadIsConnected;
	ubyte GamepadPreviousState[14];
	double GamepadPreviousTriggersState[2];
	ubyte GamepadButtonsUp[14];
	ubyte GamepadButtonsDown[14];
	double GamepadTriggers[2];
	ubyte GamepadTriggersUp[2];
	double GamepadSticks[4];
} Input;

typedef struct Clock {
	double DeltaTime;
	Uint64 CurrentTime;
	Uint64 PreviousTime;
	Uint64 TotalFrames;
	Uint64 RealTime;
	double FrameRate;
} Clock;

typedef struct Resource {
	SDL_Texture** Textures;
	Mix_Chunk** Sounds;
	Mix_Music** Music;
	int NumberOfTextures;
	int NumberOfSounds;
	int NumberOfMusics;
	int NumberOfSprites;
	int NumberOfSpriteReferences;
	int NumberOfActors;
	int NumberOfActorReferences;
	int AllocatedTextureMemory;
	int AllocatedSoundMemory;
	int AllocatedMusicMemory;
	int AllocatedSpriteMemory;
	int AllocatedSpriteReferenceMemory;
	int AllocatedActorMemory;
	int AllocatedActorReferenceMemory;
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
	Uint64 IDCounter;
	ubyte Running;
	ubyte SpriteZResortNeeded;
	ubyte ERROR_LEVEL;
	ubyte WARNING_LEVEL;
} Engine;

typedef struct ResourceInfo {
	void* Pointer;
	int SizeOfResource;
	void (*FreeFunction)(void*);
	int* AllocatedResourceMemory;
	int* NumberOfResources;
	ubyte IsPointerArray;
} ResourceInfo;

//engine stuff
int IsZero(void* Pointer, int Size);
void ThrowError(char* Message, char* Thrower, Engine* Engine);
void ThrowWarning(char* Message, char* Thrower, Engine* Engine);
Uint64 GetNewObjectID(Engine* Engine);
int QSCompactPointerPool(const void* X, const void* Y);
int QSCompactObjectPool(const void* X, const void* Y);
int QSSortSpritesByZ(const void* X, const void* Y);
int PoolCanBeShrunk(void* Pool, int AllocatedElements, int AllocatedSize);
int LinearMap(int Number, int NumberMax, int RangeMax, int RangeMin);
void SeedRNG();
int GetRandomNumber(int Min, int Max);
int handler(void* user, const char* section, const char* name, const char* value);
int UpdateConfig(char* File, Config* Config, Engine* Engine);
int LoadEngineConfig(Engine* Engine);
int InitSDL(Engine* Engine);
void CleanupSDL();
int GetSDLEvents(Engine* Engine);
int GetBasePath(Engine* Engine);
char* GetAssetPath(char* Asset, char* Output, Engine* Engine);
Engine* InitEngine(char* ConfigFile, char* WindowTitle, char* WindowIconPath, int ERROR_LEVEL, int WARNING_LEVEL);
int RunEngine(Engine* Engine);
int CleanupEngine(Engine* Engine);

//audio
int InitAudio(Engine* Engine);
int* EasyPan(int Pan, int Max, int* Output);
int PlaySound(int SoundID, int Voice, int Volume, int Pan, Engine* Engine);
int MixMusicVolume(Engine* Engine);
int PlayMusic(int MusicID, Engine* Engine);

//video
int InitVideo(Engine* Engine);
int RestartVideo(Engine* Engine);
int CleanupVideo(Engine* Engine);
int DrawTexture(SDL_Texture* Texture, Vector2 Position, Vector2 Origin, Engine* Engine);
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
Sprite* CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, int TextureID, CustomSpriteData* CustomData, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine);
Actor* CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, int Voice, CustomActorData* CustomData, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine);
int DestroySprite(Sprite* DSprite, void (*FreeFunction)(void*), Engine* Engine);
int DestroyActor(Actor* DActor, void (*FreeFunction)(void*), Engine* Engine);
Sprite* GetSpriteByName(char* Name, Engine* Engine);
Actor* GetActorByName(char* Name, Engine* Engine);
Actor* GetActorByID(Uint64 ID, Engine* Engine);

#endif