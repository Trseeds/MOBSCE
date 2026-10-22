#ifndef OBJECTS_H
#define OBJECTS_H

#include "INCLUDE.h"
#include "TYPES.h"
#include "BUTTONS.h"

#define OBJECT_NAME_SIZE 128

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
	uint32 TextureID;
	SpriteRenderParameters RenderParameters;
	// uint32 ActorReferenceIndex;
	// uint64 ExpectedActorID;
	void (*Routine)(struct Sprite*, Engine*);
	void* CustomData;
} Sprite;

typedef struct Timer {
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
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
	SDL_GameController* Gamepad;
	uint8 PreviousState[NUM_GAMEPAD_BUTTONS];
	double PreviousTriggersState[NUM_GAMEPAD_TRIGGERS];
	bool ButtonsUp[NUM_GAMEPAD_BUTTONS];
	bool ButtonsDown[NUM_GAMEPAD_BUTTONS];
	double Triggers[NUM_GAMEPAD_TRIGGERS];
	bool TriggersUp[NUM_GAMEPAD_TRIGGERS];
	double Sticks[NUM_GAMEPAD_AXES];
} Gamepad;

typedef struct Object { //use for handling void* structs generically
	bool IsUsed;
	uint64 ID;
	uint32 ReferenceIndex;
} Object;

#endif