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
#include <handles/graph.h>
#include <handles/node.h>

GLFWwindow *g_window;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
int g_width;
int g_height;


Editor editor;
std::map<std::string, Graph> graphs;

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

    #ifdef __EMSCRIPTEN__
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
    editorCallbacks.openGraph = [](void* context, Editor editorHdl, const char* id, size_t idSizeBytes) -> EditorResult 
    {
        // TODO: Execute the commands returned by the JS callback here in C++
        #ifdef __EMSCRIPTEN__
        char* str = (char*)EM_ASM_PTR(
            {
                console.log("OpenGraph js callback invoked!");
                var jsStr = onOpenGraph(UTF8ToString($0));
                var lengthBytes = lengthBytesUTF8(jsStr) + 1;
                var stringOnWasmHeap = _malloc(lengthBytes);
                stringToUTF8(jsStr, stringOnWasmHeap, lengthBytes);
                return stringOnWasmHeap;
            }, id);
        printf("string result from openGraph callback: %s\n", str);
        free(str);
        #endif

        return RESULT_OK;
    };
    editorCallbacks.closeGraph = [](void* context, Editor editorHdl, const char* id, size_t idSizeBytes, Graph graph) -> EditorResult
    {
        #ifdef __EMSCRIPTEN__
        EM_ASM(
            {
                onCloseGraph(UTF8ToString($0));
            }, id);
        #endif
        return RESULT_OK;
    };
    auto result = initializeEditor(editorCallbacks, EDITORFLAGS_NONE, &editor);

    auto nodes_data = R"(
    {
        "time": {
            "outputs": {
                "time_sec": "float"
            }
        },
        "functions/sine": {
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
        "functions/square": {
            "inputs": {
                "baseline": "float",
                "phase": "float",
                "amplitude": "float",
                "frequencyHz": "float",
                "dutyCycle": "float",
                "x": "float"
            },
            "outputs": {
                "y": "float"
            }
        },
        "functions/sawtooth": {
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
        "functions/triangle": {
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
        "math/const_float": {
            "outputs": {
                "value": "float"
            },
            "properties": {
                "value": "float"
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
    )";
    result = registerNodes(editor, nodes_data, strlen(nodes_data));
    // TODO: Continue here
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
    Graph graph;
    result = editGraph(editor, "misc/foo", strlen("misc/foo"), nullptr, 0, graphCallbacks, &graph);
    graphs.insert({"misc/foo", graph});

    result = editGraph(editor, "misc/bar", strlen("misc/bar"), nullptr, 0, graphCallbacks, &graph);
    graphs.insert({"misc/bar", graph});

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