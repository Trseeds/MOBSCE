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