#pragma once
#include "patterns/singleton/singleton.h"

#include "handlers/assetsHandler.h"
#include "handlers/testHandler.h"

#include "game.h"
#include "cJSON/cJSON.h"

#include <SDL3/SDL.h>
#include <fstream>
#include <string>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/val.h>
#endif

namespace sge
{
    class SaveDataHandler : public patterns::Singleton<SaveDataHandler>
    {
        friend class patterns::Singleton<SaveDataHandler>;

    public:
        SaveDataHandler() = default;
        ~SaveDataHandler();

        void CleanUp();
        void Init();

        bool IsReady();

        void LoadFromDisk();
        void SaveToDisk();

        void SaveData(const std::string& key, const std::string& value);
        void SaveData(const std::string& key, int value);
        void SaveData(const std::string& key, float value);
        void SaveData(const std::string& key, bool value);
        void SaveData(const std::string& key, cJSON* value);

        std::string LoadStringData(const std::string& key, std::string defaultValue = "");
        int LoadIntData(const std::string& key, int defaultValue = 0);
        float LoadFloatData(const std::string& key, float defaultValue = 0.0f);
        bool LoadBoolData(const std::string& key, bool defaultValue = false);
        cJSON* LoadJsonData(const std::string& key, cJSON* defaultValue = nullptr);

    private:
        std::string m_path;
        cJSON* m_root;
    };
}
