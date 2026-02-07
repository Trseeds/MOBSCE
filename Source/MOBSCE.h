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
#include "GAME.h"

#define STRING_BUFFER_SIZE 1024
#define OBJECT_NAME_SIZE 64
#define MIN_ALLOCATE 16
#define EVENT_QUEUE_SIZE 512
#define INVALID_ENGINE -1
#define TINT_NOCHANGE 255

enum Flip {
	FLIP_H = SDL_FLIP_HORIZONTAL,
	FLIP_V = SDL_FLIP_VERTICAL,
	FLIP_NONE = SDL_FLIP_NONE
};

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
	int TextureID;
	SpriteRenderParameters RenderParameters;
	Actor* Actor;
	void (*Routine)(struct Sprite*, struct Engine*);
	CustomSpriteData CustomData;
} Sprite;

typedef struct Wiregon {
	int ID;
	Vector2* Verticies;
	Vector3 Position;
	Vector3 Color;
	int Alpha;
	int NumberOfVerticies;
} Wiregon;

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
	int NumberOfWiregons;
	int AllocatedTextureMemory;
	int AllocatedSoundMemory;
	int AllocatedMusicMemory;
	int AllocatedSpriteMemory;
	int AllocatedActorMemory;
	int AllocatedWiregonMemory;
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
	Wiregon** Wiregons;
	SDL_Event Events[EVENT_QUEUE_SIZE];
	Uint64 IDCounter;
	int Running;
	int SpriteZResortNeeded;
	int WiregonZResortNeeded;
} Engine;

typedef struct ResourceInfo {
	void* Pointer;
	void (*FreeFunction)(void*);
	int* AllocatedResourceMemory;
	int* NumberOfResources;
} ResourceInfo;

//engine stuff
void ThrowError(char* Message, char* Thrower, Engine* Engine);
void ThrowWarning(char* Message, char* Thrower);
Uint64 GetNewObjectID(Engine* Engine);
int CompactArray(const void* X, const void* Y);
int SortSpritesByZ(const void* X, const void* Y);
int SortWiregonsByZ(const void* X, const void* Y);
int PoolCanBeShrunk(void* Pool, int AllocatedElements, int AllocatedSize);
int LinearMap(int Number, int NumberMax, int RangeMax, int RangeMin);
void SeedRNG();
int GetRandomNumber(int Min, int Max);
int handler(void* user, const char* section, const char* name, const char* value);
int UpdateConfig(char* File, Config* Config);
void LoadEngineConfig(Engine* Engine);
int InitSDL(Engine* Engine);
void CleanupSDL();
void GetSDLEvents(Engine* Engine);
int GetBasePath(Engine* Engine);
char* GetAssetPath(char* Asset, Engine* Engine);
Engine* InitEngine(char* ConfigFile, char* WindowTitle, char* WindowIconPath);
void RunEngine(Engine* Engine);
void CleanupEngine(Engine* Engine);

//audio
int InitAudio(Engine* Engine);
int* EasyPan(int Pan, int Max, int* Output);
int PlaySound(int SoundID, int Voice, int Volume, int Pan, Engine* Engine);
void MixMusicVolume(Engine* Engine);
int PlayMusic(int MusicID, Engine* Engine);

//video
int InitVideo(Engine* Engine);
void RestartVideo(Engine* Engine);
void CleanupVideo(Engine* Engine);
int DrawTexture(SDL_Texture* Texture, Vector2 Position, Vector2 Origin, Engine* Engine);
int DrawSprite(Sprite* Sprite, Engine* Engine);
int DrawWiregon(Wiregon* Wiregon, Engine* Engine);
void Render(Engine* Engine);

//input
void GetKeyboardInput(Engine* Engine);
void GetMouseInput(Engine* Engine);
void GetGamepadInput(Engine* Engine);
void GetInput(Engine* Engine); 
void RumbleGamepad(int Strength, int Duration, Engine* Engine);

//clock
void KeepTime(Engine* Engine);

//resource
int InitResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
int ExtendResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
int ShrinkResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
void CleanupResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
//audio
int CacheSound(char* File, Engine* Engine);
int CacheMusic(char* File, Engine* Engine);
//video
int CacheTexture(char* File, Engine* Engine);
//objects
Sprite* CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, int TextureID, CustomSpriteData CustomData, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine);
Actor* CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, int Voice, CustomActorData CustomData, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine);
Wiregon* CreateWiregon(Vector2* Verticies, Vector3 Position, int NumberOfVerticies, Vector3 Color, int Alpha, Engine* Engine);
void DestroySprite(Sprite* DSprite, Engine* Engine);
void DestroyActor(Actor* DActor, Engine* Engine);
void DestroyWiregon(Wiregon* DWiregon, Engine* Engine);
Sprite* GetSpriteByName(char* Name, Engine* Engine);
Actor* GetActorByName(char* Name, Engine* Engine);

#endif