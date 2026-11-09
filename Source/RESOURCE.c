#include "MOBSCE.h"

#ifdef _WIN32
    #include <windows.h>
    void* OSMemoryAllocate(size_t Size)
    {
        void* Pointer = VirtualAlloc(NULL,Size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
        if(Pointer)
        {
            memset(Pointer,0,Size);
        }
        return(Pointer);
    }
    void OSMemoryFree(void* Pointer, size_t Size)
    {
        //memset(Pointer,0,Size);
        VirtualFree(Pointer,0,MEM_RELEASE);
    }
#elif defined(__unix__)
    #include <sys/mman.h>
    void* OSMemoryAllocate(size_t Size)
    {
        void* Pointer = mmap(NULL,Size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
        if(Pointer == MAP_FAILED)
        {
            return(NULL);
        }
        memset(Pointer,0,Size);
        return(Pointer);
    }
    void OSMemoryFree(void* Pointer, size_t Size)
    {
        //memset(Pointer,0,Size);
        munmap(Pointer,Size);
    }
#endif

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

bool PoolCanBeShrunk(void* Array, uint32 AllocatedElements, uint32 AllocatedSize, Engine* Engine)
{
    return((AllocatedSize - AllocatedElements) >= MIN_ALLOCATE && (AllocatedSize - MIN_ALLOCATE) > MIN_ALLOCATE);
}

void* FindOpenResourceSpace(void* Pool, uint32 PoolSize, size_t Size)
{
    for(uint32 i = 0; i < PoolSize; i += Size)
    {
        if(!*(bytepointer)(Pool+i))
        {
            return(Pool+i);
        }
    }
    return(NULL);
}

uint32 FindOpenReferenceSpace(void* Pool, uint32 AllocatedReferenceMemory)
{
    void** RealPool = (void**)Pool;
    for(uint32 i = 0; i < AllocatedReferenceMemory; i++)
    {
        if(!RealPool[i])
        {
            return(i);
        }
    }
    return(SENTINEL_OBJECT);
}

void RebuildReferences(void* ReferencePool, void* Pool, size_t SizeOfObject, uint32 AllocatedResources)
{
    Object** RP = (Object**)ReferencePool;
    for(uint32 i = 0; i < AllocatedResources; i++)
    {
        Object* O = Pool+((uint64)i*(uint64)SizeOfObject);
        if(O->IsUsed)
        {
            RP[O->ReferenceIndex] = O;
        }
    }
}

int8 InitResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    const char Name[] = "InitResourcePool";
    const char Parameters[] = "(ResourceInfo ResourceInfo: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources)
        {
            *(void**)ResourceInfo.Pointer = OSMemoryAllocate(MIN_ALLOCATE*ResourceInfo.SizeOfResource);
            if(!*(void**)ResourceInfo.Pointer)
            {
                Error Error = {ERROR_MEMORY,"\0","Failed to allocate memory!","\0","\0"};
                snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,&ResourceInfo,Engine);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"OSMemoryAllocate failed and returned 0x%p",*(void**)ResourceInfo.Pointer);
                ThrowError(&Error,Engine);
                return(ERROR_MEMORY);
            }

            *(uint32*)ResourceInfo.AllocatedResourceMemory = MIN_ALLOCATE;
            *(uint32*)ResourceInfo.NumberOfResources = 0;
            return(RETURN_SUCCESS);
        }       
    }
    return(ERROR_INVALID_ENGINE);
}

