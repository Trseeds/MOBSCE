typedef struct CustomSpriteData {
    unsigned char Byte;
} CustomSpriteData;

typedef struct CustomActorData {
    unsigned char Byte;
} CustomActorData;

#include "MOBSCE.h"

void ScreenCrawl(Actor* Actor, Engine* Engine)
{
    Actor->Position.X++;
    Actor->Position.Y++;
    if(Actor->Position.X > Engine->Video.LogicalDimensions.X)
    {
        Actor->Position.X = 0;
    }
    if(Actor->Position.Y > Engine->Video.LogicalDimensions.Y)
    {
        Actor->Position.Y = 0;
    }
}

void AlignSpriteToActor(Sprite* Sprite, Engine* Engine)
{
    if(Sprite->Actor)
    {
        Sprite->Position.X = Sprite->Actor->Position.X;
        Sprite->Position.Y = Sprite->Actor->Position.Y;
    }
}

void CreateTestObject(Engine* Engine)
{
    Vector3 SpritePosition;
    Vector2 ActorPosition;
    Vector4 Origin;
    Vector2 Dimensions;
    ActorPosition.X = GetRandomNumber(Engine->Video.LogicalDimensions.X); ActorPosition.Y = GetRandomNumber(Engine->Video.LogicalDimensions.Y);
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 0;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 16; Origin.W = 16;
    Dimensions.X = 16; Dimensions.Y = 16;
    CreateActor("Test Actor",ActorPosition,Dimensions,0,&ScreenCrawl,Engine);
    CreateSprite("Test Sprite",SpritePosition,Origin,Dimensions,1,1,Engine->Actors[Engine->Resource.NumberOfActors-1],&AlignSpriteToActor,Engine);
}

void InitGame(Engine* Engine)
{
    char BG[1024] = "Assets/Images/Backgrounds/TestBG.bmp";
    char Player[1024] = "Assets/Images/Sprites/Player.bmp";
    CacheTexture(GetAssetPath(BG,Engine),Engine);
    CacheTexture(GetAssetPath(Player,Engine),Engine);

    Vector3 SpritePosition;
    Vector2 ActorPosition;
    Vector4 Origin;
    Vector2 Dimensions;
    ActorPosition.X = 0; ActorPosition.Y = 0;
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 0;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 640; Origin.W = 480;
    Dimensions.X = 640; Dimensions.Y = 480;
    CreateSprite("BG",SpritePosition,Origin,Dimensions,0,1,NULL,NULL,Engine);
}

int main(int argc, char* argv[])
{
    Engine* Engine1 = InitEngine("Config.ini");
    InitGame(Engine1);
    while(Engine1->Running)
    {
        if(Engine1->Input.KeysDown[SDL_SCANCODE_S])
        {
            CreateTestObject(Engine1);
        }
        if(Engine1->Input.KeysDown[SDL_SCANCODE_K])
        {
            if(Engine1->Sprites[1])
            {
                Actor* ActorFriend = Engine1->Sprites[1]->Actor;
                DestroySprite(Engine1->Sprites[1],Engine1);
                if(ActorFriend)
                {
                    DestroyActor(ActorFriend,Engine1);
                }
            }
        }
        RunEngine(Engine1);
        Render(Engine1);
        if(Engine1->Event->type == SDL_QUIT || Engine1->Input.KeysUp[SDL_SCANCODE_ESCAPE])
        {
            CleanupEngine(Engine1);
            free(Engine1);
            return(0);
        }
    }
}