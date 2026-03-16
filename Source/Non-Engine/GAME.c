#include "MOBSCE.h"
#include "GAME.h"

Vector4 WV[6] = 
    {
        {0,0,1279,7},
        {0,0,7,951},
        {0,952,1279,959},
        {8,456,639,479},
        {1272,0,1279,959},
        {697,884,728,907}
    };

/*
Edit these functions to free the appropriate resources inside the custom data functions
and then free themselves, called by the engine.
*/
/***************************************************************************************/
void CustomSpriteDataFreeFunction(CustomSpriteData* CustomSpriteData, Engine* Engine)
{
    free(CustomSpriteData);
}

void CustomActorDataFreeFunction(CustomActorData* CustomActorData, Engine* Engine)
{
    free(CustomActorData);
}
/***************************************************************************************/

/*SPRITE AND ACTOR ROUTINES*************************************************************/
void RenderNumberWiregon(int NUM, int Scale, Wiregon* Wiregon, Engine* Engine)
{
    if(NUM > 9 || NUM < 0)
    {
        return;
    }
    int NumberOfVerticies = 8;
    Vector2* Verticies = calloc(NumberOfVerticies,sizeof(Vector2));
    switch(NUM)
    {
        case 0:
            Verticies[0].X = 0; Verticies[0].Y = 0;
            Verticies[1].X = 2; Verticies[1].Y = 0;
            Verticies[2].X = 2; Verticies[2].Y = 4;
            Verticies[3].X = 0; Verticies[3].Y = 4;
            Verticies[4].X = 0; Verticies[4].Y = 0;
            Verticies[5].X = 0; Verticies[5].Y = 0;
            Verticies[6].X = 0; Verticies[6].Y = 0;
            Verticies[7].X = 0; Verticies[7].Y = 0;
            break;
        case 1:
            Verticies[0].X = 2; Verticies[0].Y = 0;
            Verticies[1].X = 2; Verticies[1].Y = 4;
            Verticies[2].X = 2; Verticies[2].Y = 0;
            Verticies[3].X = 2; Verticies[3].Y = 0;
            Verticies[4].X = 2; Verticies[4].Y = 0;
            Verticies[5].X = 2; Verticies[5].Y = 0;
            Verticies[6].X = 2; Verticies[6].Y = 0;
            Verticies[7].X = 2; Verticies[7].Y = 0;
            break;
        case 2:
            Verticies[0].X = 0; Verticies[0].Y = 0;
            Verticies[1].X = 2; Verticies[1].Y = 0;
            Verticies[2].X = 2; Verticies[2].Y = 2;
            Verticies[3].X = 0; Verticies[3].Y = 2;
            Verticies[4].X = 0; Verticies[4].Y = 4;
            Verticies[5].X = 2; Verticies[5].Y = 4;
            Verticies[6].X = 2; Verticies[6].Y = 4;
            Verticies[7].X = 2; Verticies[7].Y = 4;
            break;
        case 3:
            Verticies[0].X = 0; Verticies[0].Y = 0;
            Verticies[1].X = 2; Verticies[1].Y = 0;
            Verticies[2].X = 2; Verticies[2].Y = 2;
            Verticies[3].X = 0; Verticies[3].Y = 2;
            Verticies[4].X = 2; Verticies[4].Y = 2;
            Verticies[5].X = 2; Verticies[5].Y = 4;
            Verticies[6].X = 0; Verticies[6].Y = 4;
            Verticies[7].X = 0; Verticies[7].Y = 4;
            break;
        case 4:
            Verticies[0].X = 0; Verticies[0].Y = 0;
            Verticies[1].X = 0; Verticies[1].Y = 2;
            Verticies[2].X = 2; Verticies[2].Y = 2;
            Verticies[3].X = 2; Verticies[3].Y = 0;
            Verticies[4].X = 2; Verticies[4].Y = 4;
            Verticies[5].X = 2; Verticies[5].Y = 4;
            Verticies[6].X = 2; Verticies[6].Y = 4;
            Verticies[7].X = 2; Verticies[7].Y = 4;
            break;
        case 5:
            Verticies[0].X = 0; Verticies[0].Y = 0;
            Verticies[1].X = 2; Verticies[1].Y = 0;
            Verticies[2].X = 0; Verticies[2].Y = 0;
            Verticies[3].X = 0; Verticies[3].Y = 2;
            Verticies[4].X = 2; Verticies[4].Y = 2;
            Verticies[5].X = 2; Verticies[5].Y = 4;
            Verticies[6].X = 0; Verticies[6].Y = 4;
            Verticies[7].X = 0; Verticies[7].Y = 4;
            break;
        case 6:
            Verticies[0].X = 0; Verticies[0].Y = 0;
            Verticies[1].X = 2; Verticies[1].Y = 0;
            Verticies[2].X = 0; Verticies[2].Y = 0;
            Verticies[3].X = 0; Verticies[3].Y = 4;
            Verticies[4].X = 2; Verticies[4].Y = 4;
            Verticies[5].X = 2; Verticies[5].Y = 2;
            Verticies[6].X = 0; Verticies[6].Y = 2;
            Verticies[7].X = 0; Verticies[7].Y = 2;
            break;
        case 7:
            Verticies[0].X = 0; Verticies[0].Y = 0;
            Verticies[1].X = 2; Verticies[1].Y = 0;
            Verticies[2].X = 2; Verticies[2].Y = 4;
            Verticies[3].X = 2; Verticies[3].Y = 4;
            Verticies[4].X = 2; Verticies[4].Y = 4;
            Verticies[5].X = 2; Verticies[5].Y = 4;
            Verticies[6].X = 2; Verticies[6].Y = 4;
            Verticies[7].X = 2; Verticies[7].Y = 4;
            break;
        case 8:
            Verticies[0].X = 0; Verticies[0].Y = 0;
            Verticies[1].X = 0; Verticies[1].Y = 4;
            Verticies[2].X = 2; Verticies[2].Y = 4;
            Verticies[3].X = 2; Verticies[3].Y = 0;
            Verticies[4].X = 2; Verticies[4].Y = 2;
            Verticies[5].X = 0; Verticies[5].Y = 2;
            Verticies[6].X = 0; Verticies[6].Y = 0;
            Verticies[7].X = 2; Verticies[7].Y = 0;
            break;
        case 9:
            Verticies[0].X = 0; Verticies[0].Y = 0;
            Verticies[1].X = 0; Verticies[1].Y = 2;
            Verticies[2].X = 2; Verticies[2].Y = 2;
            Verticies[3].X = 2; Verticies[3].Y = 0;
            Verticies[4].X = 0; Verticies[4].Y = 0;
            Verticies[5].X = 2; Verticies[5].Y = 0;
            Verticies[6].X = 2; Verticies[6].Y = 4;
            Verticies[7].X = 2; Verticies[7].Y = 4;
            break;

        default:
            break;
    }
    for(int i = 0; i < NumberOfVerticies; i++)
    {
        Verticies[i].X *= Scale;
        Verticies[i].Y *= Scale;
    }
    free(Wiregon->Verticies);
    Wiregon->Verticies = Verticies;
    Wiregon->NumberOfVerticies = NumberOfVerticies;
}

