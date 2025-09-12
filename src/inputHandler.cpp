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
        SDL_KeyboardEvent e = event->key;
        Game::GetInstance().Logger("InputHandler", "Key down: %s (scancode=%d)", SDL_GetKeyName(e.key), e.scancode);
    }
    else if (event->type == SDL_EVENT_KEY_UP)
    {
        SDL_KeyboardEvent e = event->key;
        Game::GetInstance().Logger("InputHandler", "Key up: %s (scancode=%d)", SDL_GetKeyName(e.key), e.scancode);
    }

    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        SDL_MouseButtonEvent e = event->button;
        Game::GetInstance().Logger("InputHandler", "Mouse button %d down", e.button);
    }
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
    {
        SDL_MouseButtonEvent e = event->button;
        Game::GetInstance().Logger("InputHandler", "Mouse button %d up", e.button);
    }
    else if (event->type == SDL_EVENT_MOUSE_MOTION)
    {
        SDL_MouseMotionEvent e = event->motion;
        Sint32 mouseX = e.x;
        Sint32 mouseY = e.y;

        Sint32 mouseRelX = e.xrel;
        Sint32 mouseRelY = e.yrel;

        Game::GetInstance().Logger("InputHandler", "Mouse moved to (%d, %d), relative (%d, %d)", mouseX, mouseY, mouseRelX, mouseRelY);
    }
    else if (event->type == SDL_EVENT_MOUSE_WHEEL)
    {
        SDL_MouseWheelEvent e = event->wheel; // use .wheel member instead of casting
        Sint32 scrollX = e.x;
        Sint32 scrollY = e.y;

        // flip if necessary
        if (e.direction == SDL_MOUSEWHEEL_FLIPPED)
            scrollY = -scrollY;

        Game::GetInstance().Logger("InputHandler", "Mouse wheel scrolled: x=%d, y=%d", scrollX, scrollY);
    }

    // Touch events
    else if (event->type == SDL_EVENT_FINGER_DOWN)
    {
        SDL_TouchFingerEvent e = event->tfinger;

        SDL_Log("Finger %lld down at (%.3f, %.3f) pressure %.3f", (long long)e.fingerID, e.x, e.y, e.pressure);
    }
    else if (event->type == SDL_EVENT_FINGER_UP)
    {
        SDL_TouchFingerEvent e = event->tfinger;
        SDL_Log("Finger %lld up at (%.3f, %.3f)", (long long)e.fingerID, e.x, e.y);
    }
    else if (event->type == SDL_EVENT_FINGER_MOTION)
    {
        SDL_TouchFingerEvent e = event->tfinger;

        Sint32 fingerX = e.x;
        Sint32 fingerY = e.y;

        Sint32 fingerRelX = e.dx;
        Sint32 fingerRelY = e.dy;

        SDL_Log("Finger %lld moved to (%.3f, %.3f) delta (%.3f, %.3f)", (long long)e.fingerID, fingerX, fingerY, fingerRelX, fingerRelY);
    }

    // Gamepad events
    else if (event->type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
    {
        SDL_GamepadButtonEvent e = event->gbutton;
        SDL_Log("Gamepad %d button %d down", e.which, e.button);
    }
    else if (event->type == SDL_EVENT_GAMEPAD_BUTTON_UP)
    {
        SDL_GamepadButtonEvent e = event->gbutton;
        SDL_Log("Gamepad %d button %d up", e.which, e.button);
    }
    else if (event->type == SDL_EVENT_GAMEPAD_AXIS_MOTION)
    {
        SDL_GamepadAxisEvent e = event->gaxis;

        Sint32 axisValue = e.value;

        SDL_Log("Gamepad %d axis %d value %d", e.which, e.axis, axisValue);
    }

    // Gamepad hotplugging
    else if (event->type == SDL_EVENT_GAMEPAD_ADDED)
    {
        SDL_GamepadDeviceEvent e = event->gdevice;
        SDL_Gamepad *pad = SDL_OpenGamepad(e.which); // e.which = device index
        if (pad)
            SDL_Log("Gamepad %d connected: %s", e.which, SDL_GetGamepadName(pad));
        
        else
            SDL_Log("Could not open gamepad %d: %s", e.which, SDL_GetError());
        
    }
    else if (event->type == SDL_EVENT_GAMEPAD_REMOVED)
    {
        SDL_GamepadDeviceEvent e = event->gdevice;
        SDL_CloseGamepad(SDL_GetGamepadFromID(e.which));
    }
}

#pragma endregion

#pragma region Private Methods
#pragma endregion

#pragma region Public Methods
#pragma endregion

#pragma region Getters / Setters
#pragma endregion
