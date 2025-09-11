#pragma region Includes / Defines

#include "audioHandler.h"

using namespace std;
using namespace sge;

#pragma endregion

#pragma region Constructor / Destructor

AudioHandler::~AudioHandler()
{
    CleanUp();
}

#pragma endregion

#pragma region Init / Cleanup

void AudioHandler::Init()
{
    int infoDataId = AssetsHandler::GetInstance().UsedJson("assets/data/info.json");
    cJSON *infoJson = AssetsHandler::GetInstance().GetJson(infoDataId);

    m_musicTrackPoolSize = cJSON_GetObjectItem(infoJson, "music-track-pool-size")->valueint;
    m_soundEffectTrackPoolSize = cJSON_GetObjectItem(infoJson, "sound-effect-track-pool-size")->valueint;

    float defaultAudioVolume = float(cJSON_GetObjectItem(infoJson, "default-audio-volume")->valuedouble);
    float defaultMusicVolume = float(cJSON_GetObjectItem(infoJson, "default-music-volume")->valuedouble);
    float defaultSoundVolume = float(cJSON_GetObjectItem(infoJson, "default-sound-volume")->valuedouble);

    AssetsHandler::GetInstance().UnUsedJson(infoDataId);

    m_audioVolume = SaveDataHandler::GetInstance().LoadFloatData("audio-volume", defaultAudioVolume);
    m_musicVolume = SaveDataHandler::GetInstance().LoadFloatData("music-volume", defaultMusicVolume);
    m_soundVolume = SaveDataHandler::GetInstance().LoadFloatData("sound-volume", defaultSoundVolume);

    m_musicTracks.reserve(m_musicTrackPoolSize);
    m_soundEffectTracks.reserve(m_soundEffectTrackPoolSize);

    for (int i = 0; i < m_musicTrackPoolSize; i++)
        m_musicTracks.push_back(MIX_CreateTrack(Game::GetInstance().GetMixer()));

    for (int i = 0; i < m_soundEffectTrackPoolSize; i++)
        m_soundEffectTracks.push_back(MIX_CreateTrack(Game::GetInstance().GetMixer()));

    ChangedAudioVolume();
    ChangedMusicVolume();
    ChangedSoundVolume();
}

void AudioHandler::CleanUp()
{
    for (auto &track : m_musicTracks)
    {
        if (track)
            MIX_DestroyTrack(track);
    }
    m_musicTracks.clear();

    for (auto &track : m_soundEffectTracks)
    {
        if (track)
            MIX_DestroyTrack(track);
    }
    m_soundEffectTracks.clear();
}

#pragma endregion

#pragma region Engine Methods
#pragma endregion

#pragma region Private Methods

int AudioHandler::GetUnusedMusicTrack()
{
    for (size_t i = 0; i < m_musicTrackPoolSize; ++i)
    {
        MIX_Track *track = m_musicTracks[i];
        if (!MIX_TrackPlaying(track))
            return static_cast<int>(i);
    }

    if (m_musicTracks.empty())
    {
        int trackId = static_cast<int>(rand() % m_musicTrackPoolSize);
        StopMusic(trackId, 0);
        return trackId;
    }

    Game::GetInstance().ShowMessage("Error playing music", "GetUnusedMusicTrack, no available track");
    return -1;
}

int AudioHandler::GetUnusedSoundEffectTrack()
{
    for (size_t i = 0; i < m_soundEffectTrackPoolSize; ++i)
    {
        MIX_Track *track = m_soundEffectTracks[i];
        if (!MIX_TrackPlaying(track))
            return static_cast<int>(i);
    }

    if (m_soundEffectTracks.empty())
    {
        int trackId = static_cast<int>(rand() % m_soundEffectTrackPoolSize);
        StopSoundEffect(trackId, 0);
        return trackId;
    }

    Game::GetInstance().ShowMessage("Error playing sound effect", "GetUnusedSoundEffectTrack, no available track");
    return -1;
}

void AudioHandler::ChangedAudioVolume()
{
    SaveDataHandler::GetInstance().SaveData("audio-volume", m_audioVolume);

    for (auto &track : m_musicTracks)
        if (track) MIX_SetTrackGain(track, m_audioVolume * m_musicVolume);

    for (auto &track : m_soundEffectTracks)
        if (track) MIX_SetTrackGain(track, m_audioVolume * m_soundVolume);
}

void AudioHandler::ChangedMusicVolume()
{
    SaveDataHandler::GetInstance().SaveData("music-volume", m_musicVolume);

    for (auto &track : m_musicTracks)
        if (track) MIX_SetTrackGain(track, m_audioVolume * m_musicVolume);
}

void AudioHandler::ChangedSoundVolume()
{
    SaveDataHandler::GetInstance().SaveData("sound-volume", m_soundVolume);

    for (auto &track : m_soundEffectTracks)
        if (track) MIX_SetTrackGain(track, m_audioVolume * m_soundVolume);
}

#pragma endregion

#pragma region Public Methods

