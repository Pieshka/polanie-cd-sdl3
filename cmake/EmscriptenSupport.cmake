# --------------------------------------
# Emscripten Support
# --------------------------------------

# Borrowed from: https://github.com/isledecomp/isle-portable/blob/master/CMakeLists.txt

if(EMSCRIPTEN)
    add_compile_options(-pthread -gsource-map)
    add_link_options(-sUSE_WEBGL2=1 -sMIN_WEBGL_VERSION=2 -sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=128mb -sMAXIMUM_MEMORY=2gb -pthread -sPROXY_TO_PTHREAD=1 -sOFFSCREENCANVAS_SUPPORT=0 -sPTHREAD_POOL_SIZE_STRICT=0 -sFORCE_FILESYSTEM=1 -sWASMFS=1 -sEXIT_RUNTIME=1 -sABORT_ON_WASM_EXCEPTIONS=1 -sEXPORTED_RUNTIME_METHODS=addRunDependency,removeRunDependency -g2 -gsource-map)
    set(SDL_PTHREADS ON CACHE BOOL "Enable SDL pthreads" FORCE)
    find_program(LLVM_OBJCOPY_BIN NAMES llvm-objcopy HINTS "${EMSCRIPTEN_ROOT_PATH}/../bin" REQUIRED)
    set(POLANIE_EMSCRIPTEN_VERSION_DIR "${CMAKE_BINARY_DIR}/generated")

    # Resolve git HEAD to find file dependencies for change detection.
    # .git/HEAD changes on branch switch; the ref file it points to changes on commit.
    set(_git_head "${CMAKE_SOURCE_DIR}/.git/HEAD")
    set(_git_deps)
    if(EXISTS "${_git_head}")
        list(APPEND _git_deps "${_git_head}")
        file(READ "${_git_head}" _head_ref)
        string(STRIP "${_head_ref}" _head_ref)
        if(_head_ref MATCHES "^ref: (.+)$")
            set(_git_ref "${CMAKE_SOURCE_DIR}/.git/${CMAKE_MATCH_1}")
            if(EXISTS "${_git_ref}")
                list(APPEND _git_deps "${_git_ref}")
            endif()
        endif()
    endif()

    add_custom_command(
            OUTPUT ${POLANIE_EMSCRIPTEN_VERSION_DIR}/version.js ${POLANIE_EMSCRIPTEN_VERSION_DIR}/sourceMappingURL
            COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_SOURCE_DIR} -DOUTPUT_DIR=${POLANIE_EMSCRIPTEN_VERSION_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/EmscriptenVersion.cmake
            DEPENDS ${_git_deps}
            COMMENT "Generating emscripten version files"
    )
    add_custom_target(emscripten_version DEPENDS
            ${POLANIE_EMSCRIPTEN_VERSION_DIR}/version.js
            ${POLANIE_EMSCRIPTEN_VERSION_DIR}/sourceMappingURL
    )
endif()