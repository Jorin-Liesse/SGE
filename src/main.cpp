#pragma region Includes / Defines

#include "main.h"

using namespace sge;

#pragma endregion

int main(int argc, char *argv[])
{
    Game::GetInstance().Init();

#ifdef __EMSCRIPTEN__
emscripten_set_main_loop_arg(
    [](void* arg) { static_cast<Game*>(arg)->Run(); },
    &Game::GetInstance(),
    1, // use requestAnimationFrame
    1);
emscripten_set_main_loop_timing(EM_TIMING_SETTIMEOUT, 1000 / 30);
#else
    while (Game::GetInstance().GetRunning())
        Game::GetInstance().Run();

#endif

    return 0;
}

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif
