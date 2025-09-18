#pragma region Includes / Defines

#include "handlers/testHandler.h"

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
    m_io = nullptr;

    m_showDemoWindow = true;
    m_showAnotherWindow = false;
    m_clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    m_io = &ImGui::GetIO();
    (void)m_io;
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    m_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();

    ImGui_ImplSDL3_InitForSDLRenderer(Game::GetInstance().GetWindow(), Game::GetInstance().GetRenderer());
    ImGui_ImplSDLRenderer3_Init(Game::GetInstance().GetRenderer());

    m_intSaveData = SaveDataHandler::GetInstance().LoadIntData("test-count-i", 0);
    m_boolSaveData = SaveDataHandler::GetInstance().LoadBoolData("test-count-b", false);
    m_floatSaveData = SaveDataHandler::GetInstance().LoadFloatData("test-count-f", 0.0f);
    m_stringSaveData = SaveDataHandler::GetInstance().LoadStringData("test-count-s", "a");
    m_jsonSaveData = cJSON_Print(SaveDataHandler::GetInstance().LoadJsonData("test-count-j", cJSON_CreateObject()));

    TestTextInit();
    TestAudioInit();
    TestImageInit();
}

void TestHandler::Cleanup()
{
    if (m_messageTex)
        SDL_DestroyTexture(m_messageTex);
    if (m_imageTex)
        SDL_DestroyTexture(m_imageTex);

    if (m_fontId != -1)
        AssetsHandler::GetInstance().UnUsedFont(m_fontId);

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

#pragma endregion

#pragma region Engine Methods

void TestHandler::Event(SDL_Event *event)
{
    ImGui_ImplSDL3_ProcessEvent(event);
}

void TestHandler::Execute()
{
}

void TestHandler::Render()
{
    int w, h;
    SDL_GetCurrentRenderOutputSize(Game::GetInstance().GetRenderer(), &w, &h);
    SDL_FRect fullscreen = {0, 0, (float)w, (float)h};

    SDL_SetRenderDrawColorFloat(Game::GetInstance().GetRenderer(), m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
    SDL_RenderFillRect(Game::GetInstance().GetRenderer(), &fullscreen);

    SDL_RenderTexture(Game::GetInstance().GetRenderer(), m_imageTex, NULL, NULL);
    SDL_RenderTexture(Game::GetInstance().GetRenderer(), m_messageTex, NULL, &m_messageDest);
}

void TestHandler::ImGuiRender()
{
    SDL_Texture *targetTexture;
    float scaleX, scaleY;

    targetTexture = SDL_GetRenderTarget(Game::GetInstance().GetRenderer());
    SDL_GetRenderScale(Game::GetInstance().GetRenderer(), &scaleX, &scaleY);

    SDL_SetRenderTarget(Game::GetInstance().GetRenderer(), NULL);
    SDL_SetRenderScale(Game::GetInstance().GetRenderer(), m_io->DisplayFramebufferScale.x, m_io->DisplayFramebufferScale.y);
    

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImguiLoggerWindow();
    ImguiDebugWindow();

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), Game::GetInstance().GetRenderer());

    SDL_SetRenderScale(Game::GetInstance().GetRenderer(), scaleX, scaleY);
    SDL_SetRenderTarget(Game::GetInstance().GetRenderer(), targetTexture);
}

#pragma endregion

#pragma region Private Methods

