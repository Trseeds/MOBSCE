#include "MOBSCE.h"

int InitSpritePool(Engine* Engine)
{
    Engine->Sprites = (Sprite**)calloc(MIN_ALLOCATE,sizeof(Sprite*));
    if(!Engine->Sprites)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"InitSpritePool(0x%X)",Engine);
        ThrowError("Failed to allocate memory!",Traceback,Engine);
        return(1);
    }
    Engine->Resource.AllocatedSpriteMem = MIN_ALLOCATE;
    Engine->Resource.NumberOfSprites = 0;
    return(0);
}

int ExtendSpritePool(Engine* Engine)
{
    Sprite** OldPtr = Engine->Sprites;
    int NewSize = Engine->Resource.AllocatedSpriteMem+MIN_ALLOCATE;
    Engine->Resource.AllocatedSpriteMem += MIN_ALLOCATE;
    Engine->Sprites = (Sprite**)realloc(Engine->Sprites,sizeof(Sprite*)*NewSize);
    if(!Engine->Sprites)
    {
        Engine->Sprites = OldPtr;
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"ExtendSpritePool(0x%X)",Engine);
        ThrowError("Failed to allocate extended memory!",Traceback,Engine);
        return(1);
    }

    for(int i = Engine->Resource.AllocatedSpriteMem-MIN_ALLOCATE; i < Engine->Resource.AllocatedSpriteMem; i++)
    {
        Engine->Sprites[i] = NULL;
    }

    return(0);
}

void CleanupSpritePool(Engine* Engine)
{
    for(int i = 0; i < Engine->Resource.AllocatedSpriteMem; i++)
    {
        free(Engine->Sprites[i]);
    }
    free(Engine->Sprites);
}

int CreateSprite(char* Name, Vector3 Position, Vector4 Origin, Vector2 Dimensions, int TextureID, int Visible, void (*Routine)(struct Sprite*, struct Engine*), Engine* Engine)
{
    Sprite* NewSprite = (Sprite*)calloc(1,sizeof(Sprite));
    if(!NewSprite)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"CreateSprite(%s, 0x%X, 0x%X, 0x%X, %d, %d, 0x%X, 0x%X)",Name,Position,Origin,Dimensions,TextureID,Visible,Routine,Engine);
        ThrowError("Failed to allocate memory!",Traceback,Engine);
        return(1);
    }

    if(Engine->Resource.NumberOfSprites+1 == Engine->Resource.AllocatedSpriteMem)
    {
        ExtendSpritePool(Engine);
    }

    strcpy(NewSprite->Name,Name);
    NewSprite->Position.X = Position.X; NewSprite->Position.Y = Position.Y; NewSprite->Position.Z = Position.Z;
    NewSprite->Origin.X = Origin.X; NewSprite->Origin.Y = Origin.Y; NewSprite->Origin.Z = Origin.Z; NewSprite->Origin.W = Origin.W; 
    NewSprite->Dimensions.X = Dimensions.X; NewSprite->Dimensions.Y = Dimensions.Y;
    NewSprite->TextureID = TextureID;
    NewSprite->Texture = Engine->Resource.Textures[TextureID];
    NewSprite->Visible = Visible;
    NewSprite->Routine = Routine;

    Engine->Sprites[Engine->Resource.NumberOfSprites] = NewSprite;
    Engine->Resource.NumberOfSprites++;
    return(0);
}

int InitActorPool(Engine* Engine)
{
    Engine->Actors = (Actor**)calloc(MIN_ALLOCATE,sizeof(Actor*));
    if(!Engine->Actors)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"InitActorPool(0x%X)",Engine);
        ThrowError("Failed to allocate memory!",Traceback,Engine);
        return(1);
    }
    Engine->Resource.AllocatedActorMem = MIN_ALLOCATE;
    Engine->Resource.NumberOfActors = 0;
    return(0);
}

int ExtendActorPool(Engine* Engine)
{
    Actor** OldPtr = Engine->Actors;
    int NewSize = Engine->Resource.AllocatedActorMem+MIN_ALLOCATE;
    Engine->Resource.AllocatedActorMem += MIN_ALLOCATE;
    Engine->Actors = (Actor**)realloc(Engine->Actors,sizeof(Actor*)*NewSize);
    if(!Engine->Actors)
    {
        Engine->Actors = OldPtr;
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"ExtendActorPool(0x%X)",Engine);
        ThrowError("Failed to allocate extended memory!",Traceback,Engine);
        return(1);
    }

    for(int i = Engine->Resource.AllocatedActorMem-MIN_ALLOCATE; i < Engine->Resource.AllocatedActorMem; i++)
    {
        Engine->Actors[i] = NULL;
    }

    return(0);
}

