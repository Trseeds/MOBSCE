#define NOP __asm__("nop")

typedef struct Scene {
    uint8 SceneNumber;
    uint32 Textures[10];
    uint32 Sounds[10];
    uint32 Musics[10];
} Scene;

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

typedef struct CADGOD{
    CADHeader Header;
    Scene* CurrentScene;
    bool ResetScene;
} CADGOD;

typedef struct CADMonster {
    CADHeader Header;
    CADPhysics Physics;
    uint32 GodReference;
    uint64 GodID;
    uint32 TargetReferenceIndex;
    uint64 TargetID;
    FVector2 TargetPosition;
} CADMonster;

typedef struct CADPlayer {
    CADHeader Header;
    CADPhysics Physics;
    uint32 GodReference;
    uint64 GodID;
    uint8 PlayerNumber;
} CADPlayer;

typedef struct CADGeneric {
    CADHeader Header;
    CADPhysics Physics;
    uint32 GodReference;
    uint64 GodID;
} CADGeneric;

enum CADTypes {
    CAD_GENERIC,
    CAD_GOD,
    CAD_PLAYER,
    CAD_MONSTER
};

enum ActorTypes {
    ACT_GOD,
    ACT_BG,
    ACT_PLAYER,
    ACT_MONSTER
};

typedef struct CSD {
    uint32 ActorReference;
    uint64 ActorID;
} CSD;

enum Textures {
    TXTR_BG,
    TXTR_PLAYER,
    TXTR_MONSTER
};

enum Sounds {
    SND_MONSTER_DIE
};

enum Musics {
    MUS_TRACK01
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

Scene* CreateScene(Scene* Scene, Engine* Engine);
void ResetScene(Engine* Engine);

void CreateMonsterGame(uint32 GodRef, Engine* Engine);
void KillMonsterGame(uint32 GodRef, Engine* Engine);