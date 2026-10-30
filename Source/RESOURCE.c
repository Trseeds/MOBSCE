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
    const char Parameters[] = "(ResourceInfo ResourceInfo: Pointer: 0x%p SizeOfResource: %hi FreeFunction: 0x%p AllocatedResourceMemory: 0x%p NumberOfResources: 0x%p IsPointerArray: %hd)";
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources)
        {
            *(void**)ResourceInfo.Pointer = OSMemoryAllocate(MIN_ALLOCATE*ResourceInfo.SizeOfResource);
            if(!*(void**)ResourceInfo.Pointer)
            {
                Error Error = {ERROR_MEMORY,"InitResourcePool","Failed to allocate memory!","\0","\0"};
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,ResourceInfo.Pointer,ResourceInfo.SizeOfResource,ResourceInfo.FreeFunction,ResourceInfo.AllocatedResourceMemory,ResourceInfo.NumberOfResources,ResourceInfo.IsPointerArray);
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
    const char Parameters[] = "(ResourceInfo ResourceInfo: Pointer: 0x%p SizeOfResource: %hi FreeFunction: 0x%p AllocatedResourceMemory: 0x%p NumberOfResources: 0x%p IsPointerArray: %hd)";
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
                Error Error = {ERROR_MEMORY,"ExtendResourcePool","Failed to allocate memory!","\0","\0"};
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,ResourceInfo.Pointer,ResourceInfo.SizeOfResource,ResourceInfo.FreeFunction,ResourceInfo.AllocatedResourceMemory,ResourceInfo.NumberOfResources,ResourceInfo.IsPointerArray);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"OSMemoryAllocate failed and returned 0x%p",*(void**)ResourceInfo.Pointer);
                ThrowError(&Error,Engine);
                return(ERROR_MEMORY);
            }

            memcpy(*(void**)ResourceInfo.Pointer,OldPtr,OldSize);
            OSMemoryFree(OldPtr,OldSize);
            *(uint32*)ResourceInfo.AllocatedResourceMemory += MIN_ALLOCATE;

            return(RETURN_SUCCESS);
        }
        Error Error = {ERROR_INVALID_PARAMETER,"ExtendResourcePool","Invalid resource info!","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,ResourceInfo.Pointer,ResourceInfo.SizeOfResource,ResourceInfo.FreeFunction,ResourceInfo.AllocatedResourceMemory,ResourceInfo.NumberOfResources,ResourceInfo.IsPointerArray);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"Caller passed in an invalid ResourceInfo struct. If this was not you, file a bug report. If it was you, stop.");
        ThrowError(&Error,Engine);
        return(ERROR_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 ShrinkResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    const char Parameters[] = "(ResourceInfo ResourceInfo: Pointer: 0x%p SizeOfResource: %hi FreeFunction: 0x%p AllocatedResourceMemory: 0x%p NumberOfResources: 0x%p IsPointerArray: %hd)";
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
                Error Error = {ERROR_MEMORY,"ShrinkResourcePool","Failed to allocate memory!","\0","\0"};
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,ResourceInfo.Pointer,ResourceInfo.SizeOfResource,ResourceInfo.FreeFunction,ResourceInfo.AllocatedResourceMemory,ResourceInfo.NumberOfResources,ResourceInfo.IsPointerArray);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"OSMemoryAllocate failed and returned 0x%p",*(void**)ResourceInfo.Pointer);
                ThrowError(&Error,Engine);
                return(ERROR_MEMORY);
            }

            memcpy(*(void**)ResourceInfo.Pointer,OldPtr,NewSize);
            OSMemoryFree(OldPtr,OldSize);
            *(uint32*)ResourceInfo.AllocatedResourceMemory -= MIN_ALLOCATE;

            return(RETURN_SUCCESS);
        }
        Error Error = {ERROR_INVALID_PARAMETER,"ShrinkResourcePool","Invalid resource info!","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,ResourceInfo.Pointer,ResourceInfo.SizeOfResource,ResourceInfo.FreeFunction,ResourceInfo.AllocatedResourceMemory,ResourceInfo.NumberOfResources,ResourceInfo.IsPointerArray);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"Caller passed in an invalid ResourceInfo struct. If this was not you, file a bug report. If it was you, stop.");
        ThrowError(&Error,Engine);
        return(ERROR_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 CleanupResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    const char Parameters[] = "(ResourceInfo ResourceInfo: Pointer: 0x%p SizeOfResource: %hi FreeFunction: 0x%p AllocatedResourceMemory: 0x%p NumberOfResources: 0x%p IsPointerArray: %hd)";
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
        Error Error = {ERROR_INVALID_PARAMETER,"CleanupResourcePool","Invalid resource info!","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,ResourceInfo.Pointer,ResourceInfo.SizeOfResource,ResourceInfo.FreeFunction,ResourceInfo.AllocatedResourceMemory,ResourceInfo.NumberOfResources,ResourceInfo.IsPointerArray);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"Caller passed in an invalid ResourceInfo struct. If this was not you, file a bug report. If it was you, stop.");
        ThrowError(&Error,Engine);
        return(ERROR_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

//create ==============================================================================================================================================================================================================
void* CreateObject(ResourceInfo Objects, ResourceInfo References, Engine* Engine)
{
    const char Parameters[] = "(ResourceInfo Objects: 0x%p, ResourceInfo References: 0x%p, Engine* Engine: 0x%p)";
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
            Error Error = {WARNING_SDL_FAILURE,"CreateSound","Could not create sound.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_LoadWAV failed and returned 0x%p.",SoundT);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }

        ResourceInfo Objects = {&Engine->Sounds,sizeof(struct Sound),&SoundFreeFunction,&Engine->Resource.AllocatedSoundMemory,&Engine->Resource.NumberOfSounds,false};
        ResourceInfo References = {&Engine->SoundReferences,sizeof(Sound*),NULL,&Engine->Resource.AllocatedSoundReferenceMemory,&Engine->Resource.NumberOfSoundReferences,true};
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
            Error Error = {WARNING_SDL_FAILURE,"CreateMusic","Could not create music.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_LoadMUS failed and returned 0x%p.",MusicT);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }

        ResourceInfo Objects = {&Engine->Musics,sizeof(struct Music),&MusicFreeFunction,&Engine->Resource.AllocatedMusicMemory,&Engine->Resource.NumberOfMusics,false};
        ResourceInfo References = {&Engine->MusicReferences,sizeof(Music*),NULL,&Engine->Resource.AllocatedMusicReferenceMemory,&Engine->Resource.NumberOfMusicReferences,true};
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
            Error Error = {WARNING_SDL_FAILURE,"CreateTexture","Could not create surface.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"IMG_Load failed and returned 0x%p.",S);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }
        SDL_Texture* TextureT = SDL_CreateTextureFromSurface(Engine->Video.Renderer,S);
        SDL_FreeSurface(S);
        if(!TextureT)
        {
            Error Error = {WARNING_SDL_FAILURE,"CreateTexture","Could not create texture.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_CreateTextureFromSurface failed and returned 0x%p.",TextureT);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }

        ResourceInfo Objects = {&Engine->Textures,sizeof(struct Texture),&TextureFreeFunction,&Engine->Resource.AllocatedTextureMemory,&Engine->Resource.NumberOfTextures,false};
        ResourceInfo References = {&Engine->TextureReferences,sizeof(Texture*),NULL,&Engine->Resource.AllocatedTextureReferenceMemory,&Engine->Resource.NumberOfTextureReferences,true};
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
        ResourceInfo Objects = {&Engine->Sprites,sizeof(struct Sprite),&SpriteFreeFunction,&Engine->Resource.AllocatedSpriteMemory,&Engine->Resource.NumberOfSprites,false};
        ResourceInfo References = {&Engine->SpriteReferences,sizeof(Sprite*),NULL,&Engine->Resource.AllocatedSpriteReferenceMemory,&Engine->Resource.NumberOfSpriteReferences,true};
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
        ResourceInfo Objects = {&Engine->Actors,sizeof(struct Actor),&ActorFreeFunction,&Engine->Resource.AllocatedActorMemory,&Engine->Resource.NumberOfActors,false};
        ResourceInfo References = {&Engine->ActorReferences,sizeof(Actor*),NULL,&Engine->Resource.AllocatedActorReferenceMemory,&Engine->Resource.NumberOfActorReferences,true};
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
        ResourceInfo Objects = {&Engine->Timers,sizeof(struct Timer),NULL,&Engine->Resource.AllocatedTimerMemory,&Engine->Resource.NumberOfTimers,false};
        ResourceInfo References = {&Engine->TimerReferences,sizeof(Timer*),NULL,&Engine->Resource.AllocatedTimerReferenceMemory,&Engine->Resource.NumberOfTimerReferences,true};
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
        ResourceInfo Objects = {&Engine->Input.Gamepads,sizeof(struct Gamepad),NULL,&Engine->Resource.AllocatedGamepadMemory,&Engine->Resource.NumberOfGamepads,false};
        ResourceInfo References = {&Engine->Input.GamepadReferences,sizeof(Gamepad*),NULL,&Engine->Resource.AllocatedGamepadReferenceMemory,&Engine->Resource.NumberOfGamepadReferences,true};
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
                memset(Object,0,sizeof(struct Object)); //clears the header, not the actual object. dead objects are beheaded, not cremated.
                return;
            }
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroyObject","Invalid object.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Objects,References,Object,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"Object is NULL or dead.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroySprite(Sprite* DSprite, Engine* Engine)
{
    const char Parameters[] = "(Sprite* DSprite: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DSprite)
        {   
            ResourceInfo Objects = {&Engine->Sprites,sizeof(struct Sprite),&SpriteFreeFunction,&Engine->Resource.AllocatedSpriteMemory,&Engine->Resource.NumberOfSprites,false};
            ResourceInfo References = {&Engine->SpriteReferences,sizeof(Sprite*),NULL,&Engine->Resource.AllocatedSpriteReferenceMemory,&Engine->Resource.NumberOfSpriteReferences,true};
            DestroyObject(Objects,References,(void*)DSprite,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroySprite","Invalid sprite.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DSprite,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DSprite is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroyActor(Actor* DActor, Engine* Engine)
{
    const char Parameters[] = "(Actor* DActor: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DActor)
        {   
            ResourceInfo Objects = {&Engine->Actors,sizeof(struct Actor),&ActorFreeFunction,&Engine->Resource.AllocatedActorMemory,&Engine->Resource.NumberOfActors,false};
            ResourceInfo References = {&Engine->ActorReferences,sizeof(Actor*),NULL,&Engine->Resource.AllocatedActorReferenceMemory,&Engine->Resource.NumberOfActorReferences,true};
            DestroyObject(Objects,References,(void*)DActor,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroyActor","Invalid actor.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DActor,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DActor is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroyTimer(Timer* DTimer, Engine* Engine)
{
    const char Parameters[] = "(Timer* DTimer: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DTimer)
        {   
            ResourceInfo Objects = {&Engine->Timers,sizeof(struct Timer),NULL,&Engine->Resource.AllocatedTimerMemory,&Engine->Resource.NumberOfTimers,false};
            ResourceInfo References = {&Engine->TimerReferences,sizeof(Timer*),NULL,&Engine->Resource.AllocatedTimerReferenceMemory,&Engine->Resource.NumberOfTimerReferences,true};
            DestroyObject(Objects,References,(void*)DTimer,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroyTimer","Invalid timer.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DTimer,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DTimer is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroyGamepad(Gamepad* DGamepad, Engine* Engine)
{
    const char Parameters[] = "(Gamepad* DGamepad: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DGamepad)
        {   
            ResourceInfo Objects = {&Engine->Input.Gamepads,sizeof(struct Gamepad),&GamepadFreeFunction,&Engine->Resource.AllocatedGamepadMemory,&Engine->Resource.NumberOfGamepads,false};
            ResourceInfo References = {&Engine->Input.GamepadReferences,sizeof(Gamepad*),NULL,&Engine->Resource.AllocatedGamepadReferenceMemory,&Engine->Resource.NumberOfGamepadReferences,true};
            DestroyObject(Objects,References,(void*)DGamepad,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroyGamepad","Invalid gamepad.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DGamepad,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DGamepad is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroyTexture(Texture* DTexture, Engine* Engine)
{
    const char Parameters[] = "(Texture* DTexture: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DTexture)
        {   
            ResourceInfo Objects = {&Engine->Textures,sizeof(struct Texture),&TextureFreeFunction,&Engine->Resource.AllocatedTextureMemory,&Engine->Resource.NumberOfTextures,false};
            ResourceInfo References = {&Engine->TextureReferences,sizeof(Texture*),NULL,&Engine->Resource.AllocatedTextureReferenceMemory,&Engine->Resource.NumberOfTextureReferences,true};
            DestroyObject(Objects,References,(void*)DTexture,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroyTexture","Invalid texture.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DTexture,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DTexture is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroyMusic(Music* DMusic, Engine* Engine)
{
    const char Parameters[] = "(Music* DMusic: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DMusic)
        {   
            ResourceInfo Objects = {&Engine->Musics,sizeof(struct Music),&MusicFreeFunction,&Engine->Resource.AllocatedMusicMemory,&Engine->Resource.NumberOfMusics,false};
            ResourceInfo References = {&Engine->MusicReferences,sizeof(Music*),NULL,&Engine->Resource.AllocatedMusicReferenceMemory,&Engine->Resource.NumberOfMusicReferences,true};
            DestroyObject(Objects,References,(void*)DMusic,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroyMusic","Invalid music.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DMusic,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DMusic is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

void DestroySound(Sound* DSound, Engine* Engine)
{
    const char Parameters[] = "(Music* DMusic: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DSound)
        {   
            ResourceInfo Objects = {&Engine->Sounds,sizeof(struct Sound),&SoundFreeFunction,&Engine->Resource.AllocatedSoundMemory,&Engine->Resource.NumberOfSounds,false};
            ResourceInfo References = {&Engine->SoundReferences,sizeof(Sound*),NULL,&Engine->Resource.AllocatedSoundReferenceMemory,&Engine->Resource.NumberOfSoundReferences,true};
            DestroyObject(Objects,References,(void*)DSound,Engine);
            return;
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroySound","Invalid sound.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DSound,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DSound is NULL.");
        ThrowWarning(&Error,Engine);
    }
}

//find =================================================================================================================================================================================================================
uint32 GetSpriteByName(char* Name, Engine* Engine)
{
    const char Parameters[] = "(char* Name: %s, Engine* Engine: 0x%p)";
    if(Engine)
    {
        int ASM = Engine->Resource.AllocatedSpriteMemory;
        Sprite* S = Engine->Sprites;
        if(S)
        {
            for(int i = 0; i < ASM; i++)
            {
                if(S[i].IsUsed)
                {
                    if(!strcmp(S[i].Name,Name))
                    {
                        return(S[i].ReferenceIndex);
                    }
                }
            }
            Error Error = {WARNING_INVALID_PARAMETER,"GetSpriteByName","Could not find sprite.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Name,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Could not find a sprite with name \"%s\".",Name);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }
    }
    return(SENTINEL_OBJECT);
}

uint32 GetSpriteByID(Uint64 ID, Engine* Engine)
{
    const char Parameters[] = "(int ID: %d, Engine* Engine: 0x%p)";
    if(Engine)
    {
        Sprite* S = Engine->Sprites;
        int ASM = Engine->Resource.AllocatedSpriteMemory;
        if(S)
        {
            for(int i = 0; i < ASM; i++)
            {
                if(S[i].IsUsed)
                {
                    if(S[i].ID == ID)
                    {
                        return(S[i].ReferenceIndex);
                    }
                }
            }
            Error Error = {WARNING_INVALID_PARAMETER,"GetSpriteByID","Could not find sprite.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,ID,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Could not find a sprite with ID %d.",ID);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }
    }
    return(SENTINEL_OBJECT);
}

uint32 GetSpriteByProperty(void* Property, uint32 Offset, uint32 Size, Engine* Engine)
{
    const char Parameters[] = "(void* Property: 0x%p, uint32 Offset: %d, uint32 Size: %d, Engine* Engine: 0x%p)";
    if(Engine)
    {
        Sprite* S = Engine->Sprites;
        int ASM = Engine->Resource.AllocatedSpriteMemory;
        if(S)
        {
            for(int i = 0; i < ASM; i++)
            {
                if(S[i].IsUsed)
                {
                    if(!memcmp((bytepointer)(&S[i])+Offset,Property,Size))
                    {
                        return(S[i].ReferenceIndex);
                    }
                }
            }
            Error Error = {WARNING_INVALID_PARAMETER,"GetSpriteByProperty","Could not find sprite.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Property,Offset,Size,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Could not find a sprite with property defined at 0x%p.",Property);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }
    }
    return(SENTINEL_OBJECT);
}

uint32 GetActorByName(char* Name, Engine* Engine)
{
    const char Parameters[] = "(char* Name: %s, Engine* Engine: 0x%p)";
    if(Engine)
    {
        Actor* A = Engine->Actors;
        int AAM = Engine->Resource.AllocatedActorMemory;
        if(A)
        {
            for(int i = 0; i < AAM; i++)
            {
                if(A[i].IsUsed)
                {
                    if(!strcmp(A[i].Name,Name))
                    {
                        return(A[i].ReferenceIndex);
                    }
                }
            }
            Error Error = {WARNING_INVALID_PARAMETER,"GetActorByName","Could not find Actor.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Name,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Could not find an actor with name \"%s\".",Name);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }
    }
    return(SENTINEL_OBJECT);
}

uint32 GetActorByID(Uint64 ID, Engine* Engine)
{
    const char Parameters[] = "(int ID: %d, Engine* Engine: 0x%p)";
    if(Engine)
    {
        Actor* A = Engine->Actors;
        int AAM = Engine->Resource.AllocatedActorMemory;
        if(A)
        {
            for(int i = 0; i < AAM; i++)
            {
                if(A[i].IsUsed)
                {
                    if(A[i].ID == ID)
                    {
                        return(A[i].ReferenceIndex);
                    }
                }
            }
            Error Error = {WARNING_INVALID_PARAMETER,"GetActorByID","Could not find actor.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,ID,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Could not find an actor with ID %d.",ID);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }
    }
    return(SENTINEL_OBJECT);
}

uint32 GetActorByProperty(void* Property, uint32 Offset, uint32 Size, Engine* Engine)
{
    const char Parameters[] = "(void* Property: 0x%p, uint32 Offset: %d, uint32 Size: %d, Engine* Engine: 0x%p)";
    if(Engine)
    {
        Actor* A = Engine->Actors;
        int AAM = Engine->Resource.AllocatedActorMemory;
        if(A)
        {
            for(int i = 0; i < AAM; i++)
            {
                if(A[i].IsUsed)
                {
                    // printf("\"");
                    // for(int j = 0; j < sizeof(struct Actor); j++)
                    // {
                    //     char* c = (char*)(&A[i]);
                    //     printf("%c",*(c+j));
                    // }
                    // printf("\"\n");
                    if(!memcmp((bytepointer)(&A[i])+Offset,Property,Size))
                    {
                        return(A[i].ReferenceIndex);
                    }
                }
            }
            Error Error = {WARNING_INVALID_PARAMETER,"GetActorByProperty","Could not find actor.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Property,Offset,Size,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Could not find an actor with property defined at 0x%p.",Property);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }
    }
    return(SENTINEL_OBJECT);
}

uint32 GetTextureByName(char* Name, Engine* Engine)
{
    const char Parameters[] = "(char* Name: %s, Engine* Engine: 0x%p)";
    if(Engine)
    {
        Texture* T = Engine->Textures;
        int ATM = Engine->Resource.AllocatedTextureMemory;
        if(T)
        {
            for(int i = 0; i < ATM; i++)
            {
                if(T[i].IsUsed)
                {
                    if(!strcmp(T[i].Name,Name))
                    {
                        return(T[i].ReferenceIndex);
                    }
                }
            }
            Error Error = {WARNING_INVALID_PARAMETER,"GetTextureByName","Could not find texture.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Name,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Could not find a texture with name \"%s\".",Name);
            ThrowWarning(&Error,Engine);
            return(SENTINEL_OBJECT);
        }
    }
    return(SENTINEL_OBJECT);
}
