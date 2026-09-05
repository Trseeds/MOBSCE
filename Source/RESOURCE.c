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

int InitResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources)
        {
            *(void**)ResourceInfo.Pointer = OSMemoryAllocate(MIN_ALLOCATE*ResourceInfo.SizeOfResource);
            if(!*(void**)ResourceInfo.Pointer)
            {
                char Traceback[STRING_BUFFER_SIZE];
                snprintf(Traceback,STRING_BUFFER_SIZE,"InitResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
                ThrowError("Failed to allocate memory!",Traceback,Engine);
                return(ERROR_MEMORY);
            }

            *(int*)ResourceInfo.AllocatedResourceMemory = MIN_ALLOCATE;
            *(int*)ResourceInfo.NumberOfResources = 0;
            return(RETURN_SUCCESS);
        }       
    }
    return(ERROR_INVALID_ENGINE);
}

int ExtendResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
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
                *(void**)ResourceInfo.Pointer = OldPtr;
                char Traceback[STRING_BUFFER_SIZE];
                snprintf(Traceback,STRING_BUFFER_SIZE,"ExtendResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
                ThrowError("Failed to allocate new memory!",Traceback,Engine);
                return(ERROR_MEMORY);
            }

            memcpy(*(void**)ResourceInfo.Pointer,OldPtr,OldSize);
            OSMemoryFree(OldPtr,OldSize);
            *(int*)ResourceInfo.AllocatedResourceMemory += MIN_ALLOCATE;

            return(RETURN_SUCCESS);
        }
        char Traceback[STRING_BUFFER_SIZE];
        snprintf(Traceback,STRING_BUFFER_SIZE,"ExtendResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
        ThrowError("Invalid Resource Info! (critical engine error!!!)",Traceback,Engine);
        return(ERROR_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

int ShrinkResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
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
                *(void**)ResourceInfo.Pointer = OldPtr;
                char Traceback[STRING_BUFFER_SIZE];
                snprintf(Traceback,STRING_BUFFER_SIZE,"ShrinkResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
                ThrowError("Failed to allocate new memory!",Traceback,Engine);
                return(ERROR_MEMORY);
            }

            memcpy(*(void**)ResourceInfo.Pointer,OldPtr,NewSize);
            OSMemoryFree(OldPtr,OldSize);
            *(int*)ResourceInfo.AllocatedResourceMemory -= MIN_ALLOCATE;

            return(RETURN_SUCCESS);
        }
        char Traceback[STRING_BUFFER_SIZE];
        snprintf(Traceback,STRING_BUFFER_SIZE,"ShrinkResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
        ThrowWarning("Invalid Resource Info! Skipping shrink. (this is a memory leak, you must fix it.)",Traceback,Engine);
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

int CleanupResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources)
        {
            byte* Pool = *(void**)ResourceInfo.Pointer;
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
            OSMemoryFree(Pool,Size);
            return(RETURN_SUCCESS);
        }
        char Traceback[STRING_BUFFER_SIZE];
        snprintf(Traceback,STRING_BUFFER_SIZE,"CleanupResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
        ThrowWarning("Invalid Resource Info! Skipping cleanup. (this is a memory leak, you must fix it.)",Traceback,Engine);
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

void* FindOpenObjectSpace(void* Pool, int PoolSize, int Size)
{
    for(int i = 0; i < PoolSize; i += Size)
    {
        if(!*(byte*)(Pool+i))
        {
            return(Pool+i);
        }
    }
}

Uint32 FindOpenReferenceSpace(void* Pool, int AllocatedReferenceMemory)
{
    void** RealPool = (void**)Pool;
    for(int i = 0; i < AllocatedReferenceMemory; i++)
    {
        if(!RealPool[i])
        {
            return(i);
        }
    }
}

Sprite* CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, int TextureID, CustomSpriteData* CustomData, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine)
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
            qsort(Engine->Sprites, Engine->Resource.NumberOfSprites, sizeof(Sprite), CompactArrayOfObjects);
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

        Sprite* NewSprite = FindOpenObjectSpace(Engine->Sprites,Engine->Resource.AllocatedSpriteMemory*sizeof(Sprite),sizeof(Sprite));
        NewSprite->ReferenceIndex = FindOpenReferenceSpace(Engine->SpriteReferences,Engine->Resource.AllocatedSpriteReferenceMemory);
        Engine->SpriteReferences[NewSprite->ReferenceIndex] = NewSprite;
        NewSprite->IsUsed = true;
        strncpy(NewSprite->Name,Name,OBJECT_NAME_SIZE);
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
        return(NewSprite);
    }
    return(WARNING_NULL);
}

