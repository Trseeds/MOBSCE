#ifndef RESOURCE_H
#define RESOURCE_H

#include "TYPES.h"
#include "OBJECTS.h"

#define MIN_ALLOCATE Engine->Resource.AllocationThreshold
#define STRING_BUFFER_SIZE 5120

typedef struct StructMember {
	void* Pointer; //&struct.member - &struct
	void* Value; //what are we looking for?
	size_t Size; //how big is it?
} StructMember;

typedef struct ResourceInfo {
	void* Pointer;
	uint16 SizeOfResource;
	void (*FreeFunction)(void*);
	uint32* AllocatedResourceMemory;
	uint32* NumberOfResources;
	bool IsPointerArray;
} ResourceInfo;

typedef struct Resource {
	uint32 AllocationThreshold;
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
	uint32 NumberOfTimers;
	uint32 NumberOfTimerReferences;
	uint32 NumberOfGamepads;
	uint32 NumberOfGamepadReferences;
	uint32 AllocatedTextureMemory;
	uint32 AllocatedSoundMemory;
	uint32 AllocatedMusicMemory;
	uint32 AllocatedSpriteMemory;
	uint32 AllocatedSpriteReferenceMemory;
	uint32 AllocatedActorMemory;
	uint32 AllocatedActorReferenceMemory;
	uint32 AllocatedTimerMemory;
	uint32 AllocatedTimerReferenceMemory;
	uint32 AllocatedGamepadMemory;
	uint32 AllocatedGamepadReferenceMemory;
} Resource;

void* OSMemoryAllocate(size_t Size);
void OSMemoryFree(void* Pointer, size_t Size);
int QSCompactPointerPool(const void* X, const void* Y);
int QSCompactObjectPool(const void* X, const void* Y);
int QSSortSpritesByZ(const void* X, const void* Y);
bool PoolCanBeShrunk(void* Array, uint32 AllocatedElements, uint32 AllocatedSize, Engine* Engine);
void* FindOpenResourceSpace(void* Pool, uint32 PoolSize, size_t Size);
uint32 FindOpenReferenceSpace(void* Pool, uint32 AllocatedReferenceMemory);
void RebuildReferences(void* ReferencePool, void* Pool, size_t SizeOfObject, uint32 AllocatedResources);
int8 InitResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
int8 ExtendResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
int8 ShrinkResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
int8 CleanupResourcePool(ResourceInfo ResourceInfo, Engine* Engine);
void SpriteFreeFunction(void* SpritePtr);
void ActorFreeFunction(void* ActorPtr);

int8 CacheSound(char* File, Engine* Engine);
int8 CacheMusic(char* File, Engine* Engine);

int8 CacheTexture(char* File, Engine* Engine);

uint32 CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, uint32 TextureID, void* CustomData, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine);
uint32 CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, uint32 Voice, void* CustomData, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine);
int8 DestroySprite(Sprite* DSprite, void (*FreeFunction)(void*), Engine* Engine);
int8 DestroyActor(Actor* DActor, void (*FreeFunction)(void*), Engine* Engine);
uint32 GetSpriteByName(char* Name, Engine* Engine);
uint32 GetSpriteByID(uint64 ID, Engine* Engine);
uint32 GetSpriteByProperty(void* Property, uint32 Offset, uint32 Size, Engine* Engine); //update to use StructMember
uint32 GetActorByName(char* Name, Engine* Engine);
uint32 GetActorByID(Uint64 ID, Engine* Engine);
uint32 GetActorByProperty(void* Property, uint32 Offset, uint32 Size, Engine* Engine);
uint32 CreateTimer(uint8 TimerType, uint8 UnitType, uint64 StopAt, Engine* Engine);
int8 DestroyTimer(Timer* DTimer, Engine* Engine);
uint32 CreateGamepad(SDL_GameController* Controller, Engine* Engine);
int8 DestroyGamepad(Gamepad* DGamepad, Engine* Engine);

#endif