void AlignSpriteToActor(Sprite* Sprite, Engine* Engine)
{
    if(Sprite->Actor)
    {
        Sprite->RenderParameters.Position.X = Sprite->Actor->Position.X;
        Sprite->RenderParameters.Position.Y = Sprite->Actor->Position.Y;
    }
}

void AlignSpriteToActorOnCamera(Sprite* Sprite, Engine* Engine)
{
    if(Sprite->Actor)
    {
        Actor* Camera = Sprite->Actor->CustomData->Camera;
        Sprite->RenderParameters.Visible = false;

        int ScreenX = Sprite->Actor->Position.X - Camera->Position.X;
        int ScreenY = Sprite->Actor->Position.Y - Camera->Position.Y;
        if(ScreenX + Sprite->RenderParameters.Dimensions.X >= 0 && 
           ScreenX <= Camera->Dimensions.X &&
           ScreenY + Sprite->RenderParameters.Dimensions.Y >= 0 && 
           ScreenY <= Camera->Dimensions.Y)
        {
            Sprite->RenderParameters.Position.X = ScreenX;
            Sprite->RenderParameters.Position.Y = ScreenY;
            Sprite->RenderParameters.Visible = true;
        }
    }
}

void CameraTrack(Actor* Camera, Engine* Engine)
{
    if(!Camera->CustomData->Target)
    {
        Camera->CustomData->Target = GetActorByName("Player",Engine);
    }
    Camera->Position.X = (Camera->CustomData->Target->Position.X + Camera->CustomData->Target->Dimensions.X/2) - (Camera->Dimensions.X/2);
    Camera->Position.Y = (Camera->CustomData->Target->Position.Y + Camera->CustomData->Target->Dimensions.Y/2) - (Camera->Dimensions.Y/2);
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

void ActorScreenCollision(Actor* Actor, Engine* Engine)
{
    if(Actor->Position.X+Actor->Dimensions.X > Engine->Video.LogicalDimensions.X)
    {
        Actor->Position.X = Engine->Video.LogicalDimensions.X-Actor->Dimensions.X;
        Actor->CustomData->Velocity.X = 0;
    }
    if(Actor->Position.X < 0)
    {
        Actor->Position.X = 0;
        Actor->CustomData->Velocity.X = 0;
    }
    if(Actor->Position.Y+Actor->Dimensions.Y > Engine->Video.LogicalDimensions.Y)
    {
        Actor->Position.Y = Engine->Video.LogicalDimensions.Y-Actor->Dimensions.Y;
        Actor->CustomData->Velocity.Y = 0;
    }
    if(Actor->Position.Y < 0)
    {
        Actor->Position.Y = 0;
        Actor->CustomData->Velocity.Y = 0;
    }
}

void ActorSideScroller(Actor* Actor, Engine* Engine)
{
    if(Actor->Position.Y+Actor->Dimensions.Y > Engine->Video.LogicalDimensions.Y)
    {
        Actor->Position.Y = Engine->Video.LogicalDimensions.Y-Actor->Dimensions.Y;
        Actor->CustomData->Velocity.Y = 0;
    }
    if(Actor->Position.Y < 0)
    {
        Actor->Position.Y = 0;
        Actor->CustomData->Velocity.Y = 0;
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
    //gravity
    PhysProps->Velocity.Y += PhysProps->Gravity;

    Actor->Position.X += PhysProps->Velocity.X;
    Actor->Position.Y += PhysProps->Velocity.Y;
}

void PlayerRoutine(Actor* Player, Engine* Engine)
{
    ActorWorldCollide(Player,Engine);
    ApplyPhysics(Player);
    if(Engine->Input.KeysDown[K_UP] && Player->CustomData->Velocity.Y == 0.500000)
    {
        Player->CustomData->Velocity.Y -= Player->CustomData->JumpForce;
        Player->Position.Y--;
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
    ActorWorldCollide(Monster,Engine);
    ApplyPhysics(Monster);
    if(!Monster->CustomData->Target)
    {
        Monster->CustomData->Target = GetActorByName("Player",Engine);
    }

    if(Monster->Position.X < Monster->CustomData->Target->Position.X)
    {
        Monster->CustomData->Velocity.X += 0.2;
    }
    if(Monster->Position.X > Monster->CustomData->Target->Position.X)
    {
        Monster->CustomData->Velocity.X -= 0.2;
    }
    if(Monster->Position.Y < Monster->CustomData->Target->Position.Y)
    {
        Monster->CustomData->Velocity.Y += 0.2;
    }
    if(Monster->Position.Y > Monster->CustomData->Target->Position.Y && Monster->CustomData->Velocity.Y == 0.500000)
    {
        Monster->CustomData->Velocity.Y -= Monster->CustomData->JumpForce;
        Monster->Position.Y--;
    }
}
/*SPRITE AND ACTOR ROUTINES END*********************************************************/

void CreateCamera(Engine* Engine)
{
    Vector2 ActorPosition;
    Vector2 ActorDimensions;
    int Voice = 0;
    CustomActorData* ActorData = calloc(1,sizeof(CustomActorData));
    ActorPosition.X = 0; ActorPosition.Y = 0;
    ActorDimensions.X = 640; ActorDimensions.Y = 480;
    Actor* Actor = CreateActor("Camera",ActorPosition,ActorDimensions,Voice,ActorData,&CameraTrack,Engine);
}

void CreateFPSCounter(Wiregon** Output, Engine* Engine)
{
    Vector3 Position = {0,0,1000};
    Vector3 Color = {0,0,0};
    Output[0] = CreateWiregon(NULL,Position,0,Color,255,Engine);
    Position.X = 4*5;
    Output[1] = CreateWiregon(NULL,Position,0,Color,255,Engine);
    Position.X = 8*5;
    Output[2] = CreateWiregon(NULL,Position,0,Color,255,Engine);
}

void CreateObjectCounter(Wiregon** Output, Engine* Engine)
{
    Vector3 Position = {0,0,1000};
    Vector3 Color = {0,0,0};
    Position.X = 0*5;
    Position.Y = 6*5;
    Output[0] = CreateWiregon(NULL,Position,0,Color,255,Engine);
    Position.X = 4*5;
    Output[1] = CreateWiregon(NULL,Position,0,Color,255,Engine);
    Position.X = 8*5;
    Output[2] = CreateWiregon(NULL,Position,0,Color,255,Engine);
    Position.X = 12*5;
    Output[3] = CreateWiregon(NULL,Position,0,Color,255,Engine);
    Position.X = 16*5;
    Output[4] = CreateWiregon(NULL,Position,0,Color,255,Engine);
}

void CacheTexturesGame(Engine* Engine)
{
    char Buffer[STRING_BUFFER_SIZE];
    char Player[STRING_BUFFER_SIZE];
    char Monster[STRING_BUFFER_SIZE];
    CacheTexture(GetAssetPath("Assets/Images/Backgrounds/TestBG.png",Buffer,Engine),Engine);
    CacheTexture(GetAssetPath("Assets/Images/Sprites/Player.png",Buffer,Engine),Engine);
    CacheTexture(GetAssetPath("Assets/Images/Sprites/Monster.png",Buffer,Engine),Engine);
}

void CreateBGGame(Engine* Engine)
{
    Vector2 ActorPosition;
    Vector2 ActorDimensions;
    int Voice = 0;
    CustomActorData* ActorData = calloc(1,sizeof(CustomActorData));
    ActorData->Camera = GetActorByName("Camera",Engine);
    ActorPosition.X = 0; ActorPosition.Y = 0;
    ActorDimensions.X = 1280; ActorDimensions.Y = 960;
    Actor* Actor = CreateActor("Background",ActorPosition,ActorDimensions,Voice,ActorData,NULL,Engine);
    /***********************************************************************************/
    Vector3 Position;
    Vector4 Origin;
    Vector2 Dimensions;
    CustomActorData* SpriteData = calloc(1,sizeof(CustomSpriteData));
    Position.X = 0; Position.Y = 0; Position.Z = 0;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 1280; Origin.W = 960;
    Dimensions.X = 1280; Dimensions.Y = 960;
    CreateSprite("Background",Position,Origin,Dimensions,TXTR_BG,SpriteData,Actor,&AlignSpriteToActorOnCamera,Engine);
}

void CreatePlayerGame(Engine* Engine)
{
    Vector2 ActorPosition;
    Vector2 ActorDimensions;
    int Voice = 0;
    CustomActorData* ActorData = calloc(1,sizeof(CustomActorData));
    ActorData->JumpForce = 15;
    ActorData->Drag = 0.01;
    ActorData->Gravity = 0.5;
    ActorData->Camera = GetActorByName("Camera",Engine);
    ActorData->World = WV;
    ActorPosition.X = 100; ActorPosition.Y = 100;
    ActorDimensions.X = 16; ActorDimensions.Y = 16;
    Actor* Actor = CreateActor("Player",ActorPosition,ActorDimensions,Voice,ActorData,&PlayerRoutine,Engine);
    /***********************************************************************************/
    Vector3 SpritePosition;
    Vector4 SpriteOrigin;
    Vector2 SpriteDimensions;
    CustomActorData* SpriteData = calloc(1,sizeof(CustomSpriteData));
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 2;
    SpriteOrigin.X = 0; SpriteOrigin.Y = 0; SpriteOrigin.Z = 16; SpriteOrigin.W = 16;
    SpriteDimensions.X = 16; SpriteDimensions.Y = 16;
    CreateSprite("Player",SpritePosition,SpriteOrigin,SpriteDimensions,TXTR_PLAYER,SpriteData,Actor,&AlignSpriteToActorOnCamera,Engine);
}

void CreateMonsterGame(Engine* Engine)
{
    Vector2 ActorPosition;
    Vector2 ActorDimensions;
    int Voice = 1;
    CustomActorData* ActorData = calloc(1,sizeof(CustomActorData));
    ActorData->JumpForce = GetRandomNumber(5,20);
    ActorData->Drag = 0.01;
    ActorData->Gravity = 0.5;
    ActorData->Camera = GetActorByName("Camera",Engine);
    ActorData->World = WV;
    ActorPosition.X = 300; ActorPosition.Y = 300;
    ActorDimensions.X = GetRandomNumber(8,64); ActorDimensions.Y = GetRandomNumber(8,64);
    Actor* Actor = CreateActor("Monster",ActorPosition,ActorDimensions,Voice,ActorData,&MonsterRoutine,Engine);
    /***********************************************************************************/
    Vector3 SpritePosition;
    Vector4 SpriteOrigin;
    Vector2 SpriteDimensions;
    CustomActorData* SpriteData = calloc(1,sizeof(CustomSpriteData));
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 1;
    SpriteOrigin.X = 0; SpriteOrigin.Y = GetRandomNumber(0,4)*32; SpriteOrigin.Z = 32; SpriteOrigin.W = 32;
    SpriteDimensions.X = ActorDimensions.X; SpriteDimensions.Y = ActorDimensions.Y;
    Sprite* Sprite = CreateSprite("Monster",SpritePosition,SpriteOrigin,SpriteDimensions,TXTR_MONSTER,SpriteData,Actor,&AlignSpriteToActorOnCamera,Engine);
    Sprite->RenderParameters.Tint.X = GetRandomNumber(0,255);
    Sprite->RenderParameters.Tint.Y = GetRandomNumber(0,255);
    Sprite->RenderParameters.Tint.Z = GetRandomNumber(0,255);
}

void InitGame(Wiregon** FPS, Wiregon** OBJ, Engine* Engine)
{
    CacheTexturesGame(Engine);
    CreateCamera(Engine);
    CreateBGGame(Engine);
    CreatePlayerGame(Engine);
    CreateMonsterGame(Engine);
    CreateFPSCounter(FPS,Engine);
    CreateObjectCounter(OBJ,Engine);
}

int main(int argc, char* argv[])
{
    Engine* Engine = InitEngine("Config.ini","Monster","Assets/Images/Icon.png",ERROR_SHOW_ALL,WARNING_SHOW_ALL);

    Wiregon* FPSCounter[3];
    Wiregon* OBJCounter[5];
    InitGame(FPSCounter,OBJCounter,Engine);

    while(Engine->Running)
    {
        for(int i = 0; i < EVENT_QUEUE_SIZE; i++)
        {
            if(Engine->Events[i].type == SDL_QUIT)
            {
                CleanupEngine(Engine);
                free(Engine);
                return(0);
            }
        }

        RenderNumberWiregon((int)(Engine->Clock.FrameRate)/100,5,FPSCounter[0],Engine);
        RenderNumberWiregon(((int)(Engine->Clock.FrameRate)/10)%10,5,FPSCounter[1],Engine);
        RenderNumberWiregon((int)(Engine->Clock.FrameRate)%10,5,FPSCounter[2],Engine);
        RenderNumberWiregon((int)(Engine->Resource.NumberOfActors)/10000,5,OBJCounter[0],Engine);
        RenderNumberWiregon(((int)(Engine->Resource.NumberOfActors)/1000)%10,5,OBJCounter[1],Engine);
        RenderNumberWiregon(((int)(Engine->Resource.NumberOfActors)/100)%10,5,OBJCounter[2],Engine);
        RenderNumberWiregon(((int)(Engine->Resource.NumberOfActors)/10)%10,5,OBJCounter[3],Engine);
        RenderNumberWiregon((int)(Engine->Resource.NumberOfActors)%10,5,OBJCounter[4],Engine);
        if(Engine->Clock.FrameRate >= 1000)
        {
            RenderNumberWiregon(9,5,FPSCounter[0],Engine);
            RenderNumberWiregon(9,5,FPSCounter[1],Engine);
            RenderNumberWiregon(9,5,FPSCounter[2],Engine);
        }
        if(Engine->Resource.NumberOfActors > 99999)
        {
            RenderNumberWiregon(9,5,OBJCounter[0],Engine);
            RenderNumberWiregon(9,5,OBJCounter[1],Engine);
            RenderNumberWiregon(9,5,OBJCounter[2],Engine);
            RenderNumberWiregon(9,5,OBJCounter[3],Engine);
            RenderNumberWiregon(9,5,OBJCounter[4],Engine);
        }

        if(Engine->Input.KeysUp[K_ESCAPE])
        {
            CleanupEngine(Engine);
            free(Engine);
            return(0);
        }
        if(Engine->Input.KeysUp[K_R])
        {
            CleanupEngine(Engine);
            free(Engine);
            Engine = InitEngine("Config.ini","Monster","Assets/Images/Icon.png",ERROR_SHOW_ALL,WARNING_SHOW_ALL);
            InitGame(FPSCounter,OBJCounter,Engine);
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
                Actor* Actor = Sprite->Actor;
                DestroySprite(Sprite,&CustomSpriteDataFreeFunction,Engine);
                DestroyActor(Actor,&CustomActorDataFreeFunction,Engine);
            }
        }

        if(Engine->Input.KeysDown[K_LEFTCONTROL] || Engine->Input.KeysDown[K_RIGHTCONTROL])
        {
            if(Engine->Input.KeysUp[K_S])
            {
                for(int i = 0; i < 10000; i++)
                {
                    CreateMonsterGame(Engine);
                }
            }
        }

        RunEngine(Engine);
    }
}