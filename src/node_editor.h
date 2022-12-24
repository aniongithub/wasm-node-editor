#pragma once

#include <string>

void initializeNodeEditor();
void registerNodes(const char* json_data);

typedef void (*OnSaved)(Graph& graph, const char* json_graph);
typedef void (*OnNodeSelected)(Graph& graph, Node& node);
typedef void (*OnNodeDeselected)(Graph& graph, Node& node);
struct GraphCallbacks_t
{
    OnSaved onSaved;
    OnNodeDeselected onNodeSelected;
    OnNodeDeselected OnNodeDeselected;
};
typedef GraphCallbacks_t GraphCallbacks;
void editGraph(const char* id, const char* json_graph, GraphCallbacks GraphCallbacks);

void createNode(const Graph& graph, const char* node_id, const char* metadata);

bool renderNodeEditor();
void shutdownNodeEditor();