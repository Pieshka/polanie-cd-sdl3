# --------------------------------------
# Dependencies
# --------------------------------------

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

    add_subdirectory(3rdparty/SDL EXCLUDE_FROM_ALL)

endif()

if(NOT USE_SYSTEM_SDL3_MIXER)

    add_subdirectory(3rdparty/SDL_mixer EXCLUDE_FROM_ALL)

endif()

# --------------------------------------
# Add all the other libraries
# --------------------------------------

# Flic
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(3rdparty/flic EXCLUDE_FROM_ALL)
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