#pragma region Includes / Defines

#include "testHandler.h"

using namespace sge;
using namespace std;

#pragma endregion

#pragma region Constructor / Destructor

TestHandler::~TestHandler()
{
    Cleanup();
}

#pragma endregion

#pragma region Init / Cleanup

void TestHandler::Init()
{
#ifdef DEBUG_MODE
    m_messageTex = nullptr;
    m_imageTex = nullptr;
    m_fpsTex = nullptr;
    m_fontId = -1;
    m_bgColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    m_itemsWindowMode[0] = "windowed";
    m_itemsWindowMode[1] = "fullscreen";
    m_itemsWindowMode[2] = "borderless";

    ImguiInit();

    TestTextInit();
    TestAudioInit();
    TestImageInit();
#endif
}

void TestHandler::Cleanup()
{
#ifdef DEBUG_MODE
    if (m_messageTex)
        SDL_DestroyTexture(m_messageTex);
    if (m_imageTex)
        SDL_DestroyTexture(m_imageTex);
    if (m_fpsTex)
        SDL_DestroyTexture(m_fpsTex);

    if (m_fontId != -1)
        AssetsHandler::GetInstance().UnUsedFont(m_fontId);

    ImguiCleanup();
#endif
}

#pragma endregion

#pragma region Engine Methods

void TestHandler::Event(SDL_Event *event)
{
#ifdef DEBUG_MODE
    ImguiEvent(event);
#endif
}

void TestHandler::Execute()
{
#ifdef DEBUG_MODE
    ImguiExecute();
#endif
}

void TestHandler::Render()
{
#ifdef DEBUG_MODE
    SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), m_bgColor.x * 255, m_bgColor.y * 255, m_bgColor.z * 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(Game::GetInstance().GetRenderer(), nullptr);

    SDL_RenderTexture(Game::GetInstance().GetRenderer(), m_imageTex, NULL, NULL);
    SDL_RenderTexture(Game::GetInstance().GetRenderer(), m_messageTex, NULL, &m_messageDest);

    SDL_RenderTexture(Game::GetInstance().GetRenderer(), m_fpsTex, NULL, &m_fpsDest);

    ImguiRender();
#endif
}

#pragma endregion

#pragma region Private Methods

void TestHandler::ImguiInit()
{
    m_fps = FPSHandler::GetInstance().GetFps();
    m_vsync = FPSHandler::GetInstance().GetVsync();

    m_windowWidth = ResolutionHandler::GetInstance().GetWidth();
    m_windowHeight = ResolutionHandler::GetInstance().GetHeight();
    m_selectedWindowMode = ResolutionHandler::GetInstance().GetWindowMode() == m_itemsWindowMode[0] ? 0 : (ResolutionHandler::GetInstance().GetWindowMode() == m_itemsWindowMode[1] ? 1 : 2);

    m_volumeGlobal = AudioHandler::GetInstance().GetAudioVolume();
    m_volumeMusic = AudioHandler::GetInstance().GetMusicVolume();
    m_volumeSFX = AudioHandler::GetInstance().GetSoundVolume();

    m_intSaveData = SaveDataHandler::GetInstance().LoadIntData("test-count-i", 0);
    m_boolSaveData = SaveDataHandler::GetInstance().LoadBoolData("test-count-b", false);
    m_floatSaveData = SaveDataHandler::GetInstance().LoadFloatData("test-count-f", 0.0f);
    m_stringSaveData = SaveDataHandler::GetInstance().LoadStringData("test-count-s", "a");
    m_jsonSaveData = SaveDataHandler::GetInstance().LoadJsonData("test-count-j", cJSON_CreateObject());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = ImGui::GetIO();
    (void)m_io;
    m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    ImGuiStyle &style = ImGui::GetStyle();
    // style.ScaleAllSizes(main_scale);
    // style.FontScaleDpi = main_scale;

    ImGui_ImplSDL3_InitForSDLRenderer(Game::GetInstance().GetWindow(), Game::GetInstance().GetRenderer());
    ImGui_ImplSDLRenderer3_Init(Game::GetInstance().GetRenderer());

    m_bgColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
}

void TestHandler::ImguiEvent(SDL_Event *event)
{
    ImGui_ImplSDL3_ProcessEvent(event);
}

void TestHandler::ImguiExecute()
{
}

