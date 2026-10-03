#include "MOBSCE.h"

int handler(void* user, const char* section, const char* name, const char* value)
{
    Config* NewConfig = (Config*)user;

    /*
    top ten worst things ever:
    #1: string functions in c
    #2: |
        V
    */
    if(strcmp(section,"Audio") == 0)
    {
        if(strcmp(name,"Samplerate") == 0)
        {
            NewConfig->Samplerate = atoi(value);
        }
        else if(strcmp(name,"Channels") == 0)
        {
            NewConfig->Channels = atoi(value);
        }
        else if(strcmp(name,"Chunksize") == 0)
        {
            NewConfig->Chunksize = atoi(value);
        }
        else if(strcmp(name,"Voices") == 0)
        {
            NewConfig->Voices = atoi(value);
        }
        else if(strcmp(name,"MusicVolume") == 0)
        {
            NewConfig->MusicVolume = atoi(value);
        }
        else if(strcmp(name,"SoundVolume") == 0)
        {
            NewConfig->SoundVolume = atoi(value);
        }
        else if(strcmp(name,"Muted") == 0)
        {
            NewConfig->Muted = atoi(value);
        }
        else if (strcmp(name,"Codecs") == 0)
        {
            NewConfig->Codecs = atoi(value);
        }
        else
        {
            return(1);
        }
    }
    else if(strcmp(section,"Video") == 0)
    {
        if(strcmp(name,"LogicalX") == 0)
        {
            NewConfig->LogicalX = atoi(value);
        }
        else if(strcmp(name,"LogicalY") == 0)
        {
            NewConfig->LogicalY = atoi(value);
        }
        else if(strcmp(name,"WindowFlags") == 0)
        {
            NewConfig->WindowFlags = atoi(value);
        }
        else if(strcmp(name,"RendererFlags") == 0)
        {
            NewConfig->RendererFlags = atoi(value);
        }
        else if(strcmp(name,"WindowScale") == 0)
        {
            NewConfig->WindowScale = atof(value);
        }
        else
        {
            return(1);
        }
    }
    else
    {
        return(1);
    }
    return(RETURN_SUCCESS);
}

int8 UpdateConfig(char* File, Config* Config, Engine* Engine)
{
    Config->Samplerate = 48000;
    Config->Channels = 2;
    Config->Chunksize = 1024;
    Config->Voices = 8;
    Config->MusicVolume = 100;
    Config->SoundVolume = 100;
    Config->Muted = false;
    Config->LogicalX = 640;
    Config->LogicalY = 480;
    Config->WindowFlags = SDL_WINDOW_SHOWN;
    Config->RendererFlags = SDL_RENDERER_PRESENTVSYNC;
    Config->Codecs = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_WAVPACK;
    Config->Render = true;
    Config->Audiate = true;
    Config->WindowScale = 1.00;

    int Result = ini_parse(File,handler,Config);
    if(Result < 0)
    {
        Error Error = {WARNING_INIH_FAILURE,"UpdateConfig","Failed to load config file.","\0","\0"};
        snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(char* File: %s, Config* Config: 0x%p, Engine* Engine: 0x%p)",File,Config,Engine);
        snprintf(Error.Description,STRING_BUFFER_SIZE,"ini_parse failed and returned %d.",Result);
        ThrowWarning(&Error,Engine);
        return(WARNING_INIH_FAILURE);
    }
    
    return(RETURN_SUCCESS);
}

int8 LoadEngineConfig(Engine* Engine)
{
    if(Engine)
    {
        Engine->Audio.Samplerate = Engine->Config.Samplerate;
        Engine->Audio.Channels = Engine->Config.Channels;
        Engine->Audio.Chunksize = Engine->Config.Chunksize;
        Engine->Audio.Voices = Engine->Config.Voices;
        Engine->Audio.MusicVolume = Engine->Config.MusicVolume;
        Engine->Audio.SoundVolume = Engine->Config.SoundVolume;
        Engine->Audio.Muted = Engine->Config.Muted;
        Engine->Audio.Codecs = Engine->Config.Codecs;
        Engine->Video.LogicalDimensions.X = Engine->Config.LogicalX;
        Engine->Video.LogicalDimensions.Y = Engine->Config.LogicalY;
        Engine->Video.WindowFlags = Engine->Config.WindowFlags;
        Engine->Video.RendererFlags = Engine->Config.RendererFlags;
        Engine->Video.WindowScale = Engine->Config.WindowScale;
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}