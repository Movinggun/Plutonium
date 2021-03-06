#include <pn/render/Renderer.hpp>

namespace pn::render
{
    void Renderer::Initialize()
    {
        if(!this->initialized)
        {
            Result rc = romfsInit();
            this->okromfs = (rc == 0);
            plInitialize();
            SDL_Init(SDL_INIT_EVERYTHING);
            this->rendwd = SDL_CreateWindow("Plutonium", 0, 0, 1280, 720, SDL_WINDOW_FULLSCREEN);
            this->rendrd = SDL_CreateRenderer(this->rendwd, -1, (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));
            this->rendsf = SDL_GetWindowSurface(this->rendwd);
            SDL_SetRenderDrawBlendMode(this->rendrd, SDL_BLENDMODE_BLEND);
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
            IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF | IMG_INIT_WEBP);
            TTF_Init();
            this->initialized = true;
        }
    }

    void Renderer::Finalize()
    {
        if(this->initialized)
        {
            plExit();
            if(this->okromfs) romfsExit();
            TTF_Quit();
            IMG_Quit();
            SDL_DestroyRenderer(this->rendrd);
            SDL_FreeSurface(this->rendsf);
            SDL_DestroyWindow(this->rendwd);
            SDL_Quit();
            this->initialized = false;
        }
    }

    bool Renderer::HasInitialized()
    {
        return this->initialized;
    }

    void Renderer::Clear(draw::Color ClearColor)
    {
        SDL_SetRenderDrawColor(this->rendrd, ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A);
        SDL_RenderClear(this->rendrd);
    }

    void Renderer::Render()
    {
        SDL_RenderPresent(this->rendrd);
    }

    void Renderer::DrawImage(std::string Path, u32 X, u32 Y)
    {
        SDL_Surface *imgs = IMG_Load(Path.c_str());
        if(imgs)
        {
            SDL_Texture *imgt = SDL_CreateTextureFromSurface(this->rendrd, imgs);
            SDL_Rect position;
            position.x = X;
            position.y = Y;
            SDL_QueryTexture(imgt, NULL, NULL, &position.w, &position.h);
            SDL_RenderCopy(this->rendrd, imgt, NULL, &position);
            SDL_DestroyTexture(imgt);
        }
        SDL_FreeSurface(imgs);
    }

    void Renderer::DrawImageColorKey(std::string Path, draw::Color ColorKey, u32 X, u32 Y)
    {
        SDL_Surface *imgs = IMG_Load(Path.c_str());
        if(imgs)
        {
            SDL_Texture *imgt = SDL_CreateTextureFromSurface(this->rendrd, imgs);
            SDL_Rect position;
            position.x = X;
            position.y = Y;
            SDL_QueryTexture(imgt, NULL, NULL, &position.w, &position.h);
            SDL_RenderCopy(this->rendrd, imgt, NULL, &position);
            SDL_DestroyTexture(imgt);
        }
        SDL_FreeSurface(imgs);
    }

    void Renderer::DrawText(std::string Text, draw::SystemFont Font, u32 Size, u32 X, u32 Y, draw::Color TextColor)
    {
        PlFontData plfont;
        TTF_Font *font = NULL;
        SDL_RWops *mem = NULL;
        switch(Font)
        {
            case draw::SystemFont::Standard:
                plGetSharedFontByType(&plfont, 0);
                mem = SDL_RWFromMem(plfont.address, plfont.size);
                font = TTF_OpenFontRW(mem, 1, Size);
                break;
            case draw::SystemFont::Extended:
                plGetSharedFontByType(&plfont, 5);
                mem = SDL_RWFromMem(plfont.address, plfont.size);
                font = TTF_OpenFontRW(mem, 1, Size);
                break;
        }
        if(font == NULL) return;
        SDL_Color clr = { TextColor.R, TextColor.G, TextColor.B, TextColor.A };
        SDL_Surface *surface = TTF_RenderUTF8_Blended(font, Text.c_str(), clr);
        SDL_SetSurfaceAlphaMod(surface, 255);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(this->rendrd, surface);
        SDL_FreeSurface(surface);
        SDL_Rect position = { X, Y, surface->w, surface->h };
        SDL_RenderCopy(this->rendrd, tex, NULL, &position);
        SDL_DestroyTexture(tex);
        TTF_CloseFont(font);
    }

    void Renderer::DrawRectangle(draw::Color RecColor, u32 X, u32 Y, u32 Width, u32 Height)
    {
        SDL_Rect rect;
        rect.x = X;
        rect.y = Y;
        rect.w = Width;
        rect.h = Height;
        SDL_SetRenderDrawColor(this->rendrd, RecColor.R, RecColor.G, RecColor.B, RecColor.A);
        SDL_RenderDrawRect(this->rendrd, &rect);
    }

    void Renderer::DrawRectangleFill(draw::Color RecColor, u32 X, u32 Y, u32 Width, u32 Height)
    {
        SDL_Rect rect;
        rect.x = X;
        rect.y = Y;
        rect.w = Width;
        rect.h = Height;
        SDL_SetRenderDrawColor(this->rendrd, RecColor.R, RecColor.G, RecColor.B, RecColor.A);
        SDL_RenderFillRect(this->rendrd, &rect);
    }

    void Renderer::DrawCircle(draw::Color CircleColor, u32 X, u32 Y, u32 Radius)
    {
        int cx = X + Radius;
        int cy = Y + Radius;
        int radius = Radius;
        SDL_SetRenderDrawColor(this->rendrd, CircleColor.R, CircleColor.G, CircleColor.B, CircleColor.A);
        for(double dy = 1; dy <= radius; dy += 1.0)
        {
            double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
            int x = cx - dx;
            SDL_RenderDrawLine(this->rendrd, cx - dx, cy + dy - radius, cx + dx, cy + dy - radius);
            SDL_RenderDrawLine(this->rendrd, cx - dx, cy - dy + radius, cx + dx, cy - dy + radius);
        }
    }
}