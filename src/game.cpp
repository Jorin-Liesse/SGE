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

    AssetsHandler::GetInstance().Init();
    SaveDataHandler::GetInstance().Init();
    FPSHandler::GetInstance().Init();

    int infoDataId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");
    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    string title = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "project"));
    string iconPath = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "icon-path"));

    int defaultWidth = cJSON_GetObjectItem(infoJson, "default-width")->valueint;
    int defaultHeight = cJSON_GetObjectItem(infoJson, "default-height")->valueint;
    string defaultWindowMode = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "default-window-mode"));

    float defaultAudioVolume = float(cJSON_GetObjectItem(infoJson, "default-audio-volume")->valuedouble);
    float defaultMusicVolume = float(cJSON_GetObjectItem(infoJson, "default-music-volume")->valuedouble);
    float defaultSoundVolume = float(cJSON_GetObjectItem(infoJson, "default-sound-volume")->valuedouble);

    AssetsHandler::GetInstance().UnUsedJson(infoDataId);

    m_width = SaveDataHandler::GetInstance().LoadIntData("width", defaultWidth);
    m_height = SaveDataHandler::GetInstance().LoadIntData("height", defaultHeight);
    m_windowMode = SaveDataHandler::GetInstance().LoadStringData("window-mode", defaultWindowMode);

    m_audioVolume = SaveDataHandler::GetInstance().LoadFloatData("audio-volume", defaultAudioVolume);
    m_musicVolume = SaveDataHandler::GetInstance().LoadFloatData("music-volume", defaultMusicVolume);
    m_soundVolume = SaveDataHandler::GetInstance().LoadFloatData("sound-volume", defaultSoundVolume);

    SDL_SetWindowTitle(m_window, title.c_str());

    SDL_Surface *icon = IMG_Load(iconPath.c_str());
    SDL_SetWindowIcon(m_window, icon);
    SDL_DestroySurface(icon);

    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

    TestInit();

    // if (m_windowMode == "windowed")
    //     SDL_SetWindowSize(m_window, m_width, m_height);
    // else if (m_windowMode == "borderless")
    //     SDL_SetWindowFullscreen(m_window, SDL_WINDOW_BORDERLESS);
    // else if (m_windowMode == "fullscreen")
    //     SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);

    // if (m_vsync)
    //     SDL_SetRenderVSync(m_renderer, -1); // enable vsync
    // else
    //     SDL_SetRenderVSync(m_renderer, 0); // disable vsync

    // SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN); // SDL_WINDOW_BORDERLESS // SDL_WINDOW_FULLSCREEN //
    // SDL_SetRenderVSync(m_renderer, -1);                       // enable vsync

    return true;
}

void Game::Cleanup()
{
    SaveDataHandler::GetInstance().CleanUp();
    AssetsHandler::GetInstance().CleanUp();

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

    if (m_event->type == SDL_EVENT_QUIT)
        m_appResult = SDL_APP_SUCCESS;
}

void Game::Execute()
{
    FPSHandler::GetInstance().Execute();
    TestUpdate();
}

void Game::Render()
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    TestRender();

    SDL_RenderPresent(m_renderer);
}

#pragma endregion

#pragma region Private Methods
#pragma endregion

#pragma region Public Methods

void Game::ShowMessage(const string &title, const string &message)
{
#ifdef __EMSCRIPTEN__
    if (message.empty())
        printf("SDL Error: %s\n", SDL_GetError());
    else
        printf("%s: %s\n", title.c_str(), message.c_str());
#else
    if (message.empty())
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), SDL_GetError(), nullptr);
    else
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), message.c_str(), nullptr);

#endif
}

#pragma endregion

#pragma region Getters / Setters

SDL_Window *Game::GetWindow()
{
    return m_window;
}
void Game::SetWindow(SDL_Window *window)
{
    if (m_window == window)
        return;
    m_window = window;
}

