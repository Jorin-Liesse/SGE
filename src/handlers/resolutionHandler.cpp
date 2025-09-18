#pragma region Includes / Defines

#include "handlers/resolutionHandler.h"

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

void ResolutionHandler::Init(int infoDataId)
{
    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    int defaultWidth = cJSON_GetObjectItem(infoJson, "default-width")->valueint;
    int defaultHeight = cJSON_GetObjectItem(infoJson, "default-height")->valueint;
    string defaultWindowMode = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "default-window-mode"));
    float defaultScale = (float)cJSON_GetObjectItem(infoJson, "default-scale")->valuedouble;

    m_minWidth = cJSON_GetObjectItem(infoJson, "min-width")->valueint;
    m_minHeight = cJSON_GetObjectItem(infoJson, "min-height")->valueint;

    m_width = SaveDataHandler::GetInstance().LoadIntData("width", defaultWidth);
    m_height = SaveDataHandler::GetInstance().LoadIntData("height", defaultHeight);
    m_windowMode = SaveDataHandler::GetInstance().LoadStringData("window-mode", defaultWindowMode);
    m_scale = SaveDataHandler::GetInstance().LoadFloatData("scale", defaultScale);

    SDL_DisplayID displayId = SDL_GetDisplayForWindow(Game::GetInstance().GetWindow());
    SDL_Rect displayBounds;
    SDL_GetDisplayBounds(displayId, &displayBounds);
    SDL_SetWindowMinimumSize(Game::GetInstance().GetWindow(), m_minWidth, m_minHeight);
    SDL_SetWindowMaximumSize(Game::GetInstance().GetWindow(), displayBounds.w, displayBounds.h);

    ChangedWindowSize();
    ChangedWindowMode();
    ChangedScale();

    SDL_SetWindowPosition(Game::GetInstance().GetWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

#ifdef __EMSCRIPTEN__
    emscripten_set_resize_callback(
        EMSCRIPTEN_EVENT_TARGET_WINDOW,
        this,
        EM_TRUE,
        +[](int eventType, const EmscriptenUiEvent *uiEvent, void *userData) -> EM_BOOL
        {
            ResolutionHandler *handler = static_cast<ResolutionHandler *>(userData);
            handler->SetWindowWidth(EM_ASM_INT({ return window.innerWidth; }));
            handler->SetWindowHeight(EM_ASM_INT({ return window.innerHeight; }));
            return EM_TRUE;
        });

    SetWindowWidth(EM_ASM_INT({ return window.innerWidth; }));
    SetWindowHeight(EM_ASM_INT({ return window.innerHeight; }));
#endif
}

void ResolutionHandler::Cleanup()
{
}

#pragma endregion

#pragma region Engine Methods

void ResolutionHandler::Event(SDL_Event *event)
{
    if (event->type == SDL_EVENT_WINDOW_RESIZED)
    {
        SDL_WindowEvent *e = (SDL_WindowEvent *)event;
        m_width = e->data1;
        m_height = e->data2;

        ChangedWindowSize();
    }
}

#pragma endregion

#pragma region Private Methods

void ResolutionHandler::ChangedWindowSize()
{
    SDL_SetWindowSize(Game::GetInstance().GetWindow(), m_width, m_height);

    SaveDataHandler::GetInstance().SaveData("width", m_width);
    SaveDataHandler::GetInstance().SaveData("height", m_height);

    OnResolutionChange.invoke(m_width, m_height);

    ChangedScale();
}

void ResolutionHandler::ChangedWindowMode()
{
    if (m_windowMode == "windowed")
        SDL_SetWindowSize(Game::GetInstance().GetWindow(), m_width, m_height);
    else if (m_windowMode == "fullscreen")
        SDL_SetWindowFullscreen(Game::GetInstance().GetWindow(), SDL_WINDOW_FULLSCREEN);
    else if (m_windowMode == "borderless")
    {
        SDL_SetWindowFullscreen(Game::GetInstance().GetWindow(), SDL_WINDOW_BORDERLESS);
        int displayIndex = SDL_GetDisplayForWindow(Game::GetInstance().GetWindow());

        SDL_Rect displayBounds;
        SDL_GetDisplayBounds(displayIndex, &displayBounds);
        SDL_SetWindowSize(Game::GetInstance().GetWindow(), displayBounds.w, displayBounds.h);
    }

    SDL_GetWindowSize(Game::GetInstance().GetWindow(), &m_width, &m_height);

    SaveDataHandler::GetInstance().SaveData("width", m_width);
    SaveDataHandler::GetInstance().SaveData("height", m_height);
    SaveDataHandler::GetInstance().SaveData("window-mode", m_windowMode);

    OnWindowModeChange.invoke(m_windowMode);
}

void ResolutionHandler::ChangedScale()
{
    SaveDataHandler::GetInstance().SaveData("scale", m_scale);

    SDL_Texture *targetTexture = SDL_CreateTexture(Game::GetInstance().GetRenderer(),
                                                   SDL_PIXELFORMAT_RGBA32,
                                                   SDL_TEXTUREACCESS_TARGET,
                                                   (int)(m_width * m_scale), (int)(m_height * m_scale));

    Game::GetInstance().SetTargetTexture(targetTexture);

    OnScaleChange.invoke(m_scale);
}

#pragma endregion

#pragma region Public Methods
#pragma endregion

#pragma region Getters / Setters

int ResolutionHandler::GetWindowWidth() const
{
    return m_width;
}

void ResolutionHandler::SetWindowWidth(int width)
{
    if (m_width == width)
        return;

    if (width < m_minWidth)
        width = m_minWidth;

    m_width = width;

    ChangedWindowSize();
}

int ResolutionHandler::GetWindowHeight() const
{
    return m_height;
}

void ResolutionHandler::SetWindowHeight(int height)
{
    if (m_height == height)
        return;

    if (height < m_minHeight)
        height = m_minHeight;

    m_height = height;

    ChangedWindowSize();
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
}

float ResolutionHandler::GetScale() const
{
    return m_scale;
}

void ResolutionHandler::SetScale(float scale)
{
    if (m_scale == scale)
        return;

    m_scale = scale;

    ChangedScale();
}

float ResolutionHandler::GetAspectRatio() const
{
    if (m_height == 0)
        return 0.0f;
    return (float)m_width / (float)m_height;
}
int ResolutionHandler::GetResolutionWidth() const
{
    return (int)(m_width * m_scale);
}
int ResolutionHandler::GetResolutionHeight() const
{
    return (int)(m_height * m_scale);
}

#pragma endregion
