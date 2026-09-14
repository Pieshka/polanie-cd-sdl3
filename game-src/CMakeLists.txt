set(GAME_SOURCES
	BATTLE.CPP
	GRAPHICS.CPP
	IMAGE13H.CPP
	LADOWANI.CPP
	MAIN.CPP
	MAPA.CPP
	MOUSE.CPP
	MOVER1.CPP
	WORLD.CPP
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