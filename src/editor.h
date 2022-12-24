#pragma once

enum EditorResult_t {

};
typedef enum EditorResult_t EditorResult;

struct Editor_t;
typedef struct Editor_t Editor;

struct Graph_t;
typedef struct Graph_t* Graph;

struct Node_t;
typedef struct Node_t* Node;

struct InputPort_t;
typedef struct InputPort_t* InputPort;

struct OutputPort_t;
typedef struct OutputPort_t* OutputPort;

typedef EditorResult (*OnOpenGraph)(Editor editorHdl, const char* id);
typedef EditorResult (*OnCloseGraph)(Editor editorHdl, const char* id, Graph graph);

struct EditorCallbacks_t
{
    OnOpenGraph openGraph;
    OnCloseGraph closeGraph;
};
typedef struct EditorCallbacks_t EditorCallbacks;

struct Link_t;
typedef struct Link_t* Link;
EditorResult getLinkInfo(Link linkHdl, Node* startNodeHdl, OutputPort* startPortHdl, Node* endNodeHdl, InputPort* endPortHdl);

typedef EditorResult (*OnNodeCreated)(Graph graphHdl, const char* json_node_metadata, Node* nodeHdl);
typedef EditorResult (*OnNodeDeleted)(Graph graphHdl, Node nodeHdl);
typedef EditorResult (*OnLinkCreated)(Graph graphHdl, const char* json_link_metadata, Link* linkHdl);
typedef EditorResult (*OnLinkDeleted)(Graph graphHdl, Link linkHdl);

struct GraphCallbacks_t
{
    OnNodeCreated nodeCreated;
    OnNodeDeleted nodeDeleted;
    OnLinkCreated linkCreated;
    OnLinkDeleted linkDeleted;
};
typedef GraphCallbacks_t GraphCallbacks;
EditorResult createGraph(Editor editorHdl, const char* id, const char* json_graph_data, GraphCallbacks callbacks, Graph* graphHdl);
EditorResult destroyGraph(Editor editorHdl, Graph graphHdl);

EditorResult initializeEditor(EditorCallbacks callbacks, Editor* editorHdl);
EditorResult registerNodes(Editor editorHdl, const char* json_data);
EditorResult renderEditor(Editor editorHdl);
EditorResult shutdownEditor(Editor editorHdl);