//more error management than audio related work.

#include "MOBSCE.h"

int8 InitAudio(Engine* Engine)
{
    const char Name[] = "InitAudio";
    const char Parameters[] = "(Engine* Engine: 0x%p)";
    if(Engine)
    {
        int Result = Mix_OpenAudio(Engine->Audio.Samplerate,MIX_DEFAULT_FORMAT,Engine->Audio.Channels,Engine->Audio.Chunksize);
        if(Result != 0)
        {
            Error Error = {WARNING_SDL_FAILURE,"\0","Failed to intialize audio.","\0","\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_OpenAudio failed and returned %d",Result);
            ThrowError(&Error,Engine);
            return(WARNING_SDL_FAILURE);
        }

        Result = Mix_AllocateChannels(Engine->Audio.Voices);
        if(Result != Engine->Audio.Voices)
        {
            Error Error = {WARNING_SDL_FAILURE,"\0","Failed to allocate Voices.","\0","\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_AllocateChannels failed and returned %d",Result);
            ThrowWarning(&Error,Engine);
            return(WARNING_SDL_FAILURE);
        }

        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

uint8 ScreenPan(int32 X, uint32 EdgeDistance, Engine* Engine)
/*edge distance is how far away from the screen boundaries x can be before becoming fully silent.
Note that this function is incapable of controlling the volume; use in conjunction with ScreenVolume.
*/
{
    int64 DomainMin = 0 - (int64)EdgeDistance;
    int64 DomainMax = Engine->Video.LogicalDimensions.X + (int64)EdgeDistance;
    int64 Pan = LinearMap(X,DomainMin,DomainMax,0,100);
    if(Pan > 100)
    {
        Pan = 100;
    }
    if(Pan < 0)
    {
        Pan = 0;
    }
    return((uint8)Pan);
}

uint8 ScreenVolume(int32 X, uint32 EdgeDistance, Engine* Engine)
{
    int64 DomainMin = 0 - (int64)EdgeDistance;
    int64 DomainMax = Engine->Video.LogicalDimensions.X + (int64)EdgeDistance;
    int64 Volume = LinearMap(X,DomainMin,DomainMax,0,200);
    if(Volume > 100)
    {
        Volume = 200 - Volume;
    }
    if(Volume < 0)
    {
        Volume = 0;
    }
    return((uint8)Volume);
}

int8 PlaySound(Mix_Chunk* Sound, int32 Voice, uint8 Volume, uint8 Pan, Engine* Engine)
{
    const char Name[] = "PlaySound";
    const char Parameters[] = "(Mix_Chunk* Sound: 0x%p, int Voice: %d, int Volume: %d, int Pan: %d, Engine* Engine: 0x%p)";
    if(Engine)
    {
        
        if(!Sound)
        {
            Error Error = {WARNING_INVALID_PARAMETER,"\0","Sound is not valid.","\0","\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Sound,Voice,Volume,Pan,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Pointer to sound is NULL.");
            ThrowWarning(&Error,Engine);
            return(WARNING_INVALID_PARAMETER);
        }
        
        if(Engine->Audio.Muted)
        {
            return(RETURN_SUCCESS);
        }

        if(Volume > 100)
        {
            Volume = 100;
        }
        if(Volume < 0)
        {
            Volume = 0;
        }
        int MixVolume = LinearMap(Volume,0,100,0,255);
        int RealVolume = LinearMap(MixVolume,0,128,0,Engine->Audio.SoundVolume);

        int RPan[2];
        int FPan = LinearMap(Pan,0,100,0,255);
        RPan[0] = 255 - FPan;
        RPan[1] = FPan;

        if(Voice != -1)
        {
            Mix_HaltChannel(Voice);
        }
        else
        {
            Error Error = {WARNING_INVALID_PARAMETER,"\0","Sound will not pan and volume will not change.","\0","\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Sound,Voice,Volume,Pan,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Sounds played on channel -1 can't be panned and will not have their volumes changed. Sound will play at the maximum volume in all channels.");
            ThrowWarning(&Error,Engine);
            RealVolume = 128;
        }

        Mix_Volume(Voice,RealVolume);
        Mix_SetPanning(Voice,RPan[0],RPan[1]);
        int Result = Mix_PlayChannel(Voice, Sound, 0);
        if(Result < 0)
        {
            Error Error = {WARNING_SDL_FAILURE,"\0","Could not play sound.","\0","\0"};
            snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Sound,Voice,Volume,Pan,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_PlayChannel failed and returned %d.",Result);
            ThrowWarning(&Error,Engine);
            return(WARNING_SDL_FAILURE);
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 MixMusicVolume(Engine* Engine)
{
    if(Engine)
    {
        Audio* A = &Engine->Audio;
        Mix_VolumeMusic(A->MusicVolume);
        if(A->Muted)
        {
            Mix_VolumeMusic(0);
            Mix_HaltChannel(-1);
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 PlayMusic(Mix_Music* Music, int8 Loops, Engine* Engine)
{
    const char Name[] = "PlayMusic";
    const char Parameters[] = "(Mix_Music: 0x%p, int8 Loops: %d, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(Music)
        {
            if(Engine->Audio.Muted)
            {
                return(RETURN_SUCCESS);
            }
            int Result = Mix_PlayMusic(Music,Loops);
            if(Result < 0)
            {
                Error Error = {WARNING_SDL_FAILURE,"\0","Could not play music.","\0","\0"};
                snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Music,Loops,Engine);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_PlayMusic failed and returned %d.",Result);
                ThrowWarning(&Error,Engine);
                return(WARNING_SDL_FAILURE);
            }
            return(RETURN_SUCCESS);
        }
        Error Error = {WARNING_INVALID_PARAMETER,"\0","Music is not valid.","\0","\0"};
        snprintf(Error.Thrower,STRING_BUFFER_SIZE,"%s",Name);
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Music,Loops,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"Music provided is NULL.");
        ThrowWarning(&Error,Engine);
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}