#pragma once
#include "patterns/singleton/singleton.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"

#include "handlers/assetsHandler.h"
#include "handlers/saveDataHandler.h"
#include "handlers/fpsHandler.h"
// #include "handlers/statusHandler.h"
#include "handlers/audioHandler.h"
// #include "handlers/resolutionHandler.h"
// #include "handlers/inputHandler.h"

#include "logEntry.h"
#include "game.h"
#include "cJSON/cJSON.h"

// #include <SDL3/SDL.h>
// #include <SDL3_ttf/SDL_ttf.h>
// #include <SDL3_mixer/SDL_mixer.h>
// #include <SDL3_image/SDL_image.h>

#include <string>
#include <format>
#include <vector>
#include <unordered_map>

namespace sge
{
    class TestHandler : public patterns::Singleton<TestHandler>
    {
        friend class patterns::Singleton<TestHandler>;

    public:
        TestHandler() = default;
        ~TestHandler();

        void Init();
        void Cleanup();

        void Execute();
        void Render();
        void Event(SDL_Event *event);

        void ImGuiRender();

        void Logger(const std::string &title, const char *fmt = "", ...);

    private:
        SDL_Texture *m_messageTex, *m_imageTex;
        SDL_FRect m_messageDest;

        int m_fontId;

        // int m_fps;
        // bool m_vsync;

        // int m_windowWidth;
        // int m_windowHeight;
        // int m_selectedWindowMode;
        // const char *m_itemsWindowMode[3];

        // float m_volumeGlobal;
        // float m_volumeMusic;
        // float m_volumeSFX;

        int m_intSaveData;
        bool m_boolSaveData;
        float m_floatSaveData;
        std::string m_stringSaveData;
        char m_stringBuffer[1024];
        std::string m_jsonSaveData;
        char m_jsonBuffer[4096];
        bool m_isJsonValid;

        ImGuiIO *m_io;

        bool m_showDemoWindow;
        bool m_showAnotherWindow;
        ImVec4 m_clearColor;

        std::vector<LogEntry> m_itemsLog;
        std::unordered_map<std::string, bool> m_filterTitles;

        void ClearLog();

        void ImguiLoggerWindow();
        void ImguiDebugWindow();

        void TestTextInit();
        void TestAudioInit();
        void TestImageInit();
        // void TestStatusInit();
    };
}
