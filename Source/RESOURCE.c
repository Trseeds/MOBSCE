#include "MOBSCE.h"

int InitResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources)
        {
            *(void**)ResourceInfo.Pointer = calloc(MIN_ALLOCATE,sizeof(void*));
            if(!*(void**)ResourceInfo.Pointer)
            {
                char Traceback[ERROR_BUFFER_SIZE];
                snprintf(Traceback,ERROR_BUFFER_SIZE,"InitResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
                ThrowError("Failed to allocate memory!",Traceback,Engine);
                return(1);
            }

            *(int*)ResourceInfo.AllocatedResourceMemory = MIN_ALLOCATE;
            *(int*)ResourceInfo.NumberOfResources = 0;
            return(0);
        }       
    }
    return(-1);
}

int ExtendResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources)
        {
            void** OldPtr = *(void**)ResourceInfo.Pointer;
            int NewSize = *(int*)ResourceInfo.AllocatedResourceMemory+MIN_ALLOCATE;
            *(void**)ResourceInfo.Pointer = realloc(*(void**)ResourceInfo.Pointer,NewSize*sizeof(void*));
            if(!*(void**)ResourceInfo.Pointer)
            {
                *(void**)ResourceInfo.Pointer = OldPtr;
                char Traceback[ERROR_BUFFER_SIZE];
                snprintf(Traceback,ERROR_BUFFER_SIZE,"ExtendResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
                ThrowError("Failed to allocate new memory!",Traceback,Engine);
                return(1);
            }

            *(int*)ResourceInfo.AllocatedResourceMemory += MIN_ALLOCATE;
            void** Pool = *(void**)ResourceInfo.Pointer;

            for(int i = *(int*)ResourceInfo.AllocatedResourceMemory-MIN_ALLOCATE; i < *(int*)ResourceInfo.AllocatedResourceMemory; i++)
            {
                Pool[i] = NULL;
            }

            return(0);
        }
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"CleanupResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
        ThrowError("Invalid Resource Info! No more memory can be allocated!",Traceback,Engine);
        return(2);
    }
    return(-1);
}

int ShrinkResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources)
        {
            void** OldPtr = *(void**)ResourceInfo.Pointer;
            int NewSize = *(int*)ResourceInfo.AllocatedResourceMemory-MIN_ALLOCATE;
            *(void**)ResourceInfo.Pointer = realloc(*(void**)ResourceInfo.Pointer,NewSize*sizeof(void*));
            if(!*(void**)ResourceInfo.Pointer)
            {
                *(void**)ResourceInfo.Pointer = OldPtr;
                char Traceback[ERROR_BUFFER_SIZE];
                snprintf(Traceback,ERROR_BUFFER_SIZE,"ShrinkResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
                ThrowError("Failed to allocate new memory!",Traceback,Engine);
                return(1);
            }
            *(int*)ResourceInfo.AllocatedResourceMemory -= MIN_ALLOCATE;
        }
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"ShrinkResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
        ThrowWarning("Invalid Resource Info! Skipping shrink. (this is a memory leak, you must fix it.)",Traceback);
        return(2);
    }
    return(-1);
}

void CleanupResourcePool(ResourceInfo ResourceInfo, Engine* Engine)
{
    if(Engine)
    {
        if(ResourceInfo.Pointer && ResourceInfo.AllocatedResourceMemory && ResourceInfo.NumberOfResources && ResourceInfo.FreeFunction)
        {
            void** Pool = (void**)ResourceInfo.Pointer;

            for(int i = 0; i < *(int*)ResourceInfo.AllocatedResourceMemory; i++)
            {
                if(Pool[i])
                {
                    ResourceInfo.FreeFunction(Pool[i]);
                }
            }
            free(Pool);
            return;
        }
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"CleanupResourcePool(0x%X, 0x%X)",&ResourceInfo,Engine);
        ThrowWarning("Invalid Resource Info! Skipping free. (this is a memory leak, you must fix it.)",Traceback);
        return;
    }
}

int CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, int TextureID, int Visible, Actor* Actor, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine)
{
    if(Engine)
    {
        Sprite* NewSprite = (Sprite*)calloc(1,sizeof(Sprite));
        if(!NewSprite)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"CreateSprite(%s, 0x%X, 0x%X, 0x%X, %d, %d, 0x%X, 0x%X)",Name,Position,Origin,Dimensions,TextureID,Visible,Routine,Engine);
            ThrowError("Failed to allocate memory!",Traceback,Engine);
            return(1);
        }

        if(Engine->Resource.NumberOfSprites+1 >= Engine->Resource.AllocatedSpriteMemory)
        {
            ResourceInfo ResourceInfo;
            ResourceInfo.Pointer = &Engine->Sprites;
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSprites;
            ExtendResourcePool(ResourceInfo,Engine);
        }

        strcpy(NewSprite->Name,Name);
        NewSprite->ID = GetNewObjectID(Engine);
        NewSprite->Position.X = Position.X; NewSprite->Position.Y = Position.Y; NewSprite->Position.Z = Position.Z;
        NewSprite->Origin.X = Origin.X; NewSprite->Origin.Y = Origin.Y; NewSprite->Origin.Z = Origin.Z; NewSprite->Origin.W = Origin.W; 
        NewSprite->Dimensions.X = Dimensions.X; NewSprite->Dimensions.Y = Dimensions.Y;
        NewSprite->TextureID = TextureID;
        NewSprite->Texture = Engine->Resource.Textures[TextureID];
        NewSprite->Visible = Visible;
        NewSprite->Actor = Actor;
        NewSprite->Routine = Routine;

        Engine->Sprites[Engine->Resource.NumberOfSprites] = NewSprite;
        Engine->Resource.NumberOfSprites++;
        Engine->SpriteZResortNeeded = true;
        return(0);
    }
    return(-1);
}

void DestroySprite(Sprite* DSprite, Engine* Engine)
{
    if(Engine)
    {
        if(DSprite)
        {
            int Index;
            for(int i = 0; i < Engine->Resource.NumberOfSprites; i++)
            {
                if(Engine->Sprites[i]->ID == DSprite->ID)
                {
                    Engine->Sprites[i] = NULL;
                }
            }

            free(DSprite);
            qsort(Engine->Sprites,Engine->Resource.NumberOfSprites,sizeof(Sprite*),CompactArray);
            Engine->Resource.NumberOfSprites--;

            if(ArrayCanBeShrunk(Engine->Actors,Engine->Resource.AllocatedActorMemory,Engine->Resource.NumberOfActors))
            {
                ResourceInfo ResourceInfo;
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSpriteMemory;
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSprites;
                ShrinkResourcePool(ResourceInfo,Engine);
            }
        }
    }
}

int CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, int Voice, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine)
{
    if(Engine)
    {
        Actor* NewActor = (Actor*)calloc(1,sizeof(Actor));
        if(!NewActor)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"CreateActor(%s, 0x%X, 0x%X, %d, 0x%X, 0x%X)",Name,Position,Dimensions,Voice,Routine,Engine);
            ThrowError("Failed to allocate memory!",Traceback,Engine);
            return(1);
        }

        if(Engine->Resource.NumberOfActors+1 >= Engine->Resource.AllocatedActorMemory)
        {
            ResourceInfo ResourceInfo;
            ResourceInfo.Pointer = &Engine->Actors;
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedActorMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfActors;
            ExtendResourcePool(ResourceInfo,Engine);
        }

        strcpy(NewActor->Name,Name);
        NewActor->ID = GetNewObjectID(Engine);
        NewActor->Position.X = Position.X; NewActor->Position.Y = Position.Y;
        NewActor->Dimensions.X = Dimensions.X; NewActor->Dimensions.Y = Dimensions.Y;
        NewActor->Voice = Voice;
        NewActor->Routine = Routine;

        Engine->Actors[Engine->Resource.NumberOfActors] = NewActor;
        Engine->Resource.NumberOfActors++;
        return(0);
    }
    return(-1);
}

