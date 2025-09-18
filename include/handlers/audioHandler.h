#pragma once
#include "patterns/singleton/singleton.h"
#include "patterns/event/event.h"

#include "handlers/testHandler.h"

#include "game.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

namespace sge
{
    class AudioHandler: public patterns::Singleton<AudioHandler>
    {
        friend class patterns::Singleton<AudioHandler>;

    public:
        AudioHandler() = default;
        ~AudioHandler();

        void CleanUp();
        void Init(int infoDataId);

        int PlayMusic(const std::string &filePath, int loops = -1);
        void StopMusic(int musicId, int fadeOutMs = 0);

        int PlaySoundEffect(const std::string &filePath, int loops = 0);
        void StopSoundEffect(int soundEffectId, int fadeOutMs = 0);

        float GetAudioVolume();
        void SetAudioVolume(float volume);

        float GetMusicVolume();
        void SetMusicVolume(float volume);

        float GetSoundVolume();
        void SetSoundVolume(float volume);

        patterns::Event<float> OnAudioVolumeChange;
        patterns::Event<float> OnMusicVolumeChange;
        patterns::Event<float> OnSoundVolumeChange;

    private:
        int m_musicTrackPoolSize;
        int m_soundEffectTrackPoolSize;

        float m_audioVolume;
        float m_musicVolume;
        float m_soundVolume;

        void ChangedAudioVolume();
        void ChangedMusicVolume();
        void ChangedSoundVolume();

        std::unordered_map<MIX_Track*, int> m_trackMusicMap;
        std::unordered_map<MIX_Track*, int> m_trackSoundEffectMap;

        std::vector<MIX_Track*> m_musicTracks;
        std::vector<MIX_Track*> m_soundEffectTracks;

        int GetUnusedMusicTrack();
        int GetUnusedSoundEffectTrack();
    };
}