void TestHandler::ImguiDebugWindow()
{
    ImGui::Begin("Debug");
    ImGui::ColorEdit4("Background Color", (float *)&m_clearColor);

    ImGui::Separator();

    ImGui::Text("Window Size: %dx%d", ResolutionHandler::GetInstance().GetWindowWidth(), ResolutionHandler::GetInstance().GetWindowHeight());

    float scale = ResolutionHandler::GetInstance().GetScale();
    if (ImGui::SliderFloat("Quality", &scale, 0.25f, 1.0f))
        ResolutionHandler::GetInstance().SetScale(scale);

    ImGui::Separator();

    ImGui::Text("FPS: %.2f", FpsHandler::GetInstance().GetCurrentFPS());

    int fps = FpsHandler::GetInstance().GetFps();
    if (ImGui::InputInt("FPS", &fps))
        FpsHandler::GetInstance().SetFps(fps);

    bool vsync = FpsHandler::GetInstance().GetVsync();
    if (ImGui::Checkbox("VSync", &vsync))
        FpsHandler::GetInstance().SetVsync(vsync);

    ImGui::Separator();

    float volumeGlobal = AudioHandler::GetInstance().GetAudioVolume();
    if (ImGui::SliderFloat("Audio Volume", &volumeGlobal, 0.0f, 2.0f))
        AudioHandler::GetInstance().SetAudioVolume(volumeGlobal);

    float volumeMusic = AudioHandler::GetInstance().GetMusicVolume();
    if (ImGui::SliderFloat("Music Volume", &volumeMusic, 0.0f, 2.0f))
        AudioHandler::GetInstance().SetMusicVolume(volumeMusic);

    float volumeSFX = AudioHandler::GetInstance().GetSoundVolume();
    if (ImGui::SliderFloat("SFX Volume", &volumeSFX, 0.0f, 2.0f))
        AudioHandler::GetInstance().SetSoundVolume(volumeSFX);

    ImGui::Separator();

    if (ImGui::InputInt("Integer Save Data", &m_intSaveData))
        SaveDataHandler::GetInstance().SaveData("test-count-i", m_intSaveData);

    if (ImGui::InputFloat("Float Save Data", &m_floatSaveData))
        SaveDataHandler::GetInstance().SaveData("test-count-f", m_floatSaveData);

    if (ImGui::Checkbox("Boolean Save Data", &m_boolSaveData))
        SaveDataHandler::GetInstance().SaveData("test-count-b", m_boolSaveData);

    std::snprintf(m_stringBuffer, sizeof(m_stringBuffer), "%s", m_stringSaveData.c_str());
    if (ImGui::InputText("String Save Data", m_stringBuffer, sizeof(m_stringBuffer)))
    {
        m_stringSaveData = std::string(m_stringBuffer);
        SaveDataHandler::GetInstance().SaveData("test-count-s", m_stringSaveData);
    }

    ImVec4 textColor = m_isJsonValid ? ImVec4(1,1,1,1) : ImVec4(1,0,0,1);
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);

    std::snprintf(m_jsonBuffer, sizeof(m_jsonBuffer), "%s", m_jsonSaveData.c_str());
    if (ImGui::InputTextMultiline("JSON Save Data", m_jsonBuffer, sizeof(m_jsonBuffer)))
    {
        m_jsonSaveData = std::string(m_jsonBuffer);

        cJSON *json = cJSON_Parse(m_jsonBuffer);
        if (json)
        {
            SaveDataHandler::GetInstance().SaveData("test-count-j", json);
            m_isJsonValid = true;
        }
        else
        {
            m_isJsonValid = false;
        }
    }

    ImGui::PopStyleColor();
    ImGui::End();
}

void TestHandler::ImguiLoggerWindow()
{
    ImGui::Begin("Logger");

    // Filters section
    if (ImGui::CollapsingHeader("Filters", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto it = m_filterTitles.begin(); it != m_filterTitles.end(); ++it)
        {
            ImGui::Checkbox(it->first.c_str(), &it->second);

            // Place next checkbox on same line, unless we're near the right edge
            if ((std::next(it) != m_filterTitles.end()) && ImGui::GetContentRegionAvail().x > 150.0f)
                ImGui::SameLine();
        }
    }

    // Clear button (right-aligned above log window)
    ImGui::Separator();
    if (ImGui::Button("Clear"))
        ClearLog();

    ImGui::SameLine();
    ImGui::Text("Log entries: %d", static_cast<int>(m_itemsLog.size()));

    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    for (const auto &entry : m_itemsLog)
    {
        // Skip if this title is turned off
        if (!m_filterTitles[entry.title])
            continue;

        ImGui::Text("[%s] %s", entry.title.c_str(), entry.message.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
}

void TestHandler::ClearLog()
{
    m_filterTitles.clear();
    m_itemsLog.clear();
}

void TestHandler::TestTextInit()
{
    m_messageTex = nullptr;
    m_fontId = -1;

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
    m_imageTex = nullptr;

    int textureId = AssetsHandler::GetInstance().UsedTexture("assets/sprites/gs_tiger.svg");
    m_imageTex = AssetsHandler::GetInstance().GetTexture(textureId);
}

#pragma endregion

#pragma region Public Methods

void TestHandler::Logger(const std::string &title, const char *fmt, ...)
{
    // #ifdef DEBUG_MODE
    char buffer[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    m_itemsLog.push_back({title, buffer});

    if (m_filterTitles.find(title) == m_filterTitles.end())
        m_filterTitles[title] = true;
    // #endif
}

#pragma endregion

#pragma region Getters / Setters
#pragma endregion
