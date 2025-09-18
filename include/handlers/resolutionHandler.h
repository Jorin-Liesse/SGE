#pragma once
#include "patterns/singleton/singleton.h"
#include "patterns/event/event.h"

#include "handlers/assetsHandler.h"
#include "handlers/saveDataHandler.h"
#include "handlers/testHandler.h"

#include "game.h"
#include "cJSON/cJSON.h"

#include <SDL3/SDL.h>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

namespace sge
{
    class ResolutionHandler : public patterns::Singleton<ResolutionHandler>
    {
        friend class patterns::Singleton<ResolutionHandler>;

    public:
        ResolutionHandler() = default;
        ~ResolutionHandler();

        void Init(int infoDataId);
        void Cleanup();

        void Event(SDL_Event *event);

        patterns::Event<int, int> OnResolutionChange;
        patterns::Event<std::string> OnWindowModeChange;
        patterns::Event<float> OnScaleChange;

        int GetWindowWidth() const;
        void SetWindowWidth(int width);

        int GetWindowHeight() const;
        void SetWindowHeight(int height);

        float GetScale() const;
        void SetScale(float scale);

        float GetAspectRatio() const;
        int GetResolutionWidth() const;
        int GetResolutionHeight() const;

        std::string GetWindowMode() const;
        void SetWindowMode(const std::string &mode);

    private:
        int m_width;
        int m_height;
        float m_scale;
        std::string m_windowMode;

        int m_minWidth;
        int m_minHeight;

        void ChangedWindowSize();
        void ChangedWindowMode();
        void ChangedScale();
    };
}
