#include "MOBSCE.h"

void ThrowError(char* Message, char* Thrower, Engine* Engine)
{
    char BoxErrorMessage[STRING_BUFFER_SIZE];
    if(Engine->ERROR_LEVEL != -1)
    {
        if(Engine->ERROR_LEVEL == 0)
        {
            snprintf(BoxErrorMessage,STRING_BUFFER_SIZE,"An error has occurred.");
        }
        if(Engine->ERROR_LEVEL == 1)
        {
            snprintf(BoxErrorMessage,STRING_BUFFER_SIZE,"Error Message: %s",Message);
        }
        if(Engine->ERROR_LEVEL == 2)
        {
            snprintf(BoxErrorMessage,STRING_BUFFER_SIZE,"Error Message: %s\nThrower: %s",Message,Thrower);
        }
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Fatal Error!",BoxErrorMessage,NULL);
    }
    CleanupEngine(Engine);
}

void ThrowWarning(char* Message, char* Thrower, Engine* Engine)
{
    if(Engine->WARNING_LEVEL > 0)
    {
        if(Engine->WARNING_LEVEL == 1)
        {
            printf("\n\nWarning: %s\n\n",Message);
        }
        if(Engine->WARNING_LEVEL == 2)
        {
            printf("\n\nWarning: %s\nThrower: %s\n\n",Message,Thrower);
        }
    }
}

Uint64 GetNewObjectID(Engine* Engine)
{
    Engine->IDCounter++;
    return(Engine->IDCounter-1);
}

int CompactArray(const void* X, const void* Y)
{
    const void* NX = *(const void**)X;
    const void* NY = *(const void**)Y;
    if(NX == NULL && NY == NULL)
    {
        return(0);
    }
    if(NX != NULL && NY != NULL)
    {
        return(0);
    }
    if(NX == NULL)
    {
        return(1);
    }
    if(NY == NULL)
    {
        return(-1);
    }
    return(0);
}

int SortSpritesByZ(const void* X, const void* Y)
{
    Sprite* Sprite1 = *(Sprite**)X;
    Sprite* Sprite2 = *(Sprite**)Y;
    if (Sprite1 == NULL && Sprite2 == NULL)
    {
        return(0);
    } 
    if (Sprite1 == NULL)
    {
        return(-1);
    }
    if (Sprite2 == NULL)
    {
        return(1);
    }
    if(Sprite1->RenderParameters.Position.Z == Sprite2->RenderParameters.Position.Z)
    {
        return(0);
    }
    if(Sprite1->RenderParameters.Position.Z < Sprite2->RenderParameters.Position.Z)
    {
        return(-1);
    }
    if(Sprite1->RenderParameters.Position.Z > Sprite2->RenderParameters.Position.Z)
    {
        return(1);
    }
    return(0);
}

int SortWiregonsByZ(const void* X, const void* Y)
{
    Wiregon* Wiregon1 = *(Wiregon**)X;
    Wiregon* Wiregon2 = *(Wiregon**)Y;
    if (Wiregon1 == NULL && Wiregon2 == NULL)
    {
        return(0);
    } 
    if (Wiregon1 == NULL)
    {
        return(-1);
    }
    if (Wiregon2 == NULL)
    {
        return(1);
    }
    if(Wiregon1->Position.Z == Wiregon2->Position.Z)
    {
        return(0);
    }
    if(Wiregon1->Position.Z < Wiregon2->Position.Z)
    {
        return(-1);
    }
    if(Wiregon1->Position.Z > Wiregon2->Position.Z)
    {
        return(1);
    }
    return(0);
}

int PoolCanBeShrunk(void* Array, int AllocatedElements, int AllocatedSize)
{
    if((AllocatedSize - AllocatedElements) >= MIN_ALLOCATE)
    {
        return(true);
    }
    return(false);
}

int LinearMap(int Number, int NumberMax, int RangeMax, int RangeMin)
{
    if(Number < RangeMin)
    {
        Number = RangeMin;
    }
    if(Number > NumberMax)
    {
        Number = NumberMax;
    }
    return((Number * RangeMax) / NumberMax);
}

void SeedRNG()
{
    srand(time(NULL));
}

int GetRandomNumber(int Min, int Max)
{
    return(Min+(rand()%(Max-Min)));
}