void DestroyActor(Actor* DActor, Engine* Engine)
{
    if(Engine)
    {
        if(DActor)
        {
            Actor* OldPtr = DActor;
            int Index;
            for(int i = 0; i < Engine->Resource.NumberOfActors; i++)
            {
                if(Engine->Actors[i]->ID == DActor->ID)
                {
                    Engine->Actors[i] = NULL;
                }
            }
            
            free(DActor);
            qsort(Engine->Actors, Engine->Resource.NumberOfActors, sizeof(Actor*), CompactArray);
            Engine->Resource.NumberOfActors--;
            
            if(ArrayCanBeShrunk(Engine->Actors,Engine->Resource.AllocatedActorMemory,Engine->Resource.NumberOfActors))
            {
                ResourceInfo ResourceInfo;
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedActorMemory;
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfActors;
                ShrinkResourcePool(ResourceInfo,Engine);
            }

            for(int i = 0; i < Engine->Resource.NumberOfSprites; i++)
            {
                if(Engine->Sprites[i])
                {
                    if(Engine->Sprites[i]->Actor == OldPtr)
                    {
                        Engine->Sprites[i]->Actor = NULL;
                    }
                }
            }
        }
    }
}

int CacheSound(char* File, Engine* Engine)
{
    if(Engine)
    {
        Mix_Chunk* NewSound = Mix_LoadWAV(File);

        if(!NewSound)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"CacheSound(%s, 0x%X)",File,Engine);
            ThrowWarning("Could not create sound.",Traceback);
            return(1);
        }

        if(Engine->Resource.NumberOfSounds+1 >= Engine->Resource.AllocatedSoundMemory)
        {
            ResourceInfo ResourceInfo;
            ResourceInfo.Pointer = &Engine->Resource.Sounds;
            ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedSoundMemory;
            ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfSounds;
            ExtendResourcePool(ResourceInfo,Engine);
        }

        Engine->Resource.Sounds[Engine->Resource.NumberOfSounds] = NewSound;
        Engine->Resource.NumberOfSounds++;
        return(0);
    }
    return(-1);
}

int CacheMusic(char* File, Engine* Engine)
{
    if(Engine)
    {
        Mix_Music* NewMusic = Mix_LoadMUS(File);

        if(!NewMusic)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"CacheMusic(%s, 0x%X)",File,Engine);
            ThrowWarning("Could not create music.",Traceback);
            return(1);
        }

        if(Engine->Resource.NumberOfMusics+1 >= Engine->Resource.AllocatedMusicMemory)
        {
            ResourceInfo ResourceInfo;
                ResourceInfo.Pointer = &Engine->Resource.Music;
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedMusicMemory;
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfMusics;
                ExtendResourcePool(ResourceInfo,Engine);
        }

        Engine->Resource.Music[Engine->Resource.NumberOfMusics] = NewMusic;
        Engine->Resource.NumberOfMusics++;
        return(0);
    }
    return(-1);
}

int CacheTexture(char* File, Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Resource.Textures)
        {
            SDL_Surface* Surface = SDL_LoadBMP(File);
            if(!Surface)
            {
                char Traceback[ERROR_BUFFER_SIZE];
                snprintf(Traceback,ERROR_BUFFER_SIZE,"CacheTexture(%s, 0x%X)",File,Engine);
                ThrowWarning("Could not create surface.",Traceback);
                return(1);
            }

            SDL_Texture* NewTexture = SDL_CreateTextureFromSurface(Engine->Video.Renderer,Surface);
            SDL_FreeSurface(Surface);
            if(!NewTexture)
            {
                char Traceback[ERROR_BUFFER_SIZE];
                snprintf(Traceback,ERROR_BUFFER_SIZE,"CacheTexture(%s, 0x%X)",File,Engine);
                ThrowWarning("Could not create texture.",Traceback);
                return(2);   
            }

            if(Engine->Resource.NumberOfTextures+1 >= Engine->Resource.AllocatedTextureMemory)
            {
                ResourceInfo ResourceInfo;
                ResourceInfo.Pointer = &Engine->Resource.Textures;
                ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTextureMemory;
                ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTextures;
                ExtendResourcePool(ResourceInfo,Engine);
            }

            Engine->Resource.Textures[Engine->Resource.NumberOfTextures] = NewTexture;
            Engine->Resource.NumberOfTextures++;
            return(0);
        }
    }
    return(-1);
}
