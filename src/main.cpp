#pragma region Includes / Defines

#include "main.h"

using namespace std;
using namespace sge;

#pragma endregion

//--------------

// int main()
// {
//     if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD))
//         Game::GetInstance().SetAppResult(SDL_APP_FAILURE);

//     if (not TTF_Init())
//         Game::GetInstance().SetAppResult(SDL_APP_FAILURE);

//     if (not MIX_Init())
//         Game::GetInstance().SetAppResult(SDL_APP_FAILURE);

//     Game::GetInstance().Init();
//     Game::GetInstance().SetEvent(new SDL_Event());

// #ifdef __EMSCRIPTEN__
//     emscripten_set_main_loop_arg(
//         [](void *arg)
//         {
//             Game *game = static_cast<Game *>(arg);
//             if (game->GetAppResult() == SDL_APP_CONTINUE)
//             {
                
//                 while (SDL_PollEvent(game->GetEvent()))
//                 {
//                     game->Event();
//                 }
//                 game->Execute();
//                 game->Render();
//             }
//             else
//             {
//                 game->Cleanup();
//                 emscripten_cancel_main_loop();
//             }
//         },
//         &Game::GetInstance(),
//         0,
//         1);
// #else
//     while (Game::GetInstance().GetAppResult() == SDL_APP_CONTINUE)
//     {
//         while (SDL_PollEvent(Game::GetInstance().GetEvent()))
//         {
//             Game::GetInstance().Event();
//         }

//         Game::GetInstance().Execute();
//         Game::GetInstance().Render();
//     }

//     Game::GetInstance().Cleanup();
//     TTF_Quit();
//     MIX_Quit();
//     SDL_Quit();
// #endif
//     return 0;
// }

// #ifdef _WIN32
// int WinMain()
// {
//     return main();
// }
// #endif

//--------------

#pragma region Engine Methods

SDL_AppResult SDL_Fail()
{
    Game::GetInstance().SetAppResult(SDL_APP_FAILURE);
    return Game::GetInstance().GetAppResult();
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD))
        return SDL_Fail();

    if (not TTF_Init())
        return SDL_Fail();

    if (not MIX_Init())
        return SDL_Fail();

    Game::GetInstance().Init();

    Game::GetInstance().SetAppResult(SDL_APP_CONTINUE);
    return Game::GetInstance().GetAppResult();
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    Game::GetInstance().SetEvent(event);
    Game::GetInstance().Event();

    return Game::GetInstance().GetAppResult();
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    Game::GetInstance().Execute();
    Game::GetInstance().Render();

    return Game::GetInstance().GetAppResult();
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    Game::GetInstance().Cleanup();

    TTF_Quit();
    MIX_Quit();
    SDL_Quit();
}

#pragma endregion
