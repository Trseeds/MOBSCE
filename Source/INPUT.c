#include "MOBSCE.h"

void GetInput(Engine* Engine)
{
    if(Engine)
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
            if(Engine->Input.SDL_PreviousKeystate[i] && !Engine->Input.SDL_Keystate[i])
            {
                Engine->Input.KeysUp[i] = true;
            }
        }
        
        memcpy(Engine->Input.SDL_PreviousKeystate,Engine->Input.SDL_Keystate,sizeof(Engine->Input.SDL_PreviousKeystate));
    }
}