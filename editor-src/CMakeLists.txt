# --------------------------------------
# Application sources
# --------------------------------------

set(EDITOR_SOURCES
        edit.cpp
        graphics.cpp
        image13h.cpp
        main.cpp
        mouse.cpp
        mover1.cpp
        world.cpp
)

add_executable(polanie-editor2
        ${EDITOR_SOURCES}
        res/polanie-editor2.rc
)


target_include_directories(polanie-editor2
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/compatlib/include
)

target_link_libraries(polanie-editor2
        PRIVATE
        compatlib
)

# --------------------------------------
# Compile definitions
# --------------------------------------

target_compile_definitions(polanie-editor2 PRIVATE
        APP_VERSION_STRING="${APP_VERSION_STRING}"
        APP_VERSION_MAJOR=${APP_VERSION_MAJOR}
        APP_VERSION_MINOR=${APP_VERSION_MINOR}
        APP_VERSION_PATCH=${APP_VERSION_PATCH}
)

# --------------------------------------
# Compile options
# --------------------------------------

# (Open)Watcom assumes char = unsigned char
# Polanie also assumes that, so -funsigned-char \ /J
if(MSVC)
    target_compile_options(polanie-editor2 PRIVATE
            /W4
            /J
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(polanie-editor2 PRIVATE
            -Wall
            -Wextra
            -funsigned-char
            -fno-strict-aliasing
            -fbounds-check
    )
endif()

# --------------------------------------
# Install definitions
# --------------------------------------

include(GNUInstallDirs)

install(
        TARGETS polanie-editor2
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(WIN32)
    # Install C++ standard library and its dependencies
    get_filename_component(CLANG_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    install(
            FILES
            "${CLANG_BIN_DIR}/libc++.dll"
            "${CLANG_BIN_DIR}/libunwind.dll"
            DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

endif()