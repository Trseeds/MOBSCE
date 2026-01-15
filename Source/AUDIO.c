#include "MOBSCE.h"

int InitAudio(Engine* Engine)
{
    if(Engine)
    {
        int Result = Mix_OpenAudio(Engine->Audio.Samplerate,MIX_DEFAULT_FORMAT,Engine->Audio.Channels,Engine->Audio.Chunksize);
        if(Result != 0)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"InitAudio(0x%X)",Engine);
            ThrowError("Failed to intialize audio!",Traceback,Engine);
            return(1);
        }

        Result = Mix_AllocateChannels(Engine->Audio.Voices);
        if(Result != Engine->Audio.Voices)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"InitAudio(0x%X)",Engine);
            ThrowError("Failed to allocate voices!",Traceback,Engine);
            return(2);
        }

        return(0);
    }
    return(INVALID_ENGINE);
}

int* EasyPan(int Pan, int Max, int* Output)
{
    if(Pan < 1)
    {
        Pan = 1;
    }
    if(Pan > Max)
    {
        Pan = Max;
    }
    Output[1] = LinearMap(Pan,Max,255,1);
    Output[0] = 255 - Output[1];
    return(Output);
}

int PlaySound(int SoundID, int Voice, int Volume, int Pan, Engine* Engine)
{
    if(Engine)
    {
        if(!Engine->Resource.Sounds[SoundID])
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"PlaySound(%d, %d, %d, %d, 0x%X)",SoundID,Voice,Volume,Pan,Engine);
            ThrowWarning("Sound is not valid.",Traceback);
            return(1);
        }
        
        Mix_HaltChannel(Voice);
        if(Engine->Audio.Muted)
        {
            return(0);
        }

        int MixVolume = LinearMap(Volume,100,128,1);
        int RealVolume = LinearMap(MixVolume,128,Engine->Audio.SoundVolume,1);

        int PanArr[2];
        EasyPan(Pan, Engine->Video.LogicalDimensions.X, PanArr);


        Mix_Volume(Voice,RealVolume);
        Mix_SetPanning(Voice,PanArr[0],PanArr[1]);
        int Result = Mix_PlayChannel(Voice, Engine->Resource.Sounds[SoundID], 0);
        if(Result < 0)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"PlaySound(%d, %d, %d, %d, 0x%X)",SoundID,Voice,Volume,Pan,Engine);
            ThrowWarning("Could not play sound.",Traceback);
            return(2);
        }
        return(0);
    }
    return(INVALID_ENGINE);
}

void MixMusicVolume(Engine* Engine)
{
    if(Engine)
    {
        Mix_VolumeMusic(Engine->Audio.MusicVolume);
    }
}

int PlayMusic(int MusicID, Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Resource.Music[MusicID])
        {
            if(Engine->Audio.Muted)
            {
                return(0);
            }
            if(Mix_PlayMusic(Engine->Resource.Music[MusicID],-1) < 0)
            {
                char Traceback[STRING_BUFFER_SIZE];
                snprintf(Traceback,STRING_BUFFER_SIZE,"PlayMusic(%d, 0x%X)",MusicID,Engine);
                ThrowWarning("Could not play music.",Traceback);
                return(2);
            }
            return(0);
        }
        char Traceback[STRING_BUFFER_SIZE];
        snprintf(Traceback,STRING_BUFFER_SIZE,"PlayMusic(%d, 0x%X)",MusicID,Engine);
        ThrowWarning("Music is not valid.",Traceback);
        return(1);
    }
    return(INVALID_ENGINE);
}