SDL_Renderer *Game::GetRenderer()
{
    return m_renderer;
}
void Game::SetRenderer(SDL_Renderer *renderer)
{
    if (m_renderer == renderer)
        return;
    m_renderer = renderer;
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

MIX_Mixer *Game::GetMixer()
{
    return m_mixer;
}
void Game::SetMixer(MIX_Mixer *mixer)
{
    if (m_mixer == mixer)
        return;
    m_mixer = mixer;
}

#pragma endregion

void Game::TestInit()
{
    TestTextInit();
    TestAudioInit();
    TestImageInit();
    TestSaveDataInit();
    TestFPSInit();
}

void Game::TestUpdate()
{
    m_time = SDL_GetTicks() / 1000.f;
    m_red = (sin(m_time) + 1) / 2.0 * 255;
    m_green = (sin(m_time / 2) + 1) / 2.0 * 255;
    m_blue = (sin(m_time) * 2 + 1) / 2.0 * 255;

    TestFPSUpdate();
}

void Game::TestRender()
{
    SDL_SetRenderDrawColor(m_renderer, m_red, m_green, m_blue, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(m_renderer, nullptr);

    SDL_RenderTexture(m_renderer, m_imageTex, NULL, NULL);
    SDL_RenderTexture(m_renderer, m_messageTex, NULL, &m_messageDest);

    SDL_RenderTexture(m_renderer, m_fpsTex, NULL, &m_fpsDest);
}

void Game::TestTextInit()
{
    int fontId = AssetsHandler::GetInstance().UsedFont("assets/fonts/Inter-VariableFont.ttf", 36);
    TTF_Font *font = AssetsHandler::GetInstance().GetFont(fontId);

    const std::string_view text = "Hello SDL!";
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, text.data(), text.length(), {255, 255, 255});
    m_messageTex = SDL_CreateTextureFromSurface(m_renderer, surfaceMessage);
    auto messageTexProps = SDL_GetTextureProperties(m_messageTex);
    m_messageDest = SDL_FRect{
        .x = 0,
        .y = 0,
        .w = float(SDL_GetNumberProperty(messageTexProps, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)),
        .h = float(SDL_GetNumberProperty(messageTexProps, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0))};

    AssetsHandler::GetInstance().UnUsedFont(fontId);
    SDL_DestroySurface(surfaceMessage);
}

void Game::TestAudioInit()
{
    m_track = MIX_CreateTrack(m_mixer);

    int musicId = AssetsHandler::GetInstance().UsedMusic("assets/audio/the_entertainer.ogg");
    MIX_Audio *music = AssetsHandler::GetInstance().GetMusic(musicId);

    MIX_SetTrackAudio(m_track, music);
    MIX_PlayTrack(m_track, -1);
}

void Game::TestImageInit()
{
    m_time = 0;
    m_red = 0;
    m_green = 0;
    m_blue = 0;

    int textureId = AssetsHandler::GetInstance().UsedTexture("assets/sprites/gs_tiger.svg");
    m_imageTex = AssetsHandler::GetInstance().GetTexture(textureId);
}

void Game::TestSaveDataInit()
{
    int count_i = SaveDataHandler::GetInstance().LoadIntData("test-count-i", 0);
    count_i++;
    SaveDataHandler::GetInstance().SaveData("test-count-i", count_i);

    float count_f = SaveDataHandler::GetInstance().LoadFloatData("test-count-f", 0.0f);
    count_f += 0.1f;
    SaveDataHandler::GetInstance().SaveData("test-count-f", count_f);

    bool count_b = SaveDataHandler::GetInstance().LoadBoolData("test-count-b", false);
    count_b = not count_b;
    SaveDataHandler::GetInstance().SaveData("test-count-b", count_b);

    string count_s = SaveDataHandler::GetInstance().LoadStringData("test-count-s", "a");
    count_s += "a";
    SaveDataHandler::GetInstance().SaveData("test-count-s", count_s);

    ShowMessage("Integer", to_string(count_i));
    ShowMessage("Float", to_string(count_f));
    ShowMessage("Boolean", count_b ? "true" : "false");
    ShowMessage("String", count_s);
}

void Game::TestFPSInit()
{
    m_fontId = AssetsHandler::GetInstance().UsedFont("assets/fonts/Inter-VariableFont.ttf", 36);
    TTF_Font *font = AssetsHandler::GetInstance().GetFont(m_fontId);

    const std::string_view text = "0 FPS";
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, text.data(), text.length(), {255, 255, 255});
    m_fpsTex = SDL_CreateTextureFromSurface(m_renderer, surfaceMessage);
    auto messageTexProps = SDL_GetTextureProperties(m_fpsTex);
    m_fpsDest = SDL_FRect{
        .x = 0,
        .y = 100,
        .w = float(SDL_GetNumberProperty(messageTexProps, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)),
        .h = float(SDL_GetNumberProperty(messageTexProps, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0))};

    // AssetsHandler::GetInstance().UnUsedFont(m_fontId);
    SDL_DestroySurface(surfaceMessage);
}

void Game::TestFPSUpdate()
{
    float deltaTime = FPSHandler::GetInstance().GetDeltaTime();
    float fps = (deltaTime > 0.0f) ? static_cast<float>(1.0f / deltaTime) : 0;
    TTF_Font *font = AssetsHandler::GetInstance().GetFont(m_fontId);

    const string text = format("{:.2f} FPS", FPSHandler::GetInstance().GetCurrentFPS());
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), text.length(), {255, 255, 255});
    if (m_fpsTex)
        SDL_DestroyTexture(m_fpsTex);
    m_fpsTex = SDL_CreateTextureFromSurface(m_renderer, surfaceMessage);
    auto messageTexProps = SDL_GetTextureProperties(m_fpsTex);
    m_fpsDest = SDL_FRect{
        .x = 0,
        .y = 100,
        .w = float(SDL_GetNumberProperty(messageTexProps, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)),
        .h = float(SDL_GetNumberProperty(messageTexProps, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0))};

    SDL_DestroySurface(surfaceMessage);
}
