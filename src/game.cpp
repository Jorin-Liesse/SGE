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

    int infoDataId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");
    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    string title = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "project"));
    string iconPath = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "icon-path"));

    AssetsHandler::GetInstance().UnUsedJson(infoDataId);

    SDL_SetWindowTitle(m_window, title.c_str());

    SDL_Surface *icon = IMG_Load(iconPath.c_str());
    SDL_SetWindowIcon(m_window, icon);
    SDL_DestroySurface(icon);

    TestInit();

    return true;
}

void Game::Cleanup()
{
    SaveDataHandler::GetInstance().CleanUp();
    AssetsHandler::GetInstance().CleanUp();
    FPSHandler::GetInstance().CleanUp();
    StatusHandler::GetInstance().Cleanup();
    AudioHandler::GetInstance().CleanUp();
    ResolutionHandler::GetInstance().Cleanup();
    InputHandler::GetInstance().Cleanup();

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

    StatusHandler::GetInstance().HandleEvents(m_event);
    ResolutionHandler::GetInstance().HandleEvents(m_event);
    InputHandler::GetInstance().HandleEvents(m_event);

    TestEvent(m_event);

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

void Game::Logger(const std::string &title, const char *fmt, ...)
{

    // Format the message
    char buffer[1024]; // enough for most log messages
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

#ifdef __EMSCRIPTEN__
    if (fmt == nullptr || *fmt == '\0')
        SDL_Log("SDL Error: %s\n", SDL_GetError());
    else
        SDL_Log("%s: %s\n", title.c_str(), buffer);
#else
    if (fmt == nullptr || *fmt == '\0')
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), SDL_GetError(), nullptr);
    else
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), buffer, nullptr);
#endif
    // #ifdef __EMSCRIPTEN__
    //     if (message.empty())
    //         printf("SDL Error: %s\n", SDL_GetError());
    //     else
    //         printf("%s: %s\n", title.c_str(), message.c_str());
    // #else
    //     if (message.empty())
    //         SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), SDL_GetError(), nullptr);
    //     else
    //         SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), message.c_str(), nullptr);

    // #endif
}

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

void Game::TestInit()
{
    TestTextInit();
    TestAudioInit();
    TestImageInit();
    TestSaveDataInit();
    TestFPSInit();
    TestStatusInit();
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

void Game::TestEvent(SDL_Event *event)
{
    if (m_event->type == SDL_EVENT_KEY_DOWN)
    {
        if (m_event->key.key == SDLK_KP_0)
        {
            int jsonId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");
            cJSON *infoJson = AssetsHandler::GetInstance().GetJson(jsonId);

            string title = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "project"));
            string iconPath = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "icon-path"));

            int defaultWidth = cJSON_GetObjectItem(infoJson, "default-width")->valueint;
            int defaultHeight = cJSON_GetObjectItem(infoJson, "default-height")->valueint;
            string defaultWindowMode = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "default-window-mode"));

            float defaultAudioVolume = float(cJSON_GetObjectItem(infoJson, "default-audio-volume")->valuedouble);
            float defaultMusicVolume = float(cJSON_GetObjectItem(infoJson, "default-music-volume")->valuedouble);
            float defaultSoundVolume = float(cJSON_GetObjectItem(infoJson, "default-sound-volume")->valuedouble);

            int defaultFps = cJSON_GetObjectItem(infoJson, "default-fps")->valueint;
            bool defaultVsync = cJSON_GetObjectItem(infoJson, "default-vsync")->valueint;

            Logger("Info Json",
                   "Project: {}\n"
                   "Icon Path: {}\n"
                   "Default Width: {}\n"
                   "Default Height: {}\n"
                   "Default Window Mode: {}\n"
                   "Default Audio Volume: {:.2f}\n"
                   "Default Music Volume: {:.2f}\n"
                   "Default Sound Volume: {:.2f}\n"
                   "Default FPS: {}\n"
                   "Default VSync: {}",
                   title,
                   iconPath,
                   defaultWidth,
                   defaultHeight,
                   defaultWindowMode,
                   defaultAudioVolume,
                   defaultMusicVolume,
                   defaultSoundVolume,
                   defaultFps,
                   defaultVsync ? "true" : "false");
                   
            AssetsHandler::GetInstance().UnUsedJson(jsonId);
        }

        // else if (m_event->key.key == SDLK_KP_1)
        // {
        //     m_width = SaveDataHandler::GetInstance().LoadIntData("width");
        //     m_height = SaveDataHandler::GetInstance().LoadIntData("height");
        //     m_windowMode = SaveDataHandler::GetInstance().LoadStringData("window-mode");

        //     m_fps = SaveDataHandler::GetInstance().LoadIntData("fps");
        //     m_vsync = SaveDataHandler::GetInstance().LoadBoolData("vsync");

        //     m_audioVolume = SaveDataHandler::GetInstance().LoadFloatData("audio-volume");
        //     m_musicVolume = SaveDataHandler::GetInstance().LoadFloatData("music-volume");
        //     m_soundVolume = SaveDataHandler::GetInstance().LoadFloatData("sound-volume");

        //     ShowMessage("Load Settings", format("Width: {}\nHeight: {}\nWindow Mode: {}\nFPS: {}\nVSync: {}\nAudio Volume: {:.2f}\nMusic Volume: {:.2f}\nSound Volume: {:.2f}",
        //                                         m_width, m_height, m_windowMode, m_fps, m_vsync ? "true" : "false", m_audioVolume, m_musicVolume, m_soundVolume));
        // }

        else if (m_event->key.key == SDLK_LEFT)
        {
            AudioHandler::GetInstance().SetAudioVolume(AudioHandler::GetInstance().GetAudioVolume() - 0.1f);
            Logger("Audio Volume", "{:.2f}", AudioHandler::GetInstance().GetAudioVolume());
        }
        else if (m_event->key.key == SDLK_RIGHT)
        {
            AudioHandler::GetInstance().SetAudioVolume(AudioHandler::GetInstance().GetAudioVolume() + 0.1f);
            Logger("Audio Volume", "{:.2f}", AudioHandler::GetInstance().GetAudioVolume());
        }
    }
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
    AudioHandler::GetInstance().PlayMusic("assets/audio/the_entertainer.ogg", -1);
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

    // ShowMessage("Integer", to_string(count_i));
    // ShowMessage("Float", to_string(count_f));
    // ShowMessage("Boolean", count_b ? "true" : "false");
    // ShowMessage("String", count_s);
}

void Game::TestStatusInit()
{
    StatusHandler::GetInstance().OnVisibilityChange.subscribe([](bool visibility)
                                                              { printf("Visibility changed: %s\n", visibility ? "true" : "false"); });

    StatusHandler::GetInstance().OnFocusChange.subscribe([](bool focused)
                                                         { printf("Focus changed: %s\n", focused ? "true" : "false"); });

    StatusHandler::GetInstance().OnWasHiddenChange.subscribe([](bool wasHidden)
                                                             { printf("WasHidden changed: %s\n", wasHidden ? "true" : "false"); });
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
