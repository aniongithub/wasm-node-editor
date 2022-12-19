#include "node.h"

#include <map>
#include <imgui.h>
#include <imnodes.h>

#include "utils.h"
#include "id_generator.h"
#include "factory.hpp"

Port::Port(std::shared_ptr<Node> parent, std::string name, std::string type):
    _parent(parent),
    _name(name),
    _type(type)
{
    ENABLE_SHARED_FROM_THIS_IN_CTOR(Port);
    name_to_id::instance().getId(generate_uuid_v4(), _render_id);
}


void InputPort::render()
{
    if (_parent == Node::empty())
        return;

    ImNodes::BeginInputAttribute(_render_id);
    ImGui::TextUnformatted(_name.data());
    ImNodes::EndInputAttribute();
}

void OutputPort::render()
{
    if (_parent == Node::empty())
        return;

    ImNodes::BeginOutputAttribute(_render_id);
    ImGui::TextUnformatted(_name.data());
    ImNodes::EndOutputAttribute();
}

Property::Property(std::shared_ptr<Node> parent, std::string name, json type):
    _parent(parent),
    _name(name),
    _type(type)
{  
    ENABLE_SHARED_FROM_THIS_IN_CTOR(Property);

    // Deal with simple typenames
    if (_type.is_string())
    {
        auto type = _type.get<std::string>();
        _type = json({ {"type", type} });
    }
    
    auto type_name = _type.value("type", "unknown");
    auto editor_name = _type.value("editor", type_name);
    _editor = CREATE_PROPERTYEDITOR(editor_name, shared_from_this());
}

std::string Property::fullname()
{
    return _parent->name() + "." + _name;
}

void Property::render()
{
    if (_parent == Node::empty())
        return;

    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(_name.c_str());
    
    ImGui::TableNextColumn();
    _editor->render();
}

template <typename T>
void Property::setData(T& value)
{
    allocateData(sizeof(value));
    memcpy(getData<T>(), &value, sizeof(value));
}

template <>
void Property::setData(std::string& value)
{
    allocateData(max(value.size(), dataSizeBytes()));
    memcpy(getData<char>(), &value[0], value.size());
}


std::string PropertyEditor::fullname()
{
    return "##" + _parent->fullname() + "_editor";
}

Node::Node(std::string id, json node_metadata):
    _id(id),
    _node_metadata(node_metadata)
{
    ENABLE_SHARED_FROM_THIS_IN_CTOR(Node);

    // Don't attempt to initialize when creating an empty node
    if (id == "")
        return;

    name_to_id::instance().getId(generate_uuid_v4(), _render_id);

    auto idParts = StringSplitter<'/'>(_id).allTokens();
    auto nameRoot = idParts[idParts.size() - 1];

    auto name_suffix = 1;
    
    static std::map<std::string, int> typeNameCounters;
    auto nameCounterIter = typeNameCounters.find(_id);
    if (nameCounterIter == typeNameCounters.end())
        typeNameCounters[_id] = name_suffix;
    else
        {
            nameCounterIter->second++;
            name_suffix = nameCounterIter->second;
        }
    
    // Add the "name" property
    _properties.insert({"name", std::make_shared<Property>(shared_from_this(), "name", json({{"type", "string"}, {"editor", "name_editor"}}))});
    auto name = nameRoot + std::to_string(name_suffix);
    _properties["name"]->setData<std::string>(name);
    
    if (_node_metadata.contains("inputs"))
    {
        for (auto i = _node_metadata["inputs"].begin(); i != _node_metadata["inputs"].end(); i++)
            _inputs.insert({i.key(), std::make_shared<InputPort>(shared_from_this(), i.key(), i.value())});
    }
    if (_node_metadata.contains("outputs"))
    {
        for (auto o = _node_metadata["outputs"].begin(); o != _node_metadata["outputs"].end(); o++)
            _outputs.insert({o.key(), std::make_shared<OutputPort>(shared_from_this(), o.key(), o.value())});
    }
    
    if (_node_metadata.contains("properties"))
    {
        for (auto o = _node_metadata["properties"].begin(); o != _node_metadata["properties"].end(); o++)
            _properties.insert({o.key(), std::make_shared<Property>(shared_from_this(), o.key(), o.value())});
    }
}

std::string Node::name()
{
    return std::string(_properties["name"]->getData<char>());
}

void Node::render()
{
    ImNodes::BeginNode(_render_id);

    ImNodes::BeginNodeTitleBar();

    auto node_name = name();
    ImGui::TextUnformatted(node_name.c_str());
    ImNodes::EndNodeTitleBar();

    ImVec2 cell_padding(10, 3);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);
    if (ImGui::BeginTable(node_name.c_str(), 2))
    {
        ImGui::TableSetupColumn("inputs", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("outputs", ImGuiTableColumnFlags_WidthFixed);

        auto input_it = _inputs.begin();
        auto output_it = _outputs.begin();
        for (auto i = 0; i < max(_inputs.size(), _outputs.size()); i++)
        {
            auto input = input_it != _inputs.end() ? *input_it++: std::make_pair("", InputPort::empty());
            auto output = output_it != _outputs.end() ? *output_it++: std::make_pair("", OutputPort::empty());

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            input.second->render();
            
            ImGui::TableNextColumn();
            output.second->render();
        }

        ImGui::EndTable();
    }
    ImGui::PopStyleVar();

    ImNodes::EndNode();
}

void Node::renderProperties()
{
    auto node_name = name();
    ImVec2 cell_padding(5, 5);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);
    if (ImGui::BeginTable(node_name.c_str(), 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_Resizable))
    {
        ImGui::TableSetupColumn("name");
        ImGui::TableSetupColumn("value");
        ImGui::TableHeadersRow();

        for (auto& property: _properties)
            property.second->render();

        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}