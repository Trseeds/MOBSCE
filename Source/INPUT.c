#include "MOBSCE.h"

int InitInput(Engine* Engine)
{
    Engine->Input.SDL_Keystate = (Uint8*)calloc(1,sizeof(Uint8*)*SDL_NUM_SCANCODES);
    if(!Engine->Input.SDL_Keystate)
    {
        char Traceback[ERROR_BUFFER_SIZE];
        snprintf(Traceback,ERROR_BUFFER_SIZE,"InitInput(%X)",Engine);
        ThrowError("Failed to allocate memory!",Traceback,Engine);
        return(1);
    }
}

void GetInput(Engine* Engine)
{
    Engine->Input.SDL_Keystate = SDL_GetKeyboardState(NULL);
    memset(Engine->Input.KeysUp,0,sizeof(Engine->Input.KeysUp));
    memset(Engine->Input.KeysDown,0,sizeof(Engine->Input.KeysDown));

    for(int i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        if(Engine->Input.SDL_Keystate[i])
        {
            Engine->Input.KeysDown[i] = true;
        }
        if(Engine->Input.SDL_PreviousKeystate && !Engine->Input.SDL_Keystate)
        {
            Engine->Input.KeysUp[i] = true;
        }
    }
    
    memcpy(Engine->Input.SDL_PreviousKeystate,Engine->Input.SDL_Keystate,sizeof(Engine->Input.SDL_PreviousKeystate));
}

void CleanupInput(Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Input.SDL_Keystate)
        {
            free(Engine->Input.SDL_Keystate);
        }
    }
}