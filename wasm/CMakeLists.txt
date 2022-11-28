project (node-editor-wasm)
cmake_minimum_required(VERSION 3.1)

option(JS_ONLY "Compile to native JS (No WASM)" ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11")

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ../src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES}
	"$ENV{IMGUI_ROOT}/imgui.cpp"
	"$ENV{IMGUI_ROOT}/imgui_draw.cpp"
	"$ENV{IMGUI_ROOT}/imgui_widgets.cpp"
	"$ENV{IMGUI_ROOT}/imgui_tables.cpp"
	"$ENV{IMGUI_ROOT}/backends/imgui_impl_glfw.cpp"
	"$ENV{IMGUI_ROOT}/backends/imgui_impl_opengl3.cpp"
	"$ENV{IMNODES_ROOT}/imnodes.cpp")

target_include_directories(${PROJECT_NAME} 
PRIVATE 
	${OPENGL_INCLUDE_DIR}
	$ENV{IMGUI_ROOT}
	$ENV{IMNODES_ROOT}
	$ENV{JSON_ROOT}/include)

target_link_libraries(${PROJECT_NAME} 
PRIVATE 
	${OPENGL_LIBRARIES} 
	${GLFW3_LIBRARIES})

set(LINK_OPTIONS
	"-s WASM=0"
	"-s EXPORTED_FUNCTIONS='[_main]'"
	"-s USE_WEBGL2=1"
	"-s USE_GLFW=3"
	"-s FULL_ES3=1")

if(JS_ONLY)
	message(STATUS "Setting compilation target to native JavaScript")
	set(CMAKE_EXECUTABLE_SUFFIX ".js")
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s WASM=0 -s EXPORTED_FUNCTIONS='[_main]' -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1")
else(JS_ONLY)
	message(STATUS "Setting compilation target to WASM")
	set(CMAKE_EXECUTABLE_SUFFIX ".wasm.js")
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s WASM=0 -s EXPORTED_FUNCTIONS='[_main]' -s USE_WEBGL2=1 -s USE_GLFW=3 -s FULL_ES3=1")
endif(JS_ONLY)