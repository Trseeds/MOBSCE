#include "MOBSCE.h"

void CrazyColors(Sprite* Sprite, Engine* Engine)
{
    int RedMod = GetRandomNumber(0,16) - 8;
    int GreenMod = GetRandomNumber(0,16) - 8;
    int BlueMod = GetRandomNumber(0,16) - 8;
    Sprite->RenderParameters.Tint.X += RedMod%255;
    Sprite->RenderParameters.Tint.Y += GreenMod%255;
    Sprite->RenderParameters.Tint.Z += BlueMod%255;
}

void RainbowColors(Sprite* Sprite, Engine* Engine)
{
    Vector3* oc = &Sprite->RenderParameters.Tint;
    static double Speed = 0;
    Speed += 0.05;
    oc->X = (sin(Speed) * 127) + 128;
    oc->Y = (sin(Speed + 2.094) * 127) + 128;
    oc->Z = (sin(Speed + 4.188) * 127) + 128;
}

void CursorFunction(Actor* Actor, Engine* Engine)
{
    if(!Engine->Input.GamepadIsConnected)
    {
        FollowMouse(Actor,Engine);
    }
    else
    {
        Actor->Position.X += Engine->Input.GamepadSticks[GP_STK_LEFT_X] * 5;
        Actor->Position.Y += Engine->Input.GamepadSticks[GP_STK_LEFT_Y] * 5;
    }
    if(Engine->Input.MouseDown[MB_LEFT] || Engine->Input.GamepadTriggers[GP_TRGR_RIGHT] > 0.1)
    {
        RainbowColors(Actor->CustomData.Sprite,Engine);
    }
    if((Engine->Input.MouseUp[MB_LEFT] || Engine->Input.GamepadTriggersUp[GP_TRGR_RIGHT]))
    {
        RumbleGamepad(25,250,Engine);
    }
    if((Engine->Input.MouseUp[MB_RIGHT] || Engine->Input.GamepadTriggersUp[GP_TRGR_LEFT]) && Actor->CustomData.Sprite->RenderParameters.Tint.X == 0)
    {
        Actor->CustomData.Sprite->RenderParameters.Tint.X = 255;
    }
    if(Actor->CustomData.Sprite->RenderParameters.Tint.X > 1)
    {
        Actor->CustomData.Sprite->RenderParameters.Tint.X -= 4;
    }
    if(Actor->CustomData.Sprite->RenderParameters.Tint.X < 1)
    {
        Actor->CustomData.Sprite->RenderParameters.Tint.X = 0;
    }
}

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

void MouseDrag(Actor* ActorA, Actor* Cursor, Engine* Engine)
{
    ActorA->Position.X = Cursor->Position.X - (ActorA->Dimensions.X/2);
    ActorA->Position.Y = Cursor->Position.Y - (ActorA->Dimensions.Y/2);
}

void TestActorFunction(Actor* ActorA, Engine* Engine)
{
    Actor* Cursor = GetActorByName("Mouse Cursor",Engine);
    if(!ActorA->CustomData.Dragged)
    {
        ActorA->CustomData.MousedOver = false;
    }
    if(!Engine->Input.MouseDown[MB_LEFT])
    {
        ActorA->CustomData.Dragged = false;
    }
    if(Cursor->Position.X > ActorA->Position.X && Cursor->Position.X < ActorA->Position.X+ActorA->Dimensions.X)
    {
        if(Cursor->Position.Y > ActorA->Position.Y && Cursor->Position.Y < ActorA->Position.Y+ActorA->Dimensions.Y)
        {
            if(Engine->Input.MouseDown[MB_LEFT] || Engine->Input.GamepadTriggers[GP_TRGR_RIGHT] >= 0.9)
            {
                ActorA->CustomData.Dragged = true;
            }
            if(Engine->Input.MouseUp[MB_RIGHT] || Engine->Input.GamepadTriggersUp[GP_TRGR_LEFT])
            {
                DestroySprite(ActorA->CustomData.Sprite,Engine);
                DestroyActor(ActorA,Engine);
                PlaySound(0,ActorA->Voice,100,ActorA->Position.X,Engine);
                RumbleGamepad(100,250,Engine);
            }
            ActorA->CustomData.MousedOver = true;
        }
    }

    if(ActorA->CustomData.Dragged)
    {
        MouseDrag(ActorA, Cursor, Engine);
    }
    else
    {
        ScreenCrawl(ActorA, Engine);
    }
}

