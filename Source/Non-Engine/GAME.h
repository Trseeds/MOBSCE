#define NOP __asm__("nop")

/*
Keep these delcarations, it's hacky, I know, and I'm sorry.
You can change them as you wish, just dont remove them.
*/
/***************************************************************************************/
typedef struct CustomSpriteData {
    unsigned char Byte;
} CustomSpriteData;

typedef struct CustomActorData {
    FVector2 Velocity;
    Uint64 TargetID;
    Uint32 TargetReferenceIndex;
    float Drag;
} CustomActorData;
/***************************************************************************************/

enum Textures {
    TXTR_BG,
    TXTR_PLAYER,
    TXTR_MONSTER,
    TXTR_NUMBERS
};

enum Sounds {
    SND_COUGH
};

enum Music {
    MUS_WFRTP
};