#pragma once

#include "patterns/singleton/singleton.h"

#include "handlers/testHandler.h"
#include "handlers/assetsHandler.h"
#include "handlers/saveDataHandler.h"
#include "handlers/audioHandler.h"
#include "handlers/fpsHandler.h"
#include "handlers/statusHandler.h"
#include "handlers/resolutionHandler.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

namespace sge
{
    class Game : public patterns::Singleton<Game>
    {
        friend class patterns::Singleton<Game>;

    public:
        Game() = default;
        ~Game();

        void Init();
        void Cleanup();

        void Event();

        void Render();
        void Execute();

        SDL_Window *GetWindow();
        SDL_Renderer *GetRenderer();
        MIX_Mixer *GetMixer();
        SDL_Event *GetEvent();

        SDL_Texture *GetTargetTexture();
        void SetTargetTexture(SDL_Texture *texture);

        void SetEvent(SDL_Event *event);

        SDL_AppResult GetAppResult();
        void SetAppResult(SDL_AppResult result);

    private:
        SDL_Event *m_event;
        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
        MIX_Mixer *m_mixer;

        SDL_AppResult m_appResult;

        SDL_Texture *m_targetTexture;

        void InitSDL(int infoDataId);
    };
}
