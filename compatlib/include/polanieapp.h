#ifndef POLANIE_POLANIEAPP_H
#define POLANIE_POLANIEAPP_H

#include <SDL3/SDL.h>
#include "mouse.h"

#ifndef NULL
#define NULL 0
#endif
#define cprintf SDL_Log

class PolanieApp
{
public:
    PolanieApp();
    ~PolanieApp();

    int Init(int p_isEditor);
    void Close();

    void ProcessEvents();
    void RenderFramebuffer();
    void TickCounter();

    void PlayFlic(const char *filename);

    Uint8* GetFrameBuffer();
    const char* GetFilePath(const char* p_filename);
    int IsExiting();

    void SetPalette(const Uint8* palette);

    void EnableTextInput(int x, int y, int width, int height);
    void DisableTextInput();

    bool VerifyFilesystem();

private:
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;
    SDL_Texture *m_texture;
    SDL_Color m_palette[256]{};
    Uint8 m_dosFramebuffer[64000]{};
    int m_windowWidth, m_windowHeight;
    int m_exiting;

    Mouse *m_mouse;
    SDL_Gamepad *m_gamepad;
    float m_virtualMouseX, m_virtualMouseY;
    int m_virtualMouseButton;
};

extern PolanieApp* g_polanie;

#endif //POLANIE_POLANIEAPP_H
