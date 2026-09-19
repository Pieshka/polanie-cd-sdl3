#ifndef POLANIE_SOUND_H
#define POLANIE_SOUND_H

#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL_stdinc.h>

class Sound
{
public:
    Sound();
    ~Sound();

    int Init();

    /* Play effect */
    int operator()(int p_effectNumber);
    int IsPlaying();

    int PlayWAV(const char *filepath);
    void StopWAV();

    int GetMusicVolume();

    void SetMusicVolume(int p_musicVolume);

    int PlayTrack(int p_trackNumber);
    int PlayNextTrack();
    int PlayPreviousTrack();
    int StopTrack();
    int GetCurrentTrack();

private:
    void LoadEffects();

    struct Effect
    {
        MIX_Audio *audio;
    } *m_effects;
    Uint8 *m_effectBuffer;
    int m_effectCount;
    int m_track;

    MIX_Mixer *m_mixer;
    MIX_Track *m_musicTrack;
    MIX_Track *m_effectTrack;

};

#endif //POLANIE_SOUND_H
