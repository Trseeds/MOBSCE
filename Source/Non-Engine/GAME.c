#include "MOBSCE.h"
#include "GAME.h"

/*
Edit these functions to free the appropriate resources inside the custom data functions
and then free themselves, called by the engine. Do NOT Free the objects themselves, this
will result in a crash.
*/
void SpriteFreeFunction(void* SpritePtr)
{
    Sprite* FSprite = (Sprite*)SpritePtr;
    if(FSprite->CustomData)
    {
        free(FSprite->CustomData);
    }
}

void ActorFreeFunction(void* ActorPtr)
{
    Actor* FActor = (Actor*)ActorPtr;
    if(FActor->CustomData)
    {
        free(FActor->CustomData);
    }
}



void AlignSpriteToActor(Sprite* Sprite, Engine* Engine)
{
    SpriteRenderParameters* SRP = &Sprite->RenderParameters;
    CSD* CD = Sprite->CustomData;
    Actor* Actor = Engine->ActorReferences[CD->ActorReference];
    if(Actor)
    {
        if(Actor->ID == CD->ActorID)
        {
            SRP->Position.X = Actor->Position.X;
            SRP->Position.Y = Actor->Position.Y;
        }
    }
}

void ActorScreenWrap(uint32 ActorRef, Engine* Engine)
{
    Actor* Actor = Engine->ActorReferences[ActorRef];
    FVector2* AP = &Actor->Position;
    Vector2* LD = &Engine->Video.LogicalDimensions;
    if(AP->X < 0)
    {
        AP->X = LD->X;
    }
    if(AP->X > LD->X)
    {
        AP->X = 0;
    }
    if(AP->Y < 0)
    {
        AP->Y = LD->Y;
    }
    if(AP->Y > LD->Y)
    {
        AP->Y = 0;
    }
}

void ApplyPhysics(uint32 ActorRef, Engine* Engine)
{
    Actor* A = Engine->ActorReferences[ActorRef];
    CADGeneric* CD = A->CustomData;
    CD->Physics.Velocity.X *= CD->Physics.Drag;
    CD->Physics.Velocity.Y *= CD->Physics.Drag;
    A->Position.X += CD->Physics.Velocity.X;
    A->Position.Y += CD->Physics.Velocity.Y;
}

void PlayerRoutine(Actor* Player, Engine* Engine)
{
    CADPlayer* CD = Player->CustomData;
    Input* I = &Engine->Input;
    bool* KD = I->KeysDown;
    int PN = CD->PlayerNumber;
    
    if(KD[PlayerKeybinds[PN][PB_UP]])
    {
        CD->Physics.Velocity.Y -= CD->Physics.MoveSpeed;
    }
    if(KD[PlayerKeybinds[PN][PB_DOWN]])
    {
        CD->Physics.Velocity.Y += CD->Physics.MoveSpeed;
    }
    if(KD[PlayerKeybinds[PN][PB_LEFT]])
    {
        CD->Physics.Velocity.X -= CD->Physics.MoveSpeed;
    }
    if(KD[PlayerKeybinds[PN][PB_RIGHT]])
    {
        CD->Physics.Velocity.X += CD->Physics.MoveSpeed;
    }

    ActorScreenWrap(Player->ReferenceIndex,Engine);
    ApplyPhysics(Player->ReferenceIndex,Engine);
}

