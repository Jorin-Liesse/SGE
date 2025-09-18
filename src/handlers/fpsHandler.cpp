#pragma region Includes / Defines

#include "handlers/fpsHandler.h"

using namespace std;
using namespace sge;

#pragma endregion

#pragma region Constructor / Destructor

FpsHandler::~FpsHandler()
{
    CleanUp();
}

#pragma endregion

#pragma region Init / Cleanup

void FpsHandler::Init(int infoDataId)
{
#ifdef __EMSCRIPTEN__
    m_previousTime = emscripten_get_now();
    m_lastTime = emscripten_get_now();
#else
    m_previousTime = SDL_GetPerformanceCounter();
    m_lastTime = SDL_GetPerformanceCounter();
#endif
    m_previousTime = SDL_GetPerformanceCounter();
    m_lastTime = SDL_GetPerformanceCounter();
    m_frames = 0;

    m_currentFPS = 0.0f;
    m_deltaTime = 0.0f;

    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    int defaultFps = cJSON_GetObjectItem(infoJson, "default-fps")->valueint;
    bool defaultVsync = cJSON_GetObjectItem(infoJson, "default-vsync")->valueint;

    m_fps = SaveDataHandler::GetInstance().LoadIntData("fps", defaultFps);
    m_vsync = SaveDataHandler::GetInstance().LoadBoolData("vsync", defaultVsync);

#ifdef __EMSCRIPTEN__
    EM_ASM({
        window.addEventListener("beforeunload", function() { FS.syncfs(false, function(err) {
                if (err) console.error("Final save error", err); }); });
    });
#endif

    ChangedFps();
    ChangedVsync();
}

void FpsHandler::CleanUp()
{
}

#pragma endregion

#pragma region Engine Methods

void FpsHandler::Execute()
{
#ifdef __EMSCRIPTEN__
    double frameStart = emscripten_get_now();
    m_deltaTime = (frameStart - m_previousTime) / 1000.0f;
    m_previousTime = frameStart;

    double elapsed = (frameStart - m_lastTime) / 1000.0f;
#else
    double frameStart = SDL_GetPerformanceCounter();
    m_deltaTime = (float)(frameStart - m_previousTime) / SDL_GetPerformanceFrequency();
    m_previousTime = frameStart;

    double elapsed = (frameStart - m_lastTime) / SDL_GetPerformanceFrequency();

    if (m_fps > 0)
    {
        double frameTimeMs = (double)(SDL_GetPerformanceCounter() - frameStart) / SDL_GetPerformanceFrequency() * 1000.0;
        double targetFrameTime = 1000.0 / static_cast<double>(m_fps);

        if (frameTimeMs < targetFrameTime)
        {
            SDL_Delay(static_cast<Uint32>(targetFrameTime - frameTimeMs));
        }
    }
#endif

    m_frames++;

    if (elapsed >= 1.0 || elapsed < 0)
    {
        m_currentFPS = m_frames / elapsed;
        m_frames = 0;
        m_lastTime = frameStart;
    }
}

#pragma endregion

#pragma region Private Methods

void FpsHandler::ChangedFps()
{
    SaveDataHandler::GetInstance().SaveData("fps", m_fps);

    OnFpsChange.invoke(m_fps);
}

void FpsHandler::ChangedVsync()
{
    if (m_vsync)
        SDL_SetRenderVSync(Game::GetInstance().GetRenderer(), -1); // enable vsync
    else
        SDL_SetRenderVSync(Game::GetInstance().GetRenderer(), 0); // disable vsync

    SaveDataHandler::GetInstance().SaveData("vsync", m_vsync);

    OnVsyncChange.invoke(m_vsync);
}

#pragma endregion

#pragma region Public Methods

#ifdef __EMSCRIPTEN__
bool FpsHandler::CanContinue()
{
    double now = emscripten_get_now() / 1000.0f;
    double last = m_previousTime / 1000.0f;

    if ((now - last) >= (1.0f / m_fps) || (now - last) <= 0 || m_fps <= 0)
        return true;

    return false;
}
#endif

#pragma endregion

#pragma region Getters / Setters

bool FpsHandler::GetVsync()
{
    return m_vsync;
}
void FpsHandler::SetVsync(bool vsync)
{
    if (m_vsync == vsync)
        return;
    m_vsync = vsync;
    ChangedVsync();
}

int FpsHandler::GetFps()
{
    return m_fps;
}
void FpsHandler::SetFps(int fps)
{
    if (m_fps == fps)
        return;
    m_fps = fps;
    ChangedFps();
}

float FpsHandler::GetCurrentFPS()
{
    return m_currentFPS;
}

float FpsHandler::GetDeltaTime()
{
    return m_deltaTime;
}

#pragma endregion
