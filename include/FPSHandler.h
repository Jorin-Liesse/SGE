#pragma once
#include "patterns/singleton/singleton.h"
#include "game.h"

#include <SDL3/SDL_stdinc.h>

namespace sge
{
    class FPSHandler : public sge::patterns::singleton::Singleton<FPSHandler>
    {
        friend class sge::patterns::singleton::Singleton<FPSHandler>;

    public:
        FPSHandler() = default;
        ~FPSHandler();

        void CleanUp();
        void Init();

        void Execute();

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

        void ChangeFps();
        void ChangeVsync();
    };
}
