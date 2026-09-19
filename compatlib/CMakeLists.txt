# --------------------------------------
# Library sources
# --------------------------------------

set(COMPATLIB_SOURCES
        src/polanieapp.cpp
        src/mouse.cpp
        src/sound.cpp
        src/polaniefiles.cpp
)

set(COMPATLIB_HEADERS
        include/polanieapp.h
        include/mouse.h
        include/sound.h
        src/compat.h
)

add_library(compatlib STATIC
        ${COMPATLIB_SOURCES}
        ${COMPATLIB_HEADERS}
)

target_include_directories(compatlib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(compatlib
        PRIVATE
        ${flic_lib_SOURCE_DIR}
)

target_link_libraries(compatlib
        PUBLIC
        SDL3::SDL3
        SDL3_mixer::SDL3_mixer
)

target_link_libraries(compatlib
        PRIVATE
        flic-lib
)

set_target_properties(compatlib PROPERTIES
        OUTPUT_NAME "compat"
)

# --------------------------------------
# Compile definitions
# --------------------------------------

target_compile_definitions(compatlib PRIVATE
        APP_VERSION_STRING="${APP_VERSION_STRING}"
        APP_VERSION_MAJOR=${APP_VERSION_MAJOR}
        APP_VERSION_MINOR=${APP_VERSION_MINOR}
        APP_VERSION_PATCH=${APP_VERSION_PATCH}
)

target_compile_definitions(compatlib PRIVATE $<$<CONFIG:Debug>:DEBUG_MODE>)

# --------------------------------------
# Compile options
# --------------------------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(compatlib PRIVATE
            -Wall
            -Wextra
    )
endif()