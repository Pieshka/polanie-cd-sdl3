# --------------------------------------
# Library sources
# --------------------------------------

set(COMPATLIB_SOURCES
	src/flic/flic_entry.cpp
	src/sound/menegdma_entry.cpp
	src/dos/i86_entry.c
	src/dos/conio_entry.c
	src/sound/cd_entry.c
	src/io/timer.c
	src/io/mouse.c
	src/vga/sdl.c
	src/misc/preinit.c
		src/misc/fopen.c
)

set(COMPATLIB_HEADERS
	include/i86.h
	include/bios.h
	include/cd.h
	include/graph.h
	include/menegdma.h
	include/playfli.h
	include/dos.h
	include/conio.h
	include/constants.h
	include/mem.h
	include/process.h
)

add_library(compatlib STATIC
	${COMPATLIB_SOURCES}
	${COMPATLIB_HEADERS}
)

target_include_directories(compatlib
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_SOURCE_DIR}/3rdparty/flic/
)

target_link_libraries(compatlib
	PRIVATE
		SDL3::SDL3
		SDL3_mixer::SDL3_mixer
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