#ifndef EMSCRIPTEN_MESSAGEBOX_H
#define EMSCRIPTEN_MESSAGEBOX_H

#include <SDL3/SDL_messagebox.h>

bool Emscripten_ShowSimpleMessageBox(
    SDL_MessageBoxFlags flags,
    const char* title,
    const char* message,
    SDL_Window* window
);

#endif //EMSCRIPTEN_MESSAGEBOX_H
