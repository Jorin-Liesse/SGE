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

        void SaveData(const std::string& key, const std::string& value);
        void SaveData(const std::string& key, int value);
        void SaveData(const std::string& key, float value);
        void SaveData(const std::string& key, bool value);

        std::string LoadStringData(const std::string& key);
        int LoadIntData(const std::string& key);
        float LoadFloatData(const std::string& key);
        bool LoadBoolData(const std::string& key);

    private:
        std::string m_path;
    };
}