void MonsterRoutine(Actor* Monster, Engine* Engine)
{
    ActorScreenWrap(Monster->ReferenceIndex,Engine);
    ApplyPhysics(Monster->ReferenceIndex,Engine);

    if(!(Engine->Clock.TotalTicks % 5))
    {
        CADMonster* CD = Monster->CustomData;
        Actor* Target = NULL;
        if(CD->TargetReferenceIndex != SENTINEL_OBJECT)
        {
            Target = Engine->ActorReferences[CD->TargetReferenceIndex];
        }

        if(Target)
        {
            if(Target->ID == CD->TargetID)
            {
                *(&CD->TargetPosition) = (FVector2){Target->Position.X,Target->Position.Y};
            }
        }
        else
        {
            if(!(Engine->Clock.TotalTicks % 60))
            {
                *(&CD->TargetPosition) = (FVector2){GetRandomNumber(0,Engine->Video.LogicalDimensions.X),GetRandomNumber(0,Engine->Video.LogicalDimensions.Y)};
                uint32 AR = GetActorByName("Player",Engine);
                if(AR != SENTINEL_OBJECT)
                {
                    CD->TargetReferenceIndex = AR;
                    CD->TargetID = Engine->ActorReferences[AR]->ID;
                }
            }
        }

        if(Monster->Position.X < CD->TargetPosition.X)
        {
            CD->Physics.Velocity.X += CD->Physics.MoveSpeed;
        }
        if(Monster->Position.X > CD->TargetPosition.X)
        {
            CD->Physics.Velocity.X -= CD->Physics.MoveSpeed;
        }
        if(Monster->Position.Y < CD->TargetPosition.Y)
        {
            CD->Physics.Velocity.Y += CD->Physics.MoveSpeed;
        }
        if(Monster->Position.Y > CD->TargetPosition.Y)
        {
            CD->Physics.Velocity.Y -= CD->Physics.MoveSpeed;
        }
    }
}



void CacheTexturesGame(Engine* Engine)
{
    char Buffer[STRING_BUFFER_SIZE];
    CacheTexture(GetAssetPath("Assets/Images/Backgrounds/TestBG.png",Buffer,Engine),Engine);
    CacheTexture(GetAssetPath("Assets/Images/Backgrounds/EvilBG.png",Buffer,Engine),Engine);
    CacheTexture(GetAssetPath("Assets/Images/Sprites/Player.png",Buffer,Engine),Engine);
    CacheTexture(GetAssetPath("Assets/Images/Sprites/Monster.png",Buffer,Engine),Engine);
    CacheTexture(GetAssetPath("Assets/Images/Sprites/Font.png",Buffer,Engine),Engine);
}

void CacheSoundsAndMusicGame(Engine* Engine)
{
    char Buffer[STRING_BUFFER_SIZE];
    CacheSound(GetAssetPath("Assets/Sounds/Cough.wav",Buffer,Engine),Engine);
    CacheMusic(GetAssetPath("Assets/Sounds/Music/Waiting for Romero to Play.mp3",Buffer,Engine),Engine);
}

void CreateBGGame(Engine* Engine)
{
    Vector3 SP = {0,0,0};
    Vector4 SO = {0,0,640,480};
    Vector2 SD = {640,480};
    void* CSD = NULL;
    int ST = TXTR_EBG;
    CreateSprite("BackGround",SP,SO,SD,ST,CSD,NULL,NULL,Engine);
}

void CreatePlayerGame(uint8 PlayerNumber, Engine* Engine)
{
    Vector2 AP = {GetRandomNumber(0,Engine->Video.LogicalDimensions.X),GetRandomNumber(0,Engine->Video.LogicalDimensions.Y)};
    Vector2 AD = {16,16};
    int AV = 0;
    CADPlayer* CAD = calloc(1,sizeof(struct CADPlayer));
    *CAD = (CADPlayer)
    {
        (CADHeader){CAD_PLAYER,ACT_PLAYER},
        (CADPhysics){(FVector2){0.00,0.00},0.50,0.99,0.75},
        PlayerNumber
    };
    uint32 AR = CreateActor("Player",AP,AD,AV,CAD,&PlayerRoutine,Engine);
    Actor* A;
    if(AR != SENTINEL_OBJECT)
    {
        A = Engine->ActorReferences[AR];
    }
    else
    {
        return;
    }

    Vector3 SP = {AP.X,AP.Y,1000};
    Vector4 SO = {0,0,16,16};
    Vector2 SD = {16,16};
    CSD* CSDD = calloc(1,sizeof(struct CSD));
    *CSDD = (CSD){A->ReferenceIndex,A->ID};
    int ST = TXTR_PLAYER;
    CreateSprite("Player",SP,SO,SD,ST,CSDD,A,AlignSpriteToActor,Engine);
}

