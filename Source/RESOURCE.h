#ifndef RESOURCE_H
#define RESOURCE_H

#include "TYPES.h"

typedef struct Object Object;
typedef struct Sound Sound;
typedef struct Music Music;
typedef struct Actor Actor;
typedef struct Sprite Sprite;
typedef struct Timer Timer;
typedef struct Gamepad Gamepad;
typedef struct Sound Sound;
typedef struct Music Music;
typedef struct Texture Texture;

#define MIN_ALLOCATE Engine->Resource.AllocationThreshold
#define STRING_BUFFER_SIZE 5120

typedef struct ResourceInfo {
	void* Pointer;
	size_t SizeOfResource;
	void (*FreeFunction)(void*);
	uint32* AllocatedResourceMemory;
	uint32* NumberOfResources;
	bool IsPointerArray;
} ResourceInfo;

typedef struct Resource {
	uint32 AllocationThreshold;

	uint32 NumberOfTextures;
	uint32 AllocatedTextureMemory;
	uint32 NumberOfTextureReferences;
	uint32 AllocatedTextureReferenceMemory;
	
	uint32 NumberOfSounds;
	uint32 AllocatedSoundMemory;
	uint32 NumberOfSoundReferences;
	uint32 AllocatedSoundReferenceMemory;

	uint32 NumberOfMusics;
	uint32 AllocatedMusicMemory;
	uint32 NumberOfMusicReferences;
	uint32 AllocatedMusicReferenceMemory;
	
	uint32 NumberOfSprites;
	uint32 AllocatedSpriteMemory;
	uint32 NumberOfSpriteReferences;
	uint32 AllocatedSpriteReferenceMemory;

	uint32 NumberOfActors;
	uint32 AllocatedActorMemory;
	uint32 NumberOfActorReferences;
	uint32 AllocatedActorReferenceMemory;

	uint32 NumberOfTimers;
	uint32 AllocatedTimerMemory;
	uint32 NumberOfTimerReferences;
	uint32 AllocatedTimerReferenceMemory;
	
	uint32 NumberOfGamepads;
	uint32 AllocatedGamepadMemory;
	uint32 NumberOfGamepadReferences;
	uint32 AllocatedGamepadReferenceMemory;
} Resource;

enum ResourceTypes {
	RSRC_ACT = 		0b00000001,
	RSRC_ACT_REF = 	0b10000001,
	RSRC_SPR = 		0b00000010,
	RSRC_SPR_REF = 	0b10000010,
	RSRC_TXTR = 	0b00000011,
	RSRC_TXTR_REF =	0b10000011,
	RSRC_SND = 		0b00000100,
	RSRC_SND_REF =	0b10000100,
	RSRC_MUS = 		0b00000101,
	RSRC_MUS_REF =	0b10000101,
	RSRC_TMR = 		0b00000110,
	RSRC_TMR_REF = 	0b10000110,
	RSRC_GP = 		0b00000111,
	RSRC_GP_REF =	0b10000111
};

//misc
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

//populate
ResourceInfo* PopulateResourceInfo(uint8 ResourceType, ResourceInfo* ResourceInfo, Engine* Engine);

//create
void* CreateObject(ResourceInfo Objects, ResourceInfo References, Engine* Engine);
uint32 CreateSound(char* File, char* Name, Engine* Engine);
uint32 CreateMusic(char* File, char* Name, Engine* Engine);
uint32 CreateTexture(char* File, char* Name, Engine* Engine);
uint32 CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, uint32 TextureReference, void* CustomData, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine);
uint32 CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, uint32 Voice, void* CustomData, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine);
uint32 CreateTimer(char* Name, uint8 TimerType, uint8 UnitType, uint64 StopAt, Engine* Engine);
uint32 CreateGamepad(SDL_GameController* Controller, Engine* Engine);

//free
void SpriteFreeFunction(void* SpritePtr);
void ActorFreeFunction(void* ActorPtr);
void TextureFreeFunction(void* Ptr);
void SoundFreeFunction(void* Ptr);
void MusicFreeFunction(void* Ptr);
void GamepadFreeFunction(void* Ptr);

//destroy
void DestroyObject(ResourceInfo Objects, ResourceInfo References, Object* Object, Engine* Engine);
void DestroySound(Sound* DSound, Engine* Engine);
void DestroyMusic(Music* DMusic, Engine* Engine);
void DestroyTexture(Texture* DTexture, Engine* Engine);
void DestroySprite(Sprite* DSprite, Engine* Engine);
void DestroyActor(Actor* DActor, Engine* Engine);
void DestroyTimer(Timer* DTimer, Engine* Engine);
void DestroyGamepad(Gamepad* DGamepad, Engine* Engine);

//find
uint32 GetObjectByProperty(void* MemberValue, size_t MemberOffset, size_t MemberSize, ResourceInfo Objects);
uint32 GetSoundByName(char* Name, Engine* Engine);
uint32 GetSoundByID(uint64 ID, Engine* Engine);
uint32 GetSoundByFile(char* File, Engine* Engine);
uint32 GetMusicByName(char* Name, Engine* Engine);
uint32 GetMusicByID(uint64 ID, Engine* Engine);
uint32 GetMusicByFile(char* File, Engine* Engine);
uint32 GetTextureByName(char* Name, Engine* Engine);
uint32 GetTextureByID(uint64 ID, Engine* Engine);
uint32 GetTextureByFile(char* File, Engine* Engine);
uint32 GetSpriteByName(char* Name, Engine* Engine);
uint32 GetSpriteByID(uint64 ID, Engine* Engine);
uint32 GetSpriteByProperty(void* MemberValue, size_t MemberOffset, size_t MemberSize, Engine* Engine);
uint32 GetActorByName(char* Name, Engine* Engine);
uint32 GetActorByID(Uint64 ID, Engine* Engine);
uint32 GetActorByProperty(void* MemberValue, size_t MemberOffset, size_t MemberSize, Engine* Engine);
uint32 GetTimerByName(char* Name, Engine* Engine);
uint32 GetTimerByID(uint64 ID, Engine* Engine);
uint32 GetTimerByProperty(void* MemberValue, size_t MemberOffset, size_t MemberSize, Engine* Engine);

#endif