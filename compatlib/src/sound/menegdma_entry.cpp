#include <cstdio>
#include <SDL3/SDL_log.h>
#include <SDL3_mixer/SDL_mixer.h>
#include "menegdma.h"

extern "C"
{
#include "constants.h"
extern int audioSubsystemCounter;
extern MIX_Mixer *globalMixer;
extern int init_audio_subsystem();
extern void deinit_audio_subsystem();
}

static MIX_Track *soundTrack;
static MIX_Track *effectTrack;

struct SoundEffect
{
    MIX_Audio *audio;
};

SoundEffect *effects;

MENEGERDMA::MENEGERDMA(int sound_count, int paragraphs)
{}

MENEGERDMA::~MENEGERDMA()
{
    MIX_DestroyTrack(soundTrack);
    soundTrack = NULL;

    MIX_DestroyTrack(effectTrack);
    effectTrack = NULL;

    delete [] effects;
    delete [] sounds;
    delete [] pcm_table;

    if (audioSubsystemCounter == 1)
        deinit_audio_subsystem();
}

int MENEGERDMA::jest_odtwarzany = 0;

int MENEGERDMA::operator()(int i)
{
    SDL_AudioSpec audioSpec;
    audioSpec.freq = 22050;
    audioSpec.channels = 1;
    audioSpec.format = SDL_AUDIO_U8;

    MIX_SetTrackAudio(effectTrack, effects[i].audio);
    MIX_PlayTrack(effectTrack, 0);

    jest_odtwarzany = 1;
    return 0;
}

void MENEGERDMA::Init(int is_SoundBlaster, int irq, int port, int channel)
{
    if (audioSubsystemCounter == 0 && init_audio_subsystem())
        return;

    soundTrack = MIX_CreateTrack(globalMixer);
    if (!soundTrack)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Mixer sound track cannot be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    effectTrack = MIX_CreateTrack(globalMixer);
    if (!effectTrack)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Mixer effect track cannot be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }

    MIX_SetTrackStoppedCallback(soundTrack, [](void *, MIX_Track*) { jest_odtwarzany = 0; }, nullptr);
    MIX_SetTrackStoppedCallback(effectTrack, [](void *, MIX_Track*) { jest_odtwarzany = 0; }, nullptr);
}

/* "WAV" files played by this functions consists of raw PCM samples and 4-byte length header. */
/* Unsigned 8-bit, Mono 22050Hz */
int MENEGERDMA::PlayWav(const char *filepath)
{
    uint32_t data_length; uint8_t *buffer;
#ifdef SDL_PLATFORM_WINDOWS
    FILE *f = fopen(filepath, "rb");
#else
    char buf[1024];
    sprintf(buf, "%s/%s", NON_WIN_ASSET_PATH, filepath);
    FILE *f = fopen(buf, "rb");
#endif

    if(!f)
    {
        SDL_LogDebug(SDL_LOG_CATEGORY_ERROR, "Unable to load %s file!", filepath);
        return -1;
    }

    fread(&data_length, sizeof(uint32_t), 1, f);
    buffer = new uint8_t[data_length];
    fread(buffer, sizeof(uint8_t), data_length, f);
    fclose(f);

    SDL_AudioSpec audioSpec;
    audioSpec.freq = 22050;
    audioSpec.channels = 1;
    audioSpec.format = SDL_AUDIO_U8;

    MIX_Audio *audio = MIX_LoadRawAudio(globalMixer, buffer, data_length, &audioSpec);
    if (!audio)
        return 1;

    MIX_SetTrackAudio(soundTrack, audio);
    MIX_DestroyAudio(audio);
    delete [] buffer;

    MIX_PlayTrack(soundTrack, 0);

    jest_odtwarzany = 1;
    return 0;
}

/*
 * This function loads the data/sound.dat file. This data contains raw PCM samples
 * Last sample_count dwords contains sizes of the specific sound files
 */
int MENEGERDMA::LoadGlobalData(const char *filepath, int sample_count)
{
    uint32_t data_length; unsigned char *buffer_cursor;

#ifdef SDL_PLATFORM_WINDOWS
    FILE *globalData = fopen(filepath, "rb");
#else
    char buf[1024];
    sprintf(buf, "%s/%s", NON_WIN_ASSET_PATH, filepath);
    FILE *globalData = fopen(buf, "rb");
#endif
    if(!globalData)
    {
        SDL_LogDebug(SDL_LOG_CATEGORY_ERROR,"[POLANIE-PORT: MENEGERDMA] Unable to load %s file!", filepath);
        return -1;
    }

    /* Get all samples size */
    fseek(globalData, 0, SEEK_END);
    data_length = ftell(globalData);
    fseek(globalData, 0, SEEK_SET);
    data_length -= sample_count * sizeof(uint32_t);

    /* Load all samples */
    sounds = new unsigned char[data_length]; buffer_cursor = sounds;
    fread(sounds, sizeof(unsigned char), data_length, globalData);

    /* Load PCM table */
    pcm_table = new PCMTable[sample_count];
    for (int i = 0; i < sample_count; i++)
    {
        fread(&pcm_table[i].size, sizeof(uint32_t), 1, globalData);
        pcm_table[i].samples = buffer_cursor;
        buffer_cursor += pcm_table[i].size;
    }

    /* Load MIX Audio */
    SDL_AudioSpec audioSpec;
    audioSpec.freq = 22050;
    audioSpec.channels = 1;
    audioSpec.format = SDL_AUDIO_U8;

    effects = new SoundEffect[sample_count];

    for (int i = 0; i < sample_count; i++)
    {
        effects[i].audio = MIX_LoadRawAudioNoCopy(globalMixer, pcm_table[i].samples, pcm_table[i].size, &audioSpec, false);
    }

    fclose(globalData);
    return 0;
}

void MENEGERDMA::EndPlayWav()
{
    MIX_StopTrack(soundTrack, 64);
    MIX_StopTrack(effectTrack, 64);

    jest_odtwarzany = 0;
}

/* Unused */
int MENEGERDMA::InstalujWAV(const char *filepath){ return 0; }