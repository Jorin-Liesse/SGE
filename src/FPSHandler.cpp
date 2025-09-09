#pragma region Includes / Defines

#include "FPSHandler.h"

using namespace std;
using namespace sge;

#pragma endregion

#pragma region Constructor / Destructor

FPSHandler::~FPSHandler()
{
    CleanUp();
}

#pragma endregion

#pragma region Init / Cleanup

void FPSHandler::Init()
{
    int infoDataId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");
    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    int defaultFps = cJSON_GetObjectItem(infoJson, "default-fps")->valueint;
    bool defaultVsync = cJSON_GetObjectItem(infoJson, "default-vsync")->valueint;

    AssetsHandler::GetInstance().UnUsedJson(infoDataId);

    m_fps = SaveDataHandler::GetInstance().LoadIntData("fps", defaultFps);
    m_vsync = SaveDataHandler::GetInstance().LoadBoolData("vsync", defaultVsync);

    ChangeFps();
    ChangeVsync();

    m_previousTime = SDL_GetTicks();
}

void FPSHandler::CleanUp()
{
}

#pragma endregion

#pragma region Engine Methods

void FPSHandler::Execute()
{
    m_currentTime = SDL_GetTicks();
    m_deltaTime = (m_currentTime - m_previousTime) / 1000.0f;
    m_previousTime = m_currentTime;

    if (m_fps <= 0)
        return;

    Uint32 frameTime = SDL_GetTicks() - m_currentTime;
    Uint32 targetFrameTime = 1000u / static_cast<Uint32>(m_fps);
    if (frameTime < targetFrameTime)
    {
        SDL_Delay(targetFrameTime - frameTime);
    }
}

#pragma endregion

#pragma region Private Methods

void FPSHandler::ChangeFps()
{
}

void FPSHandler::ChangeVsync()
{
    if (m_vsync)
        SDL_SetRenderVSync(Game::GetInstance().GetRenderer(), -1); // enable vsync
    else
        SDL_SetRenderVSync(Game::GetInstance().GetRenderer(), 0); // disable vsync
}

#pragma endregion

#pragma region Public Methods
#pragma endregion

#pragma region Getters / Setters

bool FPSHandler::GetVsync()
{
    return m_vsync;
}
void FPSHandler::SetVsync(bool vsync)
{
    if (m_vsync == vsync)
        return;
    m_vsync = vsync;
    ChangeVsync();
}

int FPSHandler::GetFps()
{
    return m_fps;
}
void FPSHandler::SetFps(int fps)
{
    if (m_fps == fps)
        return;
    m_fps = fps;
    ChangeFps();
}

float FPSHandler::GetDeltaTime()
{
    return m_deltaTime;
}

#pragma endregion
