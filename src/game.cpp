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

    AssetsHandler::GetInstance().Init();
    SaveDataHandler::GetInstance().Init();

    int infoDataId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");
    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    string title = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "project"));
    string iconPath = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "icon-path"));

    int defaultWidth = cJSON_GetObjectItem(infoJson, "default-width")->valueint;
    int defaultHeight = cJSON_GetObjectItem(infoJson, "default-height")->valueint;
    string defaultWindowMode = cJSON_GetStringValue(cJSON_GetObjectItem(infoJson, "default-window-mode"));
    int defaultFps = cJSON_GetObjectItem(infoJson, "default-fps")->valueint;
    bool defaultVsync = cJSON_GetObjectItem(infoJson, "default-vsync")->valueint;
    float defaultAudioVolume = float(cJSON_GetObjectItem(infoJson, "default-audio-volume")->valuedouble);
    float defaultMusicVolume = float(cJSON_GetObjectItem(infoJson, "default-music-volume")->valuedouble);
    float defaultSoundVolume = float(cJSON_GetObjectItem(infoJson, "default-sound-volume")->valuedouble);

    // SaveDataHandler::GetInstance().SaveData("player_name", "Alice");
    // SaveDataHandler::GetInstance().SaveData("score", 100);
    // SaveDataHandler::GetInstance().SaveData("health", 75.5f);
    // SaveDataHandler::GetInstance().SaveData("has_key", true);

    // int width = SaveDataHandler::GetInstance().LoadIntData("width", defaultWidth);
    // int height = SaveDataHandler::GetInstance().LoadIntData("height", defaultHeight);
    // string windowMode = SaveDataHandler::GetInstance().LoadStringData("window_mode", defaultWindowMode);
    // int fps = SaveDataHandler::GetInstance().LoadIntData("fps", defaultFps);
    // bool vsync = SaveDataHandler::GetInstance().LoadBoolData("vsync", defaultVsync);
    // float audioVolume = SaveDataHandler::GetInstance().LoadFloatData("audio_volume", defaultAudioVolume);
    // float musicVolume = SaveDataHandler::GetInstance().LoadFloatData("music_volume", defaultMusicVolume);
    // float soundVolume = SaveDataHandler::GetInstance().LoadFloatData("sound_volume", defaultSoundVolume);

    // int counter = SaveDataHandler::GetInstance().LoadIntData("launch_counter", 0);
    // counter++;
    // SaveDataHandler::GetInstance().SaveData("launch_counter", counter);
    // ShowMessage("Info", "This game has been launched " + to_string(counter) + " times!");

    // ShowMessage("Info", format("Title: {}\nIcon Path: {}\nWidth: {}\nHeight: {}\nWindow Mode: {}\nFPS: {}\nVSync: {}\nAudio Volume: {}\nMusic Volume: {}\nSound Volume: {}",
    //     title, iconPath, defaultWidth, defaultHeight, defaultWindowMode, defaultFps, defaultVsync, defaultAudioVolume, defaultMusicVolume, defaultSoundVolume));

    // SaveData("savefile.json", "playerName", "Alice");
    // SaveData("savefile.json", "level", "5");

    // std::string playerName = LoadData("savefile.json", "playerName", "Unknown");
    // std::string level = LoadData("savefile.json", "level", "0");

    // ShowMessage("Save Data", "Player Name: " + playerName + "\nLevel: " + level);

    InitSaveFolderWeb();

    // SaveDataWeb("playerName", "Alice");
    // SaveDataWeb("level", "5");

    // Load data
    std::string playerName = LoadDataWeb("playerName", "Unknown");
    std::string level = LoadDataWeb("level", "0");

    int levelInt = std::stoi(level);
    levelInt++;
    SaveDataWeb("level", std::to_string(levelInt));

    ShowMessage("Save Data", "Player Name: " + playerName + "\nLevel: " + std::to_string(levelInt));

    
    AssetsHandler::GetInstance().UnUsedJson(infoDataId);

    m_window = SDL_CreateWindow(title.c_str(), 500, 500, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    m_renderer = SDL_CreateRenderer(m_window, NULL);

    SDL_Surface *icon = IMG_Load(iconPath.c_str());
    SDL_SetWindowIcon(m_window, icon);
    SDL_DestroySurface(icon);

    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

    TestInit();

    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN); // SDL_WINDOW_BORDERLESS // SDL_WINDOW_FULLSCREEN //
    SDL_SetRenderVSync(m_renderer, -1);                       // enable vysnc

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

    MIX_Quit();
    TTF_Quit();
    SDL_Quit();
}

#pragma endregion

#pragma region Engine Methods

void Game::Event()
{
    if (not m_event)
        return;

    if (m_event->type == SDL_EVENT_QUIT)
        Game::GetInstance().SetAppResult(SDL_APP_SUCCESS);
}

void Game::Update()
{
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
    if (not message.empty())
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), message.c_str(), nullptr);
    else
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), SDL_GetError(), nullptr);
}

#pragma endregion

#pragma region Getters / Setters

SDL_Window *Game::GetWindow()
{
    return m_window;
}
void Game::SetWindow(SDL_Window *window)
{
    m_window = window;
}

SDL_Renderer *Game::GetRenderer()
{
    return m_renderer;
}
void Game::SetRenderer(SDL_Renderer *renderer)
{
    m_renderer = renderer;
}

SDL_Event *Game::GetEvent()
{
    return m_event;
}
void Game::SetEvent(SDL_Event *event)
{
    m_event = event;
}

SDL_AppResult Game::GetAppResult()
{
    return m_appResult;
}
void Game::SetAppResult(SDL_AppResult result)
{
    m_appResult = result;
}

MIX_Mixer *Game::GetMixer()
{
    return m_mixer;
}
void Game::SetMixer(MIX_Mixer *mixer)
{
    m_mixer = mixer;
}

#pragma endregion

void Game::TestInit()
{
    m_time = 0;
    m_red = 0;
    m_green = 0;
    m_blue = 0;

    // load the font
#if __ANDROID__
    filesystem::path basePath = ""; // on Android we do not want to use basepath. Instead, assets are available at the root directory.
#else
    auto basePathPtr = SDL_GetBasePath();
    const filesystem::path basePath = basePathPtr;
#endif

    const auto fontPath = basePath / "assets/fonts/Inter-VariableFont.ttf";
    TTF_Font *font = TTF_OpenFont(fontPath.string().c_str(), 36);
    const std::string_view text = "Hello SDL!";
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, text.data(), text.length(), {255, 255, 255});
    m_messageTex = SDL_CreateTextureFromSurface(m_renderer, surfaceMessage);
    auto messageTexProps = SDL_GetTextureProperties(m_messageTex);
    m_messageDest = SDL_FRect{
        .x = 0,
        .y = 0,
        .w = float(SDL_GetNumberProperty(messageTexProps, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)),
        .h = float(SDL_GetNumberProperty(messageTexProps, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0))};
    TTF_CloseFont(font);
    SDL_DestroySurface(surfaceMessage);

    auto svg_surface = IMG_Load((basePath / "assets/sprites/gs_tiger.svg").string().c_str());
    m_imageTex = SDL_CreateTextureFromSurface(m_renderer, svg_surface);
    SDL_DestroySurface(svg_surface);

    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    m_track = MIX_CreateTrack(m_mixer);
    auto musicPath = basePath / "assets/audio/the_entertainer.ogg";
    auto music = MIX_LoadAudio(m_mixer, musicPath.string().c_str(), false);
    MIX_SetTrackAudio(m_track, music);
    MIX_PlayTrack(m_track, NULL);
}

void Game::TestUpdate()
{
    m_time = SDL_GetTicks() / 1000.f;
    m_red = (sin(m_time) + 1) / 2.0 * 255;
    m_green = (sin(m_time / 2) + 1) / 2.0 * 255;
    m_blue = (sin(m_time) * 2 + 1) / 2.0 * 255;
}

