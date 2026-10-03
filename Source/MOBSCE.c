#include "MOBSCE.h"

bool IsZero(void* Pointer, int32 Size)
{
    for(int i = 0; i < Size; i++)
    {
        if(*(bytepointer)(Pointer+i))
        {
            return(false);
        }
    }
    return(true);
}

void ThrowError(Error* Error, Engine* Engine)
{
    char BoxErrorMessage[STRING_BUFFER_SIZE];
    uint8 EL = Engine->ERROR_LEVEL;
    if(EL != ERROR_DISABLE)
    {
        if(EL == ERROR_SHOW)
        {
            snprintf(BoxErrorMessage,STRING_BUFFER_SIZE,"An error has occurred.");
        }
        if(EL == ERROR_SHOW_MESSAGE)
        {
            snprintf(BoxErrorMessage,STRING_BUFFER_SIZE,"Error Message: %s",Error->Message);
        }
        if(EL == ERROR_SHOW_ALL)
        {
            snprintf(BoxErrorMessage,STRING_BUFFER_SIZE,"Error Message: %s\nThrower: %s%s\nDescription: %s",Error->Message,Error->Thrower,Error->Parameters,Error->Description);
        }
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Fatal Error!",BoxErrorMessage,NULL);
    }
    CleanupEngine(Engine);
}

void ThrowWarning(Error* Warning, Engine* Engine)
{
    uint8 WL = Engine->WARNING_LEVEL;
    if(WL > WARNING_DISABLE)
    {
        if(WL == WARNING_SHOW_MESSAGE)
        {
            printf("\n\nWarning: %s\n\n",Warning->Message);
        }
        if(WL == WARNING_SHOW_ALL)
        {
            printf("\n\nWarning: %s\nThrower: %s%s\nDescription: %s\n\n",Warning->Message,Warning->Thrower,Warning->Parameters,Warning->Description);
        }
    }
}

Uint64 GetNewObjectID(Engine* Engine)
{
    return(++Engine->IDCounter); //never return id 0.
}

int QSCompactPointerPool(const void* X, const void* Y)
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

int QSCompactObjectPool(const void* X, const void* Y)
{
    const bool NX = *(bool*)X;
    const bool NY = *(bool*)Y;
    if(NX == false && NY == false)
    {
        return(0);
    }
    if(NX != false && NY != false)
    {
        return(0);
    }
    if(NX == false)
    {
        return(1);
    }
    if(NY == false)
    {
        return(-1);
    }
    return(0);
}

int QSSortSpritesByZ(const void* X, const void* Y)
{
    const bool NX = *(bool*)X;
    const bool NY = *(bool*)Y;
    const int32 SPR1Z = ((Sprite*)X)->RenderParameters.Position.Z;
    const int32 SPR2Z = ((Sprite*)Y)->RenderParameters.Position.Z;
    if(NX == false && NY == false)
    {
        return(0);
    }
    if(NX == false)
    {
        return(1);
    }
    if(NY == false)
    {
        return(-1);
    }
    
    if(SPR1Z == SPR2Z)
    {
        return(0);
    }
    if(SPR1Z < SPR2Z)
    {
        return(-1);
    }
    if(SPR1Z > SPR2Z)
    {
        return(1);
    }
    return(0);
}

bool PoolCanBeShrunk(void* Array, uint64 AllocatedElements, uint64 AllocatedSize)
{
    if((AllocatedSize - AllocatedElements) >= MIN_ALLOCATE)
    {
        return(true);
    }
    return(false);
}

int64 LinearMap(int64 Number, int64 DomainMin, int64 DomainMax, int64 RangeMin, int64 RangeMax)
{
    return(RangeMin + (Number - DomainMin) * (RangeMax - RangeMin) / (DomainMax - DomainMin));
}

void SeedRNG()
{
    srand(time(NULL));
}

int64 GetRandomNumber(int64 Min, int64 Max)
{
    return(Min+(rand()%(Max-Min)));
}

