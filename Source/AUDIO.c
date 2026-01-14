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
    return(-1);
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

int PlaySound(Mix_Chunk* Sound, int Voice, int Volume, int Pan, Engine* Engine)
{
    if(Engine)
    {
        if(!Sound)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"PlaySound(0x%X, %d, %d, %d, 0x%X)",Sound,Voice,Volume,Pan,Engine);
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
        int Result = Mix_PlayChannel(Voice, Sound, 0);
        if(Result < 0)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"PlaySound(0x%X, %d, %d, %d, 0x%X)",Sound,Voice,Volume,Pan,Engine);
            ThrowWarning("Could not play sound.",Traceback);
            return(2);
        }
        return(0);
    }
    return(-1);
}