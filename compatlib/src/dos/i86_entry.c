#include "i86.h"
#include <SDL3/SDL.h>

#define MOUSE_INTERRUPT 0x33
#define VIDEO_INTERRUPT 0x10

extern int mouse_virtual_interrupt_handler(union REGS*, union REGS*);
extern int video_virtual_interrupt_handler(union REGS*, union REGS*);

void delay(unsigned int __miliseconds)
{
    Uint64 start = SDL_GetTicks();

    while ((SDL_GetTicks() - start) < __miliseconds)
    {
        PORT_SDLPumpEvents(0);
        SDL_Delay(1);
    }
}

int int386(int interrupt, union REGS* inRegs, union REGS* outRegs)
{
    switch(interrupt)
    {
        case MOUSE_INTERRUPT:
            return mouse_virtual_interrupt_handler(inRegs, outRegs);
        case VIDEO_INTERRUPT:
            return video_virtual_interrupt_handler(inRegs, outRegs);
    }

    SDL_Log("[POLANIE-PORT: i86_Entry] int386 unhandled interrupt: %02x, ax=%02x, bx=%02x, cx=%02x, dx=%02x\n", \
        interrupt, inRegs->w.ax, inRegs->w.bx, inRegs->w.cx, inRegs->w.dx);
    return 0;
}