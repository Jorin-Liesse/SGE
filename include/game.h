#pragma once
#include "patterns/singleton/singleton.h"
#include "assetsHandler.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <string>

#include <vector>
#include <math.h>
#include <filesystem>
#include <string_view>
#include <thread>

namespace sge
{
    class Game : public sge::patterns::singleton::Singleton<Game>
    {
        friend class sge::patterns::singleton::Singleton<Game>;

    public:
        Game() = default;
        ~Game();

        bool Init();
        void Cleanup();

        void Event();

        void Render();
        void Update();

        SDL_Window *GetWindow();
        void SetWindow(SDL_Window *window);

        SDL_Renderer *GetRenderer();
        void SetRenderer(SDL_Renderer *renderer);
        
        SDL_Event *GetEvent();
        void SetEvent(SDL_Event *event);

        SDL_AppResult GetAppResult();
        void SetAppResult(SDL_AppResult result);

        MIX_Mixer *GetMixer();
        void SetMixer(MIX_Mixer *mixer);

        void ShowMessage(const std::string &title, const std::string &message);

    private:
        SDL_Event *m_event;
        SDL_Window *m_window;
        SDL_Renderer *m_renderer;
        SDL_AppResult m_appResult;
        MIX_Mixer *m_mixer;

        //

        SDL_Texture *m_messageTex, *m_imageTex;
        SDL_FRect m_messageDest;
        MIX_Track *m_track;

        float m_time;
        float m_red;
        float m_green;
        float m_blue;

        void TestInit();
        void TestUpdate();
        void TestRender();

        //

        int m_infoDataId;
    };
}
