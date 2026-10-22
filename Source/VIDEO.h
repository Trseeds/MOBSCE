#ifndef VIDEO_H
#define VIDEO_H

#include "INCLUDE.h"
#include "TYPES.h"
#include "OBJECTS.h"
#include "RESOURCE.h"

#define TINT_NOCHANGE 255

enum SpriteFlip {
	FLIP_H = SDL_FLIP_HORIZONTAL,
	FLIP_V = SDL_FLIP_VERTICAL,
	FLIP_NONE = SDL_FLIP_NONE
};

typedef struct Video {
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	Vector2 LogicalDimensions;
	Vector4 WindowBounds;
	uint16 WindowFlags;
	uint16 RendererFlags;
	uint32 Formats;
	float WindowScale;
	char WindowTitle[STRING_BUFFER_SIZE];
	char WindowIconPath[STRING_BUFFER_SIZE];
} Video;

int8 InitVideo(Engine* Engine);
int8 RestartVideo(Engine* Engine);
int8 CleanupVideo(Engine* Engine);
int8 DrawSprite(Sprite* Sprite, Engine* Engine);
int8 Render(Engine* Engine);

#endif