void TestHandler::ImguiRender()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    float deltaTime = FPSHandler::GetInstance().GetDeltaTime();
    float fps = (deltaTime > 0.0f) ? static_cast<float>(1.0f / deltaTime) : 0;

    ImGui::Begin("Debug Window");
    ImGui::Text("FPS: %.2f", fps);

    if (ImGui::InputInt("FPS", &m_fps))
        FPSHandler::GetInstance().SetFps(m_fps);

    if (ImGui::Checkbox("VSync", &m_vsync))
        FPSHandler::GetInstance().SetVsync(m_vsync);

    ImGui::Separator();

    if (ImGui::InputInt("Width", &m_windowWidth))
        ResolutionHandler::GetInstance().SetWidth(m_windowWidth);

    if (ImGui::InputInt("Height", &m_windowHeight))
        ResolutionHandler::GetInstance().SetHeight(m_windowHeight);

    if (ImGui::ListBox("Window Mode", &m_selectedWindowMode, m_itemsWindowMode, IM_ARRAYSIZE(m_itemsWindowMode), 3))
    {
        ResolutionHandler::GetInstance().SetWindowMode(string(m_itemsWindowMode[m_selectedWindowMode]));
        m_windowHeight = ResolutionHandler::GetInstance().GetHeight();
        m_windowWidth = ResolutionHandler::GetInstance().GetWidth();
    }

    ImGui::Separator();

    ImGui::ColorEdit3("Background Color", (float *)&m_bgColor);

    ImGui::Separator();

    if (ImGui::SliderFloat("Audio Volume", &m_volumeGlobal, 0.0f, 2.0f))
        AudioHandler::GetInstance().SetAudioVolume(m_volumeGlobal);

    if (ImGui::SliderFloat("Music Volume", &m_volumeMusic, 0.0f, 2.0f))
        AudioHandler::GetInstance().SetMusicVolume(m_volumeMusic);

    if (ImGui::SliderFloat("SFX Volume", &m_volumeSFX, 0.0f, 2.0f))
        AudioHandler::GetInstance().SetSoundVolume(m_volumeSFX);

    ImGui::Separator();

    if (ImGui::InputInt("Integer Save Data", &m_intSaveData))
        SaveDataHandler::GetInstance().SaveData("test-count-i", m_intSaveData);

    if (ImGui::InputFloat("Float Save Data", &m_floatSaveData))
        SaveDataHandler::GetInstance().SaveData("test-count-f", m_floatSaveData);

    if (ImGui::Checkbox("Boolean Save Data", &m_boolSaveData))
        SaveDataHandler::GetInstance().SaveData("test-count-b", m_boolSaveData);

    char buffer[1024];
    std::snprintf(buffer, sizeof(buffer), "%s", m_stringSaveData.c_str());
    if (ImGui::InputText("String Save Data", buffer, sizeof(buffer)))
    {
        m_stringSaveData = std::string(buffer);
        SaveDataHandler::GetInstance().SaveData("test-count-s", m_stringSaveData);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), Game::GetInstance().GetRenderer());
}

void TestHandler::ImguiCleanup()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}


void TestHandler::TestTextInit()
{
    int fontId = AssetsHandler::GetInstance().UsedFont("assets/fonts/Inter-VariableFont.ttf", 36);
    TTF_Font *font = AssetsHandler::GetInstance().GetFont(fontId);

    const std::string_view text = "Hello SDL!";
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(font, text.data(), text.length(), {255, 255, 255});
    m_messageTex = SDL_CreateTextureFromSurface(Game::GetInstance().GetRenderer(), surfaceMessage);
    auto messageTexProps = SDL_GetTextureProperties(m_messageTex);
    m_messageDest = SDL_FRect{
        .x = 0,
        .y = 0,
        .w = float(SDL_GetNumberProperty(messageTexProps, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)),
        .h = float(SDL_GetNumberProperty(messageTexProps, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0))};

    AssetsHandler::GetInstance().UnUsedFont(fontId);
    SDL_DestroySurface(surfaceMessage);
}

void TestHandler::TestAudioInit()
{
    AudioHandler::GetInstance().PlayMusic("assets/audio/the_entertainer.ogg", -1);
}

void TestHandler::TestImageInit()
{
    int textureId = AssetsHandler::GetInstance().UsedTexture("assets/sprites/gs_tiger.svg");
    m_imageTex = AssetsHandler::GetInstance().GetTexture(textureId);
}

void TestHandler::TestStatusInit()
{
    StatusHandler::GetInstance().OnVisibilityChange.subscribe([](bool visibility)
                                                              { printf("Visibility changed: %s\n", visibility ? "true" : "false"); });

    StatusHandler::GetInstance().OnFocusChange.subscribe([](bool focused)
                                                         { printf("Focus changed: %s\n", focused ? "true" : "false"); });

    StatusHandler::GetInstance().OnWasHiddenChange.subscribe([](bool wasHidden)
                                                             { printf("WasHidden changed: %s\n", wasHidden ? "true" : "false"); });
}

#pragma endregion

#pragma region Public Methods

void TestHandler::Logger(const std::string &title, const char *fmt, ...)
{
#ifdef DEBUG_MODE
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (fmt == nullptr || *fmt == '\0')
        SDL_Log("SDL Error: %s\n", SDL_GetError());
    else
        SDL_Log("%s: %s\n", title.c_str(), buffer);
#endif
}

#pragma endregion

#pragma region Getters / Setters
#pragma endregion

