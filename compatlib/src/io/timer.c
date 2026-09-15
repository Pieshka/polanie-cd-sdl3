#include <SDL3/SDL.h>
#include <stdatomic.h>

int licznik = 0;

void PIT_update()
{
    static uint64_t last_counter = 0;

    const uint64_t freq = SDL_GetPerformanceFrequency();
    const uint64_t now = SDL_GetPerformanceCounter();

    if (last_counter == 0)
    {
        last_counter = now;
        return;
    }

    const uint64_t delta = now - last_counter;

    /* 54.9254ms */
    const uint64_t tick_period = (freq * 549254ULL) / 10000000ULL;

    if (delta >= tick_period)
    {
        const uint64_t ticks = delta / tick_period;

        licznik += (int)ticks;

        last_counter += ticks * tick_period;
    }
}