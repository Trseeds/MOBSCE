#include "MOBSCE.h"

void ThrowError(char* Message, char* Thrower, Engine* Engine)
{
    char BoxErrorMessage[ERROR_BUFFER_SIZE];
    snprintf(BoxErrorMessage,ERROR_BUFFER_SIZE,"Error Message: %s\nThrower: %s",Message,Thrower);
    CleanupEngine(Engine);
}

void ThrowWarning(char* Message, char* Thrower)
{
    printf("\n\nWarning: %s\nThrower: %s\n\n",Message,Thrower);
}

void AlignArray(void* Array, int ElementSize, int ArraySize)
{
    if(Array == NULL || ElementSize <= 0 || ArraySize <= 0)
    {
        return;
    }
    
    unsigned char* ArrayPointer = (unsigned char*)Array;
    int WriteIndex = 0;
    
    for (int ReadIndex = 0; ReadIndex < ArraySize; ReadIndex++)
    {
        unsigned char* Element = ArrayPointer + (ReadIndex * ElementSize);
        int IsNull = true;
        for (int i = 0; i < ElementSize; i++)
        {
            if (Element[i] != 0)
            {
                IsNull = 0;
                break;
            }
        }
        
        if (!IsNull)
        {
            if (WriteIndex != ReadIndex)
            {
                unsigned char* DestinationPointer = ArrayPointer + (WriteIndex * ElementSize);
                for (int i = 0; i < ElementSize; i++)
                {
                    DestinationPointer[i] = Element[i];
                }
            }
            WriteIndex++;
        }
    }
    
    for (int i = WriteIndex; i < ArraySize; i++)
    {
        char* Element = ArrayPointer + (i * ElementSize);
        for (int j = 0; j < ElementSize; j++)
        {
            Element[j] = 0;
        }
    }
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

int GetRandomNumber(int Max)
{
    return(rand()%Max);
}

int InitSDL()
{
    int Result = SDL_Init(SDL_INIT_EVERYTHING);
    if(Result != 0)
    {
        ThrowError("Failed to start SDL!","InitSDL()",NULL);
        return(1);
    }

    Result = Mix_Init(MIX_INIT_FLAC | MIX_INIT_OGG | MIX_INIT_WAVPACK | MIX_INIT_MP3);
    if(Result != (MIX_INIT_FLAC | MIX_INIT_OGG | MIX_INIT_WAVPACK | MIX_INIT_MP3))
    {
        ThrowWarning("Some or all of the requested audio formats failed to initialize.","InitSDL()");
    }
    if(Result == 0)
    {
        ThrowError("Failed to initialize audio!","InitSDL()",NULL);
        return(2);
    }

    return(0);
}

int GetBasePath(Engine* Engine)
{
    char* Result = SDL_GetBasePath();
    if(!Result)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"GetBasePath(0x%X)",Engine);
        ThrowError("Failed to get base path!",Traceback,Engine);
        return(1);
    }
    strcpy(Engine->BasePath,Result);

    for(int i = 0; i < 1024; i++)
    {
        if(Engine->BasePath[i] == '\\')
        {
            Engine->BasePath[i] = '/';
        }
    }
    return(0);
}

char* GetAssetPath(char* Asset, Engine* Engine)
{
    char Path[1024];
    snprintf(Path,1024,"%s%s",Engine->BasePath,Asset);
    strcpy(Asset,Path);
    return(Asset);
}

void KeepTime(Engine* Engine)
{
    Engine->Clock.PreviousTime = Engine->Clock.CurrentTime;
    Engine->Clock.CurrentTime = SDL_GetPerformanceCounter();
    Engine->Clock.DeltaTime = (double)((Engine->Clock.CurrentTime-Engine->Clock.PreviousTime)/(double)SDL_GetPerformanceFrequency());
    // if(Engine->Clock.DeltaTime > 0.25)
    // {
    //     Engine->Clock.DeltaTime = 0.25;
    // }
    Engine->Clock.TotalTime = (Engine->Clock.CurrentTime-Engine->Clock.PreviousTime);
    Engine->Clock.TotalFrames++;
    Engine->Clock.RealTime = time(NULL);
    Engine->Clock.FrameRate = (double)(1/Engine->Clock.DeltaTime);
}

Engine* InitEngine()
{
    Engine* NewEngine = (Engine*)calloc(1,sizeof(Engine));
    NewEngine->Event = (SDL_Event*)calloc(1,sizeof(SDL_Event));
    if(!NewEngine || !NewEngine->Event)
    {
        ThrowError("Failed to allocate memory!","InitEngine()",NewEngine);
    }

    GetBasePath(NewEngine);
    char Asset[1024] = "Config.ini";
    UpdateEngineConfig(GetAssetPath(Asset,NewEngine),&NewEngine->Config,NewEngine);
    LoadEngineConfig(NewEngine);
    InitSDL();
    InitAudio(NewEngine);
    InitSoundCache(NewEngine);
    InitMusicCache(NewEngine);
    InitVideo(NewEngine);
    InitTextureCache(NewEngine);
    //InitInput(NewEngine);
    InitActorPool(NewEngine);
    InitSpritePool(NewEngine);
    NewEngine->Running = true;
    return(NewEngine);
}

int RunEngine(Engine* Engine)
{
    SDL_PollEvent(Engine->Event);
    GetInput(Engine);
    for(int i = 0; i < Engine->Resource.NumberOfSprites; i++)
    {
        if(Engine->Sprites[i])
        {
            if(Engine->Sprites[i]->Routine)
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
    //Clock a = Engine->Clock;
    //printf("Current Time: %lu\nPrevious Time: %lu\nDelta Time: %f\nTotal Time: %lu\nTotal Frames: %lu\nReal Time: %lu\nFramerate: %f\033[6A\r",a.CurrentTime,a.PreviousTime,a.DeltaTime,a.TotalTime,a.TotalFrames,a.RealTime,a.FrameRate);
}

void CleanupEngine(Engine* Engine)
{
    if(Engine)
    {
        CleanupVideo(Engine);
        CleanupTextureCache(Engine);
        CleanupSoundCache(Engine);
        CleanupMusicCache(Engine);
        CleanupInput(Engine);
        CleanupSpritePool(Engine);
        CleanupActorPool(Engine);
        if(Engine->Event)
        {
            free(Engine->Event);
        }
        Engine->Running = false;
    }
}

