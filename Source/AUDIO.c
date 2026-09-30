//more error management than audio related work.

#include "MOBSCE.h"

int8 InitAudio(Engine* Engine)
{
    if(Engine)
    {
        int Result = Mix_OpenAudio(Engine->Audio.Samplerate,MIX_DEFAULT_FORMAT,Engine->Audio.Channels,Engine->Audio.Chunksize);
        if(Result != 0)
        {
            Error Error = {WARNING_SDL_FAILURE,"InitAudio","Failed to intialize audio.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(Engine* Engine: 0x%p)",Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_OpenAudio failed and returned %d",Result);
            ThrowError(&Error,Engine);
            return(WARNING_SDL_FAILURE);
        }

        Result = Mix_AllocateChannels(Engine->Audio.Voices);
        if(Result != Engine->Audio.Voices)
        {
            Error Error = {WARNING_SDL_FAILURE,"InitAudio","Failed to allocate Voices.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(Engine* Engine: 0x%p)",Engine);
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
Note that this function is incapable of controlling the volume, use in conjunction with ScreenVolume.
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

int8 PlaySound(uint32 SoundID, int32 Voice, uint8 Volume, uint8 Pan, Engine* Engine)
{
    if(Engine)
    {
        if(SoundID > Engine->Resource.AllocatedSoundMemory)
        {
            if(!Engine->Resource.Sounds[SoundID])
            {
                Error Error = {WARNING_INVALID_PARAMETER,"PlaySound","Sound is not valid.","\0","\0"};
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(int SoundID: %d, int Voice: %d, int Volume: %d, int Pan: %d, Engine* Engine: 0x%p)",SoundID,Voice,Volume,Pan,Engine);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"Pointer to sound is NULL. (derived from Engine->Resource.Sounds[SoundID], SoundID is out of bounds.)");
                ThrowWarning(&Error,Engine);
                return(WARNING_INVALID_PARAMETER);
            }
            Error Error = {WARNING_INVALID_PARAMETER,"PlaySound","Sound is not valid","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(int SoundID: %d, int Voice: %d, int Volume: %d, int Pan: %d, Engine* Engine: 0x%p)",SoundID,Voice,Volume,Pan,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Sound provided does not and can not exist.");
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
            Error Error = {WARNING_INVALID_PARAMETER,"PlaySound","Sound will not pan and volume will not change.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(int SoundID: %d, int Voice: %d, int Volume: %d, int Pan: %d, Engine* Engine: 0x%p)",SoundID,Voice,Volume,Pan,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Sounds played on channel -1 can't be panned and will not have their volume changed. Sound will play at an equal volume in both channels and that volume will be the maxium volume.");
            ThrowWarning(&Error,Engine);
            RealVolume = 128;
        }

        Mix_Volume(Voice,RealVolume);
        Mix_SetPanning(Voice,RPan[0],RPan[1]);
        int Result = Mix_PlayChannel(Voice, Engine->Resource.Sounds[SoundID], 0);
        if(Result < 0)
        {
            Error Error = {WARNING_SDL_FAILURE,"PlaySound","Could not play sound.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(int SoundID: %d, int Voice: %d, int Volume: %d, int Pan: %d, Engine* Engine: 0x%p)",SoundID,Voice,Volume,Pan,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_PlayChannel failed and returned %d.",Result);
            ThrowWarning(&Error,Engine);
            return(WARNING_SDL_FAILURE);
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int MixMusicVolume(Engine* Engine)
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

int PlayMusic(int32 MusicID, Engine* Engine)
{
    if(Engine)
    {
        if(MusicID < Engine->Resource.AllocatedMusicMemory)
        {
            if(Engine->Resource.Music[MusicID])
            {
                if(Engine->Audio.Muted)
                {
                    return(RETURN_SUCCESS);
                }
                int Result = Mix_PlayMusic(Engine->Resource.Music[MusicID],-1);
                if(Result < 0)
                {
                    Error Error = {WARNING_SDL_FAILURE,"PlayMusic","Could not play music.","\0","\0"};
                    snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(int MusicID: %d, Engine* Engine: 0x%p)",MusicID,Engine);
                    snprintf(Error.Description,STRING_BUFFER_SIZE,"Mix_PlayMusic failed and returned %d.",Result);
                    ThrowWarning(&Error,Engine);
                    return(WARNING_SDL_FAILURE);
                }
                return(RETURN_SUCCESS);
            }
            Error Error = {WARNING_INVALID_PARAMETER,"PlayMusic","Sound is not valid","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(int MusicID: %d, Engine* Engine)",MusicID,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Pointer to music is NULL. (derived from Engine->Resource.Music[MusicID], MusicID is out of bounds.)");
            ThrowWarning(&Error,Engine);
            return(WARNING_INVALID_PARAMETER);
        }
        Error Error = {WARNING_INVALID_PARAMETER,"PlayMusic","Music is not valid.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(int MusicID: %d, Engine* Engine: 0x%p)",MusicID,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"Music provided does not and can not exist.");
        ThrowWarning(&Error,Engine);
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}