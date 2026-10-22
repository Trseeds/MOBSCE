#include "MOBSCE.h"

int8 GetKeyboardInput(Engine* Engine)
{
    if(Engine)
    {
        uint8* KS = Engine->Input.SDL_Keystate;
        uint8* PKS = Engine->Input.SDL_PreviousKeystate;
        bool* EKD = Engine->Input.KeysDown;
        bool* EKU = Engine->Input.KeysUp;

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

int8 GetMouseInput(Engine* Engine)
{
    if(Engine)
    {
        SDL_Event* EE = Engine->Events;
        uint32 M = Engine->Input.SDL_MouseState;
        uint32 PM = Engine->Input.SDL_PreviousMouseState;
        bool* EMD = Engine->Input.MouseDown;
        bool* EMU = Engine->Input.MouseUp;
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

int8 GetGamepadInput(Engine* Engine)
{
    if(Engine)
    {
        for(int i = 0; i < EVENT_QUEUE_SIZE; i++)
        {
            if(Engine->Events[i].type == SDL_CONTROLLERDEVICEADDED)
            {
                SDL_GameController* SDLGC = SDL_GameControllerOpen(Engine->Events[i].cdevice.which);
                if(SDLGC)
                {
                    CreateGamepad(SDLGC,Engine);
                }
            }
        }
        int AGPM = Engine->Resource.AllocatedGamepadMemory;
        Gamepad* G = Engine->Input.Gamepads;
        for(int i = 0; i < AGPM; i++)
        {
            if(G[i].IsUsed)
            {
                Gamepad* GP = &G[i];
                SDL_GameController* SDLGC = GP->Gamepad;
                if(!SDL_GameControllerGetAttached(G[i].Gamepad))
                {
                    DestroyGamepad(&G[i],Engine);
                    break;
                }

                GP->ButtonsDown[GP_FB_BOTTOM] =    SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_A);
                GP->ButtonsDown[GP_FB_RIGHT] =     SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_B);
                GP->ButtonsDown[GP_FB_LEFT] =      SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_X);
                GP->ButtonsDown[GP_FB_TOP] =       SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_Y);
                GP->ButtonsDown[GP_FB_START] =     SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_START);
                GP->ButtonsDown[GP_FB_SELECT] =    SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_BACK);
                GP->ButtonsDown[GP_FB_SUPER] =     SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_GUIDE);
                GP->ButtonsDown[GP_DP_UP] =        SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_DPAD_UP);
                GP->ButtonsDown[GP_DP_DOWN] =      SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_DPAD_DOWN);
                GP->ButtonsDown[GP_DP_LEFT] =      SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_DPAD_LEFT);
                GP->ButtonsDown[GP_DP_RIGHT] =     SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
                GP->ButtonsDown[GP_BPR_LEFT] =     SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
                GP->ButtonsDown[GP_BPR_RIGHT] =    SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
                GP->ButtonsDown[GP_STKDWN_LEFT] =  SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_LEFTSTICK);
                GP->ButtonsDown[GP_STKDWN_RIGHT] = SDL_GameControllerGetButton(SDLGC,SDL_CONTROLLER_BUTTON_RIGHTSTICK);
                GP->Triggers[GP_TRGR_LEFT] =       SDL_GameControllerGetAxis(SDLGC,SDL_CONTROLLER_AXIS_TRIGGERLEFT)/32768.0;
                GP->Triggers[GP_TRGR_RIGHT] =      SDL_GameControllerGetAxis(SDLGC,SDL_CONTROLLER_AXIS_TRIGGERRIGHT)/32768.0;
                GP->Sticks[GP_STK_LEFT_X] =        SDL_GameControllerGetAxis(SDLGC,SDL_CONTROLLER_AXIS_LEFTX)/32768.0;
                GP->Sticks[GP_STK_LEFT_Y] =        SDL_GameControllerGetAxis(SDLGC,SDL_CONTROLLER_AXIS_LEFTY)/32768.0;
                GP->Sticks[GP_STK_RIGHT_X] =       SDL_GameControllerGetAxis(SDLGC,SDL_CONTROLLER_AXIS_RIGHTX)/32768.0;
                GP->Sticks[GP_STK_RIGHT_Y] =       SDL_GameControllerGetAxis(SDLGC,SDL_CONTROLLER_AXIS_RIGHTY)/32768.0;

                for(int i = 0; i < NUM_GAMEPAD_BUTTONS; i++)
                {
                    if(!GP->ButtonsDown[i] && GP->PreviousState[i])
                    {
                        GP->ButtonsUp[i] = true;
                    }
                }
                for(int i = 0; i < NUM_GAMEPAD_TRIGGERS; i++)
                {
                    if(GP->PreviousTriggersState[i] >= 0.1 && GP->Triggers[i] <= 0.1)
                    {
                        GP->TriggersUp[i] = true;
                    }
                }
            }
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 GetInput(Engine* Engine)
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
        int AGPM = Engine->Resource.AllocatedGamepadMemory;
        Gamepad* G = Engine->Input.Gamepads;
        for(int i = 0; i < AGPM; i++)
        {
            if(G[i].IsUsed)
            {
                memset(&G[i].ButtonsUp,0,sizeof(G->ButtonsUp));
                memset(&G[i].ButtonsDown,0,sizeof(G->ButtonsDown));
                memset(&G[i].TriggersUp,0,sizeof(G->TriggersUp));
            }
        }

        GetKeyboardInput(Engine);
        GetMouseInput(Engine);
        GetGamepadInput(Engine);

        for(int i = 0; i < AGPM; i++)
        {
            if(G[i].IsUsed)
            {
                memcpy(&G[i].PreviousState,&G[i].ButtonsDown,sizeof(G->PreviousState));
                memcpy(&G[i].PreviousTriggersState,&G[i].Triggers,sizeof(G->PreviousTriggersState));
            }
        }
        memcpy(EI->SDL_PreviousKeystate,EI->SDL_Keystate,sizeof(EI->SDL_PreviousKeystate));
        EI->SDL_PreviousMouseState = EI->SDL_MouseState;
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int8 RumbleGamepad(Gamepad* Gamepad, uint8 Strength, uint32 Duration, Engine* Engine)
{
    const char Parameters[] = "(Gamepad* Gamepad: 0x%p, uint8 Strength: %d, uint32 Duration: %d, Engine* Engine: 0x%p)";
    if(Engine)
    {
        if(Gamepad)
        {
            if(Gamepad->Gamepad)
            {
                if(Strength > 100)
                {
                    Strength = 100;
                }

                uint16 RealStrength = (uint16)LinearMap(Strength,0,100,0,(uint16)UNSIGNED_TYPE_MAX_VAL);
                int Result = SDL_GameControllerRumble(Gamepad->Gamepad,RealStrength,RealStrength,Duration);
                if(Result < 0)
                {
                    Error Error = {WARNING_SDL_FAILURE,"RumbleGamepad","Controller does not support rumble.","\0","\0"};
                    snprintf(Error.Parameters,STRING_BUFFER_SIZE,Parameters,Strength,Duration,Engine);
                    snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_GameControllerRumber failed and returned %d.",Result);
                    ThrowWarning(&Error,Engine);
                    return(WARNING_SDL_FAILURE);
                }
                return(RETURN_SUCCESS);
            }
        }
        return(WARNING_INVALID_PARAMETER);
    }
    return(ERROR_INVALID_ENGINE);
}