#include "MOBSCE.h"

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
}

int64 GetRandomNumber(int64 Min, int64 Max)
{
    return(LinearMap(rand(),0,RAND_MAX,Min,Max));
}

int8 InitSDL(Engine* Engine)
{
    const char Name[] = "InitSDL";
    const char Parameters[] = "(Engine* Engine: 0x%p)";
    int Result = SDL_Init(SDL_INIT_EVERYTHING);
    if(Result != 0)
    {
        Error Error = {ERROR_SDL_FAILURE,"\0","Failed to start SDL!","\0", "\0"};
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
            Error Error = {WARNING_SDL_FAILURE,"\0","Failed to start SDL IMG.","\0", "\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"IMG_Init failed and returned %d.",Result);
            ThrowWarning(&Error,Engine);
            return(WARNING_SDL_FAILURE);
        }
    }

    if(Engine->Config.Audiate)
    {
        Result = Mix_Init(Engine->Audio.Codecs);
        if(Result != Engine->Audio.Codecs || Result == 0)
        {
            Error Error = {WARNING_SDL_FAILURE,"\0","Failed to start SDL Mixer.","\0", "\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_Init failed and returned %d.",Result);
            ThrowWarning(&Error,Engine);
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
    const char Name[] = "GetBasePath";
    const char Parameters[] = "(Engine* Engine: 0x%p)";
    if(Engine)
    {
        char* Result = SDL_GetBasePath();
        if(!Result)
        {
            Error Error = {WARNING_SDL_FAILURE,"\0","Failed to get base path!","\0", "\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
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
    return(NULL);
}

Engine* InitEngine(char* ConfigFile, char* WindowTitle, char* WindowIconPath, int8 ERROR_LEVEL, int8 WARNING_LEVEL, uint32 AllocationThreshold, bool Render, bool Audiate, Config* Config)
{
    const char Name[] = "InitEngine";
    const char Parameters[] = "(char* ConfigFile: %s, char* WindowTitle: %s, char* WindowIconpath: %s, int ERROR_LEVEL: %d, int WARNING_LEVEL: %d, bool Render: %d, bool Audiate: %d, Config* Config: 0x%p)";
    Engine* NewEngine = (Engine*)OSMemoryAllocate(sizeof(Engine));
    if(!NewEngine)
    {
        Error Error = {ERROR_MEMORY,"\0","Failed to allocate memory!","\0", "\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
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

    ResourceInfo S;
    PopulateResourceInfo(RSRC_SND,&S,NewEngine);
    ResourceInfo SR;
    PopulateResourceInfo(RSRC_SND_REF,&SR,NewEngine);
    ResourceInfo M;
    PopulateResourceInfo(RSRC_MUS,&M,NewEngine);
    ResourceInfo MR;
    PopulateResourceInfo(RSRC_MUS_REF,&MR,NewEngine);
    ResourceInfo T;
    PopulateResourceInfo(RSRC_TXTR,&T,NewEngine);
    ResourceInfo TR;
    PopulateResourceInfo(RSRC_TXTR_REF,&TR,NewEngine);
    ResourceInfo SPR;
    PopulateResourceInfo(RSRC_SPR,&SPR,NewEngine);
    ResourceInfo SPRR;
    PopulateResourceInfo(RSRC_SPR_REF,&SPRR,NewEngine);
    ResourceInfo A;
    PopulateResourceInfo(RSRC_ACT,&A,NewEngine);
    ResourceInfo AR;
    PopulateResourceInfo(RSRC_ACT_REF,&AR,NewEngine);
    ResourceInfo G;
    PopulateResourceInfo(RSRC_GP,&G,NewEngine);
    ResourceInfo GR;
    PopulateResourceInfo(RSRC_GP_REF,&GR,NewEngine);
    ResourceInfo TM;
    PopulateResourceInfo(RSRC_TMR,&TM,NewEngine);
    ResourceInfo TMR;
    PopulateResourceInfo(RSRC_TMR_REF,&TMR,NewEngine);

    if(NewEngine->Config.Audiate)
    {
        InitAudio(NewEngine);
        InitResourcePool(S,NewEngine);
        InitResourcePool(SR,NewEngine);
        InitResourcePool(M,NewEngine);
        InitResourcePool(MR,NewEngine);
    }
    if(NewEngine->Config.Render)
    {
        InitVideo(NewEngine);
        InitResourcePool(T,NewEngine);
        InitResourcePool(TR,NewEngine);
    }
    InitResourcePool(SPR,NewEngine);
    InitResourcePool(SPRR,NewEngine);
    InitResourcePool(A,NewEngine);
    InitResourcePool(AR,NewEngine);
    InitResourcePool(TM,NewEngine);
    InitResourcePool(TMR,NewEngine);
    InitResourcePool(G,NewEngine);
    InitResourcePool(GR,NewEngine);

    KeepTime(NewEngine);
    SeedRNG(NewEngine);
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
        ResourceInfo S;
        PopulateResourceInfo(RSRC_SND,&S,Engine);
        ResourceInfo SR;
        PopulateResourceInfo(RSRC_SND_REF,&SR,Engine);
        ResourceInfo M;
        PopulateResourceInfo(RSRC_MUS,&M,Engine);
        ResourceInfo MR;
        PopulateResourceInfo(RSRC_MUS_REF,&MR,Engine);
        ResourceInfo T;
        PopulateResourceInfo(RSRC_TXTR,&T,Engine);
        ResourceInfo TR;
        PopulateResourceInfo(RSRC_TXTR_REF,&TR,Engine);
        ResourceInfo SPR;
        PopulateResourceInfo(RSRC_SPR,&SPR,Engine);
        ResourceInfo SPRR;
        PopulateResourceInfo(RSRC_SPR_REF,&SPRR,Engine);
        ResourceInfo A;
        PopulateResourceInfo(RSRC_ACT,&A,Engine);
        ResourceInfo AR;
        PopulateResourceInfo(RSRC_ACT_REF,&AR,Engine);
        ResourceInfo G;
        PopulateResourceInfo(RSRC_GP,&G,Engine);
        ResourceInfo GR;
        PopulateResourceInfo(RSRC_GP_REF,&GR,Engine);
        ResourceInfo TM;
        PopulateResourceInfo(RSRC_TMR,&TM,Engine);
        ResourceInfo TMR;
        PopulateResourceInfo(RSRC_TMR_REF,&TMR,Engine);

        if(Engine->Config.Audiate)
        {
            CleanupResourcePool(S,Engine);
            CleanupResourcePool(SR,Engine);
            CleanupResourcePool(M,Engine);
            CleanupResourcePool(MR,Engine);
        }
        if(Engine->Config.Render)
        {
            CleanupResourcePool(T,Engine);
            CleanupResourcePool(TR,Engine);
        }
        CleanupResourcePool(A,Engine);
        CleanupResourcePool(AR,Engine);
        CleanupResourcePool(SPR,Engine);
        CleanupResourcePool(SPRR,Engine);
        CleanupResourcePool(TM,Engine);
        CleanupResourcePool(TMR,Engine);
        CleanupResourcePool(G,Engine);
        CleanupResourcePool(GR,Engine);
        
        CleanupVideo(Engine);
        memset(Engine,0,sizeof(struct Engine));
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}