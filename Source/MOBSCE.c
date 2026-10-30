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

int64 LinearMap(int64 Number, int64 DomainMin, int64 DomainMax, int64 RangeMin, int64 RangeMax)
{
    return(RangeMin + (Number - DomainMin) * (RangeMax - RangeMin) / (DomainMax - DomainMin));
}

void SeedRNG(Engine* Engine)
{
    srand(Engine->Clock.RealTimeMS);
    Engine->RandSeeded = true;
}

int64 GetRandomNumber(int64 Min, int64 Max)
{
    return(Min+(rand()%(Max-Min)));
}

int8 InitSDL(Engine* Engine)
{
    const char Parameters[] = "(Engine* Engine: 0x%p)";
    int Result = SDL_Init(SDL_INIT_EVERYTHING);
    if(Result != 0)
    {
        Error Error = {ERROR_SDL_FAILURE,"InitSDL","Failed to start SDL!","\0", "\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_Init failed and returned %d.",Result);
        ThrowError(&Error,Engine);
        return(ERROR_SDL_FAILURE);
    }

    if(Engine->Config.Render)
    {
        Result = IMG_Init(Engine->Video.Formats);
        if(Result != Engine->Video.Formats)
        {
            Error Warning = {WARNING_SDL_FAILURE,"InitSDL","Failed to start SDL IMG.","\0", "\0"};
            snprintf(Warning.Parameters,STRING_BUFFER_SIZE,Parameters,Engine);
            snprintf(Warning.Description,STRING_BUFFER_SIZE,"IMG_Init failed and returned %d.",Result);
            ThrowWarning(&Warning,Engine);
            return(WARNING_SDL_FAILURE);
        }
    }

    if(Engine->Config.Audiate)
    {
        Result = Mix_Init(Engine->Audio.Codecs);
        if(Result != Engine->Audio.Codecs || Result == 0)
        {
            Error Warning = {WARNING_SDL_FAILURE,"InitSDL","Failed to start SDL Mixer.","\0", "\0"};
            snprintf(Warning.Parameters,STRING_BUFFER_SIZE,Parameters,Engine);
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
    const char Parameters[] = "(Engine* Engine: 0x%p)";
    if(Engine)
    {
        char* Result = SDL_GetBasePath();
        if(!Result)
        {
            Error Error = {WARNING_SDL_FAILURE,"GetBasePath","Failed to get base path!","\0", "\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Engine);
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

Engine* InitEngine(char* ConfigFile, char* WindowTitle, char* WindowIconPath, int8 ERROR_LEVEL, int8 WARNING_LEVEL, uint32 AllocationThreshold, bool Render, bool Audiate, Config* Config)
{
    const char Parameters[] = "(char* ConfigFile: %s, char* WindowTitle: %s, char* WindowIconpath: %s, int ERROR_LEVEL: %d, int WARNING_LEVEL: %d, bool Render: %d, bool Audiate: %d, Config* Config: 0x%p)";
    Engine* NewEngine = (Engine*)OSMemoryAllocate(sizeof(Engine));
    NewEngine->RandSeeded = false;
    if(!NewEngine)
    {
        Error Error = {ERROR_MEMORY,"InitEngine","Failed to allocate memory!","\0", "\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,ConfigFile,WindowTitle,WindowIconPath,ERROR_LEVEL,WARNING_LEVEL,Render,Audiate,Config);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"OSMemoryAllocate failed and returned: 0x%p",NewEngine);
        ThrowError(&Error,NewEngine);
        return(NULL);
    }

    NewEngine->ERROR_LEVEL = ERROR_LEVEL;
    NewEngine->WARNING_LEVEL = WARNING_LEVEL;
    NewEngine->Config.Render = Render;
    NewEngine->Config.Audiate = Audiate;
    NewEngine->Resource.AllocationThreshold = AllocationThreshold;

    ResourceInfo NewResourceInfo;

    GetBasePath(NewEngine);

    char ConfigF[STRING_BUFFER_SIZE];
    strncpy(NewEngine->ConfigPath,GetAssetPath(ConfigFile,ConfigF,NewEngine),STRING_BUFFER_SIZE);
    char Icon[STRING_BUFFER_SIZE];
    strncpy(NewEngine->Video.WindowIconPath,GetAssetPath(WindowIconPath,Icon,NewEngine),STRING_BUFFER_SIZE);
    strncpy(NewEngine->Video.WindowTitle,WindowTitle,STRING_BUFFER_SIZE);

    if(!Config)
    {
        UpdateConfig(ConfigF,&NewEngine->Config,NewEngine);
    }
    else
    {
        memcpy(&NewEngine->Config,Config,sizeof(struct Config));
    }
    LoadEngineConfig(NewEngine);
    InitSDL(NewEngine);

    ResourceInfo RSRCSounds = 
    {
        &NewEngine->Sounds,
        sizeof(Sound),
        &SoundFreeFunction,
        &NewEngine->Resource.AllocatedSoundMemory,
        &NewEngine->Resource.NumberOfSounds,
        false
    };
    ResourceInfo RSRCSoundRefs = 
    {
        &NewEngine->SoundReferences,
        sizeof(Sound*),
        NULL,
        &NewEngine->Resource.AllocatedSoundReferenceMemory,
        &NewEngine->Resource.NumberOfSoundReferences,
        true
    };

    ResourceInfo RSRCMusics = 
    {
        &NewEngine->Musics,
        sizeof(Music),
        &MusicFreeFunction,
        &NewEngine->Resource.AllocatedMusicMemory,
        &NewEngine->Resource.NumberOfMusics,
        false
    };
    ResourceInfo RSRCMusicRefs = 
    {
        &NewEngine->MusicReferences,
        sizeof(Music*),
        NULL,
        &NewEngine->Resource.AllocatedMusicReferenceMemory,
        &NewEngine->Resource.NumberOfMusicReferences,
        true
    };

    ResourceInfo RSRCTextures = 
    {
        &NewEngine->Textures,
        sizeof(Texture),
        &TextureFreeFunction,
        &NewEngine->Resource.AllocatedTextureMemory,
        &NewEngine->Resource.NumberOfTextures,
        false
    };
    ResourceInfo RSRCTextureRefs = 
    {
        &NewEngine->TextureReferences,
        sizeof(Texture*),
        NULL,
        &NewEngine->Resource.AllocatedTextureReferenceMemory,
        &NewEngine->Resource.NumberOfTextureReferences,
        true
    };

    ResourceInfo RSRCSprites = 
    {
        &NewEngine->Sprites,
        sizeof(Sprite),
        &SpriteFreeFunction,
        &NewEngine->Resource.AllocatedSpriteMemory,
        &NewEngine->Resource.NumberOfSprites,
        false
    };
    ResourceInfo RSRCSpriteRefs = 
    {
        &NewEngine->SpriteReferences,
        sizeof(Sprite*),
        NULL,
        &NewEngine->Resource.AllocatedSpriteReferenceMemory,
        &NewEngine->Resource.NumberOfSpriteReferences,
        true
    };

    ResourceInfo RSRCActors = 
    {
        &NewEngine->Actors,
        sizeof(Actor),
        &ActorFreeFunction,
        &NewEngine->Resource.AllocatedActorMemory,
        &NewEngine->Resource.NumberOfActors,
        false
    };
    ResourceInfo RSRCActorRefs = 
    {
        &NewEngine->ActorReferences,
        sizeof(Actor*),
        NULL,
        &NewEngine->Resource.AllocatedActorReferenceMemory,
        &NewEngine->Resource.NumberOfActorReferences,
        true
    };

    ResourceInfo RSRCGamepads = 
    {
        &NewEngine->Input.Gamepads,
        sizeof(Gamepad),
        &GamepadFreeFunction,
        &NewEngine->Resource.AllocatedGamepadMemory,
        &NewEngine->Resource.NumberOfGamepads,
        false
    };
    ResourceInfo RSRCGamepadRefs = 
    {
        &NewEngine->Input.GamepadReferences,
        sizeof(Gamepad*),
        NULL,
        &NewEngine->Resource.AllocatedGamepadReferenceMemory,
        &NewEngine->Resource.NumberOfGamepadReferences,
        true
    };

    ResourceInfo RSRCTimers = 
    {
        &NewEngine->Timers,
        sizeof(Timer),
        NULL,
        &NewEngine->Resource.AllocatedTimerMemory,
        &NewEngine->Resource.NumberOfTimerReferences,
        false
    };
    ResourceInfo RSRCTimerRefs = 
    {
        &NewEngine->TimerReferences,
        sizeof(Timer*),
        NULL,
        &NewEngine->Resource.AllocatedTimerReferenceMemory,
        &NewEngine->Resource.NumberOfTimerReferences,
        true
    };

    if(NewEngine->Config.Audiate)
    {
        InitAudio(NewEngine);
        InitResourcePool(RSRCSounds,NewEngine);
        InitResourcePool(RSRCSoundRefs,NewEngine);
        InitResourcePool(RSRCMusics,NewEngine);
        InitResourcePool(RSRCMusicRefs,NewEngine);
    }
    if(NewEngine->Config.Render)
    {
        InitVideo(NewEngine);
        InitResourcePool(RSRCTextures,NewEngine);
        InitResourcePool(RSRCTextureRefs,NewEngine);
    }
    InitResourcePool(RSRCSprites,NewEngine);
    InitResourcePool(RSRCSpriteRefs,NewEngine);
    InitResourcePool(RSRCActors,NewEngine);
    InitResourcePool(RSRCActorRefs,NewEngine);
    InitResourcePool(RSRCTimers,NewEngine);
    InitResourcePool(RSRCTimerRefs,NewEngine);
    InitResourcePool(RSRCGamepads,NewEngine);
    InitResourcePool(RSRCGamepadRefs,NewEngine);

    NewEngine->Running = true;
    return(NewEngine);
}

void TickObjects(Engine* Engine)
{
    for(int i = 0; i < Engine->Resource.AllocatedSpriteMemory; i++)
    {
        Sprite* S = Engine->Sprites;
    
        if(S[i].IsUsed)
        {
            if(S[i].Routine != NULL)
            {
                S[i].Routine(&S[i],Engine);
            }
        }
    }
    for(int i = 0; i < Engine->Resource.AllocatedActorMemory; i++)
    {
        Actor* A = Engine->Actors;
        if(A[i].IsUsed)
        {
            if(((Actor)A[i]).Routine)
            {
                A[i].Routine(&A[i],Engine);
            }
        }
    }
}

int8 RunEngine(Engine* Engine)
{
    if(Engine)
    {
        GetSDLEvents(Engine);
        GetInput(Engine);
        TickObjects(Engine);
        KeepTime(Engine);
        TickTimers(Engine);
        MixMusicVolume(Engine);
        Render(Engine);
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 CleanupEngine(Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo RSRCSounds = 
        {
            &Engine->Sounds,
            sizeof(Sound),
            &SoundFreeFunction,
            &Engine->Resource.AllocatedSoundMemory,
            &Engine->Resource.NumberOfSounds,
            false
        };
        ResourceInfo RSRCSoundRefs = 
        {
            &Engine->SoundReferences,
            sizeof(Sound*),
            NULL,
            &Engine->Resource.AllocatedSoundReferenceMemory,
            &Engine->Resource.NumberOfSoundReferences,
            true
        };

        ResourceInfo RSRCMusics = 
        {
            &Engine->Musics,
            sizeof(Music),
            &MusicFreeFunction,
            &Engine->Resource.AllocatedMusicMemory,
            &Engine->Resource.NumberOfMusics,
            false
        };
        ResourceInfo RSRCMusicRefs = 
        {
            &Engine->MusicReferences,
            sizeof(Music*),
            NULL,
            &Engine->Resource.AllocatedMusicReferenceMemory,
            &Engine->Resource.NumberOfMusicReferences,
            true
        };

        ResourceInfo RSRCTextures = 
        {
            &Engine->Textures,
            sizeof(Texture),
            &TextureFreeFunction,
            &Engine->Resource.AllocatedTextureMemory,
            &Engine->Resource.NumberOfTextures,
            false
        };
        ResourceInfo RSRCTextureRefs = 
        {
            &Engine->TextureReferences,
            sizeof(Texture*),
            NULL,
            &Engine->Resource.AllocatedTextureReferenceMemory,
            &Engine->Resource.NumberOfTextureReferences,
            true
        };

        ResourceInfo RSRCSprites = 
        {
            &Engine->Sprites,
            sizeof(Sprite),
            &SpriteFreeFunction,
            &Engine->Resource.AllocatedSpriteMemory,
            &Engine->Resource.NumberOfSprites,
            false
        };
        ResourceInfo RSRCSpriteRefs = 
        {
            &Engine->SpriteReferences,
            sizeof(Sprite*),
            NULL,
            &Engine->Resource.AllocatedSpriteReferenceMemory,
            &Engine->Resource.NumberOfSpriteReferences,
            true
        };

        ResourceInfo RSRCActors = 
        {
            &Engine->Actors,
            sizeof(Actor),
            &ActorFreeFunction,
            &Engine->Resource.AllocatedActorMemory,
            &Engine->Resource.NumberOfActors,
            false
        };
        ResourceInfo RSRCActorRefs = 
        {
            &Engine->ActorReferences,
            sizeof(Actor*),
            NULL,
            &Engine->Resource.AllocatedActorReferenceMemory,
            &Engine->Resource.NumberOfActorReferences,
            true
        };

        ResourceInfo RSRCGamepads = 
        {
            &Engine->Input.Gamepads,
            sizeof(Gamepad),
            &GamepadFreeFunction,
            &Engine->Resource.AllocatedGamepadMemory,
            &Engine->Resource.NumberOfGamepads,
            false
        };
        ResourceInfo RSRCGamepadRefs = 
        {
            &Engine->Input.GamepadReferences,
            sizeof(Gamepad*),
            NULL,
            &Engine->Resource.AllocatedGamepadReferenceMemory,
            &Engine->Resource.NumberOfGamepadReferences,
            true
        };

        ResourceInfo RSRCTimers = 
        {
            &Engine->Timers,
            sizeof(Timer),
            NULL,
            &Engine->Resource.AllocatedTimerMemory,
            &Engine->Resource.NumberOfTimers,
            false
        };
        ResourceInfo RSRCTimerRefs = 
        {
            &Engine->TimerReferences,
            sizeof(Timer*),
            NULL,
            &Engine->Resource.AllocatedTimerReferenceMemory,
            &Engine->Resource.NumberOfTimerReferences,
            true
        };

        if(Engine->Config.Audiate)
        {
            CleanupResourcePool(RSRCSounds,Engine);
            CleanupResourcePool(RSRCSoundRefs,Engine);
            CleanupResourcePool(RSRCMusics,Engine);
            CleanupResourcePool(RSRCMusicRefs,Engine);
        }
        if(Engine->Config.Render)
        {
            CleanupResourcePool(RSRCTextures,Engine);
            CleanupResourcePool(RSRCTextureRefs,Engine);
        }
        CleanupResourcePool(RSRCActors,Engine);
        CleanupResourcePool(RSRCActorRefs,Engine);
        CleanupResourcePool(RSRCSprites,Engine);
        CleanupResourcePool(RSRCSpriteRefs,Engine);
        CleanupResourcePool(RSRCTimers,Engine);
        CleanupResourcePool(RSRCTimerRefs,Engine);
        CleanupResourcePool(RSRCGamepads,Engine);
        CleanupResourcePool(RSRCGamepadRefs,Engine);
        
        CleanupVideo(Engine);
        memset(Engine,0,sizeof(struct Engine));
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}