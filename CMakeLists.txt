cmake_minimum_required(VERSION 3.30)

# --------------------------------------
# Version
# --------------------------------------

include(cmake/Version.cmake)

# --------------------------------------
# Project settings
# --------------------------------------

project(polanie VERSION ${APP_VERSION_STRING} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# --------------------------------------
# Position Independent Code settings
# --------------------------------------

include(CheckPIESupported)
check_pie_supported()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --------------------------------------
# Dependencies
# --------------------------------------

include(cmake/Dependencies.cmake)

# --------------------------------------
# Application sub-projects
# --------------------------------------

add_subdirectory(compatlib)
add_subdirectory(game-src)
add_subdirectory(editor-src)

# --------------------------------------
# CPack
# --------------------------------------
include(cmake/CPack.cmake)
include(CPack)