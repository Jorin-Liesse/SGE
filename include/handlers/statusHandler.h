#pragma once
#include "patterns/singleton/singleton.h"
#include "patterns/event/event.h"

#include "handlers/testHandler.h"

#include <SDL3/SDL.h>

namespace sge
{
    class StatusHandler : public patterns::Singleton<StatusHandler>
    {
        friend class patterns::Singleton<StatusHandler>;
        
    public:
        StatusHandler() = default;
        ~StatusHandler();

        void Init();
        void Cleanup();

        void Event(SDL_Event *event);

        patterns::Event<bool> OnVisibilityChange;
        patterns::Event<bool> OnFocusChange;
        patterns::Event<bool> OnWasHiddenChange;

        bool GetVisibility() const;
        bool GetWasHidden() const;
        bool GetFocused() const;

        void SetVisibility(bool visibility);
        void SetWasHidden(bool wasHidden);
        void SetFocused(bool focused);
        
    private:
        bool m_visibility;
        bool m_wasHidden;
        bool m_focused;
    };
}
