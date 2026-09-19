#ifndef POLANIE_COMPAT_H
#define POLANIE_COMPAT_H

#ifdef __EMSCRIPTEN__
#define Any_ShowSimpleMessageBox Emscripten_ShowSimpleMessageBox
#elif defined(__vita__)
#define Any_ShowSimpleMessageBox Vita_ShowSimpleMessageBox
#else
#define Any_ShowSimpleMessageBox SDL_ShowSimpleMessageBox
#endif

#endif //POLANIE_COMPAT_H
