#include "polanieapp.h"
#include "compat.h"
#include "icons.h"
#include <flic.h>

int licznik = 0; // For battle.cpp
extern int quitLevel; // From battle.cpp
extern int endGame; // From main.cpp
extern char EndMap; // From mapa.cpp
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
    m_virtualMouseX = 0;
    m_virtualMouseY = 0;
    m_virtualMouseButton = 0;
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

    if (!VerifyFilesystem())
        return 1;

    int count = 0;
    SDL_JoystickID* ids = SDL_GetGamepads(&count);
    m_gamepad = nullptr;

    for (int i = 0; i < count; i++)
    {
        SDL_Gamepad* gpad = SDL_OpenGamepad(ids[i]);
        if (gpad != nullptr)
        {
            m_gamepad = gpad;
            break;
        }
    }

    return 0;
}

void PolanieApp::Close()
{
    SDL_CloseGamepad(m_gamepad);
    SDL_DestroyTexture(m_texture);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_ShowCursor();
    SDL_Quit();
}

static int ConvSDLToDOSCode(SDL_Scancode sc)
{
    switch (sc)
    {
        case SDL_SCANCODE_UP:    return 72;
        case SDL_SCANCODE_DOWN:  return 80;
        case SDL_SCANCODE_LEFT:  return 75;
        case SDL_SCANCODE_RIGHT: return 77;

        default:
            return static_cast<int>(sc);
    }
}

