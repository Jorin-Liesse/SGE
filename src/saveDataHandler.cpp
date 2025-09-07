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
        if (!FS.analyzePath(folderPath).exists) {
            FS.mkdirTree(folderPath);   // ✅ creates parents automatically
            FS.mount(IDBFS, {}, folderPath);
        }
    },
    folderPath.c_str()
);
#else
    char *prefPath = SDL_GetPrefPath(company.c_str(), title.c_str());
    m_path = std::string(prefPath) + "saveData.json";
    SDL_free(prefPath);
#endif

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), m_path.c_str(), nullptr);
}

void SaveDataHandler::CleanUp()
{
}

#pragma endregion

#pragma region Private Methods
#pragma endregion

#pragma region Public Methods
#pragma endregion

#pragma region Getters / Setters
#pragma endregion