void AlignSpriteToActor(Sprite* Sprite, Engine* Engine)
{
    if(Sprite->Actor)
    {
        Sprite->RenderParameters.Position.X = Sprite->Actor->Position.X;
        Sprite->RenderParameters.Position.Y = Sprite->Actor->Position.Y;
    }
}

void TestSpriteFunction(Sprite* Sprite, Engine* Engine)
{
    CrazyColors(Sprite,Engine);
    AlignSpriteToActor(Sprite,Engine);
}

void CreateTestObject(Engine* Engine)
{
    Vector3 SpritePosition;
    Vector2 ActorPosition;
    Vector4 Origin;
    Vector2 Dimensions;
    CustomSpriteData Dummy;
    CustomActorData Data;
    ActorPosition.X = GetRandomNumber(0,Engine->Video.LogicalDimensions.X); ActorPosition.Y = GetRandomNumber(0,Engine->Video.LogicalDimensions.Y);
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 2;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 16; Origin.W = 16;
    Dimensions.X = GetRandomNumber(8,32); Dimensions.Y = GetRandomNumber(8,32);
    Actor* TestActor = CreateActor("Test Actor",ActorPosition,Dimensions,0,Data,&TestActorFunction,Engine);
    Sprite* TestSprite = CreateSprite("Test Sprite",SpritePosition,Origin,Dimensions,TXTR_PLAYER,Dummy,TestActor,&TestSpriteFunction,Engine);
    Data.Sprite = TestSprite;
    TestActor->CustomData = Data;
    TestSprite->RenderParameters.Angle = GetRandomNumber(0,360);
    TestSprite->RenderParameters.Flip = GetRandomNumber(0,2);
    TestSprite->RenderParameters.Tint.X = GetRandomNumber(0,255);
    TestSprite->RenderParameters.Tint.Y = GetRandomNumber(0,255);
    TestSprite->RenderParameters.Tint.Z = GetRandomNumber(0,255);
}

void CacheTextures(Engine* Engine)
{
    char BG[STRING_BUFFER_SIZE] = "Assets/Images/Backgrounds/TestBG.png";
    char Player[STRING_BUFFER_SIZE] = "Assets/Images/Sprites/Player.png";
    char Cursor[STRING_BUFFER_SIZE] = "Assets/Images/Sprites/Cursor.png";
    CacheTexture(GetAssetPath(BG,Engine),Engine);
    CacheTexture(GetAssetPath(Player,Engine),Engine);
    CacheTexture(GetAssetPath(Cursor,Engine),Engine);
}

void ReassignSpriteTextures(Engine* Engine)
{
    for(int i = 0; i < Engine->Resource.NumberOfSprites; i++)
    {
        Engine->Sprites[i]->RenderParameters.Texture = Engine->Resource.Textures[Engine->Sprites[i]->TextureID];
    }
}

void InitGame(Engine* Engine)
{
    CacheTextures(Engine);
    char Cough[STRING_BUFFER_SIZE] = "Assets/Sounds/Cough.wav";
    CacheSound(GetAssetPath(Cough,Engine),Engine);

    Vector3 SpritePosition;
    Vector2 ActorPosition;
    Vector4 Origin;
    Vector2 Dimensions;
    CustomSpriteData SpriteData;
    CustomActorData ActorData;
    ActorPosition.X = 0; ActorPosition.Y = 0;
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 0;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 640; Origin.W = 480;
    Dimensions.X = 640; Dimensions.Y = 480;
    Sprite* BGSprite = CreateSprite("BG",SpritePosition,Origin,Dimensions,TXTR_BG,SpriteData,NULL,&RainbowColors,Engine);
    BGSprite->RenderParameters.Tint.X = 64;
    BGSprite->RenderParameters.Tint.Y = 128;
    BGSprite->RenderParameters.Tint.Z = 192;
    SpritePosition.Z = 1000;
    Dimensions.X = 64; Dimensions.Y = 64;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 64; Origin.W = 64;
    Actor* CursorActor = CreateActor("Mouse Cursor",ActorPosition,Dimensions,0,ActorData,&CursorFunction,Engine);
    Sprite* CursorSprite = CreateSprite("Mouse Cursor",SpritePosition,Origin,Dimensions,TXTR_CURSOR,SpriteData,CursorActor,&AlignSpriteToActor,Engine);
    CursorActor->CustomData.Sprite = CursorSprite;
    CursorSprite->RenderParameters.Tint.X = 0;
    CursorSprite->RenderParameters.Tint.Y = 0;
    CursorSprite->RenderParameters.Tint.Z = 0;

}

