#pragma once
#include "patterns/singleton/singleton.h"

#include "handlers/assetEntry.h"
#include "handlers/testHandler.h"

#include "game.h"
#include "cJSON/cJSON.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <unordered_map>

namespace sge
{
    class AssetsHandler : public patterns::Singleton<AssetsHandler>
    {
        friend class patterns::Singleton<AssetsHandler>;

    public:
        AssetsHandler() = default;
        ~AssetsHandler();

        void CleanUp();
        void Init();

        int UsedTexture(const std::string &filePath);
        int UsedFont(const std::string &filePath, int fontSize);
        int UsedMusic(const std::string &filePath);
        int UsedSoundEffect(const std::string &filePath);
        int UsedJson(const std::string &filePath);

        void UnUsedTexture(int id);
        void UnUsedFont(int id);
        void UnUsedMusic(int id);
        void UnUsedSoundEffect(int id);
        void UnUsedJson(int id);

        SDL_Texture *GetTexture(int id);
        TTF_Font *GetFont(int id);
        MIX_Audio *GetMusic(int id);
        MIX_Audio *GetSoundEffect(int id);
        cJSON *GetJson(int id);

    private:
        void LoadTexture(const std::string &filePath);
        void LoadFont(const std::string &filePath, int fontSize);
        void LoadMusic(const std::string &filePath);
        void LoadSoundEffect(const std::string &filePath);
        void LoadJson(const std::string &filePath);

        void UnLoadTexture(const std::string &filePath);
        void UnLoadFont(const std::string &filePath, int fontSize);
        void UnLoadMusic(const std::string &filePath);
        void UnLoadSoundEffect(const std::string &filePath);
        void UnLoadJson(const std::string &filePath);

        std::string m_basePath;

        int m_nextTextureID;
        int m_nextFontID;
        int m_nextMusicID;
        int m_nextSoundEffectID;
        int m_nextJsonID;

        std::unordered_map<int, SDL_Texture *> m_textureIDs;
        std::unordered_map<int, TTF_Font *> m_fontIDs;
        std::unordered_map<int, MIX_Audio *> m_musicIDs;
        std::unordered_map<int, MIX_Audio *> m_soundEffectIDs;
        std::unordered_map<int, cJSON *> m_jsonIDs;

        std::unordered_map<std::string, AssetEntry<SDL_Texture>> m_textures;
        std::unordered_map<std::string, AssetEntry<TTF_Font>> m_fonts;
        std::unordered_map<std::string, AssetEntry<MIX_Audio>> m_music;
        std::unordered_map<std::string, AssetEntry<MIX_Audio>> m_soundEffects;
        std::unordered_map<std::string, AssetEntry<cJSON>> m_jsonData;
    };
}
