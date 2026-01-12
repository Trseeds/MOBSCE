#include "MOBSCE.h"

int InitVideo(Engine* Engine)
{
    if(Engine)
    {
        Engine->Video.Window = SDL_CreateWindow("MOBSCE",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,Engine->Video.LogicalDimensions.X,Engine->Video.LogicalDimensions.Y,Engine->Video.WindowFlags);
        if(!Engine->Video.Window)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"InitVideo(%X)",Engine);
            ThrowError("Failed to create window!",Traceback,Engine);
            return(1);
        }

        Engine->Video.Renderer = SDL_CreateRenderer(Engine->Video.Window,-1,Engine->Video.RendererFlags);
        if(!Engine->Video.Renderer)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"InitVideo(0x%X)",Engine);
            ThrowError("Failed to create renderer!",Traceback,Engine);
            return(2);
        }

        char Path[1024];
        snprintf(Path,1024,"%sAssets/Images/Icon.bmp",Engine->BasePath);
        SDL_Surface* Icon = SDL_LoadBMP(Path);
        if(!Icon)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"InitVideo(0x%X)",Engine);
            ThrowWarning("Failed to create window icon.",Traceback);
            puts(Path);
            fflush(stdout);
        }
        if(Icon)
        {
            SDL_SetWindowIcon(Engine->Video.Window,Icon);
            SDL_FreeSurface(Icon);
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
        SDL_RenderSetLogicalSize(Engine->Video.Renderer,Engine->Video.LogicalDimensions.X,Engine->Video.LogicalDimensions.Y);

        return(0);
    }
    return(-1);
}

void CleanupVideo(Engine* Engine)
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
    }
}

int DrawTexture(SDL_Texture* Texture, Vector2 Position, Vector2 Origin, Engine* Engine)
{
    if(Engine)
    {
        if(!Texture)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"DrawTexture(0x%X, %d, %d, 0x%S)",Texture,Position,Origin,Engine);
            ThrowWarning("Invalid texture.",Traceback);
            return(1);
        }

        Vector2 Dimensions;
        int Result = SDL_QueryTexture(Texture,NULL,NULL,&Dimensions.X,&Dimensions.Y);
        if(Result != 0)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"DrawTexture(0x%X, %d, %d, 0x%S)",Texture,Position,Origin,Engine);
            ThrowWarning("Could not query texture information.",Traceback);
            return(2);
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
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"DrawTexture(0x%X, %d, %d, 0x%S)",Texture,Position,Origin,Engine);
            ThrowWarning("Could not draw texture.",Traceback);
            return(3);
        }

        return(0);
    }
    return(-1);
}

int DrawSprite(Sprite* Sprite, Engine* Engine)
{
    if(Engine)
    {
        if(!Sprite)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"DrawSprite(0x%X, 0x%X)",Sprite,Engine);
            ThrowWarning("Invalid sprite.",Traceback);
            return(1);
        }

        SDL_Rect Source;
        SDL_Rect Destination;
        Source.x = Sprite->Origin.X;
        Source.y = Sprite->Origin.Y;
        Source.w = Sprite->Origin.Z;
        Source.h = Sprite->Origin.W;
        Destination.x = Sprite->Position.X;
        Destination.y = Sprite->Position.Y;
        Destination.w = Sprite->Dimensions.X;
        Destination.h = Sprite->Dimensions.Y;
        
        int Result = SDL_RenderCopy(Engine->Video.Renderer,Sprite->Texture,&Source,&Destination);
        if(Result != 0)
        {
            char Traceback[ERROR_BUFFER_SIZE];
            snprintf(Traceback,ERROR_BUFFER_SIZE,"DrawSprite(0x%X, 0x%X)",Sprite,Engine);
            ThrowWarning("Could not draw sprite.",Traceback);
            return(3);
        }

        return(0);
    }
    return(-1);
}

void Render(Engine* Engine)
{
    if(Engine)
    {
        SDL_RenderClear(Engine->Video.Renderer);

        if(Engine->SpriteZResortNeeded)
        {
            qsort(Engine->Sprites, Engine->Resource.NumberOfSprites, sizeof(Sprite*), SortSpritesByZ);
            Engine->SpriteZResortNeeded = false;
        }

        for(int i = 0; i < Engine->Resource.NumberOfSprites; i++)
        {
            if(Engine->Sprites[i])
            {
                if(Engine->Sprites[i]->Visible)
                {
                    DrawSprite(Engine->Sprites[i],Engine);
                }
            }
            else
            {
                char Traceback[ERROR_BUFFER_SIZE];
                snprintf(Traceback,ERROR_BUFFER_SIZE,"Render(0x%X)",Engine);
                ThrowWarning("Sprite is invalid.",Traceback);
            }
        }
        
        SDL_RenderPresent(Engine->Video.Renderer);
    }
}