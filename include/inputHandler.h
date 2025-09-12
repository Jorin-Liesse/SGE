#pragma once
#include "patterns/singleton/singleton.h"
#include "patterns/event/event.h"
#include "assetsHandler.h"
#include "saveDataHandler.h"
#include "game.h"
#include "cJSON.h"

#include <SDL3/SDL.h>
#include <string>

namespace sge
{
    class InputHandler : public patterns::Singleton<InputHandler>
    {
        friend class patterns::Singleton<InputHandler>;

    public:
        InputHandler() = default;
        ~InputHandler();

        void HandleEvents(SDL_Event *event);

        void Init();
        void Cleanup();

    private:
    };
}
