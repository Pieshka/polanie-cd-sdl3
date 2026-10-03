#include "filesystem.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_iostream.h>
#include <emscripten.h>
#include <emscripten/wasmfs.h>

static backend_t opfs = nullptr;
static backend_t fetchfs = nullptr;

extern const char* g_files[35];
extern const char* g_optionalFiles[7];
const int g_musicCount = 15;

bool Emscripten_OPFSDisabled()
{
	return MAIN_THREAD_EM_ASM_INT({return !!Module["disableOpfs"]});
}

void Emscripten_SetupFilesystem()
{
	char buffer[1024];
	SDL_snprintf(buffer, sizeof(buffer), "%s/PolanieCD", Emscripten_streamHost);
	fetchfs = wasmfs_create_fetch_backend(buffer, 512 * 1024);

	wasmfs_create_directory("/PolanieCD", 0755, fetchfs);
	wasmfs_create_directory("/PolanieCD/data", 0755, fetchfs);
	wasmfs_create_directory("/PolanieCD/levels", 0755, fetchfs);
	wasmfs_create_directory("/PolanieCD/music", 0755, fetchfs);

	const auto registerFile = [](const char* p_path) {
		char bundledPath[1024];
		SDL_snprintf(bundledPath, sizeof(bundledPath), "%s/%s", Emscripten_bundledPath, p_path);

		if (SDL_GetPathInfo(bundledPath, nullptr)) {
			SDL_Log("File %s is bundled and won't be streamed", p_path);
			return;
		}

		char wasmPath[1024];
		SDL_snprintf(wasmPath,sizeof(wasmPath),"/PolanieCD/%s", p_path);
		int fd = wasmfs_create_file(wasmPath, 0644, fetchfs);
		if (fd < 0)
		{
			SDL_Log("Failed to register streamed file: %s", wasmPath);
			return;
		}
		SDL_Log("File %s set up for streaming", wasmPath);
	};

	for (const char* file : g_files) {
		registerFile(file);
	}

	for (const char* file : g_optionalFiles) {
		registerFile(file);
	}

	for (int i = 0; i < g_musicCount; i++)
	{
		char buffer4[1024];
		SDL_snprintf(buffer4, sizeof(buffer4), "music/track%02d.flac", i);
		registerFile(buffer4);
	}

	if (!Emscripten_OPFSDisabled()) {
		if (!opfs) {
			opfs = wasmfs_create_opfs_backend();
		}

		wasmfs_create_directory(Emscripten_savePath, 0755, opfs);
	}
}