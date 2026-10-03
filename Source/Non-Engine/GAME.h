#define NOP __asm__("nop")


//these are no longer required!!!
//you can now define these structs with any name or not at all.
//The consequence of this change is that you must manually cast from void* when accessing customdata now.
typedef struct CSD {
    ubyte Byte;
} CSD;

typedef struct CAD {
    FVector2 Velocity;
    Uint64 TargetID;
    Uint32 TargetReferenceIndex;
} CAD;

enum Textures {
    TXTR_BG,
    TXTR_PLAYER,
    TXTR_MONSTER,
    TXTR_FONT
};

enum Sounds {
    SND_COUGH
};

enum Music {
    MUS_WFRTP
};
