project (node-editor-wasm)
cmake_minimum_required(VERSION 3.1)

add_subdirectory(native)

# set(CMAKE_TOOLCHAIN_FILE "cmake/emscripten.cmake")
# set(EMSCRIPTEN_ROOT_PATH "/usr/local/src/emsdk/upstream/emscripten")

# add_subdirectory(wasm)

# include(ExternalProject)
# ExternalProject_Add(node-editor-wasm
#     SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/wasm
#     CMAKE_ARGS
#     "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/emscripten.cmake"
#     "-DEMSCRIPTEN_ROOT_PATH=/usr/local/src/emsdk/upstream/emscripten"
#     "-Wno-deprecated"
#     BUILD_COMMAND $(MAKE) --silent
#     INSTALL_COMMAND "")