void CreateMonsterGame(Engine* Engine)
{
    Vector2 AP = {GetRandomNumber(0,Engine->Video.LogicalDimensions.X),GetRandomNumber(0,Engine->Video.LogicalDimensions.Y)};
    Vector2 AD = {32,32};
    int AV = 0;
    CADMonster* CAD = calloc(1,sizeof(struct CADMonster));
    *CAD = (CADMonster)
    {
        (CADHeader){CAD_MONSTER,ACT_MONSTER},
        (CADPhysics){(FVector2){0.00,0.00},1.50,0.99,0.75},
        SENTINEL_OBJECT,
        0,
        (FVector2){0.00,0.00}
    };
    uint32 AR = CreateActor("Monster",AP,AD,AV,CAD,&MonsterRoutine,Engine);
    Actor* A;
    if(AR != SENTINEL_OBJECT)
    {
        A = Engine->ActorReferences[AR];
    }
    else
    {
        return;
    }

    Vector3 SP = {AP.X,AP.Y,1};
    Vector4 SO = {0,0,32,32};
    Vector2 SD = {32,32};
    CSD* CSDD = calloc(1,sizeof(struct CSD));
    *CSDD = (CSD){A->ReferenceIndex,A->ID};
    int ST = TXTR_MONSTER;
    uint32 SR = CreateSprite("Monster",SP,SO,SD,ST,CSDD,A,AlignSpriteToActor,Engine);
    if(SR != SENTINEL_OBJECT)
    {
        Sprite* S = Engine->SpriteReferences[SR];
        S->RenderParameters.Tint = (Vector3){GetRandomNumber(0,255),GetRandomNumber(0,255),GetRandomNumber(0,255)};
    }
}



void InitGame(Engine* Engine)
{
    CacheTexturesGame(Engine);
    CacheSoundsAndMusicGame(Engine);
    CreateBGGame(Engine);
    CreatePlayerGame(1,Engine);
    PlayMusic(MUS_WFRTP,Engine);
}



int main(int argc, char* argv[])
{
    Engine* Engine = InitEngine("Config.ini","MOBSCE Demo","Assets/Images/Icon.png",ERROR_SHOW_ALL,WARNING_SHOW_ALL,1000,true,true,NULL);
    InitGame(Engine);

    while(Engine->Running)
    {
        for(int i = 0; i < EVENT_QUEUE_SIZE; i++)
        {
            if(Engine->Events[i].type == SDL_QUIT)
            {
                CleanupEngine(Engine);
                OSMemoryFree(Engine,sizeof(struct Engine));
                return(0);
            }
        }

        if(Engine->Input.KeysUp[GM_QUIT])
        {
            CleanupEngine(Engine);
            OSMemoryFree(Engine,sizeof(struct Engine));
            return(0);
        }
        if(Engine->Input.KeysUp[GM_RESTART])
        {
            CleanupEngine(Engine);
            OSMemoryFree(Engine,sizeof(struct Engine));
            Engine = InitEngine("Config.ini","MOBSCE Demo","Assets/Images/Icon.png",ERROR_SHOW_ALL,WARNING_SHOW_ALL,100000,true,true,NULL);
            InitGame(Engine);
        }
        if(Engine->Input.KeysUp[K_S])
        {
            for(int i = 0; i < 1000; i++)
            {
                CreateMonsterGame(Engine);
            }
        }
        if(Engine->Clock.TotalTicks && (int)(Engine->Clock.FrameRate))
        {
            if(!(Engine->Clock.TotalTicks % (int)(Engine->Clock.FrameRate)))
            {
                //printf("\r%d",(int)(Engine->Clock.FrameRate));
                //printf("\r%d",Engine->Resource.NumberOfActors+Engine->Resource.NumberOfSprites);
            }
        }

        RunEngine(Engine);
    }
}