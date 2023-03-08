#pragma once

#include <stddef.h>

#ifdef __EMSCRIPTEN__

#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

#endif

enum EditorResult_t {
    RESULT_UNKNOWN_TYPE = -5,
    RESULT_NOT_SUPPORTED = -4,
    RESULT_NOT_IMPLEMENTED = -3,
    RESULT_INVALID_ARGS = -2,
    RESULT_UNKNOWN_ERROR = -1,
    RESULT_OK = 0
};
typedef enum EditorResult_t EditorResult;

enum EditorFlags_t {
    EDITORFLAGS_NONE = 0,
    EDITORFLAGS_EDIT_SUBGRAPHS = 1 << 0,
    EDITORFLAGS_MULTICAST_OUTPUTS = 1 << 1,
    EDITORFLAGS_MULTICAST_INPUTS = 1 << 2
};
typedef enum EditorFlags_t EditorFlags;

struct Editor_t;
typedef struct Editor_t* Editor;

struct Graph_t;
typedef struct Graph_t* Graph;

struct Node_t;
typedef struct Node_t* Node;

struct InputPort_t;
typedef struct InputPort_t* InputPort;

struct OutputPort_t;
typedef struct OutputPort_t* OutputPort;

typedef EditorResult (*OnEditorInitialize)(void* context, Editor editorHdl);
typedef EditorResult (*OnEditorOpenGraph)(void* context, Editor editorHdl, const char* id, size_t idSizeBytes);
typedef EditorResult (*OnEditorCloseGraph)(void* context, Editor editorHdl, const char* id, size_t idSizeBytes, Graph graph);
typedef EditorResult (*OnEditorShutdown)(void* context, Editor editorHdl);

struct EditorCallbacks_t
{
    void* context;
    OnEditorInitialize initialize;
    OnEditorShutdown shutdown;
    OnEditorOpenGraph openGraph;
    OnEditorCloseGraph closeGraph;
};
typedef struct EditorCallbacks_t EditorCallbacks;

struct Link_t;
typedef struct Link_t* Link;
EditorResult getLinkInfo(Link linkHdl, Node* startNodeHdl, OutputPort* startPortHdl, Node* endNodeHdl, InputPort* endPortHdl);

typedef EditorResult (*OnGraphClosed)(void* context, Graph graphHdl);
typedef EditorResult (*OnGraphNodeCreated)(void* context, Graph graphHdl, const char* id, size_t idSizeBytes, const char* json_node_metadata, size_t json_node_medataSizeBytes, Node* nodeHdl);
typedef EditorResult (*OnGraphNodeDeleted)(void* context, Graph graphHdl, Node nodeHdl);
typedef EditorResult (*OnGraphLinkCreated)(void* context, Graph graphHdl, const char* json_link_metadata, size_t json_link_metadataSizeBytes, Link* linkHdl);
typedef EditorResult (*OnGraphLinkDeleted)(void* context, Graph graphHdl, Link linkHdl);

struct GraphCallbacks_t
{
    void* context;
    OnGraphNodeCreated nodeCreated;
    OnGraphNodeDeleted nodeDeleted;
    OnGraphLinkCreated linkCreated;
    OnGraphLinkDeleted linkDeleted;
};
typedef GraphCallbacks_t GraphCallbacks;
// TODO: This should be renamed to openGraph
EditorResult editGraph(Editor editorHdl, const char* id, size_t idSizeBytes, const char* json_graph_data, size_t json_graph_dataSizeBytes, GraphCallbacks callbacks, Graph* graphHdl);
EditorResult closeGraph(Editor editorHdl, Graph graphHdl);

EditorResult createNode(Graph graphHdl, const char* id, size_t idSizeBytes, const char* json_node_metadata, size_t json_node_medataSizeBytes, Node* nodeHdl);
EditorResult deleteNode(Graph graphHdl, Node node);

EditorResult initializeEditor(EditorCallbacks callbacks, EditorFlags flags, Editor* editorHdl);
EditorResult registerGraphs(Editor editorHdl, const char* json_data, size_t json_dataSizeBytes);
EditorResult renderEditor(Editor editorHdl);
EditorResult shutdownEditor(Editor editorHdl);