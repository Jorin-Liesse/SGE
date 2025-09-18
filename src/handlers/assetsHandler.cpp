#pragma region Includes / Defines

#include "handlers/assetsHandler.h"

using namespace std;
using namespace sge;

#pragma endregion

#pragma region Constructor / Destructor

AssetsHandler::~AssetsHandler()
{
    CleanUp();
}

#pragma endregion

#pragma region Init / Cleanup

void AssetsHandler::Init()
{
    m_nextTextureID = 0;
    m_nextFontID = 0;
    m_nextMusicID = 0;
    m_nextSoundEffectID = 0;
    m_nextJsonID = 0;

    auto basePathPtr = SDL_GetBasePath();
    m_basePath = basePathPtr ? basePathPtr : "";
}

void AssetsHandler::CleanUp()
{
    // Textures
    for (auto &pair : m_textureIDs)
    {
        if (pair.second)
            SDL_DestroyTexture(pair.second);
    }
    m_textureIDs.clear();
    m_textures.clear();

    // Fonts
    for (auto &pair : m_fontIDs)
    {
        if (pair.second)
            TTF_CloseFont(pair.second);
    }
    m_fontIDs.clear();
    m_fonts.clear();

    // Music
    for (auto &pair : m_musicIDs)
    {
        if (pair.second)
            MIX_DestroyAudio(pair.second);
    }
    m_musicIDs.clear();
    m_music.clear();

    // Sound Effects
    for (auto &pair : m_soundEffectIDs)
    {
        if (pair.second)
            MIX_DestroyAudio(pair.second);
    }
    m_soundEffectIDs.clear();
    m_soundEffects.clear();

    // JSON
    for (auto &pair : m_jsonIDs)
    {
        if (pair.second)
            cJSON_Delete(pair.second);
    }
    m_jsonIDs.clear();
    m_jsonData.clear();
}

#pragma endregion

#pragma region Private Methods

void AssetsHandler::LoadTexture(const string &filePath)
{
    if (m_textures.find(filePath) != m_textures.end())
        return;

    SDL_Texture *texture = IMG_LoadTexture(Game::GetInstance().GetRenderer(), (m_basePath + filePath).c_str());
    if (!texture)
    {
        TestHandler::GetInstance().Logger("assetsHandler", "Failed to load texture: %s", filePath.c_str());
        return;
    }

    m_textures[filePath] = {texture, {}};
}

void AssetsHandler::LoadFont(const string &filePath, int fontSize)
{
    string key = filePath + ":" + to_string(fontSize);
    if (m_fonts.find(key) != m_fonts.end())
        return;

    TTF_Font *font = TTF_OpenFont((m_basePath + filePath).c_str(), fontSize);
    if (!font)
    {
        TestHandler::GetInstance().Logger("assetsHandler", "Failed to load font: %s", filePath.c_str());
        return;
    }

    m_fonts[key] = {font, {}};
}

void AssetsHandler::LoadMusic(const string &filePath)
{
    if (m_music.find(filePath) != m_music.end())
        return;

    MIX_Audio *music = MIX_LoadAudio(Game::GetInstance().GetMixer(), (m_basePath + filePath).c_str(), false);
    if (!music)
    {
        TestHandler::GetInstance().Logger("assetsHandler", "Failed to load music: %s", (m_basePath + filePath).c_str());
        return;
    }

    m_music[filePath] = {music, {}};
}

void AssetsHandler::LoadSoundEffect(const string &filePath)
{
    if (m_soundEffects.find(filePath) != m_soundEffects.end())
        return;

    MIX_Audio *chunk = MIX_LoadAudio(Game::GetInstance().GetMixer(), (m_basePath + filePath).c_str(), true);
    if (!chunk)
    {
        TestHandler::GetInstance().Logger("assetsHandler", "Failed to load sound effect: %s", (m_basePath + filePath).c_str());
        return;
    }

    m_soundEffects[filePath] = {chunk, {}};
}

