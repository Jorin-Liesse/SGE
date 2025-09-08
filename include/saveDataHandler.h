#pragma once
#include "patterns/singleton/singleton.h"
#include "game.h"
#include "cJSON.h"

#include <SDL3/SDL.h>
#include <fstream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

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

        int m_count;

        void load_counter();
        void save_counter();
        void persist_counter();
        void after_sync();

    private:
        std::string m_path;
        cJSON* m_root;   
    };
}


