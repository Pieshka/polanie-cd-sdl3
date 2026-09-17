#include <SDL3/SDL.h>
#include "constants.h"
#include "conio.h"

#define VGA_DAC_MASK_REGISTER 0x3C6
#define VGA_DAC_CONTROL_1 0x3C8
#define VGA_DAC_CONTROL_2 0x3C9

extern void PORT_SDLPumpEvents(int);
extern unsigned video_virtual_outp_handler(unsigned port, unsigned value);

int cprintf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    SDL_Log(format, ap);
    va_end(ap);
    return 1;
}

int getch(void)
{
    PORT_SDLPumpEvents(0);

    while (state.key_queue_write == state.key_queue_read)
    {
        SDL_Delay(1);
        PORT_SDLPumpEvents(0);
    }

    char c = state.key_queue[state.key_queue_read];
    state.key_queue_read = (state.key_queue_read + 1) % 32;

    return (unsigned char)c;
}

/* This is also sort of a hacky way to flush the event pipe */
int kbhit(void)
{
    PORT_SDLPumpEvents(0);

    return state.key_queue_read != state.key_queue_write;
}

unsigned ___outp(unsigned __port, unsigned __value)
{
    switch (__port)
    {
        case VGA_DAC_MASK_REGISTER:
        case VGA_DAC_CONTROL_1:
        case VGA_DAC_CONTROL_2:
            return video_virtual_outp_handler(__port, __value);
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,"[POLANIE-PORT: conio_entry] outp unhandled port: %02x, value=%02x\n", \
        __port, __value);
    return 0;
}