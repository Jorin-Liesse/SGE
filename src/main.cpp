#pragma region Includes / Defines

#include "main.h"

using namespace sge;

#pragma endregion

#pragma region Engine Methods

SDL_AppResult SDL_Fail()
{
    Game::GetInstance().ShowMessage("Error", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
#if __ANDROID__
    SDL_SetHint(SDL_HINT_ANDROID_BLOCK_ON_PAUSE, "1");
    SDL_SetHint(SDL_HINT_ANDROID_TRAP_BACK_BUTTON, "1");
#endif

    if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
        return SDL_Fail();

    if (not TTF_Init())
        return SDL_Fail();

    if (not MIX_Init())
        return SDL_Fail();

    Game::GetInstance().Init();

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    Game::GetInstance().SetEvent(event);
    Game::GetInstance().Event();

    return SDL_APP_CONTINUE;
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
