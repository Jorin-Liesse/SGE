#pragma once
#include "patterns/singleton/singleton.h"
#include "assetsHandler.h"
#include "saveDataHandler.h"
#include "FPSHandler.h"
#include "statusHandler.h"
#include "audioHandler.h"
#include "resolutionHandler.h"
#include "inputHandler.h"
#include "testHandler.h"
#include "cJSON.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <format>
#include <cmath>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace sge
{
    class Game : public patterns::Singleton<Game>
    {
        friend class patterns::Singleton<Game>;

    public:
        Game() = default;
        ~Game();

        bool Init();
        void Cleanup();

        void Event();

        void Render();
        void Execute();

        SDL_Window *GetWindow();
        SDL_Renderer *GetRenderer();
        MIX_Mixer *GetMixer();
        
        SDL_Event *GetEvent();
        void SetEvent(SDL_Event *event);

        SDL_AppResult GetAppResult();
        void SetAppResult(SDL_AppResult result);

    private:
        SDL_Event *m_event;
        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
        SDL_AppResult m_appResult;
        MIX_Mixer *m_mixer;
    };
}
