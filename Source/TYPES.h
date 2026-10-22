#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef unsigned char* bytepointer; //use for raw byte logic
typedef unsigned char ubyte;
typedef signed char sbyte;
typedef uint8_t uint8;
typedef int8_t int8;
typedef uint16_t uint16;
typedef int16_t int16;
typedef uint32_t uint32;
typedef int32_t int32;
typedef uint64_t uint64;
typedef int64_t int64;
#define UNSIGNED_TYPE_MAX_VAL (0-1)

typedef struct Vector2 {
	int32 X;
	int32 Y;
} Vector2;

typedef struct Vector3 {
	int32 X;
	int32 Y;
	int32 Z;
} Vector3;

typedef struct Vector4 {
	int32 X;
	int32 Y;
	int32 Z;
	int32 W;
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

#endif