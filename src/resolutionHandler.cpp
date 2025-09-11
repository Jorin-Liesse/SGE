#pragma region Includes / Defines

#include "resolutionHandler.h"

using namespace sge;
using namespace std;

#pragma endregion

#pragma region Constructor / Destructor

ResolutionHandler::~ResolutionHandler()
{
    Cleanup();
}

#pragma endregion

#pragma region Init / Cleanup

void ResolutionHandler::Init()
{
    int infoDataId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");
    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    int defaultWidth = cJSON_GetObjectItem(infoJson, "default-width")->valueint;
    int defaultHeight = cJSON_GetObjectItem(infoJson, "default-height")->valueint;
    string defaultWindowMode = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "default-window-mode"));

    AssetsHandler::GetInstance().UnUsedJson(infoDataId);

    m_width = SaveDataHandler::GetInstance().LoadIntData("width", defaultWidth);
    m_height = SaveDataHandler::GetInstance().LoadIntData("height", defaultHeight);
    m_windowMode = SaveDataHandler::GetInstance().LoadStringData("window-mode", defaultWindowMode);

    ChangedResolution();
    ChangedWindowMode();
}

void ResolutionHandler::Cleanup()
{
}

#pragma endregion

#pragma region Engine Methods
#pragma endregion

#pragma region Private Methods

void ResolutionHandler::ChangedResolution()
{
    SDL_SetWindowSize(Game::GetInstance().GetWindow(), m_width, m_height);
}

void ResolutionHandler::ChangedWindowMode()
{
    if (m_windowMode == "windowed")
        SDL_SetWindowSize(Game::GetInstance().GetWindow(), m_width, m_height);
    else if (m_windowMode == "borderless")
        SDL_SetWindowFullscreen(Game::GetInstance().GetWindow(), SDL_WINDOW_BORDERLESS);
    else if (m_windowMode == "fullscreen")
        SDL_SetWindowFullscreen(Game::GetInstance().GetWindow(), SDL_WINDOW_FULLSCREEN);

    SDL_GetWindowSize(Game::GetInstance().GetWindow(), &m_width, &m_height);
}

#pragma endregion

#pragma region Public Methods
#pragma endregion

#pragma region Getters / Setters

int ResolutionHandler::GetWidth() const
{
    return m_width;
}

void ResolutionHandler::SetWidth(int width)
{
    if (m_width == width)
        return;

    m_width = width;

    ChangedResolution();
    OnResolutionChange.invoke(m_width, m_height);
}

int ResolutionHandler::GetHeight() const
{
    return m_height;
}

void ResolutionHandler::SetHeight(int height)
{
    if (m_height == height)
        return;

    m_height = height;

    ChangedResolution();
    OnResolutionChange.invoke(m_width, m_height);
}

std::string ResolutionHandler::GetWindowMode() const
{
    return m_windowMode;
}

void ResolutionHandler::SetWindowMode(const std::string &mode)
{
    if (m_windowMode == mode)
        return;

    m_windowMode = mode;

    ChangedWindowMode();
    OnWindowModeChange.invoke(m_windowMode);
}

#pragma endregion
