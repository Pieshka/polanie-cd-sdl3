#include <stdio.h>

#include "cd.h"
#include "constants.h"
#include <SDL3_mixer/SDL_mixer.h>

int track = 0;

int audioSubsystemCounter = 0;
MIX_Mixer *globalMixer;
static MIX_Track *musicTrack;

/* Here is perfect place for this */
extern void check_if_everything_is_ok();

int init_audio_subsystem()
{
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "POLANIE-PORT: Initializing SDL3 audio subsystem...");
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL audio subsystem could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    if (!MIX_Init())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    globalMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!globalMixer)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Global mixer cannot be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    audioSubsystemCounter++;
    return 0;
}

void deinit_audio_subsystem()
{
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "POLANIE-PORT: Quitting SDL3 audio subsystem...");

    MIX_DestroyMixer(globalMixer);
    globalMixer = NULL;

    MIX_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);

    audioSubsystemCounter--;
}

int InitCD()
{
    if (audioSubsystemCounter == 0 && init_audio_subsystem())
        return 1;

    musicTrack = MIX_CreateTrack(globalMixer);
    if (!musicTrack)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Mixer music track cannot be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}

void setVolume(int volume) //ustawia od 0 do 5        //!
{
    MIX_SetTrackGain(musicTrack, (float)volume/5);
}

int getVolume() //zwraca od zero do 5          //!
{
    float gain = MIX_GetTrackGain(musicTrack);
    return (int)(5 * gain);
}

int PlayNext()
{
    PlayTrack(track + 1);
    return 0;
}

int PlayPrevious()
{
    PlayTrack(track - 1);
    return 0;
}

int PlayTrack(int newTrack)
{
    if (newTrack == track)
    {
        MIX_ResumeTrack(musicTrack);
        return 0;
    }

    char filename[512];
#ifdef SDL_PLATFORM_WINDOWS
    sprintf(filename, "music/track%d.flac", newTrack);
#else
    sprintf(filename, "%s/music/track%d.flac", NON_WIN_ASSET_PATH, newTrack);
#endif

    MIX_Audio *musicAudio = MIX_LoadAudio(globalMixer, filename, false);
    if (!musicAudio)
        return 1;

    MIX_SetTrackAudio(musicTrack, musicAudio);
    MIX_DestroyAudio(musicAudio);

    MIX_SetTrackLoops(musicTrack, -1);
    MIX_PlayTrack(musicTrack, 0);

    track = newTrack;
    return 0;
}

void DeInitCD()
{
    MIX_DestroyTrack(musicTrack);
    musicTrack = NULL;

    if (audioSubsystemCounter == 1)
        deinit_audio_subsystem();
}

int StopPlaying()
{
    MIX_PauseTrack(musicTrack);
    return 0;
}

int GetCurrentTrack()
{
    return track;
}

void CheckCD() //sprawdza czy jest grany odpowiedni track i ewentualnie nawraca
{
    /* This function checks if the track variable is synchronized with what is currently played */
    /* Because we have full control of what we are playing, it doesn't concern us */
}

void SetMaxTrack(int maxTrack)
{
    /* This function is used to limit shareware capabilities */
}

void ReadNrOfTracks()
{
    /* This function reads all tracks available on our CD. Unimportant for us. */
}

void BigOffCDAudio()
{
    /* This function disables CD Audio subsystem. Unimportant for us. */
}

void BigOnCDAudio()
{
    /* This function enables CD Audio subsystem. */
    /* As it is the first function ran by the game before everything else, it is the perfect place for sanity checks. */
#ifdef DEBUG_MODE
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
#endif
    check_if_everything_is_ok();
}

void OffCDAudio()
{
    /* This function disables CD Audio subsystem. Unimportant for us. */
}

void OnCDAudio()
{
    /* This function enables CD Audio subsystem. Unimportant for us. */
}