int8 InitSDL(Engine* Engine)
{
    int Result = SDL_Init(SDL_INIT_EVERYTHING);
    if(Result != 0)
    {
        Error Error = {ERROR_SDL_FAILURE,"InitSDL","Failed to start SDL!","\0", "\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(Engine* Engine: 0x%p)",Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_Init failed and returned %d.",Result);
        ThrowError(&Error,Engine);
        return(ERROR_SDL_FAILURE);
    }

    if(Engine->Config.Render)
    {
        //Result = ImgInit(Engine->Video.) bleh
    }

    if(Engine->Config.Audiate)
    {
        Result = Mix_Init(Engine->Audio.Codecs);
        if(Result != Engine->Audio.Codecs || Result == 0)
        {
            Error Warning = {WARNING_SDL_FAILURE,"InitSDL","Failed to start SDL Mixer.","\0", "\0"};
            snprintf(Warning.Parameters,STRING_BUFFER_SIZE,"(Engine* Engine: 0x%p)",Engine);
            snprintf(Warning.Description,STRING_BUFFER_SIZE,"Mix_Init failed and returned %d.",Result);
            ThrowWarning(&Warning,Engine);
            return(WARNING_SDL_FAILURE);
        }
    }

    return(RETURN_SUCCESS);
}

void CleanupSDL()
{
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();
}

int8 GetSDLEvents(Engine* Engine)
{
    if(Engine)
    {
        SDL_Event* EE = Engine->Events;
        memset(EE,0,EVENT_QUEUE_SIZE*sizeof(SDL_Event));
        int i = 0;
        SDL_Event Event;
        while(SDL_PollEvent(&Event))
        {
            if(i < EVENT_QUEUE_SIZE)
            {
                EE[i] = Event;
                i++;
            }
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 GetBasePath(Engine* Engine)
{
    if(Engine)
    {
        char* Result = SDL_GetBasePath();
        if(!Result)
        {
            Error Error = {WARNING_SDL_FAILURE,"GetBasePath","Failed to get base path!","\0", "\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(Engine* Engine: 0x%p)",Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_GetbasePath failed and returned %d.",Result);
            ThrowError(&Error,Engine);
            return(ERROR_SDL_FAILURE);
        }
        char* EBP = Engine->BasePath;
        strncpy(EBP,Result,STRING_BUFFER_SIZE);

        for(int i = 0; i < STRING_BUFFER_SIZE; i++)
        {
            if(EBP[i] == '\\')
            {
                EBP[i] = '/';
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
        strncpy(Output,Path,STRING_BUFFER_SIZE);
        return(Output);
    }
    return(WARNING_NULL);
}

uint8 TickTimers(Engine* Engine) //i've {created {a {monster}}}
{
    if(Engine)
    {
        Timer* T = Engine->Timers;
        int ATM = Engine->Resource.AllocatedTimerMemory;
        for(int i = 0; i < ATM; i++)
        {
            if(T[i].IsUsed)
            {
                Timer* CT = &T[i];
                if(!CT->Expired)
                {
                    if(CT->TimerType == COUNT_TO)
                    {
                        if(CT->Counter >= CT->StopAt)
                        {
                            CT->Expired = true;
                        }
                        else
                        {
                            CT->Counter++;
                        }
                    }
                    if(CT->TimerType == WAIT_UNTIL)
                    {
                        if(CT->UnitType == FRAMES)
                        {
                            if(CT->StopAt <= Engine->Clock.TotalFrames)
                            {
                                CT->Expired = true;
                            }
                        }
                        if(CT->UnitType == REALTIME)
                        {
                            //Under construction
                            ;
                        }
                    }
                }
            }
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int KeepTime(Engine* Engine)
{
    if(Engine)
    {
        Clock* C = &Engine->Clock;
        C->PreviousTime = C->CurrentTime;
        C->CurrentTime = SDL_GetPerformanceCounter();
        C->DeltaTime = (double)((C->CurrentTime-C->PreviousTime)/(double)SDL_GetPerformanceFrequency());
        C->TotalFrames++;
        C->RealTime = time(NULL);
        C->FrameRate = (double)(1/C->DeltaTime);
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

Engine* InitEngine(char* ConfigFile, char* WindowTitle, char* WindowIconPath, int8 ERROR_LEVEL, int8 WARNING_LEVEL, bool Render, bool Audiate, bool ConfigProvided, Config* Config)
{
    Engine* NewEngine = (Engine*)OSMemoryAllocate(sizeof(Engine));
    if(!NewEngine)
    {
        Error Error = {ERROR_MEMORY,"InitEngine","Failed to allocate memory!","\0", "\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(char* ConfigFile: %s, char* WindowTitle: %s, char* WindowIconpath: %s, int ERROR_LEVEL: %d, int WARNING_LEVEL: %d, bool Render: %d, bool Audiate: %d, bool ConfigProvided: %d, Config* Config: 0x%p)",ConfigFile,WindowTitle,WindowIconPath,ERROR_LEVEL,WARNING_LEVEL,Render,Audiate,ConfigProvided,Config);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"OSMemoryAllocate failed and returned: 0x%p",NewEngine);
        ThrowError(&Error,NewEngine);
        return(NULL);
    }

    NewEngine->ERROR_LEVEL = ERROR_LEVEL;
    NewEngine->WARNING_LEVEL = WARNING_LEVEL;
    NewEngine->Config.Render = Render;
    NewEngine->Config.Audiate = Audiate;

    ResourceInfo NewResourceInfo;

    GetBasePath(NewEngine);

    char ConfigF[STRING_BUFFER_SIZE];
    strncpy(NewEngine->ConfigPath,GetAssetPath(ConfigFile,ConfigF,NewEngine),STRING_BUFFER_SIZE);
    char Icon[STRING_BUFFER_SIZE];
    strncpy(NewEngine->Video.WindowIconPath,GetAssetPath(WindowIconPath,Icon,NewEngine),STRING_BUFFER_SIZE);
    strncpy(NewEngine->Video.WindowTitle,WindowTitle,STRING_BUFFER_SIZE);

    if(!ConfigProvided)
    {
        UpdateConfig(ConfigF,&NewEngine->Config,NewEngine);
    }
    else
    {
        if(!Config)
        {
            Error Error = {ERROR_INVALID_PARAMETER,"InitEngine","Config provided is invalid!","\0", "\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(char* ConfigFile: %s, char* WindowTitle: %s, char* WindowIconpath: %s, int ERROR_LEVEL: %d, int WARNING_LEVEL: %d, bool Render: %d, bool Audiate: %d, bool ConfigProvided: %d, Config* Config: 0x%p)",ConfigFile,WindowTitle,WindowIconPath,ERROR_LEVEL,WARNING_LEVEL,Render,Audiate,ConfigProvided,Config);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Config is NULL.",NewEngine);
            ThrowError(&Error,NewEngine);
            return(NULL);
        }
        memcpy(&NewEngine->Config,Config,sizeof(struct Config));
    }
    LoadEngineConfig(NewEngine);
    InitSDL(NewEngine);
    if(NewEngine->Config.Audiate)
    {
        InitAudio(NewEngine);
    }
    //sounds
    NewResourceInfo.Pointer = &NewEngine->Resource.Sounds;
    NewResourceInfo.SizeOfResource = sizeof(Mix_Chunk*);
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedSoundMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfSounds;
    InitResourcePool(NewResourceInfo,NewEngine);
    //music
    NewResourceInfo.Pointer = &NewEngine->Resource.Music;
    NewResourceInfo.SizeOfResource = sizeof(Mix_Music*);
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedMusicMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfMusics;
    InitResourcePool(NewResourceInfo,NewEngine);
    if(NewEngine->Config.Render)
    {
        InitVideo(NewEngine);
    }
    //textures
    NewResourceInfo.Pointer = &NewEngine->Resource.Textures;
    NewResourceInfo.SizeOfResource = sizeof(SDL_Texture*);
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedTextureMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfTextures;
    InitResourcePool(NewResourceInfo, NewEngine);
    //actors
    NewResourceInfo.Pointer = &NewEngine->Actors;
    NewResourceInfo.SizeOfResource = sizeof(Actor);
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedActorMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfActors;
    InitResourcePool(NewResourceInfo,NewEngine);
    //actor references
    NewResourceInfo.Pointer = &NewEngine->ActorReferences;
    NewResourceInfo.SizeOfResource = sizeof(void*);
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedActorReferenceMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfActorReferences;
    InitResourcePool(NewResourceInfo,NewEngine);
    //sprites
    NewResourceInfo.Pointer = &NewEngine->Sprites;
    NewResourceInfo.SizeOfResource = sizeof(Sprite);
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedSpriteMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfSprites;
    InitResourcePool(NewResourceInfo,NewEngine);
    //sprite references
    NewResourceInfo.Pointer = &NewEngine->SpriteReferences;
    NewResourceInfo.SizeOfResource = sizeof(void*);
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedSpriteReferenceMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfSpriteReferences;
    InitResourcePool(NewResourceInfo,NewEngine);
    //timers
    NewResourceInfo.Pointer = &NewEngine->Timers;
    NewResourceInfo.SizeOfResource = sizeof(Timer);
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedTimerMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfTimers;
    InitResourcePool(NewResourceInfo,NewEngine);
    //timer references
    NewResourceInfo.Pointer = &NewEngine->TimerReferences;
    NewResourceInfo.SizeOfResource = sizeof(void*);
    NewResourceInfo.AllocatedResourceMemory = &NewEngine->Resource.AllocatedTimerReferenceMemory;
    NewResourceInfo.NumberOfResources = &NewEngine->Resource.NumberOfTimerReferences;
    InitResourcePool(NewResourceInfo,NewEngine);

    NewEngine->Running = true;
    return(NewEngine);
}

int8 RunEngine(Engine* Engine)
{
    if(Engine)
    {
        GetSDLEvents(Engine);
        GetInput(Engine);
        int ASM = Engine->Resource.AllocatedSpriteMemory;
        int AAM = Engine->Resource.AllocatedActorMemory;
        Sprite* S = Engine->Sprites;
        Actor* A = Engine->Actors;
        for(int i = 0; i < ASM; i++)
        {
            if(S[i].IsUsed)
            {
                if(S[i].Routine != NULL)
                {
                    S[i].Routine(&S[i],Engine);
                }
            }
        }
        for(int i = 0; i < AAM; i++)
        {
            if(A[i].IsUsed)
            {
                if(((Actor)A[i]).Routine)
                {
                    A[i].Routine(&A[i],Engine);
                }
            }
        }
        KeepTime(Engine);
        TickTimers(Engine);
        MixMusicVolume(Engine);
        Render(Engine);
        return(RETURN_SUCCESS);
        //Clock a = Engine->Clock;
        //printf("Current Time: %lu\nPrevious Time: %lu\nDelta Time: %f\nTotal Frames: %lu\nReal Time: %lu\nFramerate: %f\033[6A\r",a.CurrentTime,a.PreviousTime,a.DeltaTime,a.TotalFrames,a.RealTime,a.FrameRate);
        //return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 CleanupEngine(Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        
        //sounds
        ResourceInfo.Pointer = &Engine->Resource.Sounds;
        ResourceInfo.SizeOfResource = sizeof(Mix_Chunk*);
        ResourceInfo.FreeFunction = (void (*)(void*))&Mix_FreeChunk;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSounds;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSoundMemory;
        ResourceInfo.IsPointerArray = true;
        CleanupResourcePool(ResourceInfo,Engine);
        //music
        ResourceInfo.Pointer = &Engine->Resource.Music;
        ResourceInfo.SizeOfResource = sizeof(Mix_Music*);
        ResourceInfo.FreeFunction = (void (*)(void*))&Mix_FreeMusic;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfMusics;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedMusicMemory;
        ResourceInfo.IsPointerArray = true;
        CleanupResourcePool(ResourceInfo,Engine);
        //textures
        ResourceInfo.Pointer = &Engine->Resource.Textures;
        ResourceInfo.SizeOfResource = sizeof(SDL_Texture*);
        ResourceInfo.FreeFunction = (void (*)(void*))&SDL_DestroyTexture;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTextures;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTextureMemory;
        ResourceInfo.IsPointerArray = true;
        CleanupResourcePool(ResourceInfo,Engine);
        //sprites
        ResourceInfo.Pointer = &Engine->Sprites;
        ResourceInfo.SizeOfResource = sizeof(Sprite);
        ResourceInfo.FreeFunction = &SpriteFreeFunction;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSprites;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteMemory;
        ResourceInfo.IsPointerArray = false;
        CleanupResourcePool(ResourceInfo,Engine);
        //sprite references
        ResourceInfo.Pointer = &Engine->SpriteReferences;
        ResourceInfo.SizeOfResource = sizeof(void*);
        ResourceInfo.FreeFunction = NULL;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteReferenceMemory;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSpriteReferences;
        ResourceInfo.IsPointerArray = true;
        CleanupResourcePool(ResourceInfo,Engine);
        //actors
        ResourceInfo.Pointer = &Engine->Actors;
        ResourceInfo.SizeOfResource = sizeof(Actor);
        ResourceInfo.FreeFunction = &ActorFreeFunction;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfActors;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedActorMemory;
        ResourceInfo.IsPointerArray = false;
        CleanupResourcePool(ResourceInfo,Engine);
        //actor references
        ResourceInfo.Pointer = &Engine->ActorReferences;
        ResourceInfo.SizeOfResource = sizeof(void*);
        ResourceInfo.FreeFunction = NULL;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedActorReferenceMemory;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfActorReferences;
        ResourceInfo.IsPointerArray = true;
        CleanupResourcePool(ResourceInfo,Engine);
        CleanupVideo(Engine);
        //timers
        ResourceInfo.Pointer = &Engine->Timers;
        ResourceInfo.SizeOfResource = sizeof(Timer);
        ResourceInfo.FreeFunction = NULL;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTimerMemory;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTimerReferences;
        ResourceInfo.IsPointerArray = false;
        //timer references
        ResourceInfo.Pointer = &Engine->TimerReferences;
        ResourceInfo.SizeOfResource = sizeof(void*);
        ResourceInfo.FreeFunction = NULL;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTimerReferenceMemory;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTimerReferences;
        ResourceInfo.IsPointerArray = true;

        memset(Engine,0,sizeof(struct Engine));
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