int8 ExtendResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    const char Name[] = "ExtendResourcePool";
    const char Parameters[] = "(ResourceInfo ResourceInfo: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources)
        {
            void** OldPtr = *(void**)ResourceInfo.Pointer;
            uint32 OldSize = *(uint32*)ResourceInfo.AllocatedResourceMemory*ResourceInfo.SizeOfResource;
            uint32 NewSize = (*(uint32*)ResourceInfo.AllocatedResourceMemory+MIN_ALLOCATE)*ResourceInfo.SizeOfResource;
            *(void**)ResourceInfo.Pointer = OSMemoryAllocate(NewSize);
            if(!*(void**)ResourceInfo.Pointer)
            {
                Error Error = {ERROR_MEMORY,"\0","Failed to allocate memory!","\0","\0"};
                snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,&ResourceInfo,Engine);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"OSMemoryAllocate failed and returned 0x%p",*(void**)ResourceInfo.Pointer);
                ThrowError(&Error,Engine);
                return(ERROR_MEMORY);
            }

            memcpy(*(void**)ResourceInfo.Pointer,OldPtr,OldSize);
            OSMemoryFree(OldPtr,OldSize);
            *(uint32*)ResourceInfo.AllocatedResourceMemory += MIN_ALLOCATE;

            return(RETURN_SUCCESS);
        }
        Error Error = {ERROR_INVALID_PARAMETER,"\0","Invalid resource info!","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,&ResourceInfo,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"Caller passed in an invalid ResourceInfo struct. If this was not you, file a bug report. If it was you, stop.");
        ThrowError(&Error,Engine);
        return(ERROR_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 ShrinkResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    const char Name[] = "ShrinkResourcePool";
    const char Parameters[] = "(ResourceInfo ResourceInfo: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources)
        {
            void** OldPtr = *(void**)ResourceInfo.Pointer;
            uint32 OldSize = *(uint32*)ResourceInfo.AllocatedResourceMemory*ResourceInfo.SizeOfResource;
            uint32 NewSize = (*(uint32*)ResourceInfo.AllocatedResourceMemory-MIN_ALLOCATE)*ResourceInfo.SizeOfResource;
            *(void**)ResourceInfo.Pointer = OSMemoryAllocate(NewSize);
            if(!*(void**)ResourceInfo.Pointer)
            {
                Error Error = {ERROR_MEMORY,"\0","Failed to allocate memory!","\0","\0"};
                snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,&ResourceInfo,Engine);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"OSMemoryAllocate failed and returned 0x%p",*(void**)ResourceInfo.Pointer);
                ThrowError(&Error,Engine);
                return(ERROR_MEMORY);
            }

            memcpy(*(void**)ResourceInfo.Pointer,OldPtr,NewSize);
            OSMemoryFree(OldPtr,OldSize);
            *(uint32*)ResourceInfo.AllocatedResourceMemory -= MIN_ALLOCATE;

            return(RETURN_SUCCESS);
        }
        Error Error = {ERROR_INVALID_PARAMETER,"\0","Invalid resource info!","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,&ResourceInfo,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"Caller passed in an invalid ResourceInfo struct. If this was not you, file a bug report. If it was you, stop.");
        ThrowError(&Error,Engine);
        return(ERROR_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 CleanupResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    const char Name[] = "CleanupResourcePool";
    const char Parameters[] = "(ResourceInfo ResourceInfo: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources)
        {
            bytepointer Pool = *(void**)ResourceInfo.Pointer;
            size_t SizeOfResource = ResourceInfo.SizeOfResource;
            size_t Size = *(uint32*)ResourceInfo.AllocatedResourceMemory*SizeOfResource;
            

            for(size_t i = 0; i < Size; i += SizeOfResource)
            {
                void* Element = Pool+i;
                if(!ResourceInfo.IsPointerArray)
                {
                    if(ResourceInfo.FreeFunction)
                    {
                        if(((Object*)Element)->IsUsed)
                        {
                            ResourceInfo.FreeFunction(Element);
                        }
                    }
                }
                else
                {
                    if(ResourceInfo.FreeFunction)
                    {
                        ResourceInfo.FreeFunction(*(void**)Element);
                    }
                }
            }
            OSMemoryFree(Pool,Size);
            return(RETURN_SUCCESS);
        }
        Error Error = {ERROR_INVALID_PARAMETER,"\0","Invalid resource info!","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,&ResourceInfo,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"Caller passed in an invalid ResourceInfo struct. If this was not you, file a bug report. If it was you, stop.");
        ThrowError(&Error,Engine);
        return(ERROR_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

//populate ============================================================================================================================================================================================================
ResourceInfo* PopulateResourceInfo(uint8 ResourceType, ResourceInfo* ResourceInfo, Engine* Engine)
{
    switch(ResourceType)
    {
        case RSRC_SND:
        {
            ResourceInfo->Pointer = &Engine->Sounds;
            ResourceInfo->SizeOfResource = sizeof(struct Sound);
            ResourceInfo->FreeFunction = &SoundFreeFunction;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedSoundMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfSounds;
            ResourceInfo->IsPointerArray = false;
            break;
        }
        case RSRC_SND_REF:
        {
            ResourceInfo->Pointer = &Engine->SoundReferences;
            ResourceInfo->SizeOfResource = sizeof(struct Sound*);
            ResourceInfo->FreeFunction = NULL;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedSoundReferenceMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfSoundReferences;
            ResourceInfo->IsPointerArray = true;
            break;
        }

        case RSRC_MUS:
        {
            ResourceInfo->Pointer = &Engine->Musics;
            ResourceInfo->SizeOfResource = sizeof(struct Music);
            ResourceInfo->FreeFunction = &MusicFreeFunction;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedMusicMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfMusics;
            ResourceInfo->IsPointerArray = false;
            break;
        }
        case RSRC_MUS_REF:
        {
            ResourceInfo->Pointer = &Engine->MusicReferences;
            ResourceInfo->SizeOfResource = sizeof(struct Music*);
            ResourceInfo->FreeFunction = NULL;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedMusicReferenceMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfMusicReferences;
            ResourceInfo->IsPointerArray = true;
            break;
        }

        case RSRC_TXTR:
        {
            ResourceInfo->Pointer = &Engine->Textures;
            ResourceInfo->SizeOfResource = sizeof(struct Texture);
            ResourceInfo->FreeFunction = &TextureFreeFunction;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedTextureMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfTextures;
            ResourceInfo->IsPointerArray = false;
            break;
        }
        case RSRC_TXTR_REF:
        {
            ResourceInfo->Pointer = &Engine->TextureReferences;
            ResourceInfo->SizeOfResource = sizeof(struct Texture*);
            ResourceInfo->FreeFunction = NULL;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedTextureReferenceMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfTextureReferences;
            ResourceInfo->IsPointerArray = true;
            break;
        }

        case RSRC_SPR:
        {
            ResourceInfo->Pointer = &Engine->Sprites;
            ResourceInfo->SizeOfResource = sizeof(struct Sprite);
            ResourceInfo->FreeFunction = &SpriteFreeFunction;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfSprites;
            ResourceInfo->IsPointerArray = false;
            break;
        }
        case RSRC_SPR_REF:
        {
            ResourceInfo->Pointer = &Engine->SpriteReferences;
            ResourceInfo->SizeOfResource = sizeof(struct Sprite*);
            ResourceInfo->FreeFunction = NULL;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteReferenceMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfSpriteReferences;
            ResourceInfo->IsPointerArray = true;
            break;
        }

        case RSRC_ACT:
        {
            ResourceInfo->Pointer = &Engine->Actors;
            ResourceInfo->SizeOfResource = sizeof(struct Actor);
            ResourceInfo->FreeFunction = &ActorFreeFunction;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedActorMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfActors;
            ResourceInfo->IsPointerArray = false;
            break;
        }
        case RSRC_ACT_REF:
        {
            ResourceInfo->Pointer = &Engine->ActorReferences;
            ResourceInfo->SizeOfResource = sizeof(struct Actor*);
            ResourceInfo->FreeFunction = NULL;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedActorReferenceMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfActorReferences;
            ResourceInfo->IsPointerArray = true;
            break;
        }

        case RSRC_TMR:
        {
            ResourceInfo->Pointer = &Engine->Timers;
            ResourceInfo->SizeOfResource = sizeof(struct Timer);
            ResourceInfo->FreeFunction = NULL;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedTimerMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfTimers;
            ResourceInfo->IsPointerArray = false;
            break;
        }
        case RSRC_TMR_REF:
        {
            ResourceInfo->Pointer = &Engine->TimerReferences;
            ResourceInfo->SizeOfResource = sizeof(struct Timer*);
            ResourceInfo->FreeFunction = NULL;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedTimerReferenceMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfTimerReferences;
            ResourceInfo->IsPointerArray = true;
            break;
        }

        case RSRC_GP:
        {
            ResourceInfo->Pointer = &Engine->Input.Gamepads;
            ResourceInfo->SizeOfResource = sizeof(struct Gamepad);
            ResourceInfo->FreeFunction = &GamepadFreeFunction;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedGamepadMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfGamepads;
            ResourceInfo->IsPointerArray = false;
            break;
        }
        case RSRC_GP_REF:
        {
            ResourceInfo->Pointer = &Engine->Input.GamepadReferences;
            ResourceInfo->SizeOfResource = sizeof(struct Gamepad*);
            ResourceInfo->FreeFunction = NULL;
            ResourceInfo->AllocatedResourceMemory = &Engine->Resource.AllocatedGamepadReferenceMemory;
            ResourceInfo->NumberOfResources= &Engine->Resource.NumberOfGamepadReferences;
            ResourceInfo->IsPointerArray = true;
            break;
        }
    }
    return(ResourceInfo);
}

//create ==============================================================================================================================================================================================================
void* CreateObject(ResourceInfo Objects, ResourceInfo References, Engine* Engine)
{
    if(Engine)
    {
        if(*Objects.NumberOfResources+1 >= *Objects.AllocatedResourceMemory)
        {
            ExtendResourcePool(Objects,Engine);
            ExtendResourcePool(References,Engine);
            qsort(*(void**)Objects.Pointer,*Objects.NumberOfResources,Objects.SizeOfResource,QSCompactObjectPool);
            RebuildReferences(*(void**)References.Pointer,*(void**)Objects.Pointer,Objects.SizeOfResource,*Objects.AllocatedResourceMemory);
        }

        Object* NewObject = FindOpenResourceSpace(*(void**)Objects.Pointer,(*Objects.AllocatedResourceMemory)*Objects.SizeOfResource,Objects.SizeOfResource);
        NewObject->ReferenceIndex = FindOpenReferenceSpace(*(void**)References.Pointer,*References.AllocatedResourceMemory);
        ((void**)(*(void**)References.Pointer))[NewObject->ReferenceIndex] = NewObject;
        NewObject->IsUsed = true;
        NewObject->ID = GetNewObjectID(Engine);
        (*(Objects.NumberOfResources))++;
        return(NewObject);
    }
    return(NULL);
}

uint32 CreateSound(char* File, char* Name, Engine* Engine)
{
    const char Parameters[] = "(char* File: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        Mix_Chunk* SoundT = Mix_LoadWAV(File);
        if(!SoundT)
        {
            Error Error = {WARNING_SDL_FAILURE,"\0","Could not create sound.","\0","\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_LoadWAV failed and returned 0x%p.",SoundT);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }

        ResourceInfo Objects;
        ResourceInfo References;
        PopulateResourceInfo(RSRC_SND,&Objects,Engine);
        PopulateResourceInfo(RSRC_SND_REF,&References,Engine);
        Sound* S = CreateObject(Objects,References,Engine);
        S->Sound = SoundT;
        strncpy(S->Name,Name,OBJECT_NAME_SIZE);
        S->Name[OBJECT_NAME_SIZE-1] = '\0';
        return(S->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

uint32 CreateMusic(char* File, char* Name, Engine* Engine)
{
    const char Parameters[] = "(char* File: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        Mix_Music* MusicT = Mix_LoadMUS(File);
        if(!MusicT)
        {
            Error Error = {WARNING_SDL_FAILURE,"\0","Could not create music.","\0","\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_LoadMUS failed and returned 0x%p.",MusicT);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }

        ResourceInfo Objects;
        ResourceInfo References;
        PopulateResourceInfo(RSRC_MUS,&Objects,Engine);
        PopulateResourceInfo(RSRC_MUS_REF,&References,Engine);
        Music* M = CreateObject(Objects,References,Engine);
        M->Music = MusicT;
        strncpy(M->Name,Name,OBJECT_NAME_SIZE);
        M->Name[OBJECT_NAME_SIZE-1] = '\0';
        return(M->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

uint32 CreateTexture(char* File, char* Name, Engine* Engine)
{
    const char Parameters[] = "(char* File: 0x%p, char* Name: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        SDL_Surface* S = IMG_Load(File);
        if(!S)
        {
            Error Error = {WARNING_SDL_FAILURE,"\0","Could not create surface.","\0","\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"IMG_Load failed and returned 0x%p.",S);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }
        SDL_Texture* TextureT = SDL_CreateTextureFromSurface(Engine->Video.Renderer,S);
        SDL_FreeSurface(S);
        if(!TextureT)
        {
            Error Error = {WARNING_SDL_FAILURE,"\0","Could not create texture.","\0","\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_CreateTextureFromSurface failed and returned 0x%p.",TextureT);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }

        ResourceInfo Objects;
        ResourceInfo References;
        PopulateResourceInfo(RSRC_TXTR,&Objects,Engine);
        PopulateResourceInfo(RSRC_TXTR_REF,&References,Engine);
        Texture* T = CreateObject(Objects,References,Engine);
        T->Texture = TextureT;
        strncpy(T->Name,Name,OBJECT_NAME_SIZE);
        T->Name[OBJECT_NAME_SIZE-1] = '\0';
        return(T->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

uint32 CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, uint32 TextureReference, void* CustomData, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo Objects;
        ResourceInfo References;
        PopulateResourceInfo(RSRC_SPR,&Objects,Engine);
        PopulateResourceInfo(RSRC_SPR_REF,&References,Engine);
        Sprite* S = CreateObject(Objects,References,Engine);
        S->CustomData = CustomData;
        S->Routine = Routine;
        strncpy(S->Name,Name,OBJECT_NAME_SIZE);
        S->Name[OBJECT_NAME_SIZE-1] = '\0';

        S->RenderParameters.Position.X = Position.X; S->RenderParameters.Position.Y = Position.Y; S->RenderParameters.Position.Z = Position.Z;
        S->RenderParameters.Origin.X = Origin.X; S->RenderParameters.Origin.Y = Origin.Y; S->RenderParameters.Origin.Z = Origin.Z; S->RenderParameters.Origin.W = Origin.W; 
        S->RenderParameters.Dimensions.X = Dimensions.X; S->RenderParameters.Dimensions.Y = Dimensions.Y;
        if(TextureReference != SENTINEL_OBJECT)
        {
            S->TextureReference = Engine->TextureReferences[TextureReference]->ReferenceIndex;
            S->TextureID = Engine->TextureReferences[TextureReference]->ID;
            if(Engine->TextureReferences[S->TextureReference]->ID == S->TextureID)
            {
                S->RenderParameters.Texture = Engine->TextureReferences[TextureReference]->Texture;
            }
        }
        S->RenderParameters.Visible = true;
        S->RenderParameters.Angle = 0;
        S->RenderParameters.Flip = FLIP_NONE;
        S->RenderParameters.Transparency = 100;
        Vector3 Tint = {TINT_NOCHANGE,TINT_NOCHANGE,TINT_NOCHANGE};
        S->RenderParameters.Tint = Tint;
        S->CustomData = CustomData;
        S->Routine = Routine;

        Engine->SpriteZResortNeeded = true;
        return(S->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

uint32 CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, uint32 Voice, void* CustomData, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo Objects;
        ResourceInfo References;
        PopulateResourceInfo(RSRC_ACT,&Objects,Engine);
        PopulateResourceInfo(RSRC_ACT_REF,&References,Engine);
        Actor* A = CreateObject(Objects,References,Engine);
        A->Position.X = Position.X; A->Position.Y = Position.Y;
        A->Dimensions.X = Dimensions.X; A->Dimensions.Y = Dimensions.Y;
        A->Voice = Voice;
        A->CustomData = CustomData;
        A->Routine = Routine;
        strncpy(A->Name,Name,OBJECT_NAME_SIZE);
        A->Name[OBJECT_NAME_SIZE-1] = '\0';

        return(A->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

uint32 CreateTimer(char* Name, uint8 TimerType, uint8 UnitType, uint64 StopAt, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo Objects;
        ResourceInfo References;
        PopulateResourceInfo(RSRC_TMR,&Objects,Engine);
        PopulateResourceInfo(RSRC_TMR_REF,&References,Engine);
        Timer* T = CreateObject(Objects,References,Engine);   
        strncpy(T->Name,Name,OBJECT_NAME_SIZE);
        T->Name[OBJECT_NAME_SIZE-1] = '\0';
        T->TimerType = TimerType;
        T->UnitType = UnitType;
        T->StopAt = StopAt;
        T->Counter = 0;

        if(T->TimerType == TMR_T_COUNT_TO)
        {
            if(T->UnitType == TMR_UT_SECONDS)
            {
                T->StopAt = StopAt*1000 + Engine->Clock.RealTimeMS;
            }
            if(T->UnitType == TMR_UT_MILLISECONDS)
            {
                T->StopAt = StopAt + Engine->Clock.RealTimeMS;
            }
        }
        if(T->TimerType == TMR_T_WAIT_UNTIL && T->UnitType == TMR_UT_SECONDS)
        {
            T->StopAt = StopAt*1000;
        }

        return(T->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

uint32 CreateGamepad(SDL_GameController* Controller, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo Objects;
        ResourceInfo References;
        PopulateResourceInfo(RSRC_GP,&Objects,Engine);
        PopulateResourceInfo(RSRC_GP_REF,&References,Engine);
        Gamepad* G = CreateObject(Objects,References,Engine);   
        G->Gamepad = Controller;
        return(G->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

//free =================================================================================================================================================================================================================
void TextureFreeFunction(void* Ptr)
{
    if(Ptr)
    {
        if(((Texture*)(Ptr))->Texture)
        {
            SDL_DestroyTexture(((Texture*)(Ptr))->Texture);
        }
    }
}

void SoundFreeFunction(void* Ptr)
{
    if(Ptr)
    {
        if(((Sound*)(Ptr))->Sound)
        {
            Mix_FreeChunk(((Sound*)(Ptr))->Sound);
        }
    }
}

void MusicFreeFunction(void* Ptr)
{
    if(Ptr)
    {
        if(((Music*)(Ptr))->Music)
        {
            Mix_FreeMusic(((Music*)(Ptr))->Music);
        }
    }
}

void GamepadFreeFunction(void* Ptr)
{
    if(Ptr)
    {
        if(((Gamepad*)(Ptr))->Gamepad)
        {
            SDL_GameControllerClose(((Gamepad*)(Ptr))->Gamepad);
        }
    }
}

//destroy ==============================================================================================================================================================================================================
void DestroyObject(ResourceInfo Objects, ResourceInfo References, Object* Object, Engine* Engine)
{
    const char Name[] = "DestroyObject";
    const char Parameters[] = "(ResourceInfo Objects: 0x%p, ResourceInfo References: 0x%p, Object* Object: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(Object)
        {
            if(Object->IsUsed)
            {
                if(PoolCanBeShrunk(*(void**)Objects.Pointer,*Objects.NumberOfResources,*Objects.AllocatedResourceMemory,Engine))
                {
                    qsort(*(void**)Objects.Pointer,*Objects.AllocatedResourceMemory,Objects.SizeOfResource,QSCompactObjectPool);
                    ShrinkResourcePool(Objects,Engine);
                    RebuildReferences(*(void**)References.Pointer,*(void**)Objects.Pointer,Objects.SizeOfResource,*Objects.AllocatedResourceMemory);
                }

                Objects.FreeFunction(Object);
                memset(Object,0,sizeof(struct Object)); //clears the header, not the actual object. objects are beheaded, not cremated.
                return;
            }
        }
        Error Error = {WARNING_INVALID_PARAMETER,"\0","Invalid object.","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Objects,References,Object,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"Object is NULL or dead.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroySprite(Sprite* DSprite, Engine* Engine)
{
    const char Name[] = "DestroySprite";
    const char Parameters[] = "(Sprite* DSprite: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DSprite)
        {   
            ResourceInfo Objects;
            ResourceInfo References;
            PopulateResourceInfo(RSRC_SPR,&Objects,Engine);
            PopulateResourceInfo(RSRC_SPR_REF,&References,Engine);
            DestroyObject(Objects,References,(void*)DSprite,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"\0","Invalid sprite.","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DSprite,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DSprite is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroyActor(Actor* DActor, Engine* Engine)
{
    const char Name[] = "Destroy Actor";
    const char Parameters[] = "(Actor* DActor: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DActor)
        {   
            ResourceInfo Objects;
            ResourceInfo References;
            PopulateResourceInfo(RSRC_ACT,&Objects,Engine);
            PopulateResourceInfo(RSRC_ACT_REF,&References,Engine);
            DestroyObject(Objects,References,(void*)DActor,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"\0","Invalid actor.","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DActor,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DActor is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroyTimer(Timer* DTimer, Engine* Engine)
{
    const char Name[] = "DestroyTimer";
    const char Parameters[] = "(Timer* DTimer: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DTimer)
        {   
            ResourceInfo Objects;
            ResourceInfo References;
            PopulateResourceInfo(RSRC_TMR,&Objects,Engine);
            PopulateResourceInfo(RSRC_TMR_REF,&References,Engine);
            DestroyObject(Objects,References,(void*)DTimer,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"\0","Invalid timer.","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DTimer,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DTimer is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroyGamepad(Gamepad* DGamepad, Engine* Engine)
{
    const char Name[] = "DestroyGamepad";
    const char Parameters[] = "(Gamepad* DGamepad: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DGamepad)
        {   
            ResourceInfo Objects;
            ResourceInfo References;
            PopulateResourceInfo(RSRC_GP,&Objects,Engine);
            PopulateResourceInfo(RSRC_GP_REF,&References,Engine);
            DestroyObject(Objects,References,(void*)DGamepad,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"\0","Invalid gamepad.","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DGamepad,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DGamepad is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroyTexture(Texture* DTexture, Engine* Engine)
{
    const char Name[] = "DestroyTexture";
    const char Parameters[] = "(Texture* DTexture: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DTexture)
        {   
            ResourceInfo Objects;
            ResourceInfo References;
            PopulateResourceInfo(RSRC_TXTR,&Objects,Engine);
            PopulateResourceInfo(RSRC_TXTR_REF,&References,Engine);
            DestroyObject(Objects,References,(void*)DTexture,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"\0","Invalid texture.","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DTexture,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DTexture is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroyMusic(Music* DMusic, Engine* Engine)
{
    const char Name[] = "DestroyMusic";
    const char Parameters[] = "(Music* DMusic: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DMusic)
        {   
            ResourceInfo Objects;
            ResourceInfo References;
            PopulateResourceInfo(RSRC_MUS,&Objects,Engine);
            PopulateResourceInfo(RSRC_MUS_REF,&References,Engine);
            DestroyObject(Objects,References,(void*)DMusic,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"\0","Invalid music.","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DMusic,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DMusic is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroySound(Sound* DSound, Engine* Engine)
{
    const char Name[] = "DestroySound";
    const char Parameters[] = "(Music* DMusic: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DSound)
        {   
            ResourceInfo Objects;
            ResourceInfo References;
            PopulateResourceInfo(RSRC_SND,&Objects,Engine);
            PopulateResourceInfo(RSRC_SND_REF,&References,Engine);
            DestroyObject(Objects,References,(void*)DSound,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"\0","Invalid sound.","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DSound,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DSound is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

//find =================================================================================================================================================================================================================
uint32 GetObjectByProperty(void* MemberValue, size_t MemberOffset, size_t MemberSize, ResourceInfo Objects)
{
    size_t SOR = Objects.SizeOfResource;
    uint32 ARM = *Objects.AllocatedResourceMemory;
    bytepointer BP = *(bytepointer*)Objects.Pointer;
    for(uint32 i = 0; i < ARM; i++)
    {
        size_t Offset = i * SOR;
        Object* O = (Object*)(BP+Offset);
        if(O->IsUsed)
        {
            void* Member = ((bytepointer)(O))+MemberOffset;
            if(!memcmp(Member,MemberValue,MemberSize))
            {
                return(O->ReferenceIndex);
            }
        }
    }
    return(SENTINEL_OBJECT);
}

uint32 GetSoundByName(char* Name, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        char RealName[OBJECT_NAME_SIZE] = "\0";
        memcpy(RealName,Name,strlen(Name));
        PopulateResourceInfo(RSRC_SND,&ResourceInfo,Engine);
        return(GetObjectByProperty(RealName,offsetof(struct Sound,Name),OBJECT_NAME_SIZE,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetSoundByID(uint64 ID, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        PopulateResourceInfo(RSRC_SND,&ResourceInfo,Engine);
        return(GetObjectByProperty(&ID,offsetof(struct Sound,ID),sizeof(uint64),ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetSoundByFile(char* File, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        char RealFile[STRING_BUFFER_SIZE] = "\0";
        memcpy(RealFile,File,STRING_BUFFER_SIZE);
        PopulateResourceInfo(RSRC_SND,&ResourceInfo,Engine);
        return(GetObjectByProperty(RealFile,offsetof(struct Sound,File),STRING_BUFFER_SIZE,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetMusicByName(char* Name, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        char RealName[OBJECT_NAME_SIZE] = "\0";
        memcpy(RealName,Name,strlen(Name));
        PopulateResourceInfo(RSRC_MUS,&ResourceInfo,Engine);
        return(GetObjectByProperty(RealName,offsetof(struct Music,Name),OBJECT_NAME_SIZE,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetMusicByID(uint64 ID, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        PopulateResourceInfo(RSRC_MUS,&ResourceInfo,Engine);
        return(GetObjectByProperty(&ID,offsetof(struct Music,ID),sizeof(uint64),ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetMusicByFile(char* File, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        char RealFile[STRING_BUFFER_SIZE] = "\0";
        memcpy(RealFile,File,strlen(File));
        PopulateResourceInfo(RSRC_MUS,&ResourceInfo,Engine);
        return(GetObjectByProperty(RealFile,offsetof(struct Music,File),STRING_BUFFER_SIZE,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetTextureByName(char* Name, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        char RealName[OBJECT_NAME_SIZE] = "\0";
        memcpy(RealName,Name,strlen(Name));
        PopulateResourceInfo(RSRC_TXTR,&ResourceInfo,Engine);
        return(GetObjectByProperty(RealName,offsetof(struct Texture,Name),OBJECT_NAME_SIZE,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetTextureByID(uint64 ID, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        PopulateResourceInfo(RSRC_TXTR,&ResourceInfo,Engine);
        return(GetObjectByProperty(&ID,offsetof(struct Texture,ID),sizeof(uint64),ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetTextureByFile(char* File, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        char RealFile[STRING_BUFFER_SIZE] = "\0";
        memcpy(RealFile,File,strlen(File));
        PopulateResourceInfo(RSRC_TXTR,&ResourceInfo,Engine);
        return(GetObjectByProperty(RealFile,offsetof(struct Texture,File),STRING_BUFFER_SIZE,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetSpriteByName(char* Name, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        char RealName[OBJECT_NAME_SIZE] = "\0";
        memcpy(RealName,Name,strlen(Name));
        PopulateResourceInfo(RSRC_SPR,&ResourceInfo,Engine);
        return(GetObjectByProperty(RealName,offsetof(struct Sprite,Name),OBJECT_NAME_SIZE,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetSpriteByID(uint64 ID, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        PopulateResourceInfo(RSRC_SPR,&ResourceInfo,Engine);
        return(GetObjectByProperty(&ID,offsetof(struct Sprite,ID),sizeof(uint64),ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetSpriteByProperty(void* MemberValue, size_t MemberOffset, size_t MemberSize, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        PopulateResourceInfo(RSRC_SPR,&ResourceInfo,Engine);
        return(GetObjectByProperty(MemberValue,MemberOffset,MemberSize,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetActorByName(char* Name, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        char RealName[OBJECT_NAME_SIZE] = "\0";
        memcpy(RealName,Name,strlen(Name));
        PopulateResourceInfo(RSRC_ACT,&ResourceInfo,Engine);
        return(GetObjectByProperty(RealName,offsetof(struct Actor,Name),OBJECT_NAME_SIZE,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetActorByID(Uint64 ID, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        PopulateResourceInfo(RSRC_ACT,&ResourceInfo,Engine);
        return(GetObjectByProperty(&ID,offsetof(struct Actor,ID),sizeof(uint64),ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetActorByProperty(void* MemberValue, size_t MemberOffset, size_t MemberSize, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        PopulateResourceInfo(RSRC_ACT,&ResourceInfo,Engine);
        return(GetObjectByProperty(MemberValue,MemberOffset,MemberSize,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetTimerByName(char* Name, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        char RealName[OBJECT_NAME_SIZE] = "\0";
        memcpy(RealName,Name,strlen(Name));
        PopulateResourceInfo(RSRC_TMR,&ResourceInfo,Engine);
        return(GetObjectByProperty(RealName,offsetof(struct Timer,Name),OBJECT_NAME_SIZE,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetTimerByID(uint64 ID, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        PopulateResourceInfo(RSRC_TMR,&ResourceInfo,Engine);
        return(GetObjectByProperty(&ID,offsetof(struct Timer,ID),sizeof(uint64),ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}

uint32 GetTimerByProperty(void* MemberValue, size_t MemberOffset, size_t MemberSize, Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        PopulateResourceInfo(RSRC_TMR,&ResourceInfo,Engine);
        return(GetObjectByProperty(MemberValue,MemberOffset,MemberSize,ResourceInfo));
    }
    return(SENTINEL_OBJECT);
}
