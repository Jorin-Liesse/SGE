#pragma once
#include "patterns/singleton/singleton.h"
#include "patterns/event/event.h"

#include "handlers/testHandler.h"

#include "game.h"

#include <SDL3/SDL_stdinc.h>

namespace sge
{
    class FpsHandler : public patterns::Singleton<FpsHandler>
    {
        friend class patterns::Singleton<FpsHandler>;

    public:
        FpsHandler() = default;
        ~FpsHandler();

        void CleanUp();
        void Init(int infoDataId);

        void Execute();

        patterns::Event<int> OnFpsChange;
        patterns::Event<bool> OnVsyncChange;
        
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
