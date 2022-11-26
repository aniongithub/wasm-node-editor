#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#endif

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imnodes.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "node_editor.h"

GLFWwindow *g_window;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
bool show_demo_window = true;
bool show_another_window = false;
int g_width;
int g_height;

#ifdef __EMSCRIPTEN__

EM_JS(int, canvas_get_width, (), {
    return Module.canvas.width;
});

EM_JS(int, canvas_get_height, (), {
    return Module.canvas.height;
});

EM_JS(void, resize_canvas, (), {
    js_resizeCanvas();
});

#else

void on_size_changed();

int canvas_get_width()
{
    int width, height;
    glfwGetWindowSize(g_window, &width, &height);
    return width;
}

int canvas_get_height()
{
    int width, height;
    glfwGetWindowSize(g_window, &width, &height);
    return height;
}

void resize_canvas()
{
    on_size_changed();
}

#endif

void glfw_error_callback(int error, const char* description)
{
    printf("GLFW error: %d - %s\n", error, description);
}

void on_size_changed()
{
    glfwSetWindowSize(g_window, g_width, g_height);

    ImGui::SetCurrentContext(ImGui::GetCurrentContext());
}

void loop()
{
    int width = canvas_get_width();
    int height = canvas_get_height();

    if (width != g_width || height != g_height)
    {
        g_width = width;
        g_height = height;
        on_size_changed();
    }

    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    renderNodeEditor();
    ImGui::Render();

    int display_w, display_h;
    glfwMakeContextCurrent(g_window);
    glfwGetFramebufferSize(g_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwMakeContextCurrent(g_window);
}

int init_gl()
{
    // glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    // glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // Open a window and create its OpenGL context
    g_width = 800;
    g_height = 600;
    g_window = glfwCreateWindow(g_width, g_height, "Node editor", NULL, NULL);
    if (g_window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(g_window); // Initialize GLEW

    return 0;
}

int init_imgui()
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    initializeNodeEditor();
    registerNodes(R"(
    {
        "time": {
            "outputs": {
                "time_sec": "float"
            }
        },
        "math/sine": {
            "inputs": {
                "baseline": "float",
                "phase": "float",
                "amplitude": "float",
                "frequencyHz": "float",
                "x": "float"
            },
            "outputs": {
                "y": "float"
            }
        },
        "math/add": {
            "inputs": {
                "a": "float",
                "b": "float"
            },
            "outputs": {
                "result": "float"
            }
        },
        "math/subtract": {
            "inputs": {
                "a": "float",
                "b": "float"
            },
            "outputs": {
                "result": "float"
            }
        },
        "math/multiply": {
            "inputs": {
                "a": "float",
                "b": "float"
            },
            "outputs": {
                "result": "float"
            }
        }
    }
    )");


    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init();

    // Setup style
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();

    resize_canvas();

    return 0;
}

int init()
{
    init_gl();
    init_imgui();
    return 0;
}

void quit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    shutdownNodeEditor();
    glfwTerminate();
}

extern "C" int main(int argc, char **argv)
{
    if (init() != 0)
        return 1;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(loop, 0, 1);
#else
    while (!glfwWindowShouldClose(g_window))
    {
        loop();
        glfwSwapBuffers(g_window);
    }
#endif

    quit();
    return 0;
}