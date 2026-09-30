#include "MOBSCE.h"

int InitVideo(Engine* Engine)
{
    if(Engine)
    {
        Engine->Video.Window = SDL_CreateWindow(Engine->Video.WindowTitle,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,Engine->Video.LogicalDimensions.X,Engine->Video.LogicalDimensions.Y,Engine->Video.WindowFlags);
        if(!Engine->Video.Window)
        {
            Error Error = {ERROR_SDL_FAILURE,"InitVideo","Failed to create window!","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(Engine* Engine: 0x%p)",Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_CreateWindow failed and returned 0x%p",Engine->Video.Window);
            ThrowError(&Error,Engine);
            return(ERROR_SDL_FAILURE);
        }

        Engine->Video.Renderer = SDL_CreateRenderer(Engine->Video.Window,-1,Engine->Video.RendererFlags);
        if(!Engine->Video.Renderer)
        {
            Error Error = {ERROR_SDL_FAILURE,"InitVideo","Failed to create renderer!","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(Engine* Engine: 0x%p)",Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_CreateRenderer failed and returned 0x%p",Engine->Video.Renderer);
            ThrowError(&Error,Engine);
            return(ERROR_SDL_FAILURE);
        }

        SDL_Surface* Icon = IMG_Load(Engine->Video.WindowIconPath);
        if(!Icon)
        {
            Error Error = {WARNING_SDL_FAILURE,"InitVideo","Failed to create window icon.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(Engine* Engine: 0x%p)",Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"IMG_Load failed and returned 0x%p",Icon);
            ThrowWarning(&Error,Engine);
        }
        if(Icon)
        {
            SDL_SetWindowIcon(Engine->Video.Window,Icon);
            SDL_FreeSurface(Icon);
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
        SDL_RenderSetLogicalSize(Engine->Video.Renderer,Engine->Video.LogicalDimensions.X,Engine->Video.LogicalDimensions.Y);

        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int RestartVideo(Engine* Engine)
{
    if(Engine)
    {
        ResourceInfo ResourceInfo;
        ResourceInfo.Pointer = &Engine->Resource.Textures;
        ResourceInfo.SizeOfResource = sizeof(SDL_Texture*);
        ResourceInfo.FreeFunction = (void (*)(void*))&SDL_DestroyTexture;
        ResourceInfo.NumberOfResources = &Engine->Resource.NumberOfTextures;
        ResourceInfo.AllocatedResourceMemory = &Engine->Resource.AllocatedTextureMemory;
        CleanupResourcePool(ResourceInfo,Engine);
        InitResourcePool(ResourceInfo,Engine);
        CleanupVideo(Engine);
        InitVideo(Engine);
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int CleanupVideo(Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Video.Window)
        {
            SDL_DestroyWindow(Engine->Video.Window);
        }
        if(Engine->Video.Renderer)
        {
            SDL_DestroyRenderer(Engine->Video.Renderer);
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int DrawSprite(Sprite* Sprite, Engine* Engine)
{
    if(Engine)
    {
        if(!Sprite)
        {
            Error Error = {WARNING_INVALID_PARAMETER,"DrawSprite","Invalid sprite.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(Sprite* Sprite: 0x%p, Engine* Engine: 0x%p)",Sprite,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"Sprite is NULL.");
            ThrowWarning(&Error,Engine);
            return(WARNING_INVALID_PARAMETER);
        }

        SpriteRenderParameters* RP = &Sprite->RenderParameters;
        SDL_Rect Source;
        SDL_Rect Destination;
        Source.x = RP->Origin.X;
        Source.y = RP->Origin.Y;
        Source.w = RP->Origin.Z;
        Source.h = RP->Origin.W;
        Destination.x = RP->Position.X;
        Destination.y = RP->Position.Y;
        Destination.w = RP->Dimensions.X;
        Destination.h = RP->Dimensions.Y;
        
        Uint8 RealAlpha = LinearMap(RP->Transparency,0,100,0,255);
        int ResultA = SDL_SetTextureAlphaMod(RP->Texture,RealAlpha);
        int ResultC = SDL_SetTextureColorMod(RP->Texture,RP->Tint.X,RP->Tint.Y,RP->Tint.Z);

        int ResultR = SDL_RenderCopyEx(
            Engine->Video.Renderer,RP->Texture,
            &Source,
            &Destination,
            RP->Angle,
            NULL,
            RP->Flip);
        if(ResultR != 0)
        {
            Error Error = {WARNING_SDL_FAILURE,"DrawSprite","Could not draw sprite.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(Sprite* Sprite: 0x%p, Engine* Engine: 0x%p)",Sprite,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_RenderCopyEX failed and returned %d.",ResultR);
            ThrowWarning(&Error,Engine);
            return(WARNING_SDL_FAILURE);
        }
        if(ResultA != 0 || ResultC != 0)
        {
            Error Error = {WARNING_SDL_FAILURE,"DrawSprite","Special sprite effects failed to render.","\0","\0"};
            snprintf(Error.Parameters,STRING_BUFFER_SIZE,"(Sprite* Sprite: 0x%p, Engine* Engine: 0x%p)",Sprite,Engine);
            snprintf(Error.Description,STRING_BUFFER_SIZE,"SDL_SetTextureAlphaMod returned %d and SDL_SetTextureColorMod returned %d.",ResultA,ResultC);
            ThrowWarning(&Error,Engine);
            return(WARNING_SDL_FAILURE);
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}

int Render(Engine* Engine)
{
    if(Engine)
    {
        if(Engine->Config.Render)
        {
            Sprite* S = Engine->Sprites;
            int ASM = Engine->Resource.AllocatedSpriteMemory;
            Sprite** SR = Engine->SpriteReferences;
            SDL_RenderClear(Engine->Video.Renderer);
            if(Engine->SpriteZResortNeeded)
            {
                qsort(S, ASM, sizeof(Sprite), QSSortSpritesByZ);
                for(int i = 0; i < ASM; i++)
                {
                    if(S[i].IsUsed)
                    {
                        SR[S[i].ReferenceIndex] = &S[i];
                    }
                }
                Engine->SpriteZResortNeeded = false;
            }

            for(int i = 0; i < ASM; i++)
            {
                if(S[i].IsUsed)
                {
                    if(S[i].RenderParameters.Visible)
                    {
                        DrawSprite(&S[i],Engine);
                    }
                }
            }

            SDL_RenderPresent(Engine->Video.Renderer);
        }
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}