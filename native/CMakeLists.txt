project (node-editor-native)
cmake_minimum_required(VERSION 3.1)

set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED CONFIG)

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
	${CMAKE_DL_LIBS}
	${OPENGL_LIBRARIES} 
	OpenGL::GL
	OpenGL::GLX
	glfw)
