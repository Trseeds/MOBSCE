#include "MOBSCE.h"
#include "GAME.h"

/*
Edit these functions to free the appropriate resources inside the custom data functions
and then free themselves, called by the engine. Do NOT Free the objects themselves, this
cause problems.
*/
/***************************************************************************************/
void SpriteFreeFunction(void* SpritePtr)
{
    Sprite* FSprite = (Sprite*)SpritePtr;
    free(FSprite->CustomData);
}

void ActorFreeFunction(void* ActorPtr)
{
    Actor* FActor = (Actor*)ActorPtr;
    free(FActor->CustomData);
}
/***************************************************************************************/

/*SPRITE AND ACTOR ROUTINES*************************************************************/

void AlignSpriteToActor(Sprite* Sprite, Engine* Engine)
{   
    Actor* Actor = Engine->ActorReferences[Sprite->ActorReferenceIndex];
    if(Actor)
    {
        if(Actor->ID == Sprite->ExpectedActorID)
        {
            Sprite->RenderParameters.Position.X = Actor->Position.X;
            Sprite->RenderParameters.Position.Y = Actor->Position.Y;
        }
    }
}

void ActorScreenWrap(Actor* Actor, Engine* Engine)
{
    if(Actor->Position.X > Engine->Video.LogicalDimensions.X)
    {
        Actor->Position.X = 0;
    }
    if(Actor->Position.X < 0)
    {
        Actor->Position.X = Engine->Video.LogicalDimensions.X;
    }
    if(Actor->Position.Y > Engine->Video.LogicalDimensions.Y)
    {
        Actor->Position.Y = 0;
    }
    if(Actor->Position.Y < 0)
    {
        Actor->Position.Y = Engine->Video.LogicalDimensions.Y;
    }
}

void ApplyPhysics(Actor* Actor)
{
    CustomActorData* PhysProps = Actor->CustomData;
    //x drag
    if(PhysProps->Velocity.X > PhysProps->Drag)
    {
        PhysProps->Velocity.X -= PhysProps->Drag;
    }
    if(PhysProps->Velocity.X < PhysProps->Drag*-1)
    {
        PhysProps->Velocity.X += PhysProps->Drag;
    }
    if((PhysProps->Velocity.X < PhysProps->Drag) && PhysProps->Velocity.X > 0)
    {
        PhysProps->Velocity.X = 0;
    }
    if((PhysProps->Velocity.X > PhysProps->Drag*-1) && PhysProps->Velocity.X < 0)
    {
        PhysProps->Velocity.X = 0;
    }
    //y drag
    if(PhysProps->Velocity.Y > PhysProps->Drag)
    {
        PhysProps->Velocity.Y -= PhysProps->Drag;
    }
    if(PhysProps->Velocity.Y < PhysProps->Drag*-1)
    {
        PhysProps->Velocity.Y += PhysProps->Drag;
    }
    if((PhysProps->Velocity.Y < PhysProps->Drag) && PhysProps->Velocity.Y > 0)
    {
        PhysProps->Velocity.Y = 0;
    }
    if((PhysProps->Velocity.Y > PhysProps->Drag*-1) && PhysProps->Velocity.Y < 0)
    {
        PhysProps->Velocity.Y = 0;
    }

    Actor->Position.X += PhysProps->Velocity.X;
    Actor->Position.Y += PhysProps->Velocity.Y;
}

void PlayerRoutine(Actor* Player, Engine* Engine)
{
    ActorScreenWrap(Player,Engine);
    ApplyPhysics(Player);
    if(Engine->Input.KeysDown[K_UP])
    {
        Player->CustomData->Velocity.Y -= 0.5;
    }
    if(Engine->Input.KeysDown[K_DOWN])
    {
        Player->CustomData->Velocity.Y += 0.5;
    }
    if(Engine->Input.KeysDown[K_LEFT])
    {
        Player->CustomData->Velocity.X -= 0.5;
    }
    if(Engine->Input.KeysDown[K_RIGHT])
    {
        Player->CustomData->Velocity.X += 0.5;
    }
}

