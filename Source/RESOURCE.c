#include "MOBSCE.h"

#ifdef _WIN32
    #include <windows.h>
    void* OSMemoryAllocate(size_t Size)
    {
        void* Pointer = VirtualAlloc(NULL,Size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
        return(Pointer);
    }
    void OSMemoryFree(void* Pointer, size_t Size)
    {
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
        return(Pointer);
    }
    void OSMemoryFree(void* Pointer, size_t Size)
    {
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

bool PoolCanBeShrunk(void* Array, uint64 AllocatedElements, uint64 AllocatedSize, Engine* Engine)
{
    return((AllocatedSize - AllocatedElements) >= MIN_ALLOCATE && (AllocatedSize - MIN_ALLOCATE) > MIN_ALLOCATE);
}

void* FindOpenResourceSpace(void* Pool, uint32 PoolSize, int Size)
{
    for(uint32 i = 0; i < PoolSize; i += Size)
    {
        if(!*(uint8*)(Pool+i))
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
    return(-1);
}

void RebuildReferences(void* ReferencePool, void* Pool, int SizeOfObject, uint32 AllocatedResources)
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

            *(int*)ResourceInfo.AllocatedResourceMemory = MIN_ALLOCATE;
            *(int*)ResourceInfo.NumberOfResources = 0;
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
            int OldSize = *(int*)ResourceInfo.AllocatedResourceMemory*ResourceInfo.SizeOfResource;
            int NewSize = (*(int*)ResourceInfo.AllocatedResourceMemory+MIN_ALLOCATE)*ResourceInfo.SizeOfResource;
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
            *(int*)ResourceInfo.AllocatedResourceMemory += MIN_ALLOCATE;

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
            int OldSize = *(int*)ResourceInfo.AllocatedResourceMemory*ResourceInfo.SizeOfResource;
            int NewSize = (*(int*)ResourceInfo.AllocatedResourceMemory-MIN_ALLOCATE)*ResourceInfo.SizeOfResource;
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
            *(int*)ResourceInfo.AllocatedResourceMemory -= MIN_ALLOCATE;

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
            uint8* Pool = *(void**)ResourceInfo.Pointer;
            int Size = *(int*)ResourceInfo.AllocatedResourceMemory*ResourceInfo.SizeOfResource;

            for(int i = 0; i < Size; i += ResourceInfo.SizeOfResource)
            {
                void* Element = Pool+i;
                if(!IsZero(Element,ResourceInfo.SizeOfResource))
                {
                    if(ResourceInfo.FreeFunction)
                    {
                        if(ResourceInfo.IsPointerArray)
                        {
                            ResourceInfo.FreeFunction(*(void**)Element);
                        }
                        else
                        {
                            ResourceInfo.FreeFunction(Element);
                        }
                    }
                }
            }
            memset(Pool,0,Size);
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

uint32 CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, int TextureID, void* CustomData, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Resource.NumberOfSprites+1 >= Engine->Resource.AllocatedSpriteMemory)
        {
            ResourceInfo ResourceInfo;
            ResourceInfo.Pointer = &Engine->Sprites;
            ResourceInfo.SizeOfResource = sizeof(Sprite);
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSprites;
            ExtendResourcePool(ResourceInfo,Engine);
            ResourceInfo.Pointer = &Engine->SpriteReferences;
            ResourceInfo.SizeOfResource = sizeof(Sprite*);
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteReferenceMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSpriteReferences;
            ExtendResourcePool(ResourceInfo,Engine);
            qsort(Engine->Sprites, Engine->Resource.NumberOfSprites, sizeof(Sprite), QSCompactObjectPool);
            int ASM = Engine->Resource.AllocatedSpriteMemory;
            Sprite* S = Engine->Sprites;
            for(int i = 0; i < ASM; i++)
            {
                if(S[i].IsUsed)
                {
                    Engine->SpriteReferences[Engine->Sprites[i].ReferenceIndex] = &S[i];
                }
            }
        }

        Sprite* NewSprite = FindOpenResourceSpace(Engine->Sprites,Engine->Resource.AllocatedSpriteMemory*sizeof(Sprite),sizeof(Sprite));
        NewSprite->ReferenceIndex = FindOpenReferenceSpace(Engine->SpriteReferences,Engine->Resource.AllocatedSpriteReferenceMemory);
        Engine->SpriteReferences[NewSprite->ReferenceIndex] = NewSprite;
        NewSprite->IsUsed = true;
        strncpy(NewSprite->Name,Name,OBJECT_NAME_SIZE);
        NewSprite->Name[OBJECT_NAME_SIZE-1] = '\0';
        NewSprite->ID = GetNewObjectID(Engine);
        NewSprite->RenderParameters.Position.X = Position.X; NewSprite->RenderParameters.Position.Y = Position.Y; NewSprite->RenderParameters.Position.Z = Position.Z;
        NewSprite->RenderParameters.Origin.X = Origin.X; NewSprite->RenderParameters.Origin.Y = Origin.Y; NewSprite->RenderParameters.Origin.Z = Origin.Z; NewSprite->RenderParameters.Origin.W = Origin.W; 
        NewSprite->RenderParameters.Dimensions.X = Dimensions.X; NewSprite->RenderParameters.Dimensions.Y = Dimensions.Y;
        NewSprite->TextureID = TextureID;
        NewSprite->RenderParameters.Texture = Engine->Resource.Textures[TextureID];
        NewSprite->RenderParameters.Visible = true;
        NewSprite->RenderParameters.Angle = 0;
        NewSprite->RenderParameters.Flip = FLIP_NONE;
        NewSprite->RenderParameters.Transparency = 100;
        Vector3 Tint = {TINT_NOCHANGE,TINT_NOCHANGE,TINT_NOCHANGE};
        NewSprite->RenderParameters.Tint = Tint;
        if(Actor)
        {
            NewSprite->ActorReferenceIndex = Actor->ReferenceIndex;
            NewSprite->ExpectedActorID = Actor->ID;
        }
        else
        {
            NewSprite->ActorReferenceIndex = 0;
            NewSprite->ExpectedActorID = 0;
        }
        NewSprite->CustomData = CustomData;
        NewSprite->Routine = Routine;

        Engine->Resource.NumberOfSprites++;
        Engine->SpriteZResortNeeded = true;
        return(NewSprite->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

int8 DestroySprite(Sprite* DSprite, void (*FreeFunction)(void*), Engine* Engine)
{
    const char Parameters[] = "(Sprite* DSprite: 0x%p, FreeFunction: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DSprite)
        {
            if(DSprite->CustomData)
            {
                FreeFunction(DSprite);
            }
            else
            {
                Error Error = {ERROR_INVALID_PARAMETER,"DestroySprite","Invalid custom data.","\0","\0"};
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DSprite,FreeFunction,Engine);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"DSprite has a NULL CustomData member.");
                ThrowWarning(&Error,Engine);
            }
            Engine->SpriteReferences[DSprite->ReferenceIndex] = NULL;
            memset(DSprite,0,sizeof(Sprite));
            Engine->SpriteZResortNeeded = true;
            Engine->Resource.NumberOfSprites--;

            if(PoolCanBeShrunk(Engine->Sprites,Engine->Resource.NumberOfSprites,Engine->Resource.AllocatedSpriteMemory,Engine))
            {
                qsort(Engine->Sprites, Engine->Resource.AllocatedSpriteMemory, sizeof(Sprite), QSCompactObjectPool);
                ResourceInfo ResourceInfo;
                ResourceInfo.Pointer = &Engine->Sprites;
                ResourceInfo.SizeOfResource = sizeof(Sprite);
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteMemory;
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSprites;
                ShrinkResourcePool(ResourceInfo,Engine);
                ResourceInfo.Pointer = &Engine->SpriteReferences;
                ResourceInfo.SizeOfResource = sizeof(Sprite*);
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteReferenceMemory;
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSpriteReferences;
                ShrinkResourcePool(ResourceInfo,Engine);
                RebuildReferences(Engine->SpriteReferences,Engine->Sprites,sizeof(struct Sprite),Engine->Resource.AllocatedSpriteMemory);
            }
            return(RETURN_SUCCESS);
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroySprite","Invalid sprite.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DSprite,FreeFunction,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DSprite is NULL.");
        ThrowWarning(&Error,Engine);
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

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

uint32 CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, int Voice, void* CustomData, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Resource.NumberOfActors+1 >= Engine->Resource.AllocatedActorMemory)
        {
            ResourceInfo ResourceInfo;
            ResourceInfo.Pointer = &Engine->Actors;
            ResourceInfo.SizeOfResource = sizeof(Actor);
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedActorMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfActors;
            ExtendResourcePool(ResourceInfo,Engine);
            ResourceInfo.Pointer = &Engine->ActorReferences;
            ResourceInfo.SizeOfResource = sizeof(Actor*);
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedActorReferenceMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfActorReferences;
            ExtendResourcePool(ResourceInfo,Engine);
            qsort(Engine->Actors, Engine->Resource.NumberOfActors, sizeof(Actor), QSCompactObjectPool);
            int AAM = Engine->Resource.AllocatedActorMemory;
            Actor* A = Engine->Actors;
            for(int i = 0; i < AAM; i++)
            {
                if(A[i].IsUsed)
                {
                    Engine->ActorReferences[A[i].ReferenceIndex] = &A[i];
                }
            }
        }

        Actor* NewActor = FindOpenResourceSpace(Engine->Actors,Engine->Resource.AllocatedActorMemory*sizeof(Actor),sizeof(Actor));
        NewActor->ReferenceIndex = FindOpenReferenceSpace(Engine->ActorReferences,Engine->Resource.AllocatedActorReferenceMemory);
        Engine->ActorReferences[NewActor->ReferenceIndex] = NewActor;
        NewActor->IsUsed = true;
        strncpy(NewActor->Name,Name,OBJECT_NAME_SIZE);
        NewActor->Name[OBJECT_NAME_SIZE-1] = '\0';
        NewActor->ID = GetNewObjectID(Engine);
        NewActor->Position.X = Position.X; NewActor->Position.Y = Position.Y;
        NewActor->Dimensions.X = Dimensions.X; NewActor->Dimensions.Y = Dimensions.Y;
        NewActor->Voice = Voice;
        NewActor->CustomData = CustomData;
        NewActor->Routine = Routine;

        Engine->Resource.NumberOfActors++;
        return(NewActor->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

int8 DestroyActor(Actor* DActor, void (*FreeFunction)(void*), Engine* Engine)
{
    const char Parameters[] = "(Actor* DActor: 0x%p, FreeFunction: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DActor)
        {
            if(DActor->CustomData)
            {
                FreeFunction(DActor);
            }
            else
            {
                Error Error = {ERROR_INVALID_PARAMETER,"DestroyActor","Invalid custom data.","\0","\0"};
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DActor,FreeFunction,Engine);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"DActor has a NULL CustomData member.");
                ThrowWarning(&Error,Engine);
            }

            Engine->ActorReferences[DActor->ReferenceIndex] = NULL;
            memset(DActor,0,sizeof(Actor));
            Engine->Resource.NumberOfActors--;
            
            if(PoolCanBeShrunk(Engine->Actors,Engine->Resource.NumberOfActors,Engine->Resource.AllocatedActorMemory,Engine))
            {
                qsort(Engine->Actors, Engine->Resource.AllocatedActorMemory, sizeof(Actor), QSCompactObjectPool);
                ResourceInfo ResourceInfo;
                ResourceInfo.Pointer = &Engine->Actors;
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedActorMemory;
                ResourceInfo.SizeOfResource = sizeof(Actor);
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfActors;
                ShrinkResourcePool(ResourceInfo,Engine);
                ResourceInfo.Pointer = &Engine->ActorReferences;
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedActorReferenceMemory;
                ResourceInfo.SizeOfResource = sizeof(Actor*);
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfActorReferences;
                ShrinkResourcePool(ResourceInfo,Engine);
                RebuildReferences(Engine->ActorReferences,Engine->Actors,sizeof(struct Actor),Engine->Resource.AllocatedActorMemory);
            }
            return(RETURN_SUCCESS);
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroyActor","Invalid actor.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DActor,FreeFunction,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DActor is NULL.");
        ThrowWarning(&Error,Engine);
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
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

uint32 CreateTimer(uint8 TimerType, uint8 UnitType, uint64 StopAt, Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Resource.NumberOfTimers+1 >= Engine->Resource.AllocatedTimerMemory)
        {
            ResourceInfo ResourceInfo;
            ResourceInfo.Pointer = &Engine->Timers;
            ResourceInfo.SizeOfResource = sizeof(Timer);
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTimerMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTimers;
            ExtendResourcePool(ResourceInfo,Engine);
            ResourceInfo.Pointer = &Engine->TimerReferences;
            ResourceInfo.SizeOfResource = sizeof(Timer*);
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTimerReferenceMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTimerReferences;
            ExtendResourcePool(ResourceInfo,Engine);
            qsort(Engine->Timers, Engine->Resource.NumberOfTimers, sizeof(Timer), QSCompactObjectPool);
            uint32 ATM = Engine->Resource.AllocatedTimerMemory;
            Timer* T = Engine->Timers;
            for(int i = 0; i < ATM; i++)
            {
                if(T[i].IsUsed)
                {
                    Engine->TimerReferences[T[i].ReferenceIndex] = &T[i];
                }
            }
        }

        Timer* NewTimer = FindOpenResourceSpace(Engine->Timers,Engine->Resource.AllocatedTimerMemory*sizeof(Timer),sizeof(Timer));
        NewTimer->ReferenceIndex = FindOpenReferenceSpace(Engine->TimerReferences,Engine->Resource.AllocatedTimerReferenceMemory);
        Engine->TimerReferences[NewTimer->ReferenceIndex] = NewTimer;
        NewTimer->IsUsed = true;
        NewTimer->ID = GetNewObjectID(Engine);
        NewTimer->TimerType = TimerType;
        NewTimer->UnitType = UnitType;
        NewTimer->StopAt = StopAt;
        NewTimer->Counter = 0;

        Engine->Resource.NumberOfTimers++;
        return(NewTimer->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

int8 DestroyTimer(Timer* DTimer, Engine* Engine)
{
    const char Parameters[] = "(Timer* DTimer: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DTimer)
        {
            Engine->TimerReferences[DTimer->ReferenceIndex] = NULL;
            memset(DTimer,0,sizeof(struct Timer));
            Engine->Resource.NumberOfTimers--;
            
            if(PoolCanBeShrunk(Engine->Timers,Engine->Resource.NumberOfTimers,Engine->Resource.AllocatedTimerMemory,Engine))
            {
                qsort(Engine->Timers, Engine->Resource.AllocatedTimerMemory, sizeof(struct Timer), QSCompactObjectPool);
                ResourceInfo ResourceInfo;
                ResourceInfo.Pointer = &Engine->Timers;
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTimerMemory;
                ResourceInfo.SizeOfResource = sizeof(struct Timer);
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTimers;
                ShrinkResourcePool(ResourceInfo,Engine);
                ResourceInfo.Pointer = &Engine->TimerReferences;
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTimerReferenceMemory;
                ResourceInfo.SizeOfResource = sizeof(Timer*);
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTimerReferences;
                ShrinkResourcePool(ResourceInfo,Engine);
                RebuildReferences(Engine->TimerReferences,Engine->Timers,sizeof(struct Timer),Engine->Resource.AllocatedTimerMemory);
            }
            return(RETURN_SUCCESS);
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroyTimer","Invalid timer.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DTimer,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DTimer is NULL.");
        ThrowWarning(&Error,Engine);
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

uint32 CreateGamepad(SDL_GameController* Controller, Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Resource.NumberOfGamepads+1 >= Engine->Resource.AllocatedGamepadMemory)
        {
            ResourceInfo ResourceInfo;
            ResourceInfo.Pointer = &Engine->Input.Gamepads;
            ResourceInfo.SizeOfResource = sizeof(Gamepad);
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedGamepadMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfGamepads;
            ExtendResourcePool(ResourceInfo,Engine);
            ResourceInfo.Pointer = &Engine->Input.GamepadReferences;
            ResourceInfo.SizeOfResource = sizeof(Gamepad*);
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedGamepadReferenceMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfGamepadReferences;
            ExtendResourcePool(ResourceInfo,Engine);
            qsort(Engine->Input.Gamepads, Engine->Resource.NumberOfGamepads, sizeof(Gamepad), QSCompactObjectPool);
            uint32 AGPM = Engine->Resource.AllocatedGamepadMemory;
            Gamepad* GP = Engine->Input.Gamepads;
            for(int i = 0; i < AGPM; i++)
            {
                if(GP[i].IsUsed)
                {
                    Engine->Input.GamepadReferences[GP[i].ReferenceIndex] = &GP[i];
                }
            }
        }

        Gamepad* NewGamepad = FindOpenResourceSpace(Engine->Input.Gamepads,Engine->Resource.AllocatedGamepadMemory*sizeof(Gamepad),sizeof(Gamepad));
        NewGamepad->ReferenceIndex = FindOpenReferenceSpace(Engine->Input.GamepadReferences,Engine->Resource.AllocatedGamepadReferenceMemory);
        Engine->Input.GamepadReferences[NewGamepad->ReferenceIndex] = NewGamepad;
        NewGamepad->IsUsed = true;
        NewGamepad->ID = GetNewObjectID(Engine);
        NewGamepad->Gamepad = Controller;
        Engine->Resource.NumberOfGamepads++;
        return(NewGamepad->ReferenceIndex);
    }
    return(SENTINEL_OBJECT);
}

int8 DestroyGamepad(Gamepad* DGamepad, Engine* Engine)
{
    const char Parameters[] = "(Gamepad* DGamepad: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(DGamepad)
        {   
            Engine->Input.GamepadReferences[DGamepad->ReferenceIndex] = NULL;
            SDL_GameControllerClose(DGamepad->Gamepad);
            memset(DGamepad,0,sizeof(struct Gamepad));
            Engine->Resource.NumberOfGamepads--;
            
            if(PoolCanBeShrunk(Engine->Input.Gamepads,Engine->Resource.NumberOfGamepads,Engine->Resource.AllocatedGamepadMemory,Engine))
            {
                qsort(Engine->Input.Gamepads, Engine->Resource.AllocatedGamepadMemory, sizeof(struct Gamepad), QSCompactObjectPool);
                ResourceInfo ResourceInfo;
                ResourceInfo.Pointer = &Engine->Input.Gamepads;
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedGamepadMemory;
                ResourceInfo.SizeOfResource = sizeof(struct Gamepad);
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfGamepads;
                printf("%d",ResourceInfo.NumberOfResources);
                ShrinkResourcePool(ResourceInfo,Engine);
                ResourceInfo.Pointer = &Engine->Input.GamepadReferences;
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedGamepadReferenceMemory;
                ResourceInfo.SizeOfResource = sizeof(Gamepad*);
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfGamepadReferences;
                ShrinkResourcePool(ResourceInfo,Engine);
                RebuildReferences(Engine->Input.GamepadReferences,Engine->Input.Gamepads,sizeof(struct Gamepad),Engine->Resource.AllocatedGamepadMemory);
            }
            return(RETURN_SUCCESS);
        }
        Error Error = {WARNING_INVALID_PARAMETER,"DestroyGamepad","Invalid gamepad.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,DGamepad,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"DGamepad is NULL.");
        ThrowWarning(&Error,Engine);
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 CacheSound(char* File, Engine* Engine)
{
    const char Parameters[] = "(char* File: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        Mix_Chunk* NewSound = Mix_LoadWAV(File);

        if(!NewSound)
        {
            Error Error = {WARNING_SDL_FAILURE,"CacheSound","Could not create sound.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_LoadWAV failed and returned 0x%p.",NewSound);
            return(WARNING_SDL_FAILURE);
        }

        if(Engine->Resource.NumberOfSounds+1 >= Engine->Resource.AllocatedSoundMemory)
        {
            ResourceInfo ResourceInfo;
            ResourceInfo.Pointer = &Engine->Resource.Sounds;
            ResourceInfo.SizeOfResource = sizeof(Mix_Chunk*);
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSoundMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSounds;
            ExtendResourcePool(ResourceInfo,Engine);
        }

        Engine->Resource.Sounds[Engine->Resource.NumberOfSounds] = NewSound;
        Engine->Resource.NumberOfSounds++;
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 CacheMusic(char* File, Engine* Engine)
{
    const char Parameters[] = "(char* File: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        Mix_Music* NewMusic = Mix_LoadMUS(File);

        if(!NewMusic)
        {
            Error Error = {WARNING_SDL_FAILURE,"CacheMusic","Could not create music.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_LoadMUS failed and returned 0x%p",NewMusic);
            return(WARNING_SDL_FAILURE);
        }

        if(Engine->Resource.NumberOfMusics+1 >= Engine->Resource.AllocatedMusicMemory)
        {
            ResourceInfo ResourceInfo;
            ResourceInfo.Pointer = &Engine->Resource.Music;
            ResourceInfo.SizeOfResource = sizeof(Mix_Music*);
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedMusicMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfMusics;
            ExtendResourcePool(ResourceInfo,Engine);
        }

        Engine->Resource.Music[Engine->Resource.NumberOfMusics] = NewMusic;
        Engine->Resource.NumberOfMusics++;
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 CacheTexture(char* File, Engine* Engine)
{
    const char Parameters[] = "(char* File: 0x%p, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(Engine->Resource.Textures)
        {
            SDL_Surface* Surface = IMG_Load(File);
            if(!Surface)
            {
                Error Error = {WARNING_SDL_FAILURE,"CacheTexture","Could not create surface.","\0","\0"};
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"IMG_Load failed and returned 0x%p.",Surface);
                return(WARNING_SDL_FAILURE);
            }

            SDL_Texture* NewTexture = SDL_CreateTextureFromSurface(Engine->Video.Renderer,Surface);
            SDL_FreeSurface(Surface);
            if(!NewTexture)
            {
                Error Error = {WARNING_SDL_FAILURE,"CacheTexture","Could not create texture.","\0","\0"};
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,File,Engine);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_CreateTextureFromSurface failed and returned 0x%p.",NewTexture);
                return(WARNING_SDL_FAILURE);   
            }

            if(Engine->Resource.NumberOfTextures+1 >= Engine->Resource.AllocatedTextureMemory)
            {
                ResourceInfo ResourceInfo;
                ResourceInfo.Pointer = &Engine->Resource.Textures;
                ResourceInfo.SizeOfResource = sizeof(SDL_Texture*);
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTextureMemory;
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTextures;
                ExtendResourcePool(ResourceInfo,Engine);
            }

            Engine->Resource.Textures[Engine->Resource.NumberOfTextures] = NewTexture;
            Engine->Resource.NumberOfTextures++;
            return(RETURN_SUCCESS);
        }
    }
    return(ERROR_INVALID_ENGINE);
}
