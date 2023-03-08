#pragma once

#include <vector>
#include <map>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <api.h>

struct Editor_t
{
    private:
        EditorCallbacks_t _callbacks;
        EditorFlags _flags;
        
        static json nodes_data;
        static json createNode_data;

        std::map<std::string, Graph> _graphs;
        std::vector<Node> _selectedNodes;

        std::string _graphFilter;
        std::string _focusedWindow;

        EditorResult renderProperties();
        EditorResult renderNewGraphDialog(bool drawNewGraphPopup);
        
        EditorResult renderGraphTree(json nodeData, std::string currPath = "", std::string filter = "");
        EditorResult renderGraphWindow();

        static EditorResult graphNodeCreated(void* context, Graph graphHdl, const char* id, size_t idSizeBytes, const char* json_node_metadata, size_t json_node_medataSizeBytes, Node* nodeHdl);

        bool node_exists(std::string id);
    public:
        Editor_t() = delete;
        Editor_t(const Editor_t&) = delete;
        Editor_t(EditorCallbacks& callbacks, EditorFlags flags);

        EditorResult registerGraphs(std::string json_data);
        EditorResult render();

        EditorResult editGraph(std::string id, std::string json_graph_data, GraphCallbacks callbacks, Graph* graphHdl);
        EditorResult closeGraph(Graph graphHdl);

        std::vector<Node>& selectedNodes() { return _selectedNodes; }
        
        static const json& getCreateNodeData() { return createNode_data; }
        static const json& getNodesData() { return nodes_data; }
};