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
//include sdl image at some point
#include "ini.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include "GAME.h"

#define ERROR_BUFFER_SIZE 1024
#define OBJECT_NAME_SIZE 64
#define MIN_ALLOCATE 16

//basic data types
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
} Config;

//objects
typedef struct Actor {
	char Name[OBJECT_NAME_SIZE];
	Uint64 ID;
	FVector2 Position;
	Vector2 Dimensions;
	int Voice;
	void (*Routine)(struct Actor*, struct Engine*);
	CustomActorData CustomData;
} Actor;

typedef struct Sprite {
	char Name[OBJECT_NAME_SIZE];
	Uint64 ID;
	SDL_Texture* Texture;
	int TextureID;
	Vector3 Position;
	Vector4 Origin;
	Vector2 Dimensions;
	int Visible;
	Actor* Actor;
	int (*Routine)(struct Sprite*, struct Engine*);
	CustomSpriteData CustomData;
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
} Audio;

typedef struct Video {
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	Vector2 LogicalDimensions;
	Vector4 WindowBounds;
	int WindowFocused;
	int WindowFlags;
	int RendererFlags;
} Video;

typedef struct Input {
	Uint8* SDL_Keystate;
	Uint8 SDL_PreviousKeystate[SDL_NUM_SCANCODES];
	int KeysDown[SDL_NUM_SCANCODES];
	int KeysUp[SDL_NUM_SCANCODES];
	Vector2 MousePosition;
	Vector3 MouseDown;
	Vector3 MouseUp;
	Vector3 MouseScroll;
	//controller stuff
	//implement this later
	Vector4 GamePadFaceButtonsUp;
	Vector4 GamePadFaceButtonsDown;
	Vector4 GamePadDPadUp;
	Vector4 GamePadDPadDown;
	Vector2 GamePadBumpersUp;
	Vector2 GamePadBumpersDown;
	FVector2 GamePadTriggers;
	FVector4 GamePadSticks;
	Vector2 GamePadSticksPressedUp;
	Vector2 GamePadSticksPressedDown;
} Input;

typedef struct Clock {
	double DeltaTime;
	Uint64 CurrentTime;
	Uint64 PreviousTime;
	Uint64 TotalTime;
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
	int NumberOfActors;
	int AllocatedTextureMemory;
	int AllocatedSoundMemory;
	int AllocatedMusicMemory;
	int AllocatedSpriteMemory;
	int AllocatedActorMemory;
} Resource;

typedef struct Engine {
	char BasePath[1024];
	char ConfigPath[1024];
	Config Config;
	Audio Audio;
	Video Video;
	Input Input;
	Clock Clock;
	Resource Resource;
	Actor** Actors;
	Sprite** Sprites;
	SDL_Event* Event;
	Uint64 IDCounter;
	int Running;
	int SpriteZResortNeeded;
} Engine;

typedef struct ResourceInfo {
	void* Pointer;
	void (*FreeFunction)(void*);
	int* AllocatedResourceMemory;
	int* NumberOfResources;
} ResourceInfo;

//engine stuff
void ThrowError(char* Message, char* Thrower, Engine* Engine); //done
void ThrowWarning(char* Message, char* Thrower); //done
int GetNewObjectID(Engine* Engine);
int CompactArray(const void* X, const void* Y); //done
int SortSpritesByZ(const void* X, const void* Y); //done
int ArrayCanBeShrunk(void* Array, int AllocatedElements, int AllocatedSize); //done
int LinearMap(int Number, int NumberMax, int RangeMax, int RangeMin); //done
void SeedRNG(); //done
int GetRandomNumber(int Max); //done
int handler(void* user, const char* section, const char* name, const char* value); //done
int UpdateEngineConfig(char* File, Config* Config, Engine* Engine); //done
void LoadEngineConfig(Engine* Engine); //done
int InitSDL(); //done
int GetBasePath(Engine* Engine); //done
char* GetAssetPath(char* Asset, Engine* Engine); //done
Engine* InitEngine(char* ConfigFile); //done
void RunEngine(Engine* Engine); //TODO
void CleanupEngine(Engine* Engine); //TODO

//audio
int InitAudio(Engine* Engine); //done
int PlaySound(Mix_Chunk* Sound, int Voice, int Volume, int Pan); //TODO
void MixMusic(); //TODO
int PlayMusic(); //TODO

//video
int InitVideo(Engine* Engine); //done
void CleanupVideo(Engine* Engine); //done
int DrawTexture(SDL_Texture* Texture, Vector2 Position, Vector2 Origin, Engine* Engine); //done
int DrawSprite(Sprite* Sprite, Engine* Engine); //done
void Render(Engine* Engine); //done

//input
void GetInput(Engine* Engine); //TODO

//clock
void KeepTime(Engine* Engine); //done

//resource
int InitResourcePool(ResourceInfo ResourceInfo, Engine* Engine); //done
int ExtendResourcePool(ResourceInfo ResourceInfo, Engine* Engine); //done
int ShrinkResourcePool(ResourceInfo ResourceInfo, Engine* Engine); //done
void CleanupResourcePool(ResourceInfo ResourceInfo, Engine* Engine); //done
//audio
int CacheSound(char* File, Engine* Engine); //done
int CacheMusic(char* File, Engine* Engine); //done
//video
int CacheTexture(char* File, Engine* Engine); //done
//objects
int CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, int TextureID, int Visible, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine); //done
int CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, int Voice, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine); //done
void DestroySprite(Sprite* DSprite, Engine* Engine); //done
void DestroyActor(Actor* DActor, Engine* Engine); //done
Sprite* GetSpriteByName(char* Name, Engine* Engine); //TODO
Actor* GetActorByName(char* Name, Engine* Engine); //TODO

#endif