int main(int argc, char* argv[])
{
    Engine* Engine1 = InitEngine("Config.ini","MOBSCE Example","Assets/Images/Icon.png");
    InitGame(Engine1);
    while(Engine1->Running)
    {
        if(Engine1->Input.KeysUp[K_R] || Engine1->Input.GamepadButtonsUp[GP_FB_START])
        {
            CleanupEngine(Engine1);
            Engine1 = InitEngine("Config.ini","MOBSCE Example","Assets/Images/Icon.png");
            InitGame(Engine1);
        }
        if((Engine1->Input.KeysDown[K_S] && !Engine1->Input.KeysDown[K_RCTRL]) || (Engine1->Input.GamepadButtonsDown[GP_FB_BOTTOM] && !Engine1->Input.GamepadButtonsDown[GP_FB_TOP]))
        {
            CreateTestObject(Engine1);
        }
        if(Engine1->Input.KeysDown[K_K] || Engine1->Input.GamepadButtonsDown[GP_FB_LEFT])
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
        if((Engine1->Input.KeysDown[K_RCTRL] && Engine1->Input.KeysUp[K_S]) || (Engine1->Input.GamepadButtonsDown[GP_FB_TOP] && Engine1->Input.GamepadButtonsUp[GP_FB_BOTTOM]))
        {
            for(int i = 0; i < 10000; i++)
            {
                CreateTestObject(Engine1);
            }
        }
        if((Engine1->Input.KeysDown[K_LALT] || Engine1->Input.KeysDown[K_RALT]) && Engine1->Input.KeysUp[K_RETURN])
        {
            if(Engine1->Video.WindowFlags == SDL_WINDOW_SHOWN)
            {
                Engine1->Video.WindowFlags = SDL_WINDOW_FULLSCREEN_DESKTOP;
                Engine1->Video.LogicalDimensions.X = 1920;
                Engine1->Video.LogicalDimensions.Y = 1080;
                RestartVideo(Engine1);
                CacheTextures(Engine1);
                ReassignSpriteTextures(Engine1);
            }
            else
            {
                Engine1->Video.LogicalDimensions.X = 640;
                Engine1->Video.LogicalDimensions.Y = 480;
                Engine1->Video.WindowFlags = SDL_WINDOW_SHOWN;
                RestartVideo(Engine1);
                CacheTextures(Engine1);
                ReassignSpriteTextures(Engine1);
            }
        }
        RunEngine(Engine1);
        for(int i = 0; i < EVENT_QUEUE_SIZE; i++)
        {
            if(Engine1->Events[i].type == SDL_QUIT || Engine1->Input.KeysUp[K_ESCAPE] || Engine1->Input.GamepadButtonsUp[GP_FB_RIGHT])
            {
                CleanupEngine(Engine1);
                free(Engine1);
                return(0);
            }
        }
    }
}


//docs example code
// #include "MOBSCE.h"

// int main(int argc, char* argv[])
// {
//     Engine* MyEngine = InitEngine("Config.ini");
//     char TestBGPath[STRING_BUFFER_SIZE] = "Assets/Images/Backgrounds/TestBG.bmp";
//     CacheTexture(GetAssetPath(TestBGPath,MyEngine),MyEngine);
//     Vector3 Position = {0,0,0};
//     Vector4 Origin = {0,0,640,480};
//     Vector2 Dimensions = {MyEngine->Video.LogicalDimensions.X,MyEngine->Video.LogicalDimensions.Y};
//     CustomSpriteData DummyData;
//     CreateSprite(
//         "Background",
//         Position,
//         Origin,
//         Dimensions,
//         0,
//         1,
//         DummyData,
//         NULL,
//         NULL,
//         MyEngine);
//     while(MyEngine->Running)
//     {
//         RunEngine(MyEngine);
//         for(int i = 0; i < EVENT_QUEUE_SIZE; i++)
//         {
//             if(MyEngine->Events[i].type == SDL_QUIT)
//             {
//                 CleanupEngine(MyEngine);
//                 free(MyEngine);
//                 return(0);
//             }
//         }
//     }
// }