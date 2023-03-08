#pragma once

#include <string>
#include <vector>

#include "../api.h"

#include <imgui.h>
#include <imnodes.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct Graph_t
{
    private:
        Editor _parent;
        std::string _id;
        std::string _json_graph_data;
        bool _allowClose;
        bool _open;
        bool _focused;
        bool _dirty;
        bool _popup;
        ImGuiWindowFlags _windowFlags;
        GraphCallbacks _callbacks;
        std::vector<Node> _nodes;

        ImNodesEditorContext* _editorCtx;

        EditorResult renderAddNodeMenu(json createNodeData, std::string currPath = "");
        EditorResult renderContents();

    public:
        Graph_t() = delete;
        Graph_t(const Graph_t&) = delete;
        Graph_t(Editor parent, std::string id, std::string json_graph_data, GraphCallbacks callbacks);

        EditorResult prepare();
        EditorResult render();
        EditorResult handleLinks();
        EditorResult shutdown();

        EditorResult createNode(std::string id, std::string json_node_metadata, Node* nodeHdl);

        virtual EditorResult onNodeCreated(void* context, const char* id, size_t idSizeBytes, const char* json_node_metadata, size_t json_node_medataSizeBytes, Node* nodeHdl);

        std::string id() { return _id; }
        Editor parent() { return _parent; }
        bool focused() { return _focused; }
        bool open() { return _open; }
};