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

    const char *file = "saveData.json";

#ifdef __EMSCRIPTEN__
    const string folder = "/" + company + "/" + title;
    m_path = folder + "/" + file;

    EM_ASM({
    const path = UTF8ToString($0);
    FS.mkdirTree(path);
    FS.mount(IDBFS, {}, path);
    FS.syncfs(true, function(err) {
        if (err) console.error("Load sync error:", err);
        else _on_fs_synced();
    }); }, folder.c_str());

    while (!IsReady())
        emscripten_sleep(10);
#else
    char *prefPath = SDL_GetPrefPath(company.c_str(), title.c_str());
    m_path = std::string(prefPath) + file;
    SDL_free(prefPath);

    LoadFromDisk();
#endif
}

void SaveDataHandler::CleanUp()
{
    if (m_root)
        cJSON_Delete(m_root);
    m_root = nullptr;
}

#pragma endregion

#pragma region Private Methods

void SaveDataHandler::LoadFromDisk()
{
    FILE *file = fopen(m_path.c_str(), "r");
    if (!file)
        return;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = new char[size + 1];
    fread(data, 1, size, file);
    data[size] = '\0';
    fclose(file);

    m_root = cJSON_Parse(data);
    delete[] data;

    if (!m_root)
        m_root = cJSON_CreateObject();
}

void SaveDataHandler::SaveToDisk()
{
    if (!m_root)
        return;

    char *data = cJSON_PrintUnformatted(m_root);
    FILE *file = fopen(m_path.c_str(), "w");
    if (file)
    {
        fprintf(file, "%s", data);
        fclose(file);
    }
    free(data);

#ifdef __EMSCRIPTEN__
    EM_ASM(
        FS.syncfs(false, function(err) {
            if (err) console.error("Sync error:", err); }););
#endif
}

#ifdef __EMSCRIPTEN__
extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    void on_fs_synced()
    {
        SaveDataHandler::GetInstance().LoadFromDisk();
    }
}
#endif

#pragma endregion

#pragma region Public Methods

bool SaveDataHandler::IsReady()
{
    return m_root != nullptr;
}


void SaveDataHandler::SaveData(const std::string &key, const std::string &value)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (item)
        cJSON_ReplaceItemInObject(m_root, key.c_str(), cJSON_CreateString(value.c_str()));
    else
        cJSON_AddItemToObject(m_root, key.c_str(), cJSON_CreateString(value.c_str()));

    SaveToDisk();
}

void SaveDataHandler::SaveData(const std::string &key, int value)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (item)
        cJSON_ReplaceItemInObject(m_root, key.c_str(), cJSON_CreateNumber(value));
    else
        cJSON_AddItemToObject(m_root, key.c_str(), cJSON_CreateNumber(value));

    SaveToDisk();
}

void SaveDataHandler::SaveData(const std::string &key, float value)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (item)
        cJSON_ReplaceItemInObject(m_root, key.c_str(), cJSON_CreateNumber(value));
    else
        cJSON_AddItemToObject(m_root, key.c_str(), cJSON_CreateNumber(value));

    SaveToDisk();
}

void SaveDataHandler::SaveData(const std::string &key, bool value)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (item)
        cJSON_ReplaceItemInObject(m_root, key.c_str(), cJSON_CreateBool(value));
    else
        cJSON_AddItemToObject(m_root, key.c_str(), cJSON_CreateBool(value));

    SaveToDisk();
}


std::string SaveDataHandler::LoadStringData(const std::string &key, std::string defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (cJSON_IsString(item))
        return item->valuestring;
    SaveData(key, defaultValue);

    return defaultValue;
}

int SaveDataHandler::LoadIntData(const std::string &key, int defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (cJSON_IsNumber(item))
        return item->valueint;

    SaveData(key, defaultValue);
    return defaultValue;
}

float SaveDataHandler::LoadFloatData(const std::string &key, float defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (cJSON_IsNumber(item))
        return (float)item->valuedouble;

    SaveData(key, defaultValue);
    return defaultValue;
}

bool SaveDataHandler::LoadBoolData(const std::string &key, bool defaultValue)
{
    cJSON *item = cJSON_GetObjectItem(m_root, key.c_str());
    if (cJSON_IsBool(item))
        return cJSON_IsTrue(item);

    SaveData(key, defaultValue);
    return defaultValue;
}

#pragma endregion

#pragma region Getters / Setters
#pragma endregion