void CleanupActorPool(Engine* Engine)
{
    for(int i = 0; i < Engine->Resource.AllocatedActorMem; i++)
    {
        free(Engine->Actors[i]);
    }
    free(Engine->Actors);
}

int CreateActor(char* Name, Vector2 Position, Vector2 Dimensions, int Voice, void (*Routine)(struct Actor*, struct Engine*), Engine* Engine)
{
    Actor* NewActor = (Actor*)calloc(1,sizeof(Actor));
    if(!NewActor)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"CreateActor(%s, 0x%X, 0x%X, %d, 0x%X, 0x%X)",Name,Position,Dimensions,Voice,Routine,Engine);
        ThrowError("Failed to allocate memory!",Traceback,Engine);
        return(1);
    }

    if(Engine->Resource.NumberOfActors+1 == Engine->Resource.AllocatedActorMem)
    {
        ExtendActorPool(Engine);
    }

    strcpy(NewActor->Name,Name);
    NewActor->Position.X = Position.X; NewActor->Position.Y = Position.Y;
    NewActor->Dimensions.X = Dimensions.X; NewActor->Dimensions.Y = Dimensions.Y;
    NewActor->Voice = Voice;
    NewActor->Routine = Routine;

    Engine->Actors[Engine->Resource.NumberOfActors] = NewActor;
    Engine->Resource.NumberOfActors++;
    return(0);
}

int InitSoundCache(Engine* Engine)
{
    Engine->Resource.Sounds = (Mix_Chunk**)calloc(MIN_ALLOCATE,sizeof(Mix_Chunk*));
    if(!Engine->Resource.Sounds)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"InitSoundCache(0x%X)",Engine);
        ThrowError("Failed to allocate memory!",Traceback,Engine);
        return(1);
    }
    Engine->Resource.AllocatedSoundMem = MIN_ALLOCATE;
    Engine->Resource.NumberOfSounds = 0;
    return(0);
}

int ExtendSoundCache(Engine* Engine)
{
    Mix_Chunk** OldPtr = Engine->Resource.Sounds;
    int NewSize = Engine->Resource.AllocatedSoundMem+MIN_ALLOCATE;
    Engine->Resource.AllocatedSoundMem += MIN_ALLOCATE;
    Engine->Resource.Sounds = (Mix_Chunk**)realloc(Engine->Resource.Sounds,sizeof(Mix_Chunk*)*NewSize);
    if(!Engine->Resource.Sounds)
    {
        Engine->Resource.Sounds = OldPtr;
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"ExtendSoundCache(0x%X)",Engine);
        ThrowError("Failed to allocate extended memory!",Traceback,Engine);
        return(1);
    }

    for(int i = Engine->Resource.AllocatedSoundMem-MIN_ALLOCATE; i < Engine->Resource.AllocatedSoundMem; i++)
    {
        Engine->Resource.Sounds[i] = NULL;
    }

    return(0);
}

void CleanupSoundCache(Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Resource.Sounds)
        {
            for(int i = 0; i < Engine->Resource.AllocatedSoundMem; i++)
            {
                if(Engine->Resource.Sounds[i])
                {
                    Mix_FreeChunk(Engine->Resource.Sounds[i]);
                }
            }
        }
    }
}

int CacheSound(char* File, Engine* Engine)
{
    Mix_Chunk* NewSound = Mix_LoadWAV(File);

    if(!NewSound)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"CacheSound(%s, 0x%X)",File,Engine);
        ThrowWarning("Could not create sound.",Traceback);
        return(1);
    }

    if(Engine->Resource.NumberOfSounds+1 == Engine->Resource.AllocatedSoundMem)
    {
        ExtendSoundCache(Engine);
    }

    Engine->Resource.Sounds[Engine->Resource.NumberOfSounds] = NewSound;
    Engine->Resource.NumberOfSounds++;
    return(0);
}

