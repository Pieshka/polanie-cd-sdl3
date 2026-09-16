#include <stdlib.h>
#include <stdio.h>
#include <SDL3/SDL_log.h>

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
    int ok = 1;
    for (int i = 0; i < 7; i++)
    {
        FILE* f = fopen(filenames[i], "rb");
        if (!f)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[POLANIE-PORT] Can't find file / Nie można znaleźć pliku: %s\n", filenames[i]);
            ok = 0;
        }
        else
            fclose(f);
    }

    char filename[25];

    for (int i = 26; i <= 52; i++)
    {
        sprintf(filename, "levels/level.%d", i);
        FILE* f = fopen(filename, "rb");
        if (!f)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[POLANIE-PORT] Can't find file / Nie można znaleźć pliku: %s\n", filenames[i]);
            ok = 0;
        }
        else
            fclose(f);
    }

    if (!ok)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[POLANIE-PORT] The files required to run the game cannot be "
                                                   "found. If you have the game disc, insert it into the drive, and "
                                                   "then run the PolanieInstall utility to copy the necessary files "
                                                   "and music from the CD to your hard drive.\n\n");

        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[POLANIE-PORT] Nie można odnaleźć plików potrzebnych do "
                                                   "działania gry. Jeżeli posiadasz płytę z grą, włóż ją do napędu, "
                                                   "a następnie uruchom pomocniczy program PolanieInstall, aby skopiować "
                                                   "potrzebne pliki i muzykę z płyty CD na dysk.\n\n");

        exit(2);
    }
}