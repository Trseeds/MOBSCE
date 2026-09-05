#include "MOBSCE.h"

int IsZero(void* Pointer, int Size)
{
    for(int i = 0; i < Size; i++)
    {
        if(*(unsigned char*)(Pointer+i))
        {
            return(false);
        }
    }
    return(true);
}

void ThrowError(char* Message, char* Thrower, Engine* Engine)
{
    char BoxErrorMessage[STRING_BUFFER_SIZE];
    int EL = Engine->ERROR_LEVEL;
    if(EL != -1)
    {
        if(EL == 0)
        {
            snprintf(BoxErrorMessage,STRING_BUFFER_SIZE,"An error has occurred.");
        }
        if(EL == 1)
        {
            snprintf(BoxErrorMessage,STRING_BUFFER_SIZE,"Error Message: %s",Message);
        }
        if(EL == 2)
        {
            snprintf(BoxErrorMessage,STRING_BUFFER_SIZE,"Error Message: %s\nThrower: %s",Message,Thrower);
        }
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Fatal Error!",BoxErrorMessage,NULL);
    }
    CleanupEngine(Engine);
}

void ThrowWarning(char* Message, char* Thrower, Engine* Engine)
{
    int WL = Engine->WARNING_LEVEL;
    if(WL > 0)
    {
        if(WL == 1)
        {
            printf("\n\nWarning: %s\n\n",Message);
        }
        if(WL == 2)
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

int CompactArrayOfObjects(const void* X, const void* Y)
{
    const byte NX = *(byte*)X;
    const byte NY = *(byte*)Y;
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

int SortSpritesByZ(const void* X, const void* Y)
{
    const byte NX = *(byte*)X;
    const byte NY = *(byte*)Y;
    const int SPR1Z = ((Sprite*)X)->RenderParameters.Position.Z;
    const int SPR2Z = ((Sprite*)Y)->RenderParameters.Position.Z;
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
    IMG_Quit();
    Mix_Quit();
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
        strncpy(Engine->BasePath,Result,STRING_BUFFER_SIZE);

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
        strncpy(Output,Path,STRING_BUFFER_SIZE);
        return(Output);
    }
    return(WARNING_NULL);
}

int KeepTime(Engine* Engine)
{
    if(Engine)
    {
        Clock* C = &Engine->Clock;
        C->PreviousTime = C->CurrentTime;
        C->CurrentTime = SDL_GetPerformanceCounter();
        C->DeltaTime = (double)((C->CurrentTime-C->PreviousTime)/(double)SDL_GetPerformanceFrequency());
        C->TotalTime += (C->CurrentTime-C->PreviousTime);
        C->TotalFrames++;
        C->RealTime = time(NULL);
        C->FrameRate = (double)(1/C->DeltaTime);
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

Engine* InitEngine(char* ConfigFile, char* WindowTitle, char* WindowIconPath, int ERROR_LEVEL, int WARNING_LEVEL)
{
    Engine* NewEngine = (Engine*)OSMemoryAllocate(sizeof(Engine));
    if(!NewEngine)
    {
        ThrowError("Failed to allocate memory!","InitEngine()",NewEngine);
        return(WARNING_NULL);
    }

    NewEngine->ERROR_LEVEL = ERROR_LEVEL;
    NewEngine->WARNING_LEVEL = WARNING_LEVEL;

    ResourceInfo NewResourceInfo;

    GetBasePath(NewEngine);

    char Config[STRING_BUFFER_SIZE];
    strncpy(NewEngine->ConfigPath,GetAssetPath(ConfigFile,Config,NewEngine),STRING_BUFFER_SIZE);
    char Icon[STRING_BUFFER_SIZE];
    strncpy(NewEngine->Video.WindowIconPath,GetAssetPath(WindowIconPath,Icon,NewEngine),STRING_BUFFER_SIZE);
    strncpy(NewEngine->Video.WindowTitle,WindowTitle,STRING_BUFFER_SIZE);

    UpdateConfig(Config,&NewEngine->Config,NewEngine);
    LoadEngineConfig(NewEngine);
    InitSDL(NewEngine);
    InitAudio(NewEngine);
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
    InitVideo(NewEngine);
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
    NewEngine->Running = true;
    return(NewEngine);
}

int RunEngine(Engine* Engine)
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
        Engine->Running = false;
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

