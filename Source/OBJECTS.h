#ifndef OBJECTS_H
#define OBJECTS_H

#include "INCLUDE.h"
#include "TYPES.h"
#include "BUTTONS.h"
#include "RESOURCE.h"

#define OBJECT_NAME_SIZE 128

typedef struct SpriteRenderParameters {
	SDL_Texture* Texture; //this can be a raw SDL_Texture* because SDL handles it and they don't move.
	Vector3 Position;
	Vector4 Origin;
	Vector2 Dimensions;
	uint8 Transparency;
	double Angle;
	uint8 Flip;
	Vector3 Tint;
	bool Visible;
} SpriteRenderParameters;

typedef struct Actor {
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
	char Name[OBJECT_NAME_SIZE];
	FVector2 Position;
	Vector2 Dimensions;
	int32 Voice;
	void (*Routine)(struct Actor*, Engine*);
	void* CustomData;
} Actor;

typedef struct Sprite {
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
	char Name[OBJECT_NAME_SIZE];
	uint32 TextureReference; //this cannot be a Texture* because I handle it and I'm clumsy
	uint32 TextureID;
	SpriteRenderParameters RenderParameters;
	void (*Routine)(struct Sprite*, Engine*);
	void* CustomData;
} Sprite;

typedef struct Timer {
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
	char Name[OBJECT_NAME_SIZE];
	uint8 TimerType;
	uint8 UnitType;
	bool Expired;
	uint64 Counter;
	uint64 StopAt;
} Timer;

typedef struct Gamepad {
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
	char Name[OBJECT_NAME_SIZE];
	SDL_GameController* Gamepad;
	uint8 PreviousState[NUM_GAMEPAD_BUTTONS];
	double PreviousTriggersState[NUM_GAMEPAD_TRIGGERS];
	bool ButtonsUp[NUM_GAMEPAD_BUTTONS];
	bool ButtonsDown[NUM_GAMEPAD_BUTTONS];
	double Triggers[NUM_GAMEPAD_TRIGGERS];
	bool TriggersUp[NUM_GAMEPAD_TRIGGERS];
	double Sticks[NUM_GAMEPAD_AXES];
} Gamepad;

typedef struct Texture {
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
	char Name[OBJECT_NAME_SIZE];
	SDL_Texture* Texture;
	char File[STRING_BUFFER_SIZE];
} Texture;

typedef struct Sound {
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
	char Name [OBJECT_NAME_SIZE];
	Mix_Chunk* Sound;
	char File[STRING_BUFFER_SIZE];
} Sound;

typedef struct Music {
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
	char Name [OBJECT_NAME_SIZE];
	Mix_Music* Music;
	char File[STRING_BUFFER_SIZE];
} Music;

typedef struct Object { //use for handling void* structs generically
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
	char Name[OBJECT_NAME_SIZE];
} Object;

#endif