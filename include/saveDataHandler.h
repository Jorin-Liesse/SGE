#pragma once
#include "patterns/singleton/singleton.h"
#include "game.h"

#include <SDL3/SDL.h>
#include <fstream>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace sge
{
    class SaveDataHandler : public sge::patterns::singleton::Singleton<SaveDataHandler>
    {
        friend class sge::patterns::singleton::Singleton<SaveDataHandler>;

    public:
        SaveDataHandler() = default;
        ~SaveDataHandler();

        void CleanUp();
        void Init();

    private:
        // std::string m_companyName;
        // std::string m_gameName;

        std::string m_path;
    };
}
