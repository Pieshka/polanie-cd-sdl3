#include "i86.h"
#include "constants.h"
#include "icons.h"
#include <SDL3/SDL.h>
#include <stdlib.h>

#define SET_VIDEO_MODE 0x00
#define VGA_DAC_MASK_REGISTER 0x3C6
#define VGA_DAC_CONTROL_1 0x3C8
#define VGA_DAC_CONTROL_2 0x3C9

static unsigned char dos_framebuffer[64000];
AppState state = {0};
static SDL_Texture *screen;
static SDL_Color palette_colors[256];

extern void PIT_update();

void* PORT_getFakeFramebuffer()
{
    return &dos_framebuffer[0];
}

void PORT_setEditor()
{
    state.is_editor = 1;
}

static void refresh_framebuffer()
{
    void *pixels;
    int pitch;

    if (!SDL_LockTexture(screen, NULL, &pixels, &pitch))
        return;

    for (int y = 0; y < BASE_WINDOW_HEIGHT; y++)
    {
        const Uint8 *src = dos_framebuffer + y * BASE_WINDOW_WIDTH;
        Uint32 *row = (Uint32 *)((Uint8*)pixels + y * pitch);

        for (int x = 0; x < BASE_WINDOW_WIDTH; x++)
        {
            const SDL_Color c = palette_colors[src[x]];
            row[x] =
                ((Uint32)c.r << 24) |
                ((Uint32)c.g << 16) |
                ((Uint32)c.b << 8)  |
                (Uint32)c.a;
        }
    }

    SDL_UnlockTexture(screen);

    SDL_RenderClear(state.renderer);
    SDL_RenderTexture(state.renderer, screen, NULL, NULL);
    SDL_RenderPresent(state.renderer);
}

void PORT_SDLPumpEvents(int only_pump)
{
    SDL_Event event;

    //Temporary
    if (only_pump == 0)
    {
        refresh_framebuffer();
        PIT_update();
    }

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_TEXT_INPUT:
            {
                const char* text = event.text.text;

                while (*text)
                {
                    state.key_queue[state.key_queue_write] = *text;
                    state.key_queue_write = (state.key_queue_write + 1) % 32;
                    text++;
                }
                break;
            }

            case SDL_EVENT_KEY_DOWN:
            {
                switch (event.key.key)
                {
                    case SDLK_ESCAPE:
                    {
                        state.key_queue[state.key_queue_write] = 27;
                        state.key_queue_write =
                            (state.key_queue_write + 1) % 32;
                        break;
                    }

                    case SDLK_RETURN:
                    {
                        state.key_queue[state.key_queue_write] = '\r';
                        state.key_queue_write =
                            (state.key_queue_write + 1) % 32;
                        break;
                    }

                    case SDLK_BACKSPACE:
                    {
                        state.key_queue[state.key_queue_write] = '\b';
                        state.key_queue_write =
                            (state.key_queue_write + 1) % 32;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT) state.left_mouse_button_presses++;
                if (event.button.button == SDL_BUTTON_RIGHT) state.right_mouse_button_presses++;
                break;
            }

            case SDL_EVENT_QUIT:
            {
                SDL_Quit();
                exit(-1);
            }
        }
    }
}

static void init_sdl()
{
    SDL_SetAppMetadata("Polanie CD Portable", APP_VERSION_STRING, "org.polaniecd.Polanie");

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL video subsystem could not initialize! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(-1); /* We have no response for this, so just quit the app */
    }

    const char *window_title = state.is_editor ? EDITOR_TITLE : GAME_TITLE;
    if (!SDL_CreateWindowAndRenderer(window_title, BASE_WINDOW_WIDTH * 4, BASE_WINDOW_HEIGHT * 4, SDL_WINDOW_RESIZABLE, &state.window, &state.renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not create window! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(-1); /* We have no response for this, so just quit the app */
    }

    SDL_SetRenderLogicalPresentation(state.renderer, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    screen = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, BASE_WINDOW_WIDTH, BASE_WINDOW_HEIGHT);

    SDL_SetTextureScaleMode(screen, SDL_SCALEMODE_PIXELART);

    SDL_HideCursor();
    SDL_StartTextInput(state.window);

    SDL_IOStream* icon_stream = SDL_IOFromMem(state.is_editor ? editor_bmp : game_bmp, state.is_editor ? editor_bmp_len : game_bmp_len);
    if (icon_stream)
    {
        SDL_Surface* icon = SDL_LoadBMP_IO(icon_stream, true);
        if (icon)
        {
            SDL_SetWindowIcon(state.window, icon);
            SDL_DestroySurface(icon);
        }
    }
}

static void quit_sdl()
{
    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_ShowCursor();
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

int video_virtual_interrupt_handler(union REGS* inRegs, union REGS* outRegs)
{
    switch (inRegs->h.ah)
    {
        case SET_VIDEO_MODE:
            switch (inRegs->h.al)
            {
                case 0x13: /* Initialize SDL */
                    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "POLANIE-PORT: Initializing SDL3 video subsystem...");
                    init_sdl();
                    return outRegs->w.ax;
                case 0x03: /* Deinitialize SDL */
                    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "POLANIE-PORT: Quitting SDL3 video subsystem...");
                    quit_sdl();
                    return outRegs->w.ax;
            }
    }

    SDL_Log("[POLANIE-PORT: sdl.c] int386 unhandled interrupt: ax=%02x, bx=%02x, cx=%02x, dx=%02x\n", \
        inRegs->w.ax, inRegs->w.bx, inRegs->w.cx, inRegs->w.dx);
    return outRegs->w.ax;
}

static Uint8 VGAtoSDL(Uint8 value)
{
    return (Uint8)(value << 2);
}

unsigned video_virtual_outp_handler(unsigned port, unsigned value)
{
    static unsigned int current_color_number = 0;
    static unsigned int current_shade = 0;

    switch (port)
    {
        case VGA_DAC_MASK_REGISTER:
            /* value should be 0xff, ignoring */
            return 0;
        case VGA_DAC_CONTROL_1:
            /* Set current color number */
            current_color_number = value;
            current_shade = 0;
            return 0;
        case VGA_DAC_CONTROL_2:
            /* Set the palette for current color */

            switch (current_shade)
            {
                case 0:
                    palette_colors[current_color_number].r = VGAtoSDL(value);
                    current_shade++;
                    return 0;
                case 1:
                    palette_colors[current_color_number].g = VGAtoSDL(value);
                    current_shade++;
                    return 0;
                case 2:
                    palette_colors[current_color_number].b = VGAtoSDL(value);
                    palette_colors[current_color_number].a = 255;
                    return 0;
                default:
                    break;
            }
            return 0;
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION,"[POLANIE-PORT: sdl.c] outp unhandled port: %02x, value=%02x\n", \
        port, value);
    return 1;
}