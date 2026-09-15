#include "playfli.h"
#include "constants.h"
#include "flic.h"
#include "i86.h"
#include <SDL3/SDL.h>

static void render_frame(SDL_Texture *screen, flic::Frame &frame, flic::Header &header)
{
    void *pixels;
    int pitch;

    if (!SDL_LockTexture(screen, NULL, &pixels, &pitch))
        return;

    for (int y = 0; y < header.height; y++)
    {
        const Uint8 *src = frame.pixels + y * frame.rowstride;
        Uint32 *row = (Uint32 *)((Uint8*)pixels + y * pitch);

        for (int x = 0; x < header.width; x++)
        {
            const flic::Color c = frame.colormap[src[x]];
            row[x] =
                ((Uint32)c.r << 24) |
                ((Uint32)c.g << 16) |
                ((Uint32)c.b << 8)  |
                255;
        }
    }

    SDL_UnlockTexture(screen);

    SDL_RenderClear(state.renderer);
    SDL_RenderTexture(state.renderer, screen, NULL, NULL);
    SDL_RenderPresent(state.renderer);
}

void play(char *filename)
{
    FILE *f = fopen(filename,"rb");
    if(f == nullptr)
    {
        return;
    }

    flic::StdioFileInterface file(f);
    flic::Decoder decoder(&file);
    flic::Header header;

    if (!decoder.readHeader(header))
        return;

    std::vector<uint8_t> buffer(header.width * header.height);
    flic::Frame frame;
    frame.pixels = &buffer[0];
    frame.rowstride = header.width;

    SDL_Texture* screen = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, header.width, header.height);

    SDL_SetTextureScaleMode(screen, SDL_SCALEMODE_PIXELART);

    Uint64 start = SDL_GetPerformanceCounter();
    Uint64 freq = SDL_GetPerformanceFrequency();

    for (int i = 0; i < header.frames; i++)
    {
        /* Read frame */
        decoder.readFrame(frame);

        /* Render frame */
        render_frame(screen, frame, header);

        /* Pump Events */
        PORT_SDLPumpEvents(1);

        /* Check if hit ESC */
        int pos = state.key_queue_read;
        while (pos != state.key_queue_write)
        {
            if (state.key_queue[pos] == 27)
            {
                state.key_queue_read = (pos + 1) % 32;

                SDL_DestroyTexture(screen);
                fclose(f);
                return;
            }
            pos = (pos + 1) % 32;
        }

        /* Wait */
        Uint64 target = start + (Uint64(i + 1) * Uint64(header.speed) * freq) / 1000;
        Uint64 now = SDL_GetPerformanceCounter();

        if (now < target)
        {
            Uint64 remaining = target - now;
            SDL_Delay((Uint32)((remaining * 1000) / freq));
        }
    }

    SDL_DestroyTexture(screen);
    fclose(f);
}