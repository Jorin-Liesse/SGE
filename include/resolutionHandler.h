#pragma once
#include "patterns/singleton/singleton.h"
#include "patterns/event/event.h"
#include "patterns/event/event.h"
#include "assetsHandler.h"
#include "saveDataHandler.h"
#include "testHandler.h"
#include "game.h"
#include "cJSON.h"

#include <SDL3/SDL.h>
#include <string>

namespace sge
{
    class ResolutionHandler : public patterns::Singleton<ResolutionHandler>
    {
        friend class patterns::Singleton<ResolutionHandler>;

    public:
        ResolutionHandler() = default;
        ~ResolutionHandler();

        void Init();
        void Cleanup();

        void Event(SDL_Event *event);

        patterns::Event<int, int> OnResolutionChange;
        patterns::Event<std::string> OnWindowModeChange;

        int GetWidth() const;
        void SetWidth(int width);

        int GetHeight() const;
        void SetHeight(int height);

        std::string GetWindowMode() const;
        void SetWindowMode(const std::string &mode);

    private:
        int m_width;
        int m_height;

        std::string m_windowMode;

        void ChangedResolution();
        void ChangedWindowMode();
    };
}
