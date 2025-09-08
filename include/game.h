#pragma once
#include "patterns/singleton/singleton.h"
#include "assetsHandler.h"
#include "saveDataHandler.h"
#include "cJSON.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <string>

#include <filesystem>

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

        // int m_width, m_height;
        // bool m_vsync;
        // int m_fps;
        // float m_audioVolume, m_musicVolume, m_soundVolume;
        // std::string m_windowMode;

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

        bool SaveData(const std::string &filename, const std::string &key, const std::string &value);
        std::string LoadData(const std::string &filename, const std::string &key, const std::string &defaultValue = "");

        void InitSaveFolderWeb();
        bool SaveDataWeb(const std::string &key, const std::string &value);
        std::string LoadDataWeb(const std::string &key, const std::string &defaultValue = "");

        const std::string SAVE_FOLDER = "/MyCompany/MyGame";
        const std::string SAVE_FILE = SAVE_FOLDER + "/saveData.json";

        //
    };
}
