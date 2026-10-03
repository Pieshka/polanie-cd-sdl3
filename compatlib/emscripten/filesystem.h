#ifndef EMSCRIPTEN_FILESYSTEM_H
#define EMSCRIPTEN_FILESYSTEM_H

#ifndef POLANIE_EMSCRIPTEN_HOST
#define POLANIE_EMSCRIPTEN_HOST ""
#endif

static const char* Emscripten_bundledPath = "/bundled";
static const char* Emscripten_savePath = "/save";
static const char* Emscripten_streamPath = "/";
static const char* Emscripten_streamHost = POLANIE_EMSCRIPTEN_HOST;

void Emscripten_SetupFilesystem();

#endif // EMSCRIPTEN_FILESYSTEM_H