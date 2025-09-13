#pragma once
#include "patterns/singleton/singleton.h"
#include "game.h"
#include "testHandler.h"

#include <SDL3/SDL_stdinc.h>

namespace sge
{
    class FPSHandler : public patterns::Singleton<FPSHandler>
    {
        friend class patterns::Singleton<FPSHandler>;

    public:
        FPSHandler() = default;
        ~FPSHandler();

        void CleanUp();
        void Init();

        void Execute();
        
#ifdef __EMSCRIPTEN__
        bool CanContinue();
#endif

        bool GetVsync();
        void SetVsync(bool vsync);

        int GetFps();
        void SetFps(int fps);

        float GetCurrentFPS();

        float GetDeltaTime();

    private:
        bool m_vsync;
        int m_fps;
        float m_deltaTime;
        float m_currentFPS;
        double m_previousTime;
        double m_lastTime;
        int m_frames;

        void ChangedFps();
        void ChangedVsync();
    };
}
