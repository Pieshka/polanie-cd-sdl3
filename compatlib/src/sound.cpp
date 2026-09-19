#include "sound.h"
#include "compat.h"
#include "polanieapp.h"

Sound::Sound()
{
    m_effectBuffer = nullptr;
    m_effects = nullptr;
    m_effectCount = 183;
    m_track = 0;
    m_mixer = nullptr;
    m_musicTrack = nullptr;
    m_effectTrack = nullptr;
}

Sound::~Sound()
{
    MIX_DestroyTrack(m_musicTrack);
    m_musicTrack = nullptr;

    MIX_DestroyTrack(m_effectTrack);
    m_effectTrack = nullptr;

    MIX_DestroyMixer(m_mixer);
    m_mixer = nullptr;

    delete [] m_effects;
    delete [] m_effectBuffer;

    MIX_Quit();
}

int Sound::Init()
{
    if (!MIX_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "\"Polanie CD\" failed to start.\nCannot initialize mixer.\nSDL error: %s", SDL_GetError());
        Any_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Polanie CD Error",
            "\"Polanie CD\" failed to start.\nPlease quit all other applications and try again."
            "\nFailed to initialize; see logs for details",
            nullptr
        );
        return 1;
    }

    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!m_mixer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "\"Polanie CD\" failed to start.\nCannot create mixer device.\nSDL error: %s", SDL_GetError());
        Any_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Polanie CD Error",
            "\"Polanie CD\" failed to start.\nPlease quit all other applications and try again."
            "\nFailed to initialize; see logs for details",
            nullptr
        );
        return 1;
    }

    m_musicTrack = MIX_CreateTrack(m_mixer);
    if (!m_musicTrack)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "\"Polanie CD\" failed to start.\nCannot create the music track.\nSDL error: %s", SDL_GetError());
        Any_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Polanie CD Error",
            "\"Polanie CD\" failed to start.\nPlease quit all other applications and try again."
            "\nFailed to initialize; see logs for details",
            nullptr
        );
        return 1;
    }

    m_effectTrack = MIX_CreateTrack(m_mixer);
    if (!m_effectTrack)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "\"Polanie CD\" failed to start.\nCannot create the effects track.\nSDL error: %s", SDL_GetError());
        Any_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Polanie CD Error",
            "\"Polanie CD\" failed to start.\nPlease quit all other applications and try again."
            "\nFailed to initialize; see logs for details",
            nullptr
        );
        return 1;
    }

    LoadEffects();

    return 0;
}

int Sound::operator()(int p_effectNumber)
{
    if (g_polanie->IsExiting()) return 1;
    if (!m_effectBuffer) return 1;

    SDL_AudioSpec audioSpec;
    audioSpec.freq = 22050;
    audioSpec.channels = 1;
    audioSpec.format = SDL_AUDIO_U8;

    MIX_SetTrackAudio(m_effectTrack, m_effects[p_effectNumber].audio);
    MIX_PlayTrack(m_effectTrack, 0);

    return 0;
}

int Sound::IsPlaying()
{
    return MIX_TrackPlaying(m_effectTrack);
}

int Sound::PlayWAV(const char *filepath)
{
    if (g_polanie->IsExiting()) return 1;
    Uint32 dataLength;
    Uint8 *buffer;

    SDL_IOStream *f = SDL_IOFromFile(g_polanie->GetFilePath(filepath), "rb");
    if (!f)
        return 1;

    SDL_ReadIO(f, &dataLength, sizeof(Uint32));
    buffer = new Uint8[dataLength];
    SDL_ReadIO(f, buffer, sizeof(Uint8) * dataLength);
    SDL_CloseIO(f);

    SDL_AudioSpec audioSpec;
    audioSpec.freq = 22050;
    audioSpec.channels = 1;
    audioSpec.format = SDL_AUDIO_U8;

    MIX_Audio *audio = MIX_LoadRawAudio(m_mixer, buffer, dataLength, &audioSpec);
    if (!audio)
        return 1;

    MIX_SetTrackAudio(m_musicTrack, audio);
    MIX_DestroyAudio(audio);
    delete [] buffer;

    MIX_PlayTrack(m_musicTrack, 0);

    return 0;
}

void Sound::StopWAV()
{
}

int Sound::GetMusicVolume()
{
    float gain = MIX_GetTrackGain(m_musicTrack);
    return SDL_static_cast(int, gain * 5);
}

void Sound::SetMusicVolume(int p_musicVolume)
{
    MIX_SetTrackGain(m_musicTrack, SDL_static_cast(float, p_musicVolume)/5);
}

int Sound::PlayTrack(int p_trackNumber)
{
    if (g_polanie->IsExiting()) return 1;

    if (p_trackNumber == m_track)
    {
        MIX_ResumeTrack(m_musicTrack);
        return 0;
    }

    char buffer[512];
    SDL_snprintf(buffer, sizeof(buffer), "music/track%d.flac", p_trackNumber);
    if (!SDL_GetPathInfo(g_polanie->GetFilePath(buffer), nullptr))
        SDL_snprintf(buffer, sizeof(buffer), "music/track%02d.flac", p_trackNumber);
    if (!SDL_GetPathInfo(g_polanie->GetFilePath(buffer), nullptr))
        return 1;

    MIX_Audio *musicAudio = MIX_LoadAudio(m_mixer, g_polanie->GetFilePath(buffer), false);
    if (!musicAudio)
        return 1;

    MIX_SetTrackAudio(m_musicTrack, musicAudio);
    MIX_DestroyAudio(musicAudio);

    MIX_SetTrackLoops(m_musicTrack, -1);
    MIX_PlayTrack(m_musicTrack,0);

    m_track = p_trackNumber;
    return 0;
}

int Sound::PlayNextTrack()
{
    PlayTrack(m_track + 1);
    return 0;
}

int Sound::PlayPreviousTrack()
{
    PlayTrack(m_track - 1);
    return 0;
}

int Sound::StopTrack()
{
    MIX_PauseTrack(m_musicTrack);
    return 0;
}

int Sound::GetCurrentTrack()
{
    return m_track;
}

void Sound::LoadEffects()
{
    SDL_IOStream *f = SDL_IOFromFile(g_polanie->GetFilePath("data/sound.dat"), "rb");
    if (!f)
        return;

    Uint32 dataLength = SDL_GetIOSize(f) - m_effectCount * sizeof(Uint32);

    m_effectBuffer = new Uint8[dataLength];
    m_effects = new Effect[m_effectCount];
    Uint8 *buffer_cursor = m_effectBuffer;
    SDL_ReadIO(f, m_effectBuffer, dataLength);

    SDL_AudioSpec audioSpec;
    audioSpec.freq = 22050;
    audioSpec.channels = 1;
    audioSpec.format = SDL_AUDIO_U8;

    for (int i = 0; i < m_effectCount; i++)
    {
        Uint32 size;
        SDL_ReadIO(f, &size, sizeof(Uint32));
        m_effects[i].audio = MIX_LoadRawAudioNoCopy(m_mixer, buffer_cursor, size, &audioSpec, false);
        buffer_cursor += size;
    }


    SDL_CloseIO(f);
}
