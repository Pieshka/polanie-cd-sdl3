#ifndef POLANIE_MENEGDMA_H
#define POLANIE_MENEGDMA_H
#include <cstdint>

class MENEGERDMA
{
public:
    MENEGERDMA(int sound_count, int paragraphs);
    ~MENEGERDMA();
    static int jest_odtwarzany; // is_playing

    int operator()(int); // Play sample

    void Init(int is_SoundBlaster, int irq, int port, int channel);

    int InstalujWAV(const char *filepath); // load_sample
    int LoadGlobalData(const char *filepath, int sample_count);

    int PlayWav(const char *filepath);
    void EndPlayWav();

private:
    struct PCMTable
    {
        void *samples;
        uint32_t size;
    } *pcm_table;

    unsigned char *sounds;
    uint32_t sound_count;
};

#endif //POLANIE_MENEGDMA_H
