#include "MOBSCE.h"

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
    if(Engine->Input.MouseUp[MB_LEFT] || Engine->Input.GamepadTriggersUp[GP_TRGR_RIGHT])
    {
        PlaySound(0,Actor->Voice,100,Actor->Position.X,Engine);
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
            if(Engine->Input.MouseDown[MB_RIGHT] || Engine->Input.GamepadTriggers[GP_TRGR_LEFT] >= 0.9)
            {
                DestroySprite(ActorA->CustomData.Sprite,Engine);
                DestroyActor(ActorA,Engine);
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
    CustomActorData Data;
    ActorPosition.X = GetRandomNumber(Engine->Video.LogicalDimensions.X); ActorPosition.Y = GetRandomNumber(Engine->Video.LogicalDimensions.Y);
    SpritePosition.X = 0; SpritePosition.Y = 0; SpritePosition.Z = 2;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 16; Origin.W = 16;
    Dimensions.X = 16; Dimensions.Y = 16;
    Actor* TestActor = CreateActor("Test Actor",ActorPosition,Dimensions,0,Data,&TestActorFunction,Engine);
    Sprite* TestSprite = CreateSprite("Test Sprite",SpritePosition,Origin,Dimensions,TXTR_PLAYER,true,Dummy,TestActor,&AlignSpriteToActor,Engine);
    Data.Sprite = TestSprite;
    TestActor->CustomData = Data;
}

void InitGame(Engine* Engine)
{
    char BG[STRING_BUFFER_SIZE] = "Assets/Images/Backgrounds/TestBG.bmp";
    char Player[STRING_BUFFER_SIZE] = "Assets/Images/Sprites/Player.bmp";
    char Cursor[STRING_BUFFER_SIZE] = "Assets/Images/Sprites/Cursor.bmp";
    char Cough[STRING_BUFFER_SIZE] = "Assets/Sounds/Cough.wav";
    CacheTexture(GetAssetPath(BG,Engine),Engine);
    CacheTexture(GetAssetPath(Player,Engine),Engine);
    CacheTexture(GetAssetPath(Cursor,Engine),Engine);
    CacheSound(GetAssetPath(Cough,Engine),Engine);

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
    CreateSprite("BG",SpritePosition,Origin,Dimensions,TXTR_BG,true,Dummy,NULL,NULL,Engine);
    SpritePosition.Z = 1000;
    Dimensions.X = 64; Dimensions.Y = 64;
    Origin.X = 0; Origin.Y = 0; Origin.Z = 64; Origin.W = 64;
    Actor* CursorActor = CreateActor("Mouse Cursor",ActorPosition,Dimensions,0,Dummy2,&CursorFunction,Engine);
    CreateSprite("Mouse Cursor",SpritePosition,Origin,Dimensions,TXTR_CURSOR,true,Dummy,CursorActor,&AlignSpriteToActor,Engine);
}

int main(int argc, char* argv[])
{
    Engine* Engine1 = InitEngine("Config.ini");
    InitGame(Engine1);
    while(Engine1->Running)
    {
        if(Engine1->Input.KeysUp[K_R] || Engine1->Input.GamepadButtonsUp[GP_FB_START])
        {
            CleanupEngine(Engine1);
            Engine1 = InitEngine("Config.ini");
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
        RunEngine(Engine1);
        Render(Engine1);
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