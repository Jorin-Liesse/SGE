#pragma region Includes / Defines

#include "game.h"
#include <iostream>

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

bool Game::Init()
{
    m_event = nullptr;
    m_window = nullptr;
    m_renderer = nullptr;
    m_appResult = SDL_APP_CONTINUE;
    m_mixer = nullptr;

    m_window = SDL_CreateWindow("game", 500, 500, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    m_renderer = SDL_CreateRenderer(m_window, NULL);
    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

    AssetsHandler::GetInstance().Init();
    SaveDataHandler::GetInstance().Init();
    FPSHandler::GetInstance().Init();
    StatusHandler::GetInstance().Init();
    AudioHandler::GetInstance().Init();
    ResolutionHandler::GetInstance().Init();
    InputHandler::GetInstance().Init();
    TestHandler::GetInstance().Init();

    int infoDataId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");
    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    string title = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "project"));
    string iconPath = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "icon-path"));

    AssetsHandler::GetInstance().UnUsedJson(infoDataId);

    SDL_SetWindowTitle(m_window, title.c_str());
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_Surface *icon = IMG_Load(iconPath.c_str());
    SDL_SetWindowIcon(m_window, icon);
    SDL_DestroySurface(icon);

    return true;
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

    StatusHandler::GetInstance().Event(m_event);
    ResolutionHandler::GetInstance().Event(m_event);
    InputHandler::GetInstance().Event(m_event);
    TestHandler::GetInstance().Event(m_event);

    if (m_event->type == SDL_EVENT_QUIT)
        m_appResult = SDL_APP_SUCCESS;
}

void Game::Execute()
{
#ifdef __EMSCRIPTEN__
    if (not FPSHandler::GetInstance().CanContinue())
        return;
#endif

    FPSHandler::GetInstance().Execute();
    TestHandler::GetInstance().Execute();
}

void Game::Render()
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    TestHandler::GetInstance().Render();

    SDL_RenderPresent(m_renderer);
}

#pragma endregion

#pragma region Private Methods
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

#pragma endregion
