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
    Actor* Target;
    Actor* Camera;
    Vector4* World;
    Wiregon* Wiregon;
    float JumpForce;
    float Drag;
    float Gravity;
} CustomActorData;
/***************************************************************************************/

enum Textures {
    TXTR_BG,
    TXTR_PLAYER,
    TXTR_MONSTER
};

enum Sounds {
    SND_COUGH
};

void ActorWorldCollide(Actor* Actor, Engine* Engine);