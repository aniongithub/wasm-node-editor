#pragma once

#include <vector>

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

        std::vector<Graph> _graphs;
        std::vector<Node> _selectedNodes;

        EditorResult renderProperties();
        EditorResult renderMainMenu();
    public:
        Editor_t() = delete;
        Editor_t(const Editor_t&) = delete;
        Editor_t(EditorCallbacks& callbacks, EditorFlags flags);

        EditorResult registerNodes(std::string json_data);
        EditorResult render();

        EditorResult editGraph(std::string id, std::string json_graph_data, GraphCallbacks callbacks, Graph* graphHdl);
        EditorResult closeGraph(Graph graphHdl);

        std::vector<Node>& selectedNodes() { return _selectedNodes; }
        
        static const json& getCreateNodeData() { return createNode_data; }
        static const json& getNodesData() { return nodes_data; }
};