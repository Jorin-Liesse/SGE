#pragma region Includes / Defines

#include "handlers/statusHandler.h"

using namespace sge;
using namespace std;

#pragma endregion

#pragma region Constructor / Destructor

StatusHandler::~StatusHandler()
{
    Cleanup();
}

#pragma endregion

#pragma region Init / Cleanup

void StatusHandler::Init()
{
    m_visibility = true;
    m_wasHidden = false;
    m_focused = false;
}

void StatusHandler::Cleanup()
{
}

#pragma endregion

#pragma region Public Methods

void StatusHandler::Event(SDL_Event *event)
{
    if (event->type < SDL_EVENT_WINDOW_FIRST || event->type > SDL_EVENT_WINDOW_LAST)
        return;

    if (event->type == SDL_EVENT_WINDOW_FOCUS_GAINED)
    {
        SetFocused(true);
    }
    else if (event->type == SDL_EVENT_WINDOW_FOCUS_LOST)
    {
        SetFocused(false);
        SetWasHidden(true);
    }
    else if (event->type == SDL_EVENT_WINDOW_SHOWN)
    {
        SetVisibility(true);
    }
    else if (event->type == SDL_EVENT_WINDOW_HIDDEN || event->type == SDL_EVENT_WINDOW_MINIMIZED)
    {
        SetVisibility(false);
        SetWasHidden(true);
    }
}

#pragma endregion

#pragma region Getters / Setters

bool StatusHandler::GetVisibility() const
{
    return m_visibility;
}
bool StatusHandler::GetWasHidden() const
{
    return m_wasHidden;
}
bool StatusHandler::GetFocused() const
{
    return m_focused;
}

void StatusHandler::SetVisibility(bool visibility)
{
    if (m_visibility == visibility)
        return;

    m_visibility = visibility;
    OnVisibilityChange.invoke(visibility);
}
void StatusHandler::SetWasHidden(bool wasHidden)
{
    if (m_wasHidden == wasHidden)
        return;

    m_wasHidden = wasHidden;
    OnWasHiddenChange.invoke(wasHidden);
}
void StatusHandler::SetFocused(bool focused)
{
    if (m_focused == focused)
        return;
        
    m_focused = focused;
    OnFocusChange.invoke(focused);
}

#pragma endregion
