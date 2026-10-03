# --------------------------------------
# Application sources
# --------------------------------------

set(GAME_SOURCES
		battle.cpp
		graphics.cpp
		image13h.cpp
		main.cpp
		mapa.cpp
		mover1.cpp
		world.cpp
)

add_executable(polanie
		${GAME_SOURCES}
		res/polanie.rc
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

# --------------------------------------
# Compile definitions
# --------------------------------------

target_compile_definitions(polanie PRIVATE
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

# --------------------------------------
# Platform-specific code
# --------------------------------------

if(EMSCRIPTEN)
	target_compile_definitions(polanie PRIVATE "POLANIE_EMSCRIPTEN_HOST=\"${POLANIE_EMSCRIPTEN_HOST}\"")
	add_dependencies(polanie emscripten_version)
	target_link_options(polanie PRIVATE
			--pre-js ${CMAKE_SOURCE_DIR}/compatlib/emscripten/stacktracelimit.js
			--post-js ${POLANIE_EMSCRIPTEN_VERSION_DIR}/version.js
	)
	set_property(TARGET polanie PROPERTY LINK_DEPENDS
			${CMAKE_SOURCE_DIR}/compatlib/emscripten/stacktracelimit.js
			${POLANIE_EMSCRIPTEN_VERSION_DIR}/version.js
	)
	add_custom_command(TARGET polanie POST_BUILD
			COMMAND ${LLVM_OBJCOPY_BIN} --remove-section=sourceMappingURL
				--add-section=sourceMappingURL=${POLANIE_EMSCRIPTEN_VERSION_DIR}/sourceMappingURL
				$<TARGET_FILE_DIR:polanie>/polanie.wasm
			COMMENT "Patching sourceMappingURL with build-time git hash"
	)
	set_property(TARGET polanie PROPERTY SUFFIX ".html")
endif()

# --------------------------------------
# Install definitions
# --------------------------------------

include(GNUInstallDirs)

install(
		TARGETS polanie
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	# Install C++ standard library and its dependencies
	get_filename_component(CLANG_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
	install(
			FILES
			"${CLANG_BIN_DIR}/libc++.dll"
			"${CLANG_BIN_DIR}/libunwind.dll"
			DESTINATION ${CMAKE_INSTALL_BINDIR}
	)

endif()

if(EMSCRIPTEN)
	install(FILES "$<TARGET_FILE_DIR:polanie>/polanie.js" "$<TARGET_FILE_DIR:polanie>/polanie.wasm"
			DESTINATION "${CMAKE_INSTALL_BINDIR}"
	)
endif()