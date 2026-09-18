#include <stdio.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_platform_defines.h>
#include <SDL3/SDL_stdinc.h>

const char* get_asset_path(const char* file)
{
#ifdef SDL_PLATFORM_WINDOWS
    return file;
#else
    static char buffer[256];
    static char upper_file[256];

    snprintf(buffer, sizeof(buffer), "%sGames/PolanieCD/%s", SDL_GetUserFolder(SDL_FOLDER_HOME), file);

    if (SDL_GetPathInfo(buffer, NULL))
        return buffer;

    SDL_strlcpy(upper_file, file, sizeof(upper_file));
    SDL_strupr(upper_file);

    snprintf(buffer, sizeof(buffer), "%sGames/PolanieCD/%s", SDL_GetUserFolder(SDL_FOLDER_HOME), upper_file);

    if (SDL_GetPathInfo(buffer, NULL))
        return buffer;

    snprintf(buffer, sizeof(buffer), "%sGames/PolanieCD/%s", SDL_GetUserFolder(SDL_FOLDER_HOME), file);

    return buffer;
#endif
}

FILE *PORT_fopen(const char * file, const char * mode)
{
    char buffer[256];

    /* Redirect save files to prefered path */
    char* prefPath = SDL_GetPrefPath("polaniecd","polanie");
    if (SDL_strcasecmp(file, "save") > 0)
    {
        sprintf(buffer, "%s/%s", prefPath, file);
        return fopen(buffer, mode);
    }

    /* Everything else goes to the standard path */
    return fopen(get_asset_path(file), mode);
}
