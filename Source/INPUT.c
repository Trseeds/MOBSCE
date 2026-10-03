#include "MOBSCE.h"

int GetKeyboardInput(Engine* Engine)
{
    if(Engine)
    {
        uint8* KS = Engine->Input.SDL_Keystate;
        uint8* PKS = Engine->Input.SDL_PreviousKeystate;
        uint8* EKD = Engine->Input.KeysDown;
        uint8* EKU = Engine->Input.KeysUp;

        for(int i = 0; i < SDL_NUM_SCANCODES; i++)
        {
            if(KS[i])
            {
                EKD[i] = true;
            }
            if(PKS[i] && !KS[i])
            {
                EKU[i] = true;
            }
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int GetMouseInput(Engine* Engine)
{
    if(Engine)
    {
        SDL_Event* EE = Engine->Events;
        uint32 M = Engine->Input.SDL_MouseState;
        uint32 PM = Engine->Input.SDL_PreviousMouseState;
        uint8* EMD = Engine->Input.MouseDown;
        uint8* EMU = Engine->Input.MouseUp;
        uint8* EMHS = &Engine->Input.HorizontalMouseScroll;
        uint8* EMVS = &Engine->Input.VerticalMouseScroll;
        if(M & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            EMD[0] = true;
        }
        if(M & SDL_BUTTON(SDL_BUTTON_RIGHT))
        {
            EMD[1] = true;
        }
        if(M & SDL_BUTTON(SDL_BUTTON_MIDDLE))
        {
            EMD[2] = true;
        }
        if(M & SDL_BUTTON(SDL_BUTTON_X1))
        {
            EMD[3] = true;
        }
        if(M & SDL_BUTTON(SDL_BUTTON_X2))
        {
            EMD[4] = true;
        }
        if(!(M & SDL_BUTTON(SDL_BUTTON_LEFT)) && (PM & SDL_BUTTON(SDL_BUTTON_LEFT)))
        {
            EMU[0] = true;
        }
        if(!(M & SDL_BUTTON(SDL_BUTTON_RIGHT)) && (PM & SDL_BUTTON(SDL_BUTTON_RIGHT)))
        {
            EMU[1] = true;
        }
        if(!(M & SDL_BUTTON(SDL_BUTTON_MIDDLE)) && (PM & SDL_BUTTON(SDL_BUTTON_MIDDLE)))
        {
            EMU[2] = true;
        }
        if(!(M & SDL_BUTTON(SDL_BUTTON_X1)) && (PM & SDL_BUTTON(SDL_BUTTON_X1)))
        {
            EMU[3] = true;
        }
        if(!(M & SDL_BUTTON(SDL_BUTTON_X2)) && (PM & SDL_BUTTON(SDL_BUTTON_X2)))
        {
            EMU[4] = true;
        }
        
        for(int i = 0; i < EVENT_QUEUE_SIZE; i++)
        {
            if(EE[i].type == SDL_MOUSEWHEEL)
            {
                if(EE[i].wheel.y > 0)
                {
                    *EMVS = SCROLL_UP; //scroll up
                }
                if(EE[i].wheel.y < 0)
                {
                    *EMVS = SCROLL_DOWN; //scroll down
                }
                if(EE[i].wheel.x > 0)
                {
                    *EMHS = SCROLL_RIGHT; //scroll right
                }
                if(EE[i].wheel.x < 0)
                {
                    *EMHS = SCROLL_LEFT; //scroll left
                }
            }
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int GetGamepadInput(Engine* Engine) //i would fix this but its going to be demolished soon
{
    if(Engine)
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
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int GetInput(Engine* Engine)
{
    if(Engine)
    {
        Input* EI = &Engine->Input;
        EI->SDL_Keystate = (Uint8*)SDL_GetKeyboardState(NULL);
        EI->SDL_MouseState = SDL_GetMouseState(&EI->MousePosition.X,&EI->MousePosition.Y);
        EI->MousePosition.X /= Engine->Video.WindowScale;
        EI->MousePosition.Y /= Engine->Video.WindowScale;
        EI->VerticalMouseScroll = 0;
        EI->HorizontalMouseScroll = 0;
        memset(EI->KeysUp,0,sizeof(EI->KeysUp));
        memset(EI->KeysDown,0,sizeof(EI->KeysDown));
        memset(EI->MouseUp,0,sizeof(EI->MouseUp));
        memset(EI->MouseDown,0,sizeof(EI->MouseDown));
        memset(EI->GamepadButtonsUp,0,sizeof(EI->GamepadButtonsUp));
        memset(EI->GamepadButtonsDown,0,sizeof(EI->GamepadButtonsDown));
        memset(EI->GamepadTriggersUp,0,sizeof(EI->GamepadTriggersUp));

        GetKeyboardInput(Engine);
        GetMouseInput(Engine);
        GetGamepadInput(Engine);

        memcpy(EI->GamepadPreviousState,EI->GamepadButtonsDown,sizeof(EI->GamepadPreviousState));
        memcpy(EI->GamepadPreviousTriggersState,EI->GamepadTriggers,sizeof(EI->GamepadPreviousTriggersState));
        memcpy(EI->SDL_PreviousKeystate,EI->SDL_Keystate,sizeof(EI->SDL_PreviousKeystate));
        EI->SDL_PreviousMouseState = EI->SDL_MouseState;
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int RumbleGamepad(int Strength, int Duration, Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Input.GamepadIsConnected && Engine->Input.Gamepad)
        {
            if(Strength < 0)
            {
                Strength = 0;
            }
            if(Strength > 100)
            {
                Strength = 100;
            }

            Uint16 RealStrength = (Uint16)LinearMap(Strength,0,100,0,0xFFFF);

            int Result = SDL_GameControllerRumble(Engine->Input.Gamepad,RealStrength,RealStrength,Duration);
            if(Result < 0)
            {
                Error Error = {WARNING_IGNORABLE_FAILURE,"RumbleGamepad","Controller does not support rumble.","\0","\0"};
                snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(int Strength: %d, int Duration: %d, Engine* Engine: 0x%p)",Strength,Duration,Engine);
                snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_GameControllerRumber failed and returned %d.",Result);
                ThrowWarning(&Error,Engine);
                return(WARNING_IGNORABLE_FAILURE);
                
            }
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}