void MonsterRoutine(Actor* Monster, Engine* Engine)
{
    ActorScreenWrap(Monster,Engine);
    ApplyPhysics(Monster);
    Actor* Target = Engine->ActorReferences[Monster->CustomData->TargetReferenceIndex];
    if(Target->ID != Monster->CustomData->TargetID)
    {
        return;
    }

    if(Monster->Position.X < Target->Position.X)
    {
        Monster->CustomData->Velocity.X += 0.2;
    }
    if(Monster->Position.X > Target->Position.X)
    {
        Monster->CustomData->Velocity.X -= 0.2;
    }
    if(Monster->Position.Y < Target->Position.Y)
    {
        Monster->CustomData->Velocity.Y += 0.2;
    }
    if(Monster->Position.Y > Target->Position.Y)
    {
        Monster->CustomData->Velocity.Y -= 0.2;
    }
}

void UpdateDigit(int Reference, int Divisor, int Data, Engine* Engine)
{
    Sprite* Digit = Engine->SpriteReferences[Reference];
    Digit->RenderParameters.Origin.X = 32 * ((Data / Divisor) % 10);
}
/*SPRITE AND ACTOR ROUTINES END*********************************************************/

void CreateFPSCounter(int* References, Engine* Engine)
{
    Vector3 Position = {0,0,1000};
    Vector4 Origin = {0,0,32,32};
    Vector2 Dimensions = {32,32};
    References[0] = CreateSprite("FPS Digit 100,000",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[1] = CreateSprite("FPS Digit 10,000",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[2] = CreateSprite("FPS Digit 1,000",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[3] = CreateSprite("FPS Digit 100",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[4] = CreateSprite("FPS Digit 10",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[5] = CreateSprite("FPS Digit 1",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
}

void CreateObjectCounter(int* References, Engine* Engine)
{
    Vector3 Position = {0,32,1000};
    Vector4 Origin = {0,0,32,32};
    Vector2 Dimensions = {32,32};
    References[0] = CreateSprite("OBJ Digit 100,000",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[1] = CreateSprite("OBJ Digit 10,000",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[2] = CreateSprite("OBJ Digit 1,000",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[3] = CreateSprite("OBJ Digit 100",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[4] = CreateSprite("OBJ Digit 10",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[5] = CreateSprite("OBJ Digit 1",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
}

void CreateSpriteCounter(int* References, Engine* Engine)
{
    Vector3 Position = {0,64,1000};
    Vector4 Origin = {0,0,32,32};
    Vector2 Dimensions = {32,32};
    References[0] = CreateSprite("SPR Digit 100,000",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[1] = CreateSprite("SPR Digit 10,000",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[2] = CreateSprite("SPR Digit 1,000",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[3] = CreateSprite("SPR Digit 100",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[4] = CreateSprite("SPR Digit 10",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
    Position.X += 32;
    References[5] = CreateSprite("SPR Digit 1",Position,Origin,Dimensions,TXTR_NUMBERS,NULL,NULL,NULL,Engine)->ReferenceIndex;
}

void CacheTexturesGame(Engine* Engine)
{
    char Buffer[STRING_BUFFER_SIZE];
    CacheTexture(GetAssetPath("Assets/Images/Backgrounds/TestBG.png",Buffer,Engine),Engine);
    CacheTexture(GetAssetPath("Assets/Images/Sprites/Player.png",Buffer,Engine),Engine);
    CacheTexture(GetAssetPath("Assets/Images/Sprites/Monster.png",Buffer,Engine),Engine);
    CacheTexture(GetAssetPath("Assets/Images/Sprites/Numbers.png",Buffer,Engine),Engine);
}

void CacheSoundsAndMusicGame(Engine* Engine)
{
    char Buffer[STRING_BUFFER_SIZE];
    CacheSound(GetAssetPath("Assets/Sounds/Cough.wav",Buffer,Engine),Engine);
    CacheMusic(GetAssetPath("Assets/Sounds/Music/Waiting for Romero to Play.mp3",Buffer,Engine),Engine);
}

void CreateBGGame(Engine* Engine)
{
    Vector2 ActorPosition;
    Vector2 ActorDimensions;
    int Voice = 0;
    CustomActorData* ActorData = calloc(1,sizeof(CustomActorData));
    ActorPosition.X = 0; ActorPosition.Y = 0;
    ActorDimensions.X = 1280; ActorDimensions.Y = 960;
    Actor* Actor = CreateActor("Background",ActorPosition,ActorDimensions,Voice,ActorData,NULL,Engine);
    /***********************************************************************************/
    Vector3 Position;
    Vector4 Origin;
    Vector2 Dimensions;
    CustomSpriteData* SpriteData = calloc(1,sizeof(CustomSpriteData));
    Position.X = 0; Position.Y = 0; Position.Z = 0;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 1280; Origin.W = 960;
    Dimensions.X = 1280; Dimensions.Y = 960;
    CreateSprite("Background",Position,Origin,Dimensions,TXTR_BG,SpriteData,Actor,NULL,Engine);
}

void CreatePlayerGame(Engine* Engine)
{
    Vector2 ActorPosition;
    Vector2 ActorDimensions;
    int Voice = 0;
    CustomActorData* ActorData = calloc(1,sizeof(CustomActorData));
    ActorData->Drag = 0.01;
    ActorPosition.X = 100; ActorPosition.Y = 100;
    ActorDimensions.X = 16; ActorDimensions.Y = 16;
    Actor* Actor = CreateActor("Player",ActorPosition,ActorDimensions,Voice,ActorData,&PlayerRoutine,Engine);
    /***********************************************************************************/
    Vector3 SpritePosition;
    Vector4 SpriteOrigin;
    Vector2 SpriteDimensions;
    CustomSpriteData* SpriteData = calloc(1,sizeof(CustomSpriteData));
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 2;
    SpriteOrigin.X = 0; SpriteOrigin.Y = 0; SpriteOrigin.Z = 16; SpriteOrigin.W = 16;
    SpriteDimensions.X = 16; SpriteDimensions.Y = 16;
    CreateSprite("Player",SpritePosition,SpriteOrigin,SpriteDimensions,TXTR_PLAYER,SpriteData,Actor,&AlignSpriteToActor,Engine);
}

void CreateMonsterGame(Engine* Engine)
{
    Vector2 ActorPosition;
    Vector2 ActorDimensions;
    int Voice = 1;
    CustomActorData* ActorData = malloc(sizeof(CustomActorData));
    ActorData->Drag = 0.01;
    ActorPosition.X = 300; ActorPosition.Y = 300;
    ActorDimensions.X = GetRandomNumber(8,64); ActorDimensions.Y = GetRandomNumber(8,64);
    Actor* Actor = CreateActor("Monster",ActorPosition,ActorDimensions,Voice,ActorData,&MonsterRoutine,Engine);
    Actor->CustomData->TargetReferenceIndex = GetActorByName("Player",Engine)->ReferenceIndex;
    Actor->CustomData->TargetID = Engine->ActorReferences[Actor->CustomData->TargetReferenceIndex]->ID;
    /***********************************************************************************/
    Vector3 SpritePosition;
    Vector4 SpriteOrigin;
    Vector2 SpriteDimensions;
    CustomSpriteData* SpriteData = malloc(sizeof(CustomSpriteData));
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 1;
    SpriteOrigin.X = 0; SpriteOrigin.Y = GetRandomNumber(0,4)*32; SpriteOrigin.Z = 32; SpriteOrigin.W = 32;
    SpriteDimensions.X = ActorDimensions.X; SpriteDimensions.Y = ActorDimensions.Y;
    Sprite* Sprite = CreateSprite("Monster",SpritePosition,SpriteOrigin,SpriteDimensions,TXTR_MONSTER,SpriteData,Actor,&AlignSpriteToActor,Engine);
    Sprite->RenderParameters.Tint.X = GetRandomNumber(0,255);
    Sprite->RenderParameters.Tint.Y = GetRandomNumber(0,255);
    Sprite->RenderParameters.Tint.Z = GetRandomNumber(0,255);
}

void InitGame(int* FPS, int* OBJ, int* SPR, Engine* Engine)
{
    CacheTexturesGame(Engine);
    CacheSoundsAndMusicGame(Engine);
    CreateBGGame(Engine);
    CreatePlayerGame(Engine);
    CreateMonsterGame(Engine);
    CreateFPSCounter(FPS,Engine);
    CreateObjectCounter(OBJ,Engine);
    CreateSpriteCounter(SPR,Engine);
    PlayMusic(MUS_WFRTP,Engine);
}

int main(int argc, char* argv[])
{
    Engine* Engine = InitEngine("Config.ini","MOBSCE Demo","Assets/Images/Icon.png",ERROR_SHOW_ALL,WARNING_SHOW_ALL);

    int FPSCounter[6];
    int OBJCounter[6];
    int SPRCounter[6];
    InitGame(FPSCounter,OBJCounter,SPRCounter,Engine);

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

        if(Engine->Input.KeysUp[K_ESCAPE])
        {
            CleanupEngine(Engine);
            OSMemoryFree(Engine,sizeof(struct Engine));
            return(0);
        }
        if(Engine->Input.KeysUp[K_R])
        {
            CleanupEngine(Engine);
            OSMemoryFree(Engine,sizeof(struct Engine));
            Engine = InitEngine("Config.ini","MOBSCE Demo","Assets/Images/Icon.png",ERROR_SHOW_ALL,WARNING_SHOW_ALL);
            InitGame(FPSCounter,OBJCounter,SPRCounter,Engine);
        }
        if(Engine->Input.KeysDown[K_S])
        {
            CreateMonsterGame(Engine);
        }
        if(Engine->Input.KeysDown[K_K])
        {
            Sprite* Sprite = GetSpriteByName("Monster",Engine);
            if(Sprite)
            {
                Actor* Actor = Engine->ActorReferences[Sprite->ActorReferenceIndex];
                if(Actor)
                {
                    PlaySound(SND_COUGH,-1,5,Actor->Position.X,Engine);
                }
                DestroySprite(Sprite,&SpriteFreeFunction,Engine);
                DestroyActor(Actor,&ActorFreeFunction,Engine);   
            }
        }

        if(Engine->Input.KeysDown[K_LEFTCONTROL] || Engine->Input.KeysDown[K_RIGHTCONTROL])
        {
            if(Engine->Input.KeysDown[K_S])
            {
                for(int i = 0; i < 10; i++)
                {
                    CreateMonsterGame(Engine);
                }
            }
        }

        UpdateDigit(FPSCounter[0],100000,Engine->Clock.FrameRate,Engine);
        UpdateDigit(FPSCounter[1],10000,Engine->Clock.FrameRate,Engine);
        UpdateDigit(FPSCounter[2],1000,Engine->Clock.FrameRate,Engine);
        UpdateDigit(FPSCounter[3],100,Engine->Clock.FrameRate,Engine);
        UpdateDigit(FPSCounter[4],10,Engine->Clock.FrameRate,Engine);
        UpdateDigit(FPSCounter[5],1,Engine->Clock.FrameRate,Engine);
        UpdateDigit(OBJCounter[0],100000,Engine->Resource.NumberOfActors,Engine);
        UpdateDigit(OBJCounter[1],10000,Engine->Resource.NumberOfActors,Engine);
        UpdateDigit(OBJCounter[2],1000,Engine->Resource.NumberOfActors,Engine);
        UpdateDigit(OBJCounter[3],100,Engine->Resource.NumberOfActors,Engine);
        UpdateDigit(OBJCounter[4],10,Engine->Resource.NumberOfActors,Engine);
        UpdateDigit(OBJCounter[5],1,Engine->Resource.NumberOfActors,Engine);
        UpdateDigit(SPRCounter[0],100000,Engine->Resource.NumberOfSprites,Engine);
        UpdateDigit(SPRCounter[1],10000,Engine->Resource.NumberOfSprites,Engine);
        UpdateDigit(SPRCounter[2],1000,Engine->Resource.NumberOfSprites,Engine);
        UpdateDigit(SPRCounter[3],100,Engine->Resource.NumberOfSprites,Engine);
        UpdateDigit(SPRCounter[4],10,Engine->Resource.NumberOfSprites,Engine);
        UpdateDigit(SPRCounter[5],1,Engine->Resource.NumberOfSprites,Engine);

        RunEngine(Engine);
    }
}