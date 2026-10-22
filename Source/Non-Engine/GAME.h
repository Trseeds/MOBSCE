#define NOP __asm__("nop")

typedef struct CADHeader {
    uint8 CADType;
    uint8 ActorType;
    uint32 SpriteReference;
    uint64 SpriteID;
} CADHeader;

typedef struct CADPhysics {
    FVector2 Velocity;
    float MoveSpeed;
    float Drag;
    float Friction;
} CADPhysics;

typedef struct CADMonster {
    CADHeader Header;
    CADPhysics Physics;
    uint32 TargetReferenceIndex;
    uint64 TargetID;
    FVector2 TargetPosition;
} CADMonster;

typedef struct CADPlayer {
    CADHeader Header;
    CADPhysics Physics;
    uint8 PlayerNumber;
} CADPlayer;

typedef struct CADGeneric {
    CADHeader Header;
    CADPhysics Physics;
} CADGeneric;

enum CADTypes {
    CAD_PLAYER,
    CAD_MONSTER
};

enum ActorTypes {
    ACT_PLAYER,
    ACT_MONSTER
};

typedef struct CSD {
    uint32 ActorReference;
    uint64 ActorID;
} CSD;

enum Textures {
    TXTR_TBG,
    TXTR_EBG,
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

static int PlayerKeybinds[2][4] = 
{
{K_W,K_S,K_A,K_D},
{K_UP,K_DOWN,K_LEFT,K_RIGHT}
};

enum GameButtons {
    GM_QUIT = K_ESCAPE,
    GM_RESTART = K_R
};

enum PlayerButtons {
    PB_UP = 0,
    PB_DOWN = 1,
    PB_LEFT = 2,
    PB_RIGHT = 3
};