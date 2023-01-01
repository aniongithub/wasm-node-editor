#pragma once

#include <memory>
#include <map>
#include <imgui.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <api.h>
#include <handles/property.h>
#include <handles/port.h>

struct Node_t
{
    private:
        Graph _parent;
        ImGuiID _renderId;
        std::string _typeId;
        bool _selected;
        json _node_metadata;

        std::map<std::string, Property> _properties;
        std::map<std::string, InputPort> _inputs;
        std::map<std::string, OutputPort> _outputs;

        std::string getNewName();
    public:
        Node_t() = delete;
        Node_t(const Node_t&) = delete;
        Node_t(Graph parent, std::string typeId, json node_metadata);

        EditorResult prepare();
        
        EditorResult renderProperties();
        EditorResult render();

        EditorResult shutdown();

        const ImGuiID renderId() { return _renderId; }
        static Node empty()
        {
            static auto _empty = std::make_unique<Node_t>(nullptr, "", json::parse(R"({})"));
            return _empty.get();
        }

        Graph parent() { return _parent; }
        std::map<std::string, Property>& properties() { return _properties; }
        bool selected() { return _selected; }
};