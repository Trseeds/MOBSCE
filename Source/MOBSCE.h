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
#include "BUTTONS.h"
#include "GAME.h"

#define STRING_BUFFER_SIZE 1024
#define OBJECT_NAME_SIZE 64
#define MIN_ALLOCATE 16
#define EVENT_QUEUE_SIZE 512

#define INVALID_ENGINE -1

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
	int Codecs;
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
	void (*Routine)(struct Sprite*, struct Engine*);
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
} Video;

typedef struct Input {
	Uint8* SDL_Keystate;
	Uint8 SDL_PreviousKeystate[SDL_NUM_SCANCODES];
	Uint32 SDL_MouseState;
	Uint32 SDL_PreviousMouseState;
	int KeysDown[SDL_NUM_SCANCODES];
	int KeysUp[SDL_NUM_SCANCODES];
	Vector2 MousePosition;
	int MouseDown[5];
	int MouseUp[5];
	int VerticalMouseScroll;
	int HorizontalMouseScroll;
	//controller stuff
	SDL_GameController* Gamepad;
	int GamepadIsConnected;
	int GamepadPreviousState[14];
	double GamepadPreviousTriggersState[2];
	int GamepadButtonsUp[14];
	int GamepadButtonsDown[14];
	double GamepadTriggers[2];
	int GamepadTriggersUp[2];
	double GamepadSticks[4];
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
	char BasePath[STRING_BUFFER_SIZE];
	char ConfigPath[STRING_BUFFER_SIZE];
	Config Config;
	Audio Audio;
	Video Video;
	Input Input;
	Clock Clock;
	Resource Resource;
	Actor** Actors;
	Sprite** Sprites;
	SDL_Event Events[EVENT_QUEUE_SIZE];
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
Uint64 GetNewObjectID(Engine* Engine); //done
int CompactArray(const void* X, const void* Y); //done
int SortSpritesByZ(const void* X, const void* Y); //done
int PoolCanBeShrunk(void* Pool, int AllocatedElements, int AllocatedSize); //done
int LinearMap(int Number, int NumberMax, int RangeMax, int RangeMin); //done
void SeedRNG(); //done
int GetRandomNumber(int Max); //done
int handler(void* user, const char* section, const char* name, const char* value); //done
int UpdateConfig(char* File, Config* Config); //done
void LoadEngineConfig(Engine* Engine); //done
int InitSDL(Engine* Engine); //done
void CleanupSDL(); //done
void GetSDLEvents(Engine* Engine); //done
int GetBasePath(Engine* Engine); //done
char* GetAssetPath(char* Asset, Engine* Engine); //done
Engine* InitEngine(char* ConfigFile); //done
void RunEngine(Engine* Engine); //done
void CleanupEngine(Engine* Engine); //done

//audio
int InitAudio(Engine* Engine); //done
int* EasyPan(int Pan, int Max, int* Output); //done
int PlaySound(int SoundID, int Voice, int Volume, int Pan, Engine* Engine); //done
void MixMusicVolume(Engine* Engine); //done
int PlayMusic(int MusicID, Engine* Engine); //done

//video
int InitVideo(Engine* Engine); //done
void CleanupVideo(Engine* Engine); //done
int DrawTexture(SDL_Texture* Texture, Vector2 Position, Vector2 Origin, Engine* Engine); //done
int DrawSprite(Sprite* Sprite, Engine* Engine); //done
void Render(Engine* Engine); //done

//input
void GetKeyboardInput(Engine* Engine); //done
void GetMouseInput(Engine* Engine); //done
void GetGamepadInput(Engine* Engine); //done
void GetInput(Engine* Engine); //done 
void RumbleGamepad(int Strength, int Duration, Engine* Engine); //done

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
Sprite* CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, int TextureID, int Visible, CustomSpriteData CustomData, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine); //done
Actor* CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, int Voice, CustomActorData CustomData, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine); //done
void DestroySprite(Sprite* DSprite, Engine* Engine); //done
void DestroyActor(Actor* DActor, Engine* Engine); //done
Sprite* GetSpriteByName(char* Name, Engine* Engine); //done
Actor* GetActorByName(char* Name, Engine* Engine); //done

#endif