int AudioHandler::PlayMusic(const std::string &filePath, int loops)
{
    int trackId = GetUnusedMusicTrack();
    if (trackId == -1)
    {
        Game::GetInstance().ShowMessage("Error playing music", "PlayMusic, no available track");
        return -1;
    }

    MIX_Track *track = m_musicTracks[trackId];
    if (!track)
    {
        Game::GetInstance().ShowMessage("Error playing music", "PlayMusic, Failed to get track: " + to_string(trackId));
        return -1;
    }

    int musicId = AssetsHandler::GetInstance().UsedMusic(filePath);
    MIX_Audio *music = AssetsHandler::GetInstance().GetMusic(musicId);
    if (!music)
    {
        AssetsHandler::GetInstance().UnUsedMusic(musicId);
        Game::GetInstance().ShowMessage("Error playing music", "PlayMusic, Failed to load music: " + filePath);
        return -1;
    }

    MIX_SetTrackAudio(track, music);

    int channel = MIX_PlayTrack(track, loops);
    if (channel < 0)
    {
        Game::GetInstance().ShowMessage("Error playing music", "PlayMusic, MIX_PlayTrack failed");
        AssetsHandler::GetInstance().UnUsedMusic(musicId);
        return -1;
    }

    m_trackMusicMap[track] = musicId;

    MIX_SetTrackStoppedCallback(track, [](void *userdata, MIX_Track *track)
                                {
        auto* map = reinterpret_cast<std::unordered_map<MIX_Track*, int>*>(userdata);
        int musicId = (*map)[track];
        AssetsHandler::GetInstance().UnUsedMusic(musicId);
        map->erase(track); }, &m_trackMusicMap);

    MIX_SetTrackGain(track, m_audioVolume * m_musicVolume);

    return trackId;
}

void AudioHandler::StopMusic(int trackId, int fadeOutMs)
{
    if (trackId < 0 || trackId >= static_cast<int>(m_musicTracks.size()))
    {
        Game::GetInstance().ShowMessage("Error stopping music", "StopMusic, invalid trackId: " + to_string(trackId));
        return;
    }
        
    MIX_Track *track = m_musicTracks[trackId];
    if (!track)
    {
        Game::GetInstance().ShowMessage("Error stopping music", "StopMusic, Failed to get track: " + to_string(trackId));
        return;
    }

    MIX_StopTrack(track, fadeOutMs);
}

int AudioHandler::PlaySoundEffect(const string &filePath, int loops)
{
    int trackId = GetUnusedSoundEffectTrack();
    if (trackId == -1)
    {
        Game::GetInstance().ShowMessage("Error playing sound effect", "PlaySoundEffect, no available track");
        return -1;
    }

    MIX_Track *track = m_soundEffectTracks[trackId];
    if (!track)
    {
        Game::GetInstance().ShowMessage("Error playing sound effect", "PlaySoundEffect, Failed to get track: " + to_string(trackId));
        return -1;
    }

    int soundEffectId = AssetsHandler::GetInstance().UsedSoundEffect(filePath);
    MIX_Audio *soundEffect = AssetsHandler::GetInstance().GetSoundEffect(soundEffectId);
    if (!soundEffect)
    {
        Game::GetInstance().ShowMessage("Error playing sound effect", "PlaySoundEffect, Failed to load sound effect: " + filePath);
        AssetsHandler::GetInstance().UnUsedSoundEffect(soundEffectId);
        return -1;
    }

    MIX_SetTrackAudio(track, soundEffect);

    int channel = MIX_PlayTrack(track, loops);
    if (channel < 0)
    {
        Game::GetInstance().ShowMessage("Error playing sound effect", "PlaySoundEffect, MIX_PlayTrack failed");
        AssetsHandler::GetInstance().UnUsedSoundEffect(soundEffectId);
        return -1;
    }

    m_trackSoundEffectMap[track] = soundEffectId;

    MIX_SetTrackStoppedCallback(track, [](void *userdata, MIX_Track *track)
                                {
        auto* map = reinterpret_cast<std::unordered_map<MIX_Track*, int>*>(userdata);
        int soundEffectId = (*map)[track];
        AssetsHandler::GetInstance().UnUsedSoundEffect(soundEffectId);
        map->erase(track); }, &m_trackSoundEffectMap);

    MIX_SetTrackGain(track, m_audioVolume * m_soundVolume);

    return trackId;
}

void AudioHandler::StopSoundEffect(int trackId, int fadeOutMs)
{
    if (trackId < 0 || trackId >= static_cast<int>(m_soundEffectTracks.size()))
    {
        Game::GetInstance().ShowMessage("Error stopping sound effect", "StopSoundEffect, invalid trackId: " + to_string(trackId));
        return;
    }

    MIX_Track *track = m_soundEffectTracks[trackId];
    if (!track)
    {
        Game::GetInstance().ShowMessage("Error stopping sound effect", "StopSoundEffect, Failed to get track: " + to_string(trackId));
        return;
    }

    MIX_StopTrack(track, fadeOutMs);
}

#pragma endregion

#pragma region Getters / Setters

float AudioHandler::GetAudioVolume()
{
    return m_audioVolume;
}
void AudioHandler::SetAudioVolume(float volume)
{
    if (m_audioVolume == volume)
        return;

    volume = std::clamp(volume, 0.0f, 2.0f);
    m_audioVolume = volume;

    ChangedAudioVolume();
}

float AudioHandler::GetMusicVolume()
{
    return m_musicVolume;
}
void AudioHandler::SetMusicVolume(float volume)
{
    if (m_musicVolume == volume)
        return;

    volume = std::clamp(volume, 0.0f, 2.0f);
    m_musicVolume = volume;

    ChangedMusicVolume();
}

float AudioHandler::GetSoundVolume()
{
    return m_soundVolume;
}
void AudioHandler::SetSoundVolume(float volume)
{
    if (m_soundVolume == volume)
        return;

    volume = std::clamp(volume, 0.0f, 2.0f);
    m_soundVolume = volume;

    ChangedSoundVolume();
}

#pragma endregion
