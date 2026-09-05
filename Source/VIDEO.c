#include "MOBSCE.h"

int InitVideo(Engine* Engine)
{
    if(Engine)
    {
        Engine->Video.Window = SDL_CreateWindow(Engine->Video.WindowTitle,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,Engine->Video.LogicalDimensions.X,Engine->Video.LogicalDimensions.Y,Engine->Video.WindowFlags);
        if(!Engine->Video.Window)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"InitVideo(%X)",Engine);
            ThrowError("Failed to create window!",Traceback,Engine);
            return(ERROR_SDL_FAILURE);
        }

        Engine->Video.Renderer = SDL_CreateRenderer(Engine->Video.Window,-1,Engine->Video.RendererFlags);
        if(!Engine->Video.Renderer)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"InitVideo(0x%X)",Engine);
            ThrowError("Failed to create renderer!",Traceback,Engine);
            return(ERROR_SDL_FAILURE);
        }

        SDL_Surface* Icon = IMG_Load(Engine->Video.WindowIconPath);
        if(!Icon)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"InitVideo(0x%X)",Engine);
            ThrowWarning("Failed to create window icon.",Traceback,Engine);
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

int DrawTexture(SDL_Texture* Texture, Vector2 Position, Vector2 Origin, Engine* Engine)
{
    if(Engine)
    {
        if(!Texture)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"DrawTexture(0x%X, %d, %d, 0x%X)",Texture,Position,Origin,Engine);
            ThrowWarning("Invalid texture.",Traceback,Engine);
            return(WARNING_INVALID_PARAMETER);
        }

        Vector2 Dimensions;
        int Result = SDL_QueryTexture(Texture,NULL,NULL,&Dimensions.X,&Dimensions.Y);
        if(Result != 0)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"DrawTexture(0x%X, %d, %d, 0x%X)",Texture,Position,Origin,Engine);
            ThrowWarning("Could not query texture information.",Traceback,Engine);
            return(WARNING_SDL_FAILURE);
        }

        SDL_Rect Source;
        SDL_Rect Destination;
        Source.x = Origin.X;
        Source.y = Origin.Y;
        Source.w = Dimensions.X;
        Source.h = Dimensions.Y;
        Destination.x = Position.X;
        Destination.y = Position.Y;
        Destination.w = Dimensions.X;
        Destination.h = Dimensions.Y;
        
        Result = SDL_RenderCopy(Engine->Video.Renderer,Texture,&Source,&Destination);
        if(Result != 0)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"DrawTexture(0x%X, %d, %d, 0x%X)",Texture,Position,Origin,Engine);
            ThrowWarning("Could not draw texture.",Traceback,Engine);
            return(3);
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
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"DrawSprite(0x%X, 0x%X)",Sprite,Engine);
            ThrowWarning("Invalid sprite.",Traceback,Engine);
            return(WARNING_INVALID_PARAMETER);
        }

        SDL_Rect Source;
        SDL_Rect Destination;
        Source.x = Sprite->RenderParameters.Origin.X;
        Source.y = Sprite->RenderParameters.Origin.Y;
        Source.w = Sprite->RenderParameters.Origin.Z;
        Source.h = Sprite->RenderParameters.Origin.W;
        Destination.x = Sprite->RenderParameters.Position.X;
        Destination.y = Sprite->RenderParameters.Position.Y;
        Destination.w = Sprite->RenderParameters.Dimensions.X;
        Destination.h = Sprite->RenderParameters.Dimensions.Y;
        
        Uint8 RealAlpha = LinearMap(Sprite->RenderParameters.Transparency,100,255,0);
        int ResultA = SDL_SetTextureAlphaMod(Sprite->RenderParameters.Texture,RealAlpha);
        int ResultC = SDL_SetTextureColorMod(Sprite->RenderParameters.Texture,Sprite->RenderParameters.Tint.X,Sprite->RenderParameters.Tint.Y,Sprite->RenderParameters.Tint.Z);

        int ResultR = SDL_RenderCopyEx(
            Engine->Video.Renderer,Sprite->RenderParameters.Texture,
            &Source,
            &Destination,
            Sprite->RenderParameters.Angle,
            NULL,
            Sprite->RenderParameters.Flip);
        if(ResultR != 0)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"DrawSprite(0x%X, 0x%X)",Sprite,Engine);
            ThrowWarning("Could not draw sprite.",Traceback,Engine);
            return(WARNING_SDL_FAILURE);
        }
        if(ResultA != 0 || ResultC != 0)
        {
            char Traceback[STRING_BUFFER_SIZE];
            snprintf(Traceback,STRING_BUFFER_SIZE,"DrawSprite(0x%X, 0x%X)",Sprite,Engine);
            ThrowWarning("Special sprite effects failed to render.",Traceback,Engine);
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
        Sprite* S = Engine->Sprites;
        int ASM = Engine->Resource.AllocatedSpriteMemory;
        SDL_RenderClear(Engine->Video.Renderer);
        if(Engine->SpriteZResortNeeded)
        {
            qsort(S, ASM, sizeof(Sprite), SortSpritesByZ);
            for(int i = 0; i < ASM; i++)
            {
                if(S[i].IsUsed)
                {
                    Engine->SpriteReferences[S[i].ReferenceIndex] = &S[i];
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
        return(RETURN_SUCCESS);
    }
    return(ERROR_INVALID_ENGINE);
}