#ifndef INPUT_H
#define INPUT_H

#include "INCLUDE.h"
#include "TYPES.h"
#include "BUTTONS.h"
#include "OBJECTS.h"

typedef struct Input {
	uint8* SDL_Keystate;
	uint8 SDL_PreviousKeystate[NUM_KEYS];
	uint32 SDL_MouseState;
	uint32 SDL_PreviousMouseState;
	bool KeysDown[NUM_KEYS];
	bool KeysUp[NUM_KEYS];
	Vector2 MousePosition;
	bool MouseDown[NUM_MOUSE_KEYS];
	bool MouseUp[NUM_MOUSE_KEYS];
	int8 VerticalMouseScroll;
	int8 HorizontalMouseScroll;
	Gamepad* Gamepads;
	Gamepad** GamepadReferences;
} Input;

int8 GetKeyboardInput(Engine* Engine);
int8 GetMouseInput(Engine* Engine);
int8 GetGamepadInput(Engine* Engine);
int8 GetInput(Engine* Engine); 
int8 RumbleGamepad(Gamepad* Gamepad, uint8 Strength, uint32 Duration, Engine* Engine);

#endif