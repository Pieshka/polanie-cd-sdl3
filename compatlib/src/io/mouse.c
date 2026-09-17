#include "i86.h"
#include "constants.h"
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_log.h>

#define MOUSE_RESET_GET_INSTALLED 0x00
#define MOUSE_GET_POSITION_AND_BUTTON_STATUS 0x03
#define MOUSE_SET_CURSOR_POSITION 0x04
#define MOUSE_GET_BUTTON_PRESS_INFO 0x05

#define min(a,b) (((a) < (b)) ? (a) : (b))

int mouse_virtual_interrupt_handler(union REGS* inRegs, union REGS* outRegs)
{
    /* To read the mouse we need to PUMP SDL events */
    PORT_SDLPumpEvents(1);

    switch (inRegs->w.ax)
    {
        case MOUSE_RESET_GET_INSTALLED:
            /* https://stanislavs.org/helppc/int_33-0.html
             *
             * - resets mouse to default driver values:
             *  .  mouse is positioned to screen center
             *  .  mouse cursor is reset and hidden
             *  .  no interrupts are enabled (mask = 0)
             *  .  double speed threshold set to 64 mickeys per second
             *  .  horizontal mickey to pixel ratio (8 to 8)
             *  .  vertical mickey to pixel ratio (16 to 8)
             *  .  max width and height are set to maximum for video mode
             */

            /* Currently does nothing */
            outRegs->w.ax = 0xFFFF;
            outRegs->w.bx = 0x3;
            return outRegs->w.ax;
        case MOUSE_GET_POSITION_AND_BUTTON_STATUS:
            /*
             * on return:
             *  CX = horizontal (X) position  (0..639)
             *  DX = vertical (Y) position  (0..199)
             *  BX = button status:
             *
             *      |F-8|7|6|5|4|3|2|1|0|  Button Status
             *        |  | | | | | | | `---- left button (1 = pressed)
             *        |  | | | | | | `----- right button (1 = pressed)
             *        `------------------- unused
             *
             *
             *  - values returned in CX, DX are the same regardless of video mode
             */
            float mouse_x, mouse_y; SDL_MouseButtonFlags flags;
            flags = SDL_GetMouseState(&mouse_x, &mouse_y);

            int window_width, window_height;
            SDL_GetWindowSize(state.window, &window_width, &window_height);

            float scale = min((float)BASE_WINDOW_WIDTH / (float)window_width,
                (float)BASE_WINDOW_HEIGHT / (float)window_height);

            outRegs->w.cx = (int)(mouse_x * 2 * scale); /* Two times because, we need to be in 0-639 range, and we have 320px width */
            outRegs->w.dx = (int)(mouse_y * scale);

            outRegs->w.bx = flags; /* SDL output is compatible with int33h */
            return outRegs->w.ax;
        case MOUSE_SET_CURSOR_POSITION:
            /*
             * AX = 4
             * CX = horizontal position
             * DX = vertical position
             * returns nothing
             * - default cursor position is at the screen center
             * - the position must be within the range of the current video mode
             * - the position may be rounded to fit screen mode resolution
             */
            /* Originally this functions prevent mouse from escaping the screen. In windowed environment it's only annoying
            SDL_GetWindowSize(state.window, &window_width, &window_height);

            scale = min((float)BASE_WINDOW_WIDTH / (float)window_width,
                (float)BASE_WINDOW_HEIGHT / (float)window_height);

            SDL_WarpMouseInWindow(state.window, (float)inRegs->w.dx / 2 / scale, (float)inRegs->w.cx / scale);*/
            return outRegs->w.ax;
        case MOUSE_GET_BUTTON_PRESS_INFO:
           /*
            * on return:
            *   BX = count of button presses (0-32767), set to zero after call
            *   CX = horizontal position at last press
            *   DX = vertical position at last press
            *   AX = status:
            */
            flags = SDL_GetMouseState(&mouse_x, &mouse_y);

            SDL_GetWindowSize(state.window, &window_width, &window_height);

            scale = min((float)BASE_WINDOW_WIDTH / (float)window_width,
                (float)BASE_WINDOW_HEIGHT / (float)window_height);

            outRegs->w.cx = (int)(mouse_x * 2 * scale); /* Two times because, we need to be in 0-639 range, and we have 320px width */
            outRegs->w.dx = (int)(mouse_y * scale);

            outRegs->w.ax = flags; /* SDL output is compatible with int33h */

            outRegs->w.bx = ( inRegs->w.bx == 0 ? state.left_mouse_button_presses : state.right_mouse_button_presses) & 0x7FFF;
            if (inRegs->w.bx)
                state.right_mouse_button_presses = 0;
            else
                state.left_mouse_button_presses = 0;

            return outRegs->w.ax;
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,"[POLANIE-PORT: mouse.c] int386 unhandled interrupt: ax=%02x, bx=%02x, cx=%02x, dx=%02x\n", \
        inRegs->w.ax, inRegs->w.bx, inRegs->w.cx, inRegs->w.dx);
    return outRegs->w.ax;
}
