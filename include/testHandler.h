#pragma once
#include "patterns/singleton/singleton.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"

#include "assetsHandler.h"
#include "saveDataHandler.h"
#include "FPSHandler.h"
#include "statusHandler.h"
#include "audioHandler.h"
#include "resolutionHandler.h"
#include "inputHandler.h"
#include "game.h"
#include "cJSON.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <format>
#include <cmath>
#include <stdio.h>

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

        void Logger(const std::string &title, const char *fmt = "", ...);

    private:
        SDL_Texture *m_messageTex, *m_imageTex, *m_fpsTex;
        SDL_FRect m_messageDest, m_fpsDest;

        int m_fontId;

        ImVec4 m_bgColor;
        ImGuiIO m_io;

        int m_fps;
        bool m_vsync;

        int m_windowWidth;
        int m_windowHeight;
        int m_selectedWindowMode;
        const char *m_itemsWindowMode[3];

        float m_volumeGlobal;
        float m_volumeMusic;
        float m_volumeSFX;

        int m_intSaveData;
        bool m_boolSaveData;
        float m_floatSaveData;
        std::string m_stringSaveData;
        cJSON *m_jsonSaveData;

        void ImguiInit();
        void ImguiEvent(SDL_Event *event);
        void ImguiExecute();
        void ImguiRender();
        void ImguiCleanup();

        void TestTextInit();
        void TestAudioInit();
        void TestImageInit();
        void TestSaveDataInit();
        void TestStatusInit();
    };
}
