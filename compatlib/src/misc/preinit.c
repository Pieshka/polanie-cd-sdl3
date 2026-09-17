#include <stdlib.h>
#include <stdio.h>
#include <SDL3/SDL_log.h>
#include "constants.h"

static char* filenames[7] = {
    "font.dat",
    "graf.dat",
    "pal.dat",
    "pic.dat",
    "levels/level.dat",
    "levels/level.ini",
    "levels/level2.ini"
};

/* This functions checks if everything is in place before starting the game */
void check_if_everything_is_ok()
{
    char buf[1024];
    int ok = 1;
    for (int i = 0; i < 7; i++)
    {
#ifdef SDL_PLATFORM_WINDOWS
        FILE *f = fopen(filenames[i], "rb");
#else
        sprintf(buf, "%s/%s", NON_WIN_ASSET_PATH, filenames[i]);
        FILE *f = fopen(buf, "rb");
#endif
        if (!f)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[POLANIE-PORT] Can't find file / Nie można znaleźć pliku: %s\n", buf);
            ok = 0;
        }
        else
            fclose(f);
    }

    for (int i = 26; i <= 52; i++)
    {
#ifdef SDL_PLATFORM_WINDOWS
        sprintf(buf, "levels/level.%d", i);
#else
        sprintf(buf, "%s/levels/level.%d", NON_WIN_ASSET_PATH, i);
#endif
        FILE *f = fopen(buf, "rb");

        if (!f)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[POLANIE-PORT] Can't find file / Nie można znaleźć pliku: %s\n", buf);
            ok = 0;
        }
        else
            fclose(f);
    }

    if (!ok)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[POLANIE-PORT] The files required to run the game cannot be "
                                                   "found. If you have the game disc, insert it into the drive, and "
                                                   "then run the PolanieInstall utility to copy the necessary files "
                                                   "and music from the CD to your hard drive.\n\n");

        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[POLANIE-PORT] Nie można odnaleźć plików potrzebnych do "
                                                   "działania gry. Jeżeli posiadasz płytę z grą, włóż ją do napędu, "
                                                   "a następnie uruchom pomocniczy program PolanieInstall, aby skopiować "
                                                   "potrzebne pliki i muzykę z płyty CD na dysk.\n\n");

        exit(2);
    }
}