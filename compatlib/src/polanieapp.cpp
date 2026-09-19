#include "polanieapp.h"
#include "compat.h"
#include "icons.h"
#include <flic.h>

int licznik = 0; // For battle.cpp
extern int quitLevel; // From battle.cpp
extern int endGame; // From main.cpp
extern int EndMap; // From mapa.cpp
extern const char* g_files[35];

PolanieApp::PolanieApp()
{
    m_window = nullptr;
    m_renderer = nullptr;
    m_texture = nullptr;
    m_mouse = &mouse;
    m_windowWidth = 320;
    m_windowHeight = 200;
    m_exiting = 0;
    SDL_zero(m_palette);
    SDL_zero(m_dosFramebuffer);
}

PolanieApp::~PolanieApp()
= default;

int PolanieApp::Init(int p_isEditor)
{
    {
        const int version = SDL_GetVersion();
        SDL_Log("SDL version %d.%d.%d (%s)",
            SDL_VERSIONNUM_MAJOR(version),
            SDL_VERSIONNUM_MINOR(version),
            SDL_VERSIONNUM_MICRO(version),
            SDL_GetRevision());
    }

    SDL_SetAppMetadata("Polanie CD Portable", nullptr, "org.polaniecd.Polanie");

    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "0");
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");

    Uint32 initFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD;

    if (!SDL_Init(initFlags))
    {
        char buffer[256];
        SDL_snprintf(
            buffer,
            sizeof(buffer),
            "\"Polanie CD\" failed to start.\nPlease quit all other applications and try again.\nSDL error: %s",
            SDL_GetError()
        );
        Any_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Polanie CD Error", buffer, nullptr);
        return 1;
    }

    const char * windowTitle = !p_isEditor ? "Polanie CD" : "Polanie CD - Level Editor";
    if (!SDL_CreateWindowAndRenderer(windowTitle, m_windowWidth * 4, m_windowHeight * 4, SDL_WINDOW_RESIZABLE, &m_window, &m_renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "\"Polanie CD\" failed to start.\nCannot create window and renderer.\nSDL error: %s", SDL_GetError());
        Any_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Polanie CD Error",
            "\"Polanie CD\" failed to start.\nPlease quit all other applications and try again."
            "\nFailed to initialize; see logs for details",
            nullptr
        );
        return 1;
    }

    SDL_SetRenderLogicalPresentation(m_renderer, m_windowWidth, m_windowHeight, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

    SDL_IOStream* iconStream = SDL_IOFromMem(p_isEditor ? editor_bmp : game_bmp, p_isEditor ? editor_bmp_len : game_bmp_len);
    if (iconStream)
    {
        SDL_Surface* icon = SDL_LoadBMP_IO(iconStream, true);
        if (icon)
        {
            SDL_SetWindowIcon(m_window, icon);
            SDL_DestroySurface(icon);
        }
    }

    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, m_windowWidth, m_windowHeight);
    if (!m_texture)
    {
        Any_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Polanie CD Error",
            "\"Polanie CD\" failed to start.\nPlease quit all other applications and try again."
            "\nFailed to initialize; see logs for details",
            nullptr
        );
        return 1;
    }

    SDL_SetTextureScaleMode(m_texture, SDL_SCALEMODE_PIXELART);

    SDL_HideCursor();
    SDL_StartTextInput(m_window);

    if (!VerifyFilesystem())
        return 1;

    return 0;
}

void PolanieApp::Close()
{
    SDL_DestroyTexture(m_texture);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_ShowCursor();
    SDL_Quit();
}