void AssetsHandler::LoadJson(const std::string &filePath)
{
    if (m_jsonData.find(filePath) != m_jsonData.end())
        return;

    std::string fullPath = m_basePath + filePath;
    SDL_IOStream *stream = SDL_IOFromFile(fullPath.c_str(), "rb");
    if (!stream)
    {
        TestHandler::GetInstance().Logger("assetsHandler", "Failed to open JSON: %s", filePath.c_str());
        return;
    }

    size_t size = SDL_GetIOSize(stream);
    if (size == 0)
    {
        TestHandler::GetInstance().Logger("assetsHandler", "Empty or unreadable file: %s", filePath.c_str());
        SDL_CloseIO(stream);
        return;
    }

    char *data = new char[size + 1];
    size_t bytesRead = SDL_ReadIO(stream, data, size);
    data[size] = '\0';
    SDL_CloseIO(stream);

    if (bytesRead != size)
    {
        delete[] data;
        TestHandler::GetInstance().Logger("assetsHandler", "Failed to read complete JSON file: %s", filePath.c_str());
        return;
    }

    cJSON *json = cJSON_Parse(data);
    delete[] data;

    if (!json)
    {
        TestHandler::GetInstance().Logger("assetsHandler", "Failed to parse JSON: %s", filePath.c_str());
        return;
    }

    m_jsonData[filePath] = {json, {}};
}

void AssetsHandler::UnLoadTexture(const string &filePath)
{
    auto it = m_textures.find(filePath);
    if (!it->second.ids.empty())
        return; // still used
    SDL_DestroyTexture(it->second.resource);

    // Remove all associated IDs from m_textureIDs
    for (int id : it->second.ids)
    {
        m_textureIDs.erase(id);
    }

    m_textures.erase(it);
}

void AssetsHandler::UnLoadFont(const string &filePath, int fontSize)
{
    string key = filePath + ":" + to_string(fontSize);
    auto it = m_fonts.find(key);
    if (!it->second.ids.empty())
        return;
    TTF_CloseFont(it->second.resource);

    // Remove all associated IDs from m_fontIDs
    for (int id : it->second.ids)
    {
        m_fontIDs.erase(id);
    }

    m_fonts.erase(it);
}

void AssetsHandler::UnLoadMusic(const string &filePath)
{
    auto it = m_music.find(filePath);
    if (!it->second.ids.empty())
        return;
    MIX_DestroyAudio(it->second.resource);

    // Remove all associated IDs from m_musicIDs
    for (int id : it->second.ids)
    {
        m_musicIDs.erase(id);
    }

    m_music.erase(it);
}

void AssetsHandler::UnLoadSoundEffect(const string &filePath)
{
    auto it = m_soundEffects.find(filePath);
    if (!it->second.ids.empty())
        return;
    MIX_DestroyAudio(it->second.resource);

    // Remove all associated IDs from m_soundEffectIDs
    for (int id : it->second.ids)
    {
        m_soundEffectIDs.erase(id);
    }

    m_soundEffects.erase(it);
}

void AssetsHandler::UnLoadJson(const string &filePath)
{
    auto it = m_jsonData.find(filePath);
    if (!it->second.ids.empty())
        return;
    cJSON_Delete(it->second.resource);

    // Remove all associated IDs from m_jsonIDs
    for (int id : it->second.ids)
    {
        m_jsonIDs.erase(id);
    }

    m_jsonData.erase(it);
}

#pragma endregion

#pragma region Public Methods

int AssetsHandler::UsedTexture(const string &filePath)
{
    LoadTexture(filePath);
    int id = ++m_nextTextureID;
    m_textureIDs[id] = m_textures[filePath].resource;
    m_textures[filePath].ids.insert(id);
    return id;
}

int AssetsHandler::UsedFont(const string &filePath, int fontSize)
{
    string key = filePath + ":" + to_string(fontSize);
    LoadFont(filePath, fontSize);
    int id = ++m_nextFontID;
    m_fontIDs[id] = m_fonts[key].resource;
    m_fonts[key].ids.insert(id);
    return id;
}