void Game::TestRender()
{
    SDL_SetRenderDrawColor(m_renderer, m_red, m_green, m_blue, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(m_renderer, nullptr);

    SDL_RenderTexture(m_renderer, m_imageTex, NULL, NULL);
    SDL_RenderTexture(m_renderer, m_messageTex, NULL, &m_messageDest);
}

bool Game::SaveData(const std::string &filename, const std::string &key, const std::string &value)
{
    // Load existing JSON
    std::ifstream inFile(filename);
    cJSON *root = nullptr;

    if (inFile.is_open())
    {
        std::string content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        root = cJSON_Parse(content.c_str());
        if (!root)
        {
            root = cJSON_CreateObject();
        }
    }
    else
    {
        root = cJSON_CreateObject();
    }

    // Set or replace the key-value
    cJSON *item = cJSON_GetObjectItem(root, key.c_str());
    if (item)
    {
        cJSON_SetValuestring(item, value.c_str());
    }
    else
    {
        cJSON_AddStringToObject(root, key.c_str(), value.c_str());
    }

    // Write back to file
    char *jsonString = cJSON_Print(root);
    std::ofstream outFile(filename);
    if (!outFile.is_open())
    {
        cJSON_Delete(root);
        free(jsonString);
        return false;
    }

    outFile << jsonString;
    outFile.close();

    cJSON_Delete(root);
    free(jsonString);
    return true;
}

// Load a string value from JSON file by key
std::string Game::LoadData(const std::string &filename, const std::string &key, const std::string &defaultValue)
{
    std::ifstream inFile(filename);
    if (!inFile.is_open())
    {
        return defaultValue;
    }

    std::string content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    cJSON *root = cJSON_Parse(content.c_str());
    if (!root)
    {
        return defaultValue;
    }

    cJSON *item = cJSON_GetObjectItem(root, key.c_str());
    std::string value = (item && cJSON_IsString(item)) ? item->valuestring : defaultValue;

    cJSON_Delete(root);
    return value;
}


void Game::InitSaveFolderWeb() {
    #ifdef __EMSCRIPTEN__
    EM_ASM(
        {
            var folderPath = UTF8ToString($0);
            try {
                if (!FS.analyzePath(folderPath).exists) {
                    FS.mkdirTree(folderPath);
                }
                FS.mount(IDBFS, {}, folderPath);
            } catch(e) { console.log("Error mounting IDBFS:", e); }
        },
        SAVE_FOLDER.c_str()
    );

    // Load existing data from IndexedDB
    EM_ASM(
        FS.syncfs(true, function(err) { 
            if(err) console.log("Load error", err); 
            else console.log("Save folder synced from IndexedDB");
        })
    );
    #endif
}

// Save a key-value pair
bool Game::SaveDataWeb(const std::string& key, const std::string& value) {
    // Load existing JSON
    std::ifstream inFile(SAVE_FILE);
    cJSON* root = nullptr;

    if (inFile.is_open()) {
        std::string content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
        inFile.close();
        root = cJSON_Parse(content.c_str());
        if (!root) {
            root = cJSON_CreateObject();
        }
    } else {
        root = cJSON_CreateObject();
    }

    // Add or replace key-value
    cJSON* item = cJSON_GetObjectItem(root, key.c_str());
    if (item) {
        cJSON_SetValuestring(item, value.c_str());
    } else {
        cJSON_AddStringToObject(root, key.c_str(), value.c_str());
    }

    char* jsonString = cJSON_Print(root);
    std::ofstream outFile(SAVE_FILE);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing\n";
        cJSON_Delete(root);
        free(jsonString);
        return false;
    }

    outFile << jsonString;
    outFile.close();

    cJSON_Delete(root);
    free(jsonString);
#ifdef __EMSCRIPTEN__
    // Sync changes to IndexedDB
    EM_ASM(
        FS.syncfs(false, function(err) { 
            if(err) console.log("Save error", err); 
            else console.log("Save synced to IndexedDB");
        })
    );
#endif

    return true;
}

// Load a value by key
std::string Game::LoadDataWeb(const std::string& key, const std::string& defaultValue) {
    std::ifstream inFile(SAVE_FILE);
    if (!inFile.is_open()) return defaultValue;

    std::string content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    cJSON* root = cJSON_Parse(content.c_str());
    if (!root) return defaultValue;

    cJSON* item = cJSON_GetObjectItem(root, key.c_str());
    std::string value = (item && cJSON_IsString(item)) ? item->valuestring : defaultValue;

    cJSON_Delete(root);
    return value;
}
