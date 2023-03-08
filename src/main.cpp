#include <stdio.h>
#include <map>
#include <sstream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GLFW_INCLUDE_ES3
#include <GLES3/gl3.h>
#endif

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <imnodes.h>

#include <api.h>
#include <callbacks.h>
#include <handles/editor.h>
#include <handles/graph.h>
#include <handles/node.h>

GLFWwindow *g_window;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
int g_width;
int g_height;


Editor editor;

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

    auto result = renderEditor(editor);

    #ifdef __EMSCRIPTEN__
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantSaveIniSettings)
    {
        EM_ASM(
            {
                localStorage.setItem("imgui_settings", UTF8ToString($0));
            }, 
            ImGui::SaveIniSettingsToMemory());
    }
    #endif

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
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

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

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    #ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    char* imgui_settings = (char*)EM_ASM_PTR(
        {
            if ("imgui_settings" in localStorage)
            {
                var settings_jsString = localStorage.getItem("imgui_settings");
                var lengthBytes = lengthBytesUTF8(settings_jsString) + 1;
                var stringOnWasmHeap = _malloc(lengthBytes);
                stringToUTF8(settings_jsString, stringOnWasmHeap, lengthBytes);
                return stringOnWasmHeap;
            }
        });
    ImGui::LoadIniSettingsFromMemory(imgui_settings, strlen(imgui_settings));
    free(imgui_settings);
    #endif

    // Setup style
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init();

    resize_canvas();
    
    return 0;
}

int init()
{
    init_gl();
    init_imgui();

    EditorCallbacks editorCallbacks = {};
    editorCallbacks.initialize = editorInitialize;
    editorCallbacks.shutdown = editorShutdown;
    editorCallbacks.openGraph = editorOpenGraph;
    editorCallbacks.closeGraph = editorCloseGraph;
    auto result = initializeEditor(editorCallbacks, EDITORFLAGS_NONE, &editor);

    #ifndef __EMSCRIPTEN__
    #endif    

    GraphCallbacks graphCallbacks = {};
    graphCallbacks.nodeCreated = [](void* context, Graph graphHdl, const char* id, size_t idSizeBytes, const char* json_node_metadata, size_t json_node_medataSizeBytes, Node* nodeHdl) -> EditorResult
    {
        auto result = createNode(graphHdl, id, idSizeBytes, json_node_metadata, json_node_medataSizeBytes, nodeHdl);
        #ifdef __EMSCRIPTEN__
        EM_ASM(
            {
                onNodeCreated(UTF8ToString($0), UTF8ToString($1), $2);
            }, graphHdl->id().c_str(), id, (*nodeHdl)->renderId());

        #endif
        return result;
    };

    return 0;
}

void quit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

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