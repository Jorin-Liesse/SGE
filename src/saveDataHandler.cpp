#pragma region Includes / Defines

#include "saveDataHandler.h"

using namespace std;
using namespace sge;

#pragma endregion

#pragma region Constructor / Destructor

SaveDataHandler::~SaveDataHandler()
{
    CleanUp();
}

#pragma endregion

#pragma region Init / Cleanup

void SaveDataHandler::Init()
{
    m_count = -1;

    // #ifdef __EMSCRIPTEN__
    // EM_ASM(
    //     FS.mkdir('/persistent');
    //     FS.mount(IDBFS, {}, '/persistent');
    //     FS.syncfs(true, function (err) {
    //         if (err) console.error("Load sync error:", err);
    //     });
    // );
    // #endif

#ifdef __EMSCRIPTEN__
    EM_ASM({
        FS.mkdir('/persistent');
        FS.mount(IDBFS, {}, '/persistent');
        FS.syncfs(true, function(err) {
        if (err) {
            console.error("Load sync error:", err);
        } else {
            // Call back into C++ once sync is done
            _on_fs_synced();
        } });
    });
#else
    // If not emscripten, just call directly
    on_fs_synced();
#endif

    // Load existing counter
    load_counter();

    // Increment on each visit
    m_count++;

    // Save and persist
    save_counter();
    persist_counter();

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Counter", std::to_string(m_count).c_str(), nullptr);
}

void SaveDataHandler::CleanUp()
{
}

void SaveDataHandler::load_counter()
{
    FILE *f = fopen("/persistent/counter.txt", "r");
    if (f)
    {
        fscanf(f, "%d", &m_count);
        fclose(f);
    }
    else
    {
        m_count = 0;
    }
}

void SaveDataHandler::save_counter()
{
    FILE *f = fopen("/persistent/counter.txt", "w");
    if (f)
    {
        fprintf(f, "%d", m_count);
        fclose(f);
    }
}

void SaveDataHandler::persist_counter()
{
#ifdef __EMSCRIPTEN__
    EM_ASM(
        FS.syncfs(false, function(err) {
            if (err) console.error("Sync error:", err); }););
#endif
}

void SaveDataHandler::after_sync() {
    load_counter();
    m_count++;
    save_counter();
    persist_counter();

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Counter",
                             std::to_string(m_count).c_str(), nullptr);
}

#pragma endregion

#pragma region Private Methods
#pragma endregion

#pragma region Public Methods
#pragma endregion

#pragma region Getters / Setters
#pragma endregion


extern "C" {
    void on_fs_synced() {
        SaveDataHandler::GetInstance().after_sync();
    }
}