void PolanieApp::ProcessEvents()
{
    SDL_Event event;

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

            // Best getch() emulator
            case SDL_EVENT_KEY_DOWN:
            {
                // Press F11 to fullscreen
                if (event.key.key == SDLK_F11)
                {
                    SDL_SetWindowFullscreen(m_window, !(SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN));
                    break;
                }

                // Ignore modifier events
                if (event.key.key == SDLK_LSHIFT ||
                    event.key.key == SDLK_RSHIFT ||
                    event.key.key == SDLK_LCTRL  ||
                    event.key.key == SDLK_RCTRL  ||
                    event.key.key == SDLK_LALT   ||
                    event.key.key == SDLK_RALT   ||
                    event.key.key == SDLK_LGUI   ||
                    event.key.key == SDLK_RGUI)
                {
                    break;
                }

                // If not ASCII - put the scan code
                if (event.key.key > 0xFF)
                {
                    m_mouse->Key = ConvSDLToDOSCode(event.key.scancode);
                    m_mouse->SetKeyReady(1);
                    break;
                }

                SDL_Keycode keycode = SDL_GetKeyFromScancode(event.key.scancode, event.key.mod, false);
                m_mouse->Key = SDL_static_cast(int, keycode);
                m_mouse->SetKeyReady(1);
                break;
            }

            case SDL_EVENT_MOUSE_MOTION:
            {
                m_virtualMouseX = event.motion.x * scale;
                m_virtualMouseY = event.motion.y * scale;
                m_virtualMouseButton = SDL_static_cast(int, event.motion.state);
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                m_virtualMouseX = event.button.x * scale;
                m_virtualMouseY = event.button.y * scale;
                m_virtualMouseButton |= SDL_static_cast(int, SDL_BUTTON_MASK(event.button.button));
                m_mouse->IncrementPresses(event.button.button);
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                m_virtualMouseButton &= ~SDL_static_cast(int, SDL_BUTTON_MASK(event.button.button));
                break;
            }

            case SDL_EVENT_GAMEPAD_ADDED:
            {
                if (m_gamepad != nullptr)
                    break;

                m_gamepad = SDL_OpenGamepad(event.jdevice.which);
                break;
            }

            case SDL_EVENT_GAMEPAD_REMOVED:
            {
                if (m_gamepad == nullptr || event.jdevice.which != SDL_GetGamepadID(m_gamepad))
                    break;

                SDL_CloseGamepad(m_gamepad);
                m_gamepad = nullptr;
                break;
            }

            case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            {
                Sint16 axisValue = 0;
                if (event.gaxis.value < -3000 || event.gaxis.value > 3000)
                {
                    // Ignore small axis values
                    axisValue = event.gaxis.value;
                    m_mouse->SetIsInMotion(1);
                }
                else
                {
                    m_mouse->SetIsInMotion(0);
                }

                switch (event.gaxis.axis)
                {
                    case SDL_GAMEPAD_AXIS_LEFTX:
                        m_mouse->SetAxisValue(0, axisValue);
                        break;

                    case SDL_GAMEPAD_AXIS_LEFTY:
                        m_mouse->SetAxisValue(1, axisValue);
                        break;

                    default:
                        break;
                }
                break;
            }

            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            {
                switch (event.gbutton.button)
                {
                    // Map to Left Button
                    case SDL_GAMEPAD_BUTTON_SOUTH:
                    {
                        m_virtualMouseButton |= 1;
                        m_mouse->IncrementPresses(SDL_BUTTON_LEFT);
                        break;
                    }

                    // Map to Right Button
                    case SDL_GAMEPAD_BUTTON_EAST:
                    {
                        m_virtualMouseButton |= 2;
                        m_mouse->IncrementPresses(SDL_BUTTON_RIGHT);
                        break;
                    }

                    // Map to ESC button
                    case SDL_GAMEPAD_BUTTON_WEST:
                    {
                        m_mouse->Key = 27;
                        m_mouse->SetKeyReady(1);
                        break;
                    }

                    // Increase gamepad speed [TEMPORARY]
                    case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
                    {
                        m_mouse->IncreaseGamepadSpeed();
                        break;
                    }

                    // Decrease gamepad speed [TEMPORARY]
                    case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
                    {
                        m_mouse->DecreaseGamepadSpeed();
                        break;
                    }

                    default:
                        break;
                }

                break;
            }

            case SDL_EVENT_GAMEPAD_BUTTON_UP:
            {
                switch (event.gbutton.button)
                {
                    // Map to Left Button
                    case SDL_GAMEPAD_BUTTON_SOUTH:
                    {
                        m_virtualMouseButton &= ~1;
                        break;
                    }

                    // Map to Right Button
                    case SDL_GAMEPAD_BUTTON_EAST:
                    {
                        m_virtualMouseButton &= ~2;
                        break;
                    }

                    // Map to ESC button
                    case SDL_GAMEPAD_BUTTON_WEST:
                    {
                        m_mouse->Key = 0;
                        m_mouse->SetKeyReady(0);
                        break;
                    }

                    default:
                        break;
                }

                break;
            }

            default:
                break;

        }
    }

    // Gamepad motion and 10 ms elapsed
    static Uint64 gamepadRefreshedLastTime = SDL_GetTicks();
    Uint64 gamepadRefreshedNow = SDL_GetTicks();

    if (m_mouse->GetIsInMotion() && gamepadRefreshedNow - gamepadRefreshedLastTime >= 10)
    {
        gamepadRefreshedLastTime = gamepadRefreshedNow;
        m_virtualMouseX = SDL_clamp(m_virtualMouseX + SDL_static_cast(float,m_mouse->GetGamepadSpeed() * m_mouse->GetAxisValue(0)) / SDL_JOYSTICK_AXIS_MAX, 0, m_windowWidth);
        m_virtualMouseY = SDL_clamp(m_virtualMouseY + SDL_static_cast(float,m_mouse->GetGamepadSpeed() * m_mouse->GetAxisValue(1)) / SDL_JOYSTICK_AXIS_MAX, 0, m_windowHeight);
    }

    // Update mouse position
    m_mouse->X = SDL_static_cast(int, m_virtualMouseX);
    m_mouse->Y = SDL_static_cast(int, m_virtualMouseY);
    m_mouse->Button = m_virtualMouseButton;
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
    flic::Header header = {};

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

        if (m_exiting)
        {
            fclose(f);
            return;
        }

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
    if (SDL_strcasecmp(p_filename, "save") > 0)
    {
        sprintf(buffer, "%s/%s", prefPath, p_filename);
        return buffer;
    }

    SDL_snprintf(buffer, sizeof(buffer), "%sGames/PolanieCD/%s", SDL_GetUserFolder(SDL_FOLDER_HOME), p_filename);

    if (SDL_GetPathInfo(buffer, NULL))
        return buffer;

    SDL_strlcpy(upper_file, p_filename, sizeof(upper_file));
    SDL_strupr(upper_file);

    SDL_snprintf(buffer, sizeof(buffer), "%sGames/PolanieCD/%s", SDL_GetUserFolder(SDL_FOLDER_HOME), upper_file);

    if (SDL_GetPathInfo(buffer, NULL))
        return buffer;

    SDL_snprintf(buffer, sizeof(buffer), "%sGames/PolanieCD/%s", SDL_GetUserFolder(SDL_FOLDER_HOME), p_filename);

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

void PolanieApp::EnableTextInput(int x, int y, int width, int height)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;

    SDL_SetTextInputArea(m_window, &rect, 0);
    SDL_StartTextInput(m_window);
}

void PolanieApp::DisableTextInput()
{
    SDL_StopTextInput(m_window);
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
