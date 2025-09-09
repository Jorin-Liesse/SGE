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

        float GetDeltaTime();

    private:
        bool m_vsync;
        int m_fps;
        float m_deltaTime;

        Uint32 m_previousTime;
        Uint32 m_currentTime;

        void ChangeFps();
        void ChangeVsync();
    };
}