int DestroySprite(Sprite* DSprite, void (*FreeFunction)(void*), Engine* Engine)
{
    if(Engine)
    {
        if(DSprite)
        {
            Sprite* OldPtr = DSprite;
            int Index;
            if(DSprite)
            {
                FreeFunction(DSprite);
            }
            else
            {
                char Traceback[STRING_BUFFER_SIZE];
                snprintf(Traceback,STRING_BUFFER_SIZE,"DestroySprite(0x%X, 0x%X, 0x%X)",DSprite,FreeFunction,Engine);
                ThrowWarning("Invalid custom data.",Traceback,Engine);
            }
            int ASM = Engine->Resource.AllocatedSpriteMemory;
            Sprite* S = Engine->Sprites;
            for(int i = 0; i < ASM; i++)
            {
                if(S[i].ID == DSprite->ID)
                {
                    Engine->SpriteReferences[DSprite->ReferenceIndex] = NULL;
                    memset(&S[i],0,sizeof(Sprite));
                }
            }
            Engine->SpriteZResortNeeded = true;
            Engine->Resource.NumberOfSprites--;

            if(PoolCanBeShrunk(Engine->Sprites,Engine->Resource.NumberOfSprites,Engine->Resource.AllocatedSpriteMemory))
            {
                qsort(Engine->Sprites, Engine->Resource.AllocatedSpriteMemory, sizeof(Sprite), CompactArrayOfObjects);
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
                int ASM = Engine->Resource.AllocatedSpriteMemory;
                Sprite* S = Engine->Sprites;
                for(int i = 0; i < ASM; i++)
                {
                    if(S[i].IsUsed)
                    {
                        Engine->SpriteReferences[S[i].ReferenceIndex] = &S[i];
                    }
                }
            }
            return(RETURN_SUCCESS);
        }
        char Traceback[STRING_BUFFER_SIZE];
        snprintf(Traceback,STRING_BUFFER_SIZE,"DestroySprite(0x%X, 0x%X, 0x%X)",DSprite,FreeFunction,Engine);
        ThrowWarning("Invalid sprite passed.",Traceback,Engine);
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

Sprite* GetSpriteByName(char* Name, Engine* Engine)
{
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
                        return(&S[i]);
                    }
                }
            }
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"GetSpriteByName(%s, 0x%X)",Name,Engine);
            ThrowWarning("Could not find sprite.",Traceback,Engine);
            return(WARNING_NULL);
        }
    }
    return(WARNING_NULL);
}

Actor* CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, int Voice, CustomActorData* CustomData, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine)
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
            qsort(Engine->Actors, Engine->Resource.NumberOfActors, sizeof(Actor), CompactArrayOfObjects);
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

        Actor* NewActor = FindOpenObjectSpace(Engine->Actors,Engine->Resource.AllocatedActorMemory*sizeof(Actor),sizeof(Actor));
        NewActor->ReferenceIndex = FindOpenReferenceSpace(Engine->ActorReferences,Engine->Resource.AllocatedActorReferenceMemory);
        Engine->ActorReferences[NewActor->ReferenceIndex] = NewActor;
        NewActor->IsUsed = true;
        strncpy(NewActor->Name,Name,OBJECT_NAME_SIZE);
        NewActor->ID = GetNewObjectID(Engine);
        NewActor->Position.X = Position.X; NewActor->Position.Y = Position.Y;
        NewActor->Dimensions.X = Dimensions.X; NewActor->Dimensions.Y = Dimensions.Y;
        NewActor->Voice = Voice;
        NewActor->CustomData = CustomData;
        NewActor->Routine = Routine;

        Engine->Resource.NumberOfActors++;
        return(NewActor);
    }
    return(WARNING_NULL);
}