void PolanieApp::ProcessEvents()
{
    SDL_Event event;
    int mouseStateUpdated = 0;

    RenderFramebuffer();
    TickCounter();

    int windowWidth, windowHeight;
    SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);
    float scale = SDL_min(
        SDL_static_cast(float, m_windowWidth) / windowWidth,
        SDL_static_cast(float, m_windowHeight) / windowHeight
    );

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            case SDL_EVENT_QUIT:
                m_exiting = quitLevel = endGame = EndMap = 1;
                break;

            case SDL_EVENT_TEXT_INPUT:
            {
                const char* text = event.text.text;
                while (*text)
                {
                    m_mouse->Key = SDL_static_cast(Uint8, *text++);
                    m_mouse->SetKeyReady(1);
                }
                break;
            }

            case SDL_EVENT_KEY_DOWN:
            {
                switch (event.key.key)
                {
                    case SDLK_ESCAPE:
                        m_mouse->Key = 27;
                        m_mouse->SetKeyReady(1);
                        break;
                    case SDLK_RETURN:
                        m_mouse->Key = '\r';
                        m_mouse->SetKeyReady(1);
                        break;
                    case SDLK_BACKSPACE:
                        m_mouse->Key = '\b';
                        m_mouse->SetKeyReady(1);
                        break;
                    default:
                        break;
                }
                break;
            }

            case SDL_EVENT_MOUSE_MOTION:
            {
                m_mouse->X = SDL_static_cast(int, event.motion.x * scale);
                m_mouse->Y = SDL_static_cast(int, event.motion.y * scale);
                m_mouse->Button = SDL_static_cast(int, event.motion.state);
                mouseStateUpdated = 1;
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                m_mouse->X = SDL_static_cast(int, event.button.x * scale);
                m_mouse->Y = SDL_static_cast(int, event.button.y * scale);
                m_mouse->Button |= SDL_static_cast(int, SDL_BUTTON_MASK(event.button.button));
                m_mouse->IncrementPresses(event.button.button);
                mouseStateUpdated = 1;
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                m_mouse->Button &= ~SDL_static_cast(int, SDL_BUTTON_MASK(event.button.button));
                break;
            }

            default:
                break;

        }
    }

    if (!mouseStateUpdated)
    {
        float mouseX, mouseY;
        m_mouse->Button = SDL_static_cast(int, SDL_GetMouseState(&mouseX, &mouseY));
        m_mouse->X = SDL_static_cast(int, mouseX * scale);
        m_mouse->Y = SDL_static_cast(int, mouseY * scale);
        m_mouse->ClearPresses();
    }
}

void PolanieApp::RenderFramebuffer()
{
    void *pixels;
    int pitch;

    if (!SDL_LockTexture(m_texture, nullptr, &pixels, &pitch))
        return;

    for (int y = 0; y < m_windowHeight; y++)
    {
        const Uint8 *src = m_dosFramebuffer + y * m_windowWidth;
        Uint32 *row = SDL_reinterpret_cast(Uint32 *, SDL_static_cast(Uint8 *, pixels) + y * pitch);

        for (int x = 0; x < m_windowWidth; x++)
        {
            const SDL_Color c = m_palette[src[x]];
            row[x] = (c.r << 24) | (c.g << 16) | (c.b << 8) | c.a;
        }
    }

    SDL_UnlockTexture(m_texture);

    SDL_RenderClear(m_renderer);
    SDL_RenderTexture(m_renderer, m_texture, nullptr, nullptr);
    SDL_RenderPresent(m_renderer);
}

void PolanieApp::TickCounter()
{
    static Uint64 lastCounter = 0;

    const Uint64 frequency = SDL_GetPerformanceFrequency();
    const Uint64 now = SDL_GetPerformanceCounter();

    if (lastCounter == 0)
    {
        lastCounter = now;
        return;
    }

    const Uint64 delta = now - lastCounter;
    const Uint64 tickPeriod = (frequency * 549254ULL) / 10000000ULL; // 54.9254ms

    if (delta >= tickPeriod)
    {
        const Uint64 ticks = delta / tickPeriod;
        licznik += SDL_static_cast(int, ticks);
        lastCounter += ticks * tickPeriod;
    }
}

