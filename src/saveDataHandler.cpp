#pragma region Includes / Defines

#include "saveDataHandler.h"

using namespace std;
using namespace sge;

#pragma endregion

#pragma region Constructor / Destructor

SaveDataHandler::~SaveDataHandler()
{
    CleanUp();
}

#pragma endregion

#pragma region Init / Cleanup

void SaveDataHandler::Init()
{
    m_root = nullptr;

    int infoDataId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");
    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    string title = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "project"));
    string company = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "company"));

    AssetsHandler::GetInstance().UnUsedJson(infoDataId);

#ifdef __EMSCRIPTEN__
    std::string folderPath = "/" + company + "/" + title;
    m_path = folderPath + "/saveData.json";

    EM_ASM(
        {
            var folderPath = UTF8ToString($0);
            if (!FS.analyzePath(folderPath).exists)
            {
                FS.mkdirTree(folderPath);
                FS.mount(IDBFS, {}, folderPath);
            }
        },
        folderPath.c_str());

    EM_ASM(FS.syncfs(true, function(err) { if(err) console.log("Load error", err); }));
#else
    char *prefPath = SDL_GetPrefPath(company.c_str(), title.c_str());
    m_path = std::string(prefPath) + "saveData.json";
    SDL_free(prefPath);
#endif

    LoadFromDisk();

    // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), m_path.c_str(), nullptr);
}

void SaveDataHandler::CleanUp()
{
}

#pragma endregion

#pragma region Private Methods

void SaveDataHandler::LoadFromDisk()
{
    std::ifstream file(m_path);
    if (!file.is_open())
    {
        m_root = cJSON_CreateObject();
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string content = buffer.str();
    m_root = cJSON_Parse(content.c_str());
    if (!m_root)
    {
        m_root = cJSON_CreateObject();
    }
}

void SaveDataHandler::Commit()
{
    if (!m_root)
        return;

    char *jsonStr = cJSON_Print(m_root);
    if (!jsonStr)
        return;

    std::ofstream file(m_path, std::ios::binary);
    if (file.is_open())
    {
        file.write(jsonStr, strlen(jsonStr));
        file.close();
    }
    cJSON_free(jsonStr);

#ifdef __EMSCRIPTEN__
    EM_ASM(FS.syncfs(false, function(err) { if(err) console.log("Save error", err); }));
#endif
}

#pragma endregion

#pragma region Public Methods

void SaveDataHandler::SaveData(const std::string &key, const std::string &value)
{
    cJSON_ReplaceItemInObject(m_root, key.c_str(), cJSON_CreateString(value.c_str()));
    Commit();
}

void SaveDataHandler::SaveData(const std::string &key, int value)
{
    cJSON_ReplaceItemInObject(m_root, key.c_str(), cJSON_CreateNumber(value));
    Commit();
}

void SaveDataHandler::SaveData(const std::string &key, float value)
{
    cJSON_ReplaceItemInObject(m_root, key.c_str(), cJSON_CreateNumber(value));
    Commit();
}

void SaveDataHandler::SaveData(const std::string &key, bool value)
{
    cJSON_ReplaceItemInObject(m_root, key.c_str(), cJSON_CreateBool(value));
    Commit();
}

// --- LoadData overloads ---
std::string SaveDataHandler::LoadStringData(const std::string &key, std::string defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (cJSON_IsString(item))
        return item->valuestring;
    return defaultValue;
}

int SaveDataHandler::LoadIntData(const std::string &key, int defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (cJSON_IsNumber(item))
        return item->valueint;
    return defaultValue;
}

float SaveDataHandler::LoadFloatData(const std::string &key, float defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (cJSON_IsNumber(item))
        return (float)item->valuedouble;
    return defaultValue;
}

bool SaveDataHandler::LoadBoolData(const std::string &key, bool defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (cJSON_IsBool(item))
        return cJSON_IsTrue(item);
    return defaultValue;
}

#pragma endregion

#pragma region Getters / Setters
#pragma endregion
