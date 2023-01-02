#pragma once

#include <string>
#include <vector>

#include "../api.h"

#include <imgui.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct Graph_t
{
    private:
        Editor _parent;
        std::string _id;
        std::string _json_graph_data;
        bool _allowClose;
        bool _closed;
        bool _popup;
        ImGuiWindowFlags _windowFlags;
        GraphCallbacks _callbacks;
        
        EditorResult renderAddNodeMenu(json createNodeData);
        EditorResult renderContents();

        std::vector<Node> _nodes;
    public:
        Graph_t() = delete;
        Graph_t(const Graph_t&) = delete;
        Graph_t(Editor parent, std::string id, std::string json_graph_data, GraphCallbacks callbacks);

        EditorResult prepare();
        EditorResult render();
        EditorResult handleLinks();
        EditorResult shutdown();

        EditorResult createNode(std::string id, std::string json_node_metadata, Node* nodeHdl);

        Editor parent() { return _parent; }
};