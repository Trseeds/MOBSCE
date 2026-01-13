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

void FollowMouse(Actor* Actor, Engine* Engine)
{
    Actor->Position.X = Engine->Input.MousePosition.X;
    Actor->Position.Y = Engine->Input.MousePosition.Y;
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
    CustomSpriteData Dummy;
    CustomActorData Dummy2;
    ActorPosition.X = GetRandomNumber(Engine->Video.LogicalDimensions.X); ActorPosition.Y = GetRandomNumber(Engine->Video.LogicalDimensions.Y);
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 2;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 16; Origin.W = 16;
    Dimensions.X = 16; Dimensions.Y = 16;
    Actor* TestActor = CreateActor("Test Actor",ActorPosition,Dimensions,0,Dummy2,&ScreenCrawl,Engine);
    CreateSprite("Test Sprite",SpritePosition,Origin,Dimensions,1,true,Dummy,TestActor,&AlignSpriteToActor,Engine);
}

void InitGame(Engine* Engine)
{
    char BG[1024] = "Assets/Images/Backgrounds/TestBG.bmp";
    char Player[1024] = "Assets/Images/Sprites/Player.bmp";
    char Cursor[1024] = "Assets/Images/Sprites/Cursor.bmp";
    CacheTexture(GetAssetPath(BG,Engine),Engine);
    CacheTexture(GetAssetPath(Player,Engine),Engine);
    CacheTexture(GetAssetPath(Cursor,Engine),Engine);

    Vector3 SpritePosition;
    Vector2 ActorPosition;
    Vector4 Origin;
    Vector2 Dimensions;
    CustomSpriteData Dummy;
    CustomActorData Dummy2;
    ActorPosition.X = 0; ActorPosition.Y = 0;
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 0;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 640; Origin.W = 480;
    Dimensions.X = 640; Dimensions.Y = 480;
    CreateSprite("BG",SpritePosition,Origin,Dimensions,0,true,Dummy,NULL,NULL,Engine);
    SpritePosition.Z = 1000;
    Dimensions.X = 64; Dimensions.Y = 64;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 64; Origin.W = 64;
    Actor* CursorActor = CreateActor("Mouse Cursor",ActorPosition,Dimensions,0,Dummy2,&FollowMouse,Engine);
    CreateSprite("Mouse Cursor",SpritePosition,Origin,Dimensions,2,true,Dummy,CursorActor,&AlignSpriteToActor,Engine);
}

int main(int argc, char* argv[])
{
    Engine* Engine1 = InitEngine("Config.ini");
    InitGame(Engine1);
    while(Engine1->Running)
    {
        if(Engine1->Input.MouseUp[0])
        {
            Vector4 NewOrigin = {0,64,64,64};
            GetSpriteByName("Mouse Cursor",Engine1)->Origin = NewOrigin;
        }
        else if(Engine1->Input.MouseUp[1])
        {
            Vector4 NewOrigin = {0,128,64,64};
            GetSpriteByName("Mouse Cursor",Engine1)->Origin = NewOrigin;
        }
        else
        {
            Vector4 NewOrigin = {0,0,64,64};
            GetSpriteByName("Mouse Cursor",Engine1)->Origin = NewOrigin;
        }
        if(Engine1->Input.KeysUp[SDL_SCANCODE_R])
        {
            CleanupEngine(Engine1);
            Engine1 = InitEngine("Config.ini");
            InitGame(Engine1);
        }
        if(Engine1->Input.KeysDown[SDL_SCANCODE_S] && !Engine1->Input.KeysDown[SDL_SCANCODE_RCTRL])
        {
            CreateTestObject(Engine1);
        }
        if(Engine1->Input.KeysDown[SDL_SCANCODE_K])
        {
            if(Engine1->Sprites[1])
            {
                Sprite* SpriteFriend = GetSpriteByName("Test Sprite",Engine1);
                if(SpriteFriend)
                {
                    Actor* ActorFriend = SpriteFriend->Actor;
                    DestroySprite(SpriteFriend,Engine1);
                    if(ActorFriend)
                    {
                        DestroyActor(ActorFriend,Engine1);
                    }
                }
            }
        }
        if(Engine1->Input.KeysDown[SDL_SCANCODE_RCTRL] && Engine1->Input.KeysUp[SDL_SCANCODE_S])
        {
            for(int i = 0; i < 10000; i++)
            {
                CreateTestObject(Engine1);
            }
        }
        if(!(Engine1->Clock.TotalFrames % 10))
        {
            printf("%d  %d %f\r",Engine1->Resource.NumberOfActors,Engine1->Resource.AllocatedActorMemory,Engine1->Clock.FrameRate);
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