int InitMusicCache(Engine* Engine)
{
    Engine->Resource.Music = (Mix_Music**)calloc(MIN_ALLOCATE,sizeof(Mix_Music*));
    if(!Engine->Resource.Music)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"InitMusicCache(0x%X)",Engine);
        ThrowError("Failed to allocate memory!",Traceback,Engine);
        return(1);
    }
    Engine->Resource.AllocatedMusicMem = MIN_ALLOCATE;
    Engine->Resource.NumberOfSounds = 0;
    return(0);
}

int ExtendMusicCache(Engine* Engine)
{
    Mix_Music** OldPtr = Engine->Resource.Music;
    int NewSize = Engine->Resource.AllocatedMusicMem+MIN_ALLOCATE;
    Engine->Resource.AllocatedMusicMem += MIN_ALLOCATE;
    Engine->Resource.Music = (Mix_Music**)realloc(Engine->Resource.Music,sizeof(Mix_Music*)*NewSize);
    if(!Engine->Resource.Music)
    {
        Engine->Resource.Music = OldPtr;
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"ExtendMusicCache(0x%X)",Engine);
        ThrowError("Failed to allocate extended memory!",Traceback,Engine);
        return(1);
    }

    for(int i = Engine->Resource.AllocatedMusicMem-MIN_ALLOCATE; i < Engine->Resource.AllocatedMusicMem; i++)
    {
        Engine->Resource.Music[i] = NULL;
    }

    return(0);
}

void CleanupMusicCache(Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Resource.Music)
        {
            for(int i = 0; i < Engine->Resource.AllocatedMusicMem; i++)
            {
                if(Engine->Resource.Music[i])
                {
                    Mix_FreeMusic(Engine->Resource.Music[i]);
                }
            }
        }
    }
}

int CacheMusic(char* File, Engine* Engine)
{
    Mix_Music* NewMusic = Mix_LoadMUS(File);

    if(!NewMusic)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"CacheMusic(%s, 0x%X)",File,Engine);
        ThrowWarning("Could not create music.",Traceback);
        return(1);
    }

    if(Engine->Resource.NumberOfMusics+1 == Engine->Resource.AllocatedMusicMem)
    {
        ExtendMusicCache(Engine);
    }

    Engine->Resource.Music[Engine->Resource.NumberOfMusics] = NewMusic;
    Engine->Resource.NumberOfMusics++;
    return(0);
}

int InitTextureCache(Engine* Engine)
{
    Engine->Resource.Textures = (SDL_Texture**)calloc(MIN_ALLOCATE,sizeof(SDL_Texture*));
    if(!Engine->Resource.Textures)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"InitTextureCache(0x%X)",Engine);
        ThrowError("Failed to allocate memory!",Traceback,Engine);
        return(1);
    }
    Engine->Resource.AllocatedTextureMem = MIN_ALLOCATE;
    Engine->Resource.NumberOfTextures = 0;
    return(0);
}

int ExtendTextureCache(Engine* Engine)
{
    SDL_Texture** OldPtr = Engine->Resource.Textures;
    int NewSize = Engine->Resource.AllocatedTextureMem+MIN_ALLOCATE;
    Engine->Resource.AllocatedTextureMem += MIN_ALLOCATE;
    Engine->Resource.Textures = (SDL_Texture**)realloc(Engine->Resource.Textures,sizeof(SDL_Texture*)*NewSize);
    if(!Engine->Resource.Textures)
    {
        Engine->Resource.Textures = OldPtr;
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"ExtendTextureCache(0x%X)",Engine);
        ThrowError("Failed to allocate extended memory!",Traceback,Engine);
        return(1);
    }

    for(int i = Engine->Resource.AllocatedTextureMem-MIN_ALLOCATE; i < Engine->Resource.AllocatedTextureMem; i++)
    {
        Engine->Resource.Textures[i] = NULL;
    }

    return(0);
}

void CleanupTextureCache(Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Resource.Textures)
        {
            for(int i = 0; i < Engine->Resource.AllocatedTextureMem; i++)
            {
                if(Engine->Resource.Textures[i])
                {
                    SDL_DestroyTexture(Engine->Resource.Textures[i]);
                }
            }
        }
    }
}

int CacheTexture(char* File, Engine* Engine)
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

        if(Engine->Resource.NumberOfTextures+1 == Engine->Resource.AllocatedTextureMem)
        {
            ExtendTextureCache(Engine);
        }

        Engine->Resource.Textures[Engine->Resource.NumberOfTextures] = NewTexture;
        Engine->Resource.NumberOfTextures++;
        return(0);
    }
}