void PolanieApp::PlayFlic(const char *filename)
{
    FILE *f = fopen(GetFilePath(filename), "rb");
    if (!f)
        return;

    flic::StdioFileInterface file(f);
    flic::Decoder decoder(&file);
    flic::Header header = {.frames = 0,.width = 0,.height = 0,.speed = 0};

    if (!decoder.readHeader(header))
        return;

    std::vector<Uint8> buffer(header.width * header.height);
    flic::Frame frame;
    frame.pixels = &buffer[0];
    frame.rowstride = header.width;

    Uint64 start = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();

    for (int i = 0; i < header.frames; i++)
    {
        decoder.readFrame(frame);

        SDL_memcpy(m_dosFramebuffer, frame.pixels, frame.rowstride * header.height);

        for (int i = 0; i < 256; i++)
        {
            m_palette[i].r = frame.colormap[i].r;
            m_palette[i].g = frame.colormap[i].g;
            m_palette[i].b = frame.colormap[i].b;
            m_palette[i].a = 255;
        }

        ProcessEvents();

        if (m_mouse->IsInputReady() && m_mouse->Key == 27)
        {
            fclose(f);
            return;
        }

        Uint64 target = start + (i + 1) * header.speed * frequency / 1000;
        Uint64 now = SDL_GetPerformanceCounter();

        if (now < target)
        {
            Uint64 remaining = target - now;
            SDL_Delay(remaining * 1000 / frequency);
        }
    }

    fclose(f);
}

Uint8 * PolanieApp::GetFrameBuffer()
{
    return &m_dosFramebuffer[0];
}

const char * PolanieApp::GetFilePath(const char *p_filename)
{
#ifdef SDL_PLATFORM_WINDOWS
    return p_filename;
#else
    static char buffer[256];
    static char upper_file[256];

    char* prefPath = SDL_GetPrefPath("polaniecd", "polanie");
    if (SDL_strcasecmp(file, "save") > 0)
    {
        sprintf(buffer, "%s/%s", prefPath, file);
        return fopen(buffer, mode);
    }

    SDL_snprintf(buffer, sizeof(buffer), "%sGames/PolanieCD/%s", SDL_GetUserFolder(SDL_FOLDER_HOME), file);

    if (SDL_GetPathInfo(buffer, NULL))
        return buffer;

    SDL_strlcpy(upper_file, file, sizeof(upper_file));
    SDL_strupr(upper_file);

    SDL_snprintf(buffer, sizeof(buffer), "%sGames/PolanieCD/%s", SDL_GetUserFolder(SDL_FOLDER_HOME), upper_file);

    if (SDL_GetPathInfo(buffer, NULL))
        return buffer;

    SDL_snprintf(buffer, sizeof(buffer), "%sGames/PolanieCD/%s", SDL_GetUserFolder(SDL_FOLDER_HOME), file);

    return buffer;
#endif
}

int PolanieApp::IsExiting()
{
    return m_exiting;
}

void PolanieApp::SetPalette(const Uint8 *palette)
{
    for (int i = 0; i < 256; i++)
    {
        m_palette[i].r = palette[i * 3 + 0] << 2;
        m_palette[i].g = palette[i * 3 + 1] << 2;
        m_palette[i].b = palette[i * 3 + 2] << 2;
        m_palette[i].a = 255;
    }
}

bool PolanieApp::VerifyFilesystem()
{
    for (auto & file : g_files)
    {
        if (!SDL_GetPathInfo(GetFilePath(file), nullptr))
        {
            char buffer[1024];
            SDL_snprintf(
                buffer,
                sizeof(buffer),
                "\"Polanie CD\" failed to start.\nPlease make sure the file %s is located in the necessary directory",
                file
            );
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", buffer);

            Any_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Polanie CD Error",
                "\"Polanie CD\" failed to start."
                "\nFailed to find files required for the game; see logs for details",
                nullptr
            );

            return false;
        }
    }

    return true;
}