int AssetsHandler::UsedMusic(const string &filePath)
{
    LoadMusic(filePath);
    int id = ++m_nextMusicID;
    m_musicIDs[id] = m_music[filePath].resource;
    m_music[filePath].ids.insert(id);
    return id;
}

int AssetsHandler::UsedSoundEffect(const string &filePath)
{
    LoadSoundEffect(filePath);
    int id = ++m_nextSoundEffectID;
    m_soundEffectIDs[id] = m_soundEffects[filePath].resource;
    m_soundEffects[filePath].ids.insert(id);
    return id;
}

int AssetsHandler::UsedJson(const string &filePath)
{
    LoadJson(filePath);
    int id = ++m_nextJsonID;
    m_jsonIDs[id] = m_jsonData[filePath].resource;
    m_jsonData[filePath].ids.insert(id);
    return id;
}

void AssetsHandler::UnUsedTexture(int id)
{
    auto it = m_textureIDs.find(id);
    if (it == m_textureIDs.end())
        return;

    // find which filePath
    for (auto &pair : m_textures)
    {
        if (pair.second.ids.erase(id))
        {
            if (pair.second.ids.empty())
                UnLoadTexture(pair.first);
            break;
        }
    }
    m_textureIDs.erase(it);
}

void AssetsHandler::UnUsedFont(int id)
{
    auto it = m_fontIDs.find(id);
    if (it == m_fontIDs.end())
        return;

    for (auto &pair : m_fonts)
    {
        if (pair.second.ids.erase(id))
        {
            if (pair.second.ids.empty())
                UnLoadFont(pair.first.substr(0, pair.first.find(":")),
                           stoi(pair.first.substr(pair.first.find(":") + 1)));
            break;
        }
    }
    m_fontIDs.erase(it);
}

void AssetsHandler::UnUsedMusic(int id)
{
    auto it = m_musicIDs.find(id);
    if (it == m_musicIDs.end())
        return;

    for (auto &pair : m_music)
    {
        if (pair.second.ids.erase(id))
        {
            if (pair.second.ids.empty())
                UnLoadMusic(pair.first);
            break;
        }
    }
    m_musicIDs.erase(it);
}

void AssetsHandler::UnUsedSoundEffect(int id)
{
    auto it = m_soundEffectIDs.find(id);
    if (it == m_soundEffectIDs.end())
        return;

    for (auto &pair : m_soundEffects)
    {
        if (pair.second.ids.erase(id))
        {
            if (pair.second.ids.empty())
                UnLoadSoundEffect(pair.first);
            break;
        }
    }
    m_soundEffectIDs.erase(it);
}

void AssetsHandler::UnUsedJson(int id)
{
    auto it = m_jsonIDs.find(id);
    if (it == m_jsonIDs.end())
        return;

    for (auto &pair : m_jsonData)
    {
        if (pair.second.ids.erase(id))
        {
            if (pair.second.ids.empty())
                UnLoadJson(pair.first);
            break;
        }
    }
    m_jsonIDs.erase(it);
}

#pragma endregion

#pragma region Getters / Setters

SDL_Texture *AssetsHandler::GetTexture(int id)
{
    auto it = m_textureIDs.find(id);
    return it != m_textureIDs.end() ? it->second : nullptr;
}

TTF_Font *AssetsHandler::GetFont(int id)
{
    auto it = m_fontIDs.find(id);
    return it != m_fontIDs.end() ? it->second : nullptr;
}

MIX_Audio *AssetsHandler::GetMusic(int id)
{
    auto it = m_musicIDs.find(id);
    return it != m_musicIDs.end() ? it->second : nullptr;
}

MIX_Audio *AssetsHandler::GetSoundEffect(int id)
{
    auto it = m_soundEffectIDs.find(id);
    return it != m_soundEffectIDs.end() ? it->second : nullptr;
}

cJSON *AssetsHandler::GetJson(int id)
{
    auto it = m_jsonIDs.find(id);
    return it != m_jsonIDs.end() ? it->second : nullptr;
}

#pragma endregion
