#include <stdlib.h>
#include <stdio.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_locale.h>

extern const char* get_asset_path(const char* file);

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
        FILE *f = fopen(get_asset_path(filenames[i]), "rb");
        if (!f)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[POLANIE-PORT] Can't find file / Nie można znaleźć pliku: %s\n", get_asset_path(filenames[i]));
            ok = 0;
        }
        else
            fclose(f);
    }

    for (int i = 26; i <= 52; i++)
    {
        sprintf(buf, "levels/level.%d", i);
        FILE *f = fopen(get_asset_path(buf), "rb");

        if (!f)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[POLANIE-PORT] Can't find file / Nie można znaleźć pliku: %s\n", get_asset_path(buf));
            ok = 0;
        }
        else
            fclose(f);
    }

    if (!ok)
    {
        int localCount;
        SDL_Locale** prefLocales = SDL_GetPreferredLocales(&localCount);
        if (localCount > 0 && SDL_strcmp(prefLocales[0]->language, "pl") == 0)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Błąd wczytywania zasobów",
                "Nie można odnaleźć plików potrzebnych do działania gry.\n\nInformacje na temat przygotowania gry "
                "do działania i skopiowania odpowiednich zasobów z płyty CD można znaleźć na stronie projektu.", NULL);
        }
        else
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error loading resources",
                "The files required for the game to run cannot be found.\n\nInformation on how to set up the game "
                "and copy the necessary resources from the CD can be found on the project's website.", NULL);
        }

        exit(2);
    }
}