#include "MOBSCE.h"

void GetInput(Engine* Engine)
{
    if(Engine)
    {
        Engine->Input.SDL_Keystate = SDL_GetKeyboardState(NULL);
        Engine->Input.SDL_MouseState = SDL_GetMouseState(&Engine->Input.MousePosition.X,&Engine->Input.MousePosition.Y);
        memset(Engine->Input.KeysUp,0,sizeof(Engine->Input.KeysUp));
        memset(Engine->Input.KeysDown,0,sizeof(Engine->Input.KeysDown));
        memset(Engine->Input.MouseUp,0,sizeof(Engine->Input.MouseUp));
        memset(Engine->Input.MouseDown,0,sizeof(Engine->Input.MouseDown));
        //keyboard
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
        //mouse
        if(Engine->Input.SDL_MouseState & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            Engine->Input.MouseDown[0] = true;
        }
        if(Engine->Input.SDL_MouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            Engine->Input.MouseDown[1] = true;
        }
        if(Engine->Input.SDL_MouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE))
        {
            Engine->Input.MouseDown[2] = true;
        }
        if(Engine->Input.SDL_MouseState & SDL_BUTTON(SDL_BUTTON_X1))
        {
            Engine->Input.MouseDown[3] = true;
        }
        if(Engine->Input.SDL_MouseState & SDL_BUTTON(SDL_BUTTON_X2))
        {
            Engine->Input.MouseDown[4] = true;
        }
        if(!(Engine->Input.SDL_MouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) && (Engine->Input.SDL_PreviousMouseState & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            Engine->Input.MouseUp[0] = true;
        }
        if(!(Engine->Input.SDL_MouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) && (Engine->Input.SDL_PreviousMouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)))
        {
            Engine->Input.MouseUp[1] = true;
        }
        if(!(Engine->Input.SDL_MouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) && (Engine->Input.SDL_PreviousMouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)))
        {
            Engine->Input.MouseUp[2] = true;
        }
        if(!(Engine->Input.SDL_MouseState & SDL_BUTTON(SDL_BUTTON_X1)) && (Engine->Input.SDL_PreviousMouseState & SDL_BUTTON(SDL_BUTTON_X1)))
        {
            Engine->Input.MouseUp[3] = true;
        }
        if(!(Engine->Input.SDL_MouseState & SDL_BUTTON(SDL_BUTTON_X2)) && (Engine->Input.SDL_PreviousMouseState & SDL_BUTTON(SDL_BUTTON_X2)))
        {
            Engine->Input.MouseUp[4] = true;
        }
        
        memcpy(Engine->Input.SDL_PreviousKeystate,Engine->Input.SDL_Keystate,sizeof(Engine->Input.SDL_PreviousKeystate));
        Engine->Input.SDL_PreviousMouseState = Engine->Input.SDL_MouseState;
    }
}