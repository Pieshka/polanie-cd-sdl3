set(COMPATLIB_SOURCES 
	src/dummy.cpp
)

set(COMPATLIB_HEADERS
	include/dummy.h
)

add_library(compatlib STATIC
	${COMPATLIB_SOURCES}
	${COMPATLIB_HEADERS}
)

target_include_directories(compatlib
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(compatlib
	PRIVATE
		SDL3::SDL3
		SDL3_mixer::SDL3_mixer
)

set_target_properties(compatlib PROPERTIES
	OUTPUT_NAME "compat"
)