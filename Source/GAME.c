#include "MOBSCE.h"

void TestActorFunction(Actor* Actor, Engine* Engine)
{
    Actor->Position.X += 60 * Engine->Clock.DeltaTime;
    Actor->Position.Y += 60 * Engine->Clock.DeltaTime;
    if(Actor->Position.X > Engine->Video.LogicalDimensions.X)
    {
        Actor->Position.X = 0;
    }
    if(Actor->Position.Y > Engine->Video.LogicalDimensions.Y)
    {
        Actor->Position.Y = 0;
    }
}

void SpawnTestActors(int Count, Engine* Engine)
{
    Vector2 Position;
    Vector2 Dimensions;
    Position.X = 0; Position.Y = 0;
    Dimensions.X = 16; Dimensions.Y = 16;
    for(int i = 0; i < Count; i++)
    {
        CreateActor("Test Actor",Position,Dimensions,0,&TestActorFunction,Engine);
    }
}

void AlignSpriteToActor(Sprite* Sprite, Engine* Engine)
{
    Sprite->Position.X = (int)Engine->Actors[0]->Position.X;
    Sprite->Position.Y = (int)Engine->Actors[0]->Position.Y;
}

int main(int argc, char* argv[])
{
    Engine* Engine1 = InitEngine();
    Vector3 Position;
    Vector2 Position2;
    Vector4 Origin;
    Vector2 Dimensions;
    Position.X = 0; Position.Y = 0; Position.Z = 0;
    Position2.X = 0; Position2.Y = 0;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 500; Origin.W = 500;
    Dimensions.X = 1000; Dimensions.Y = 1000;
    char Asset[1024] = "Assets/Images/Backgrounds/BigTestBG.bmp";
    CacheTexture(GetAssetPath(Asset,Engine1),Engine1);
    strcpy(Asset,"Assets/Images/Sprites/Player.bmp");
    CacheTexture(GetAssetPath(Asset,Engine1),Engine1);
    CreateSprite("Checkerboard Background",Position,Origin,Dimensions,0,true,NULL,Engine1);
    Origin.Z = 16; Origin.W = 16;
    Dimensions.X = 16; Dimensions.Y = 16;
    SpawnTestActors(24,Engine1);
    CreateSprite("Test Sprite",Position,Origin,Dimensions,1,true,&AlignSpriteToActor,Engine1);
    while(Engine1->Running)
    {
        RunEngine(Engine1);
        Render(Engine1);
        if(Engine1->Input.KeysUp[SDL_SCANCODE_ESCAPE])
        {
            CleanupEngine(Engine1);
            free(Engine1);
        }
        if(Engine1->Event->type == SDL_QUIT)
        {
            CleanupEngine(Engine1);
            free(Engine1);
            return(0);
        }
    }
}