int DestroyActor(Actor* DActor, void (*FreeFunction)(void*), Engine* Engine)
{
    if(Engine)
    {
        if(DActor)
        {
            Actor* OldPtr = DActor;
            int Index;
            if(DActor)
            {
                FreeFunction(DActor);
            }
            else
            {
                char Traceback[STRING_BUFFER_SIZE];
                snprintf(Traceback,STRING_BUFFER_SIZE,"DestroyActor(0x%X, 0x%X, 0x%X)",DActor,FreeFunction,Engine);
                ThrowWarning("Invalid custom data.",Traceback,Engine);
            }
            
            int AAM = Engine->Resource.AllocatedActorMemory;
            Actor* A = Engine->Actors;
            for(int i = 0; i < AAM; i++)
            {
                if(A[i].ID == DActor->ID)
                {
                    Engine->ActorReferences[DActor->ReferenceIndex] = NULL;
                    memset(&A[i],0,sizeof(Actor));
                }
            }

            Engine->Resource.NumberOfActors--;
            
            if(PoolCanBeShrunk(Engine->Actors,Engine->Resource.NumberOfActors,Engine->Resource.AllocatedActorMemory))
            {
                qsort(Engine->Actors, Engine->Resource.AllocatedActorMemory, sizeof(Actor), CompactArrayOfObjects);
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
            return(RETURN_SUCCESS);
        }
        char Traceback[STRING_BUFFER_SIZE];
        snprintf(Traceback,STRING_BUFFER_SIZE,"DestroyActor(0x%X, 0x%X, 0x%X)",DActor,FreeFunction,Engine);
        ThrowWarning("Invalid actor passed.",Traceback,Engine);
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}

Actor* GetActorByName(char* Name, Engine* Engine)
{
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
                        return(&A[i]);
                    }
                }
            }
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"GetActorByName(%s, 0x%X)",Name,Engine);
            ThrowWarning("Could not find actor.",Traceback,Engine);
            return(WARNING_NULL);
        }
    }
    return(WARNING_NULL);
}

Actor* GetActorByID(Uint64 ID, Engine* Engine)
{
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
                        return(&A[i]);
                    }
                }
            }
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"GetActorByID(%d, 0x%X)",ID,Engine);
            ThrowWarning("Could not find actor.",Traceback,Engine);
            return(WARNING_NULL);
        }
    }
    return(WARNING_NULL);
}

int CacheSound(char* File, Engine* Engine)
{
    if(Engine)
    {
        Mix_Chunk* NewSound = Mix_LoadWAV(File);

        if(!NewSound)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"CacheSound(%s, 0x%X)",File,Engine);
            ThrowWarning("Could not create sound.",Traceback,Engine);
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

int CacheMusic(char* File, Engine* Engine)
{
    if(Engine)
    {
        Mix_Music* NewMusic = Mix_LoadMUS(File);

        if(!NewMusic)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"CacheMusic(%s, 0x%X)",File,Engine);
            ThrowWarning("Could not create music.",Traceback,Engine);
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

int CacheTexture(char* File, Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Resource.Textures)
        {
            SDL_Surface* Surface = IMG_Load(File);
            if(!Surface)
            {
                char Traceback[STRING_BUFFER_SIZE];
                snprintf(Traceback,STRING_BUFFER_SIZE,"CacheTexture(%s, 0x%X)",File,Engine);
                ThrowWarning("Could not create surface.",Traceback,Engine);
                return(WARNING_SDL_FAILURE);
            }

            SDL_Texture* NewTexture = SDL_CreateTextureFromSurface(Engine->Video.Renderer,Surface);
            SDL_FreeSurface(Surface);
            if(!NewTexture)
            {
                char Traceback[STRING_BUFFER_SIZE];
                snprintf(Traceback,STRING_BUFFER_SIZE,"CacheTexture(%s, 0x%X)",File,Engine);
                ThrowWarning("Could not create texture.",Traceback,Engine);
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
