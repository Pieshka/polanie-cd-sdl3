set(GAME_SOURCES
	battle.cpp
	graphics.cpp
	image13h.cpp
	ladowani.cpp
	main.cpp
	mapa.cpp
	mouse.cpp
	mover1.cpp
	world.cpp
)

add_executable(polanie
	${GAME_SOURCES}
)

target_include_directories(polanie
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/compatlib/include
)

target_link_libraries(polanie
	PRIVATE
		compatlib
)

if(WIN32)

	target_sources(polanie PRIVATE
			${CMAKE_SOURCE_DIR}/resources/windows/version-game.rc
	)

endif()

target_compile_definitions(polanie PRIVATE
		APP_VERSION_STRING="${APP_VERSION_STRING}"
		APP_VERSION_MAJOR=${APP_VERSION_MAJOR}
		APP_VERSION_MINOR=${APP_VERSION_MINOR}
		APP_VERSION_PATCH=${APP_VERSION_PATCH}
)

if(UNIX AND NOT APPLE)
	install(
			FILES "${CMAKE_SOURCE_DIR}/resources/common/icon-game.bmp"
			DESTINATION .
			RENAME icon.bmp
	)
endif()

# (Open)Watcom assumes char = unsigned char
# Polanie also assumes that, so -funsigned-char \ /J
if(MSVC)
	target_compile_options(polanie PRIVATE
			/W4
			/J
	)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(polanie PRIVATE
			-Wall
			-Wextra
			-funsigned-char
			-fno-strict-aliasing
			-fbounds-check
	)
endif()