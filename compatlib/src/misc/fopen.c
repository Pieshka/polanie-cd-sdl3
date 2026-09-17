#include <stdio.h>
#include "constants.h"

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
    sprintf(buffer, "%s/%s", NON_WIN_ASSET_PATH, file);
    return fopen(file, mode);
}