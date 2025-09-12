#pragma region Includes / Defines

#include "inputHandler.h"

using namespace sge;
using namespace std;

#pragma endregion

#pragma region Constructor / Destructor

InputHandler::~InputHandler()
{
    Cleanup();
}

#pragma endregion

#pragma region Init / Cleanup

void InputHandler::Init()
{
    int infoDataId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");
    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    cJSON *defaultInputMap = cJSON_GetObjectItem(infoJson, "default-input-map");

    cJSON *value = SaveDataHandler::GetInstance().LoadJsonData("input-map", defaultInputMap);

    AssetsHandler::GetInstance().UnUsedJson(infoDataId);
}

void InputHandler::Cleanup()
{
}

#pragma endregion

#pragma region Engine Methods

void InputHandler::HandleEvents(SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN)
    {
        SDL_KeyboardEvent *e = (SDL_KeyboardEvent *)event;
        Game::GetInstance().Logger("InputHandler", "Key down: %s (scancode=%d)", SDL_GetKeyName(e->key), e->scancode);
    }
    else if (event->type == SDL_EVENT_KEY_UP)
    {
        SDL_KeyboardEvent *e = (SDL_KeyboardEvent *)event;
        Game::GetInstance().Logger("InputHandler", "Key up: %s (scancode=%d)", SDL_GetKeyName(e->key), e->scancode);
    }

    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        SDL_MouseButtonEvent *e = (SDL_MouseButtonEvent *)event;
        Game::GetInstance().Logger("InputHandler", "Mouse button %d down at (%d, %d)", e->button, e->x, e->y);
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        SDL_MouseButtonEvent *e = (SDL_MouseButtonEvent *)event;
        Game::GetInstance().Logger("InputHandler", "Mouse button %d up at (%d, %d)", e->button, e->x, e->y);
    }
    // else if (event->type == SDL_EVENT_MOUSE_MOTION)
    // {
    //     SDL_MouseMotionEvent *e = (SDL_MouseMotionEvent *)event;
    //     Game::GetInstance().Logger("InputHandler", "Mouse moved to (%d, %d), relative (%d, %d)", e->x, e->y, e->xrel, e->yrel);
    // }

    // Touch events
    else if (event->type == SDL_EVENT_FINGER_DOWN)
    {
        SDL_TouchFingerEvent *e = (SDL_TouchFingerEvent *)event;
        SDL_Log("Finger %lld down at (%.3f, %.3f) pressure %.3f",
                (long long)e->fingerID, e->x, e->y, e->pressure);
    }
    else if (event->type == SDL_EVENT_FINGER_UP)
    {
        SDL_TouchFingerEvent *e = (SDL_TouchFingerEvent *)event;
        SDL_Log("Finger %lld up at (%.3f, %.3f)",
                (long long)e->fingerID, e->x, e->y);
    }
    // else if (event->type == SDL_EVENT_FINGER_MOTION)
    // {
    //     SDL_TouchFingerEvent *e = (SDL_TouchFingerEvent *)event;
    //     SDL_Log("Finger %lld moved to (%.3f, %.3f) delta (%.3f, %.3f)",
    //             (long long)e->fingerID, e->x, e->y, e->dx, e->dy);
    // }

    // Gamepad events
    else if (event->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
    {
        SDL_GamepadButtonEvent *e = (SDL_GamepadButtonEvent *)event;
        SDL_Log("Gamepad %d button %d down", e->which, e->button);
    }
    else if (event->type == SDL_EVENT_GAMEPAD_BUTTON_UP)
    {
        SDL_GamepadButtonEvent *e = (SDL_GamepadButtonEvent *)event;
        SDL_Log("Gamepad %d button %d up", e->which, e->button);
    }
    // else if (event->type == SDL_EVENT_GAMEPAD_AXIS_MOTION)
    // {
    //     SDL_GamepadAxisEvent *e = (SDL_GamepadAxisEvent *)event;
    //     SDL_Log("Gamepad %d axis %d value %d", e->which, e->axis, e->value);
    // }

    // Gamepad hotplugging
    else if (event->type == SDL_EVENT_GAMEPAD_ADDED)
    {
        SDL_GamepadDeviceEvent *e = (SDL_GamepadDeviceEvent *)event;
        SDL_Gamepad *pad = SDL_OpenGamepad(e->which);
        if (pad)
        {
            SDL_Log("Gamepad %d connected: %s", e->which, SDL_GetGamepadName(pad));
        }
    }
    else if (event->type == SDL_EVENT_GAMEPAD_REMOVED)
    {
        SDL_GamepadDeviceEvent *e = (SDL_GamepadDeviceEvent *)event;
        SDL_Log("Gamepad %d disconnected", e->which);
    }
}

#pragma endregion

#pragma region Private Methods
#pragma endregion

#pragma region Public Methods
#pragma endregion

#pragma region Getters / Setters
#pragma endregion