int InitSDL(Engine* Engine)
{
    int Result = SDL_Init(SDL_INIT_EVERYTHING);
    if(Result != 0)
    {
        char Traceback[STRING_BUFFER_SIZE];
        snprintf(Traceback,STRING_BUFFER_SIZE,"InitSDL(0x%X)",Engine);
        ThrowError("Failed to start SDL!",Traceback,NULL);
        return(ERROR_SDL_FAILURE);
    }

    Result = Mix_Init(Engine->Audio.Codecs);
    //Result = ImgInit(Engine->Video.) bleh
    if(Result != Engine->Audio.Codecs || Result == 0)
    {
        char Traceback[STRING_BUFFER_SIZE];
        snprintf(Traceback,STRING_BUFFER_SIZE,"InitSDL(0x%X)",Engine);
        ThrowWarning("Some or all of the requested audio formats failed to initialize.",Traceback,Engine);
        return(WARNING_SDL_FAILURE);
    }

    return(RETURN_SUCCESS);
}

void CleanupSDL()
{
    SDL_Quit();
}

int GetSDLEvents(Engine* Engine)
{
    if(Engine)
    {
        memset(Engine->Events,-1,sizeof(Engine->Events));
        int i = 0;
        SDL_Event Event;
        while(SDL_PollEvent(&Event))
        {
            if(i < EVENT_QUEUE_SIZE)
            {
                Engine->Events[i] = Event;
            i++;
            }
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int GetBasePath(Engine* Engine)
{
    if(Engine)
    {
        char* Result = SDL_GetBasePath();
        if(!Result)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"GetBasePath(0x%X)",Engine);
            ThrowError("Failed to get base path!",Traceback,Engine);
            return(ERROR_SDL_FAILURE);
        }
        strcpy(Engine->BasePath,Result);

        for(int i = 0; i < STRING_BUFFER_SIZE; i++)
        {
            if(Engine->BasePath[i] == '\\')
            {
                Engine->BasePath[i] = '/';
            }
        }

        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

char* GetAssetPath(char* Asset, char* Output, Engine* Engine)
{
    if(Engine)
    {
        char Path[STRING_BUFFER_SIZE];
        snprintf(Path,STRING_BUFFER_SIZE,"%s%s",Engine->BasePath,Asset);
        strcpy(Output,Path);
        return(Output);
    }
    return((char*)ERROR_INVALID_ENGINE);
}

int KeepTime(Engine* Engine)
{
    if(Engine)
    {
        Engine->Clock.PreviousTime = Engine->Clock.CurrentTime;
        Engine->Clock.CurrentTime = SDL_GetPerformanceCounter();
        Engine->Clock.DeltaTime = (double)((Engine->Clock.CurrentTime-Engine->Clock.PreviousTime)/(double)SDL_GetPerformanceFrequency());
        Engine->Clock.TotalTime += (Engine->Clock.CurrentTime-Engine->Clock.PreviousTime);
        Engine->Clock.TotalFrames++;
        Engine->Clock.RealTime = time(NULL);
        Engine->Clock.FrameRate = (double)(1/Engine->Clock.DeltaTime);
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

Engine* InitEngine(char* ConfigFile, char* WindowTitle, char* WindowIconPath, int ERROR_LEVEL, int WARNING_LEVEL)
{
    Engine* NewEngine = (Engine*)calloc(1,sizeof(Engine));
    if(!NewEngine)
    {
        ThrowError("Failed to allocate memory!","InitEngine()",NewEngine);
        return((Engine*)ERROR_MEMORY);
    }

    NewEngine->ERROR_LEVEL = ERROR_LEVEL;
    NewEngine->WARNING_LEVEL = WARNING_LEVEL;

    ResourceInfo NewResourceInfo;

    GetBasePath(NewEngine);

    char Config[STRING_BUFFER_SIZE];
    strcpy(NewEngine->ConfigPath,GetAssetPath(ConfigFile,Config,NewEngine));
    char Icon[STRING_BUFFER_SIZE];
    strcpy(NewEngine->Video.WindowIconPath,GetAssetPath(WindowIconPath,Icon,NewEngine));
    strcpy(NewEngine->Video.WindowTitle,WindowTitle);

    UpdateConfig(Config,&NewEngine->Config,NewEngine);
    LoadEngineConfig(NewEngine);
    InitSDL(NewEngine);
    InitAudio(NewEngine);
    //sounds
    NewResourceInfo.Pointer = &NewEngine->Resource.Sounds;
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedSoundMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfSounds;
    InitResourcePool(NewResourceInfo,NewEngine);
    //music
    NewResourceInfo.Pointer = &NewEngine->Resource.Music;
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedMusicMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfMusics;
    InitResourcePool(NewResourceInfo,NewEngine);
    InitVideo(NewEngine);
    //textures
    NewResourceInfo.Pointer = &NewEngine->Resource.Textures;
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedTextureMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfTextures;
    InitResourcePool(NewResourceInfo, NewEngine);
    //actors
    NewResourceInfo.Pointer = &NewEngine->Actors;
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedActorMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfActors;
    InitResourcePool(NewResourceInfo,NewEngine);
    //sprites
    NewResourceInfo.Pointer = &NewEngine->Sprites;
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedSpriteMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfSprites;
    InitResourcePool(NewResourceInfo,NewEngine);
    //Wiregons
    NewResourceInfo.Pointer = &NewEngine->Wiregons;
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedWiregonMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfWiregons;
    InitResourcePool(NewResourceInfo,NewEngine);
    NewEngine->Running = true;
    return(NewEngine);
}

int RunEngine(Engine* Engine)
{
    if(Engine)
    {
        GetSDLEvents(Engine);
        GetInput(Engine);
        for(int i = 0; i < Engine->Resource.NumberOfSprites; i++)
        {
            if(Engine->Sprites[i])
            {
                if(Engine->Sprites[i]->Routine != NULL)
                {
                    Engine->Sprites[i]->Routine(Engine->Sprites[i],Engine);
                }
            }
        }
        for(int i = 0; i < Engine->Resource.NumberOfActors; i++)
        {
            if(Engine->Actors[i])
            {
                if(Engine->Actors[i]->Routine)
                {
                    Engine->Actors[i]->Routine(Engine->Actors[i],Engine);
                }
            }
        }
        KeepTime(Engine);
        MixMusicVolume(Engine);
        Render(Engine);
        return(RETURN_SUCCESS);
        //Clock a = Engine->Clock;
        //printf("Current Time: %lu\nPrevious Time: %lu\nDelta Time: %f\nTotal Time: %lu\nTotal Frames: %lu\nReal Time: %lu\nFramerate: %f\033[6A\r",a.CurrentTime,a.PreviousTime,a.DeltaTime,a.TotalTime,a.TotalFrames,a.RealTime,a.FrameRate);
    }
    return(ERROR_INVALID_ENGINE);
}

int CleanupEngine(Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        
        //sounds
        ResourceInfo.Pointer = &Engine->Resource.Sounds;
        ResourceInfo.FreeFunction = (void (*)(void*))&Mix_FreeChunk;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSounds;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSoundMemory;
        CleanupResourcePool(ResourceInfo,Engine);
        //music
        ResourceInfo.Pointer = &Engine->Resource.Music;
        ResourceInfo.FreeFunction = (void (*)(void*))&Mix_FreeMusic;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfMusics;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedMusicMemory;
        CleanupResourcePool(ResourceInfo,Engine);
        CleanupVideo(Engine);
        //textures
        ResourceInfo.Pointer = &Engine->Resource.Textures;
        ResourceInfo.FreeFunction = (void (*)(void*))&SDL_DestroyTexture;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTextures;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTextureMemory;
        CleanupResourcePool(ResourceInfo,Engine);
        //sprites
        ResourceInfo.Pointer = &Engine->Sprites;
        ResourceInfo.FreeFunction = &free;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSprites;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteMemory;
        CleanupResourcePool(ResourceInfo,Engine);
        //actors
        ResourceInfo.Pointer = &Engine->Actors;
        ResourceInfo.FreeFunction = &free;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfActors;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedActorMemory;
        CleanupResourcePool(ResourceInfo,Engine);
        //Wiregons
        ResourceInfo.Pointer = &Engine->Wiregons;
        ResourceInfo.FreeFunction = &free;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfWiregons;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedWiregonMemory;
        CleanupResourcePool(ResourceInfo,Engine);
        CleanupSDL();
        Engine->Running = false;
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

