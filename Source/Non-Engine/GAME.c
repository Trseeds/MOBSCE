#include "MOBSCE.h"
#include "GAME.h"

//TESTING!!!
//============================================================================================================================================================================================

void PrintObject(Object* Object, char* Buffer)
{
    snprintf(Buffer,STRING_BUFFER_SIZE,"\n\tIsUsed: %d\n\tID: %lld\n\tReferenceIndex: %d\n\tName: \"%s\"",Object->IsUsed,Object->ID,Object->ReferenceIndex,Object->Name);   
}

void PrintActor(Actor* Actor, char* Buffer)
{
    snprintf(Buffer,STRING_BUFFER_SIZE,"%s\n\tPosition:\n\t\tX: %f\n\t\tY: %f\n\tDimesions:\n\t\tX: %d\n\t\tY: %d\n\tVoice: %d\n\tRoutine: 0x%p\n\tCustomData: 0x%p",Buffer,Actor->Position.X,Actor->Position.Y,Actor->Dimensions.X,Actor->Dimensions.Y,Actor->Voice,Actor->Routine,Actor->CustomData);
}

void PrintActorPool(Object* Objects, uint32 num)
{
    for(int i = 0; i < num; i++)
    {
        char Buffer[STRING_BUFFER_SIZE];
        PrintObject((Object*)(((bytepointer)(Objects))+i*sizeof(struct Actor)),Buffer);
        PrintActor((Actor*)(((bytepointer)(Objects))+i*sizeof(struct Actor)),Buffer);
        puts(Buffer);
    }
}

//============================================================================================================================================================================================

/*
Edit these functions to free the appropriate user allocated resources. 
Called by the engine. Do NOT Free the objects themselves, this will
result in a crash.
*/
void SpriteFreeFunction(void* SpritePtr)
{
    Sprite* FSprite = (Sprite*)SpritePtr;
    if(FSprite->IsUsed && FSprite->CustomData)
    {
        free(FSprite->CustomData);
    }
}

