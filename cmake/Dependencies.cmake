# --------------------------------------
# Dependencies
# --------------------------------------

include(FetchContent)

set(USE_SYSTEM_SDL3 OFF)
set(USE_SYSTEM_SDL3_MIXER OFF)

# --------------------------------------
# Try system libraries on Linux
# --------------------------------------

if(UNIX)

    find_package(SDL3 CONFIG QUIET)
    if(SDL3_FOUND)
        message(STATUS "Using system SDL3")
        set(USE_SYSTEM_SDL3 ON)
    endif()
	
	find_package(SDL3_mixer CONFIG QUIET)
    if(SDL3_mixer_FOUND)
        message(STATUS "Using system SDL3_Mixer")
        set(USE_SYSTEM_SDL3_MIXER ON)
    endif()

endif()


# --------------------------------------
# Fetch missing libraries
# --------------------------------------

# SDL3
if(NOT USE_SYSTEM_SDL3)

    message(STATUS "Fetching SDL3")

    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)
    set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG main
    )

    FetchContent_MakeAvailable(SDL3)

endif()

if(NOT USE_SYSTEM_SDL3_MIXER)

    message(STATUS "Fetching SDL3_mixer")

    set(SDLMIXER_VENDORED OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
            SDL3_mixer
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
            GIT_TAG main
    )

    FetchContent_MakeAvailable(SDL3_mixer)

endif()

# --------------------------------------
# Add all the other libraries
# --------------------------------------

# Flic
set(BUILD_SHARED_LIBS OFF)
message(STATUS "Fetching flic-lib")

FetchContent_Declare(
        flic_lib
        GIT_REPOSITORY https://github.com/Pieshka/flic.git
        GIT_TAG main
)

FetchContent_MakeAvailable(flic_lib)
set(BUILD_SHARED_LIBS ON)

# --------------------------------------
# Install runtime dependencies
# --------------------------------------

include(GNUInstallDirs)

if(TARGET SDL3-shared)
    install(
            TARGETS SDL3-shared
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()

if(TARGET SDL3_mixer-shared)
    install(
            TARGETS SDL3_mixer-shared
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
endif()