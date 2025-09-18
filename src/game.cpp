#pragma region Includes / Defines

#include "game.h"

using namespace std;
using namespace sge;

#pragma endregion

#pragma region Constructor / Destructor

Game::~Game()
{
    Cleanup();
}

#pragma endregion

#pragma region Init / Cleanup

void Game::Init()
{
    m_window = nullptr;
    m_renderer = nullptr;

    m_appResult = SDL_APP_CONTINUE;

#ifdef __ANDROID__
    SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE, "1");
    SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1");
#endif

    AssetsHandler::GetInstance().Init();
    SaveDataHandler::GetInstance().Init();

    int infoDataId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");

    InitSDL(infoDataId);

    AudioHandler::GetInstance().Init(infoDataId);
    FpsHandler::GetInstance().Init(infoDataId);
    ResolutionHandler::GetInstance().Init(infoDataId);
    StatusHandler::GetInstance().Init();

#ifdef DEBUG_MODE
    TestHandler::GetInstance().Init();
#endif

    AssetsHandler::GetInstance().UnUsedJson(infoDataId);
    SDL_ShowWindow(m_window);
}

void Game::Cleanup()
{
    if (m_renderer)
        SDL_DestroyRenderer(m_renderer);
    if (m_window)
        SDL_DestroyWindow(m_window);

    MIX_StopAllTracks(m_mixer, 0);
    MIX_DestroyMixer(m_mixer);
}

#pragma endregion

#pragma region Engine Methods

void Game::Event()
{
    if (not m_event)
        return;
    ;
#ifdef DEBUG_MODE
    TestHandler::GetInstance().Event(m_event);
#endif
    StatusHandler::GetInstance().Event(m_event);
    ResolutionHandler::GetInstance().Event(m_event);

    if (m_event->type == SDL_EVENT_QUIT)
        m_appResult = SDL_APP_SUCCESS;
    if (m_event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && m_event->window.windowID == SDL_GetWindowID(m_window))
        m_appResult = SDL_APP_SUCCESS;
}

void Game::Execute()
{
#ifdef __EMSCRIPTEN__
    if (not FpsHandler::GetInstance().CanContinue())
        return;
#endif

    FpsHandler::GetInstance().Execute();
#ifdef DEBUG_MODE
    TestHandler::GetInstance().Execute();
#endif
}

void Game::Render()
{
    SDL_SetRenderTarget(m_renderer, NULL);
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    SDL_SetRenderTarget(m_renderer, m_targetTexture);
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

#ifdef DEBUG_MODE
    TestHandler::GetInstance().Render();
#endif

    SDL_SetRenderTarget(m_renderer, NULL);
    SDL_RenderTexture(m_renderer, m_targetTexture, NULL, NULL);

#ifdef DEBUG_MODE
    TestHandler::GetInstance().ImGuiRender();
#endif

    SDL_RenderPresent(m_renderer);
}

#pragma endregion

#pragma region Private Methods

void Game::InitSDL(int infoDataId)
{
    m_window = SDL_CreateWindow("", 0, 0, SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (m_window == nullptr)
        m_appResult = SDL_APP_FAILURE;

    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    if (m_renderer == nullptr)
        m_appResult = SDL_APP_FAILURE;

    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (m_mixer == nullptr)
        m_appResult = SDL_APP_FAILURE;

    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    string title = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "project"));
    string iconPath = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "icon-path"));

    SDL_Surface *icon = IMG_Load(iconPath.c_str());
    SDL_SetWindowIcon(m_window, icon);
    SDL_DestroySurface(icon);

    SDL_SetWindowTitle(m_window, title.c_str());
    SDL_SetWindowResizable(m_window, true);

    SDL_SetRenderDrawBlendMode(Game::GetInstance().GetRenderer(), SDL_BLENDMODE_BLEND);
}

#pragma endregion

#pragma region Public Methods
#pragma endregion

#pragma region Getters / Setters

SDL_Window *Game::GetWindow()
{
    return m_window;
}

SDL_Renderer *Game::GetRenderer()
{
    return m_renderer;
}

MIX_Mixer *Game::GetMixer()
{
    return m_mixer;
}

SDL_Event *Game::GetEvent()
{
    return m_event;
}
void Game::SetEvent(SDL_Event *event)
{
    if (m_event == event)
        return;

    m_event = event;
}

SDL_AppResult Game::GetAppResult()
{
    return m_appResult;
}
void Game::SetAppResult(SDL_AppResult result)
{
    if (m_appResult == result)
        return;

    m_appResult = result;
}

SDL_Texture *Game::GetTargetTexture()
{
    return m_targetTexture;
}

void Game::SetTargetTexture(SDL_Texture *texture)
{
    if (m_targetTexture == texture)
        return;

    m_targetTexture = texture;
}

#pragma endregion