void ActorFreeFunction(void* ActorPtr)
{
    Actor* FActor = (Actor*)ActorPtr;
    if(FActor->IsUsed && FActor->CustomData)
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

void GodRoutine(Actor* God, Engine* Engine)
{
    CADGOD* CD = (CADGOD*)God->CustomData;
    uint32 GR = God->ReferenceIndex;
    CD->ResetScene = false;

    if(Engine->Input.KeysUp[K_1])
    {
        ResetScene(Engine);
        free(CD->CurrentScene);
        Scene* S = calloc(1,sizeof(struct Scene));
        CD->CurrentScene = S;
        S->SceneNumber = 0;
        CD->ResetScene = true;
        CreateScene(S,Engine);
        PlayMusic(Engine->MusicReferences[S->Musics[MUS_TRACK01]]->Music,-1,Engine);
    }
    if(Engine->Input.KeysUp[K_2])
    {
        ResetScene(Engine);
        free(CD->CurrentScene);
        Scene* S = calloc(1,sizeof(struct Scene));
        CD->CurrentScene = S;
        S->SceneNumber = 1;
        CD->ResetScene = true;
        CreateScene(S,Engine);
        PlayMusic(Engine->MusicReferences[S->Musics[MUS_TRACK01]]->Music,-1,Engine);
    }

    if((Engine->Input.KeysDown[K_RIGHTCONTROL] || Engine->Input.KeysDown[K_LEFTCONTROL]) && Engine->Input.KeysDown[K_S])
    {
        for(int i = 0; i < MIN_ALLOCATE; i++)
        {
            CreateMonsterGame(GR,Engine);
        }
    }
    if(Engine->Input.KeysDown[K_S])
    {
        CreateMonsterGame(GR,Engine);
    }
    if(Engine->Input.KeysDown[K_K])
    {
        KillMonsterGame(GR,Engine);
    }

}

void BackgroundRoutine(Actor* Background, Engine* Engine)
{
    CADGeneric* CD = Background->CustomData;
    Actor* God = NULL;
    CADGOD* GodCD = NULL;
    Sprite* S = NULL;
    if(CD->Header.SpriteReference != SENTINEL_OBJECT)
    {
        if(Engine->SpriteReferences[CD->Header.SpriteReference])
        {
            if(Engine->SpriteReferences[CD->Header.SpriteReference]->ID == CD->Header.SpriteID)
            {
                S = Engine->SpriteReferences[CD->Header.SpriteReference];
            }
        }
    }

    if(CD->GodReference != SENTINEL_OBJECT)
    {
        if(Engine->ActorReferences[CD->GodReference]->ID == CD->GodID)
        {
            God = Engine->ActorReferences[CD->GodReference];
            GodCD = God->CustomData;
        }
    }
    if(God)
    {
        if(GodCD->ResetScene)
        {
            S->TextureReference = GodCD->CurrentScene->Textures[TXTR_BG];
            S->TextureID = Engine->TextureReferences[GodCD->CurrentScene->Textures[TXTR_BG]]->ID;
            if(S->TextureReference != SENTINEL_OBJECT)
            {
                S->RenderParameters.Texture = Engine->TextureReferences[S->TextureReference]->Texture;
            }
        }
    }
}

void PlayerRoutine(Actor* Player, Engine* Engine)
{
    CADPlayer* CD = Player->CustomData;
    Input* I = &Engine->Input;
    bool* KD = I->KeysDown;
    int PN = CD->PlayerNumber;
    Actor* God = NULL;
    CADGOD* GodCD = NULL;
    Sprite* S = NULL;
    if(CD->Header.SpriteReference != SENTINEL_OBJECT)
    {
        if(Engine->SpriteReferences[CD->Header.SpriteReference])
        {
            if(Engine->SpriteReferences[CD->Header.SpriteReference]->ID == CD->Header.SpriteID)
            {
                S = Engine->SpriteReferences[CD->Header.SpriteReference];
            }
        }
    }

    if(CD->GodReference != SENTINEL_OBJECT)
    {
        if(Engine->ActorReferences[CD->GodReference]->ID == CD->GodID)
        {
            God = Engine->ActorReferences[CD->GodReference];
            GodCD = God->CustomData;
        }
    }
    if(God)
    {
        if(GodCD->ResetScene)
        {
            S->TextureReference = GodCD->CurrentScene->Textures[TXTR_PLAYER];
            S->TextureID = Engine->TextureReferences[GodCD->CurrentScene->Textures[TXTR_PLAYER]]->ID;
            if(S->TextureReference != SENTINEL_OBJECT)
            {
                S->RenderParameters.Texture = Engine->TextureReferences[S->TextureReference]->Texture;
            }
        }
    }
    
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
    Actor* God = NULL;
    CADGOD* GodCD = NULL;
    Sprite* S = NULL;
    CADMonster* CD = Monster->CustomData;
    Actor* Target = NULL;

    ActorScreenWrap(Monster->ReferenceIndex,Engine);
    ApplyPhysics(Monster->ReferenceIndex,Engine);

    if(CD->Header.SpriteReference != SENTINEL_OBJECT)
    {
        if(Engine->SpriteReferences[CD->Header.SpriteReference])
        {
            if(Engine->SpriteReferences[CD->Header.SpriteReference]->ID == CD->Header.SpriteID)
            {
                S = Engine->SpriteReferences[CD->Header.SpriteReference];
            }
        }
    }

    if(CD->GodReference != SENTINEL_OBJECT)
    {
        if(Engine->ActorReferences[CD->GodReference]->ID == CD->GodID)
        {
            God = Engine->ActorReferences[CD->GodReference];
            GodCD = God->CustomData;
        }
    }
    if(God)
    {
        if(GodCD->ResetScene)
        {
            S->TextureReference = GodCD->CurrentScene->Textures[TXTR_MONSTER];
            S->TextureID = Engine->TextureReferences[GodCD->CurrentScene->Textures[TXTR_MONSTER]]->ID;
            if(S->TextureReference != SENTINEL_OBJECT)
            {
                S->RenderParameters.Texture = Engine->TextureReferences[S->TextureReference]->Texture;
            }
        }
    }

    if(!(Engine->Clock.TotalTicks % 5))
    {   
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



uint32 CreateGodObject(Engine* Engine)
{
    Vector2 AP = {0,0};
    Vector2 AD = {0,0};
    int AV = 0;
    CADGOD* CAD = calloc(1,sizeof(struct CADGOD));
    *CAD = (CADGOD)
    {
        (CADHeader){CAD_GOD,ACT_GOD},
        NULL,
        false
    };
    return(CreateActor("God Object",AP,AD,AV,CAD,&GodRoutine,Engine));
}

void CreateBGGame(uint32 GodRef, Engine* Engine)
{
    Vector3 SP = {0,0,0};
    Vector4 SO = {0,0,640,480};
    Vector2 SD = {640,480};
    void* CSD = NULL;
    uint32 ST = ((CADGOD*)(Engine->ActorReferences[GodRef]->CustomData))->CurrentScene->Textures[TXTR_BG];
    uint32 SR = CreateSprite("BackGround",SP,SO,SD,ST,CSD,NULL,NULL,Engine);

    Vector2 AP = {0,0};
    Vector2 AD = {Engine->Video.LogicalDimensions.X,Engine->Video.LogicalDimensions.Y};
    int AV = 0;
    CADGeneric* CAD = calloc(1,sizeof(struct CADGeneric));
    *CAD = (CADGeneric)
    {
        (CADHeader){CAD_GENERIC,ACT_BG},
        (CADPhysics){(FVector2){0.00,0.00},0.50,0.99,0.75},
        GodRef,
        Engine->ActorReferences[GodRef]->ID
    };
    uint32 AR = CreateActor("Background",AP,AD,AV,CAD,&BackgroundRoutine,Engine);
    Actor* A;
    if(AR != SENTINEL_OBJECT)
    {
        A = Engine->ActorReferences[AR];
    }
    else
    {
        return;
    }

    CAD->Header.SpriteID = Engine->SpriteReferences[SR]->ID;
    CAD->Header.SpriteReference = SR;
}

void CreatePlayerGame(uint8 PlayerNumber, uint32 GodRef, Engine* Engine)
{
    Vector2 AP = {GetRandomNumber(0,Engine->Video.LogicalDimensions.X),GetRandomNumber(0,Engine->Video.LogicalDimensions.Y)};
    Vector2 AD = {16,16};
    int AV = 0;
    CADPlayer* CAD = calloc(1,sizeof(struct CADPlayer));
    *CAD = (CADPlayer)
    {
        (CADHeader){CAD_PLAYER,ACT_PLAYER},
        (CADPhysics){(FVector2){0.00,0.00},0.50,0.99,0.75},
        GodRef,
        Engine->ActorReferences[GodRef]->ID,
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
    uint32 ST = ((CADGOD*)(Engine->ActorReferences[GodRef]->CustomData))->CurrentScene->Textures[TXTR_PLAYER];
    uint32 SR = CreateSprite("Player",SP,SO,SD,ST,CSDD,A,AlignSpriteToActor,Engine);

    CAD->Header.SpriteID = Engine->SpriteReferences[SR]->ID;
    CAD->Header.SpriteReference = SR;
}

void CreateMonsterGame(uint32 GodRef, Engine* Engine)
{
    Vector2 AP = {GetRandomNumber(0,Engine->Video.LogicalDimensions.X),GetRandomNumber(0,Engine->Video.LogicalDimensions.Y)};
    Vector2 AD = {32,32};
    int AV = 0;
    CADMonster* CAD = calloc(1,sizeof(struct CADMonster));
    *CAD = (CADMonster)
    {
        (CADHeader){CAD_MONSTER,ACT_MONSTER},
        (CADPhysics){(FVector2){0.00,0.00},1.50,0.99,0.75},
        GodRef,
        Engine->ActorReferences[GodRef]->ID,
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
    uint32 ST = ((CADGOD*)(Engine->ActorReferences[GodRef]->CustomData))->CurrentScene->Textures[TXTR_MONSTER];
    uint32 SR = CreateSprite("Monster",SP,SO,SD,ST,CSDD,A,AlignSpriteToActor,Engine);
    if(SR != SENTINEL_OBJECT)
    {
        Sprite* S = Engine->SpriteReferences[SR];
        S->RenderParameters.Tint = (Vector3){GetRandomNumber(0,255),GetRandomNumber(0,255),GetRandomNumber(0,255)};
    }

    CAD->Header.SpriteID = Engine->SpriteReferences[SR]->ID;
    CAD->Header.SpriteReference = SR;
}

void KillMonsterGame(uint32 GodRef, Engine* Engine)
{
    uint32 AR = GetActorByName("Monster",Engine);
    if(AR != SENTINEL_OBJECT)
    {
        Actor* A = Engine->ActorReferences[AR];
        Sprite* S = Engine->SpriteReferences[(((CADMonster*)(A->CustomData))->Header.SpriteReference)];
        if(S->ID == (((CADMonster*)(A->CustomData))->Header.SpriteID))
        {
            DestroySprite(S,Engine);
            PlaySound(Engine->SoundReferences[((CADGOD*)(Engine->ActorReferences[((CADMonster*)(A->CustomData))->GodReference]->CustomData))->CurrentScene->Sounds[SND_MONSTER_DIE]]->Sound,A->Voice%Engine->Config.Voices,ScreenVolume((int)(A->Position.X),Engine->Video.LogicalDimensions.X/2,Engine),ScreenPan((int)(A->Position.X),Engine->Video.LogicalDimensions.X/2,Engine),Engine); //the worst function call ever written?
            DestroyActor(Engine->ActorReferences[AR],Engine);
        }
    }
}

Scene* CreateScene(Scene* Scene, Engine* Engine)
{
    char Buffer[STRING_BUFFER_SIZE];
    if(Scene->SceneNumber == 0)
    {
        Scene->Textures[TXTR_BG] = CreateTexture(GetAssetPath("Assets/Images/Backgrounds/EvilBG.png",Buffer,Engine),"BG",Engine);
        Scene->Textures[TXTR_PLAYER] = CreateTexture(GetAssetPath("Assets/Images/Sprites/Player.png",Buffer,Engine),"Player",Engine);
        Scene->Textures[TXTR_MONSTER] = CreateTexture(GetAssetPath("Assets/Images/Sprites/Monster.png",Buffer,Engine),"Monster",Engine);
        Scene->Musics[MUS_TRACK01] = CreateMusic(GetAssetPath("Assets/Sounds/Music/Waiting for Romero to Play.mp3",Buffer,Engine),"WFRTP",Engine);
        Scene->Sounds[SND_MONSTER_DIE] = CreateSound(GetAssetPath("Assets/Sounds/Cough.wav",Buffer,Engine),"Cough",Engine);
        PlayMusic(Engine->MusicReferences[Scene->Musics[MUS_TRACK01]]->Music,-1,Engine);
    }
    if(Scene->SceneNumber == 1)
    {
        Scene->Textures[TXTR_BG] = CreateTexture(GetAssetPath("Assets/Images/Backgrounds/TestBG.png",Buffer,Engine),"BG",Engine);
        Scene->Textures[TXTR_PLAYER] = CreateTexture(GetAssetPath("Assets/Images/Sprites/Monster.png",Buffer,Engine),"Player",Engine);
        Scene->Textures[TXTR_MONSTER] = CreateTexture(GetAssetPath("Assets/Images/Sprites/Player.png",Buffer,Engine),"Monster",Engine);
        Scene->Musics[MUS_TRACK01] = CreateMusic(GetAssetPath("Assets/Sounds/Music/Waiting for Romero to Play.mp3",Buffer,Engine),"WFRTP",Engine);
        Scene->Sounds[SND_MONSTER_DIE] = CreateSound(GetAssetPath("Assets/Sounds/Cough.wav",Buffer,Engine),"Cough",Engine);
    }
    return(Scene);
}

void ResetScene(Engine* Engine)
{
    for(int i = 0; i < Engine->Resource.AllocatedTextureMemory; i++)
    {
        if(Engine->Textures->IsUsed)
        {
            DestroyTexture(&Engine->Textures[i],Engine);
        }
    }
    for(int i = 0; i < Engine->Resource.AllocatedSoundMemory; i++)
    {
        if(Engine->Sounds->IsUsed)
        {
            DestroySound(&Engine->Sounds[i],Engine);
        }
    }
    for(int i = 0; i < Engine->Resource.AllocatedMusicMemory; i++)
    {
        if(Engine->Musics->IsUsed)
        {
            DestroyMusic(&Engine->Musics[i],Engine);
        }
    }
}

uint32 InitGame(Engine* Engine)
{
    Actor* GO = Engine->ActorReferences[CreateGodObject(Engine)];
    uint32 GRF = GO->ReferenceIndex;
    Scene* S = calloc(1,sizeof(struct Scene));
    CreateScene(S,Engine);
    ((CADGOD*)(GO->CustomData))->CurrentScene = S;
    CreateBGGame(GRF,Engine);
    CreatePlayerGame(1,GRF,Engine);
    return(GRF);
}



int main(int argc, char* argv[])
{
    Engine* Engine = InitEngine("Config.ini","MOBSCE Demo","Assets/Images/Icon.png",ERROR_SHOW_ALL,WARNING_SHOW_ALL,100,true,true,NULL);
    uint32 GodRef = InitGame(Engine);

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
            ResetScene(Engine);
            CleanupEngine(Engine);
            OSMemoryFree(Engine,sizeof(struct Engine));
            return(0);
        }
        if(Engine->Input.KeysUp[GM_RESTART])
        {
            ResetScene(Engine);
            CleanupEngine(Engine);
            OSMemoryFree(Engine,sizeof(struct Engine));
            Engine = InitEngine("Config.ini","MOBSCE Demo","Assets/Images/Icon.png",ERROR_SHOW_ALL,WARNING_SHOW_ALL,100,true,true,NULL);
            InitGame(Engine);
        }
        if(Engine->Input.KeysUp[K_T]) //trigger testing stuffs
        {
            //PrintActorPool((Object*)Engine->Actors,Engine->Resource.AllocatedActorMemory);
            ResourceInfo R;
            InitResourcePool(R,Engine);
        }
        if(Engine->Clock.TotalTicks && (int)(Engine->Clock.FrameRate))
        {
            if(!(Engine->Clock.TotalTicks % (int)(Engine->Clock.FrameRate)))
            {
                printf("\r%f",(Engine->Clock.FrameRate));
                // printf("\r%d",Engine->Resource.NumberOfActors+Engine->Resource.NumberOfSprites);
            }
        }

        RunEngine(Engine);
    }
}