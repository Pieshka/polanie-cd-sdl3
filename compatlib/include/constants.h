#ifndef POLANIE_CONSTANTS_H
#define POLANIE_CONSTANTS_H

#include <SDL3/SDL.h>

#define APP_NAME "Polanie CD"
#define BASE_WINDOW_WIDTH 320
#define BASE_WINDOW_HEIGHT 200

typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;

    char key_queue[32];
    int key_queue_read;
    int key_queue_write;

    uint32_t left_mouse_button_presses;
    uint32_t right_mouse_button_presses;
} AppState;

extern AppState state;

#endif //POLANIE_CONSTANTS_H
