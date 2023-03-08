#include <api.h>
#include <string.h>

EditorResult editorInitialize(void* context, Editor editorHdl)
{
   auto nodes_data = R"(
    {
        "foo/bar": {
            "editable": true
        },
        "foo/baz": {
            "editable": true
        },
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

    auto result = registerGraphs(editorHdl, nodes_data, strlen(nodes_data));
    return result;
}

EditorResult editorShutdown(void* context, Editor editorHdl)
{
    return RESULT_OK;
}

EditorResult editorOpenGraph(void* context, Editor editorHdl, const char* id, size_t idSizeBytes)
{
    return RESULT_OK;
}

EditorResult editorCloseGraph(void* context, Editor editorHdl, const char* id, size_t idSizeBytes, Graph graph)
{
    return RESULT_OK;
}

EditorResult graphCreateNode(void* context, Graph graphHdl, const char* id, size_t idSizeBytes, const char* json_node_metadata, size_t json_node_medataSizeBytes, Node* nodeHdl)
{
    return RESULT_OK;
}