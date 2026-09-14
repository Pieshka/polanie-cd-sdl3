# --------------------------------------
# Read version.txt
# --------------------------------------

file(READ "${CMAKE_SOURCE_DIR}/version.txt" APP_VERSION_STRING)

string(STRIP "${APP_VERSION_STRING}" APP_VERSION_STRING)

if(NOT APP_VERSION_STRING MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$")

    message(FATAL_ERROR "Invalid version.txt format. Expected: MAJOR.MINOR.PATCH")

endif()

set(APP_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(APP_VERSION_MINOR "${CMAKE_MATCH_2}")
set(APP_VERSION_PATCH "${CMAKE_MATCH_3}")

math(EXPR APP_VERSION_CODE
        "${APP_VERSION_MAJOR} * 10000 +
         ${APP_VERSION_MINOR} * 100 +
         ${APP_VERSION_PATCH}")

message(STATUS "Version: ${APP_VERSION_STRING} (code ${APP_VERSION_CODE})")

