#include "MOBSCE.h"

void GetKeyboardInput(Engine* Engine)
{
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
}

void GetMouseInput(Engine* Engine)
{
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
    for(int i = 0; i < EVENT_QUEUE_SIZE; i++)
    {
        if(Engine->Events[i].type == SDL_MOUSEWHEEL)
        {
            if(Engine->Events[i].wheel.y > 0)
            {
                Engine->Input.VerticalMouseScroll = 1; //scroll up
            }
            if(Engine->Events[i].wheel.y < 0)
            {
                Engine->Input.VerticalMouseScroll = -1; //scroll down
            }
            if(Engine->Events[i].wheel.x > 0)
            {
                Engine->Input.HorizontalMouseScroll = 1; //scroll right
            }
            if(Engine->Events[i].wheel.x < 0)
            {
                Engine->Input.HorizontalMouseScroll = -1; //scroll left
            }
        }
    }
}

void GetGamepadInput(Engine* Engine)
{
    for(int i = 0; i < EVENT_QUEUE_SIZE; i++)
    {
        if(Engine->Events[i].type == SDL_CONTROLLERDEVICEADDED)
        {
            Engine->Input.Gamepad = SDL_GameControllerOpen(Engine->Events[i].cdevice.which);
            if(Engine->Input.Gamepad)
            {
                Engine->Input.GamepadIsConnected = true;
            }
        }
        if(Engine->Events[i].type == SDL_CONTROLLERDEVICEREMOVED)
        {
            Engine->Input.Gamepad = NULL;
            Engine->Input.GamepadIsConnected = false;
        }
        if(Engine->Input.GamepadIsConnected && Engine->Input.Gamepad)
        {
            Engine->Input.GamepadButtonsDown[GP_FB_BOTTOM] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_A);
            Engine->Input.GamepadButtonsDown[GP_FB_RIGHT] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_B);
            Engine->Input.GamepadButtonsDown[GP_FB_LEFT] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_X);
            Engine->Input.GamepadButtonsDown[GP_FB_TOP] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_Y);

            Engine->Input.GamepadButtonsDown[GP_FB_START] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_START);
            Engine->Input.GamepadButtonsDown[GP_FB_SELECT] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_BACK);
            Engine->Input.GamepadButtonsDown[GP_FB_SUPER] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_GUIDE);
            
            Engine->Input.GamepadButtonsDown[GP_DP_UP] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_DPAD_UP);
            Engine->Input.GamepadButtonsDown[GP_DP_DOWN] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_DPAD_DOWN);
            Engine->Input.GamepadButtonsDown[GP_DP_LEFT] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_DPAD_LEFT);
            Engine->Input.GamepadButtonsDown[GP_DP_RIGHT] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_DPAD_RIGHT);

            Engine->Input.GamepadButtonsDown[GP_BPR_LEFT] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
            Engine->Input.GamepadButtonsDown[GP_BPR_RIGHT] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
            
            Engine->Input.GamepadButtonsDown[GP_STKDWN_LEFT] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_LEFTSTICK);
            Engine->Input.GamepadButtonsDown[GP_STKDWN_RIGHT] = SDL_GameControllerGetButton(Engine->Input.Gamepad,SDL_CONTROLLER_BUTTON_RIGHTSTICK);
            
            Engine->Input.GamepadTriggers[GP_TRGR_LEFT] = (SDL_GameControllerGetAxis(Engine->Input.Gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT)/32768.0);
            Engine->Input.GamepadTriggers[GP_TRGR_RIGHT] = (SDL_GameControllerGetAxis(Engine->Input.Gamepad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT)/32768.0);

            Engine->Input.GamepadSticks[GP_STK_LEFT_X] = (SDL_GameControllerGetAxis(Engine->Input.Gamepad, SDL_CONTROLLER_AXIS_LEFTX)/32768.0);
            Engine->Input.GamepadSticks[GP_STK_LEFT_Y] = (SDL_GameControllerGetAxis(Engine->Input.Gamepad, SDL_CONTROLLER_AXIS_LEFTY)/32768.0);
            Engine->Input.GamepadSticks[GP_STK_RIGHT_X] = (SDL_GameControllerGetAxis(Engine->Input.Gamepad, SDL_CONTROLLER_AXIS_RIGHTX)/32768.0);
            Engine->Input.GamepadSticks[GP_STK_RIGHT_Y] = (SDL_GameControllerGetAxis(Engine->Input.Gamepad, SDL_CONTROLLER_AXIS_RIGHTY)/32768.0);

            for(int i = 0; i < 14; i++)
            {
                if(!Engine->Input.GamepadButtonsDown[i] && Engine->Input.GamepadPreviousState[i])
                {
                    Engine->Input.GamepadButtonsUp[i] = true;
                }
            }
            for(int i = 0; i < 2; i++)
            {
                if(Engine->Input.GamepadPreviousTriggersState[i] >= 0.1 && Engine->Input.GamepadTriggers[i] <= 0.1)
                {
                    Engine->Input.GamepadTriggersUp[i] = true;
                }
            }

        }
    }
}

void GetInput(Engine* Engine)
{
    if(Engine)
    {
        Engine->Input.SDL_Keystate = SDL_GetKeyboardState(NULL);
        Engine->Input.SDL_MouseState = SDL_GetMouseState(&Engine->Input.MousePosition.X,&Engine->Input.MousePosition.Y);
        Engine->Input.VerticalMouseScroll = 0;
        Engine->Input.HorizontalMouseScroll = 0;
        memset(Engine->Input.KeysUp,0,sizeof(Engine->Input.KeysUp));
        memset(Engine->Input.KeysDown,0,sizeof(Engine->Input.KeysDown));
        memset(Engine->Input.MouseUp,0,sizeof(Engine->Input.MouseUp));
        memset(Engine->Input.MouseDown,0,sizeof(Engine->Input.MouseDown));
        memset(Engine->Input.GamepadButtonsUp,0,sizeof(Engine->Input.GamepadButtonsUp));
        memset(Engine->Input.GamepadButtonsDown,0,sizeof(Engine->Input.GamepadButtonsDown));
        memset(Engine->Input.GamepadTriggersUp,0,sizeof(Engine->Input.GamepadTriggersUp));

        GetKeyboardInput(Engine);
        GetMouseInput(Engine);
        GetGamepadInput(Engine);

        memcpy(Engine->Input.GamepadPreviousState,Engine->Input.GamepadButtonsDown,sizeof(Engine->Input.GamepadPreviousState));
        memcpy(Engine->Input.GamepadPreviousTriggersState,Engine->Input.GamepadTriggers,sizeof(Engine->Input.GamepadPreviousTriggersState));
        memcpy(Engine->Input.SDL_PreviousKeystate,Engine->Input.SDL_Keystate,sizeof(Engine->Input.SDL_PreviousKeystate));
        Engine->Input.SDL_PreviousMouseState = Engine->Input.SDL_MouseState;
    }
}