#include "node.h"

#include <map>
#include <imgui.h>
#include <imnodes.h>

#include "utils.h"
#include "id_generator.h"
#include "factory.hpp"
#include "property_editor.h"

Port::Port(Node& parent, std::string name, std::string type):
    _parent(parent),
    _name(name),
    _type(type)
{
    name_to_id::instance().getId(_parent.name() + "." + _name, _id_int);
}

void InputPort::render()
{
    if (&_parent == &Node::empty())
        return;

    ImNodes::BeginInputAttribute(_id_int);
    ImGui::TextUnformatted(_name.c_str());
    ImNodes::EndInputAttribute();
}

void OutputPort::render()
{
    if (&_parent == &Node::empty())
        return;

    ImNodes::BeginOutputAttribute(_id_int);
    ImGui::TextUnformatted(_name.c_str());
    ImNodes::EndOutputAttribute();
}

Property::Property(Node& parent, std::string name, json type):
    _parent(parent),
    _name(name),
    _type(type)
{
    name_to_id::instance().getId(_parent.name() + "." + _name, _id_int);
    if (type.is_string())
        _editor = Factory<PropertyEditor>::instance().create(type.get<std::string>());
    else
    {
        assert(type.is_object());
        assert(type.contains("type"));
        auto type_name = type["type"].get<std::string>();
        auto editor_name = type.contains("editor") ? type["editor"].get<std::string>() : type_name;
        _editor = Factory<PropertyEditor>::instance().create(editor_name);
    }
}

void Property::render()
{
    if (&_parent == &Node::empty())
        return;

    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(_name.c_str());
    
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(_type.dump(4).c_str());
}

Node::Node(std::string id, json node_metadata):
    _id(id),
    _node_metadata(node_metadata)
{
    static std::map<std::string, int> typeNameCounters;

    auto idParts = StringSplitter<'/'>(_id).allTokens();

    auto name_suffix = 1;
    _name = idParts[idParts.size() - 1];
    auto nameCounterIter = typeNameCounters.find(_id);
    if (nameCounterIter == typeNameCounters.end())
        typeNameCounters[_id] = name_suffix;
    else
        {
            nameCounterIter->second++;
            name_suffix = nameCounterIter->second;
        }
    _name += std::to_string(name_suffix);

    name_to_id::instance().getId(_name, _id_int);
    _output_ports_offset = 0;
    if (node_metadata.contains("inputs"))
    {
        for (auto i = node_metadata["inputs"].begin(); i != node_metadata["inputs"].end(); i++)
        {
            _inputs.push_back(InputPort(*this, i.key(), i.value()));
            _output_ports_offset = max(_output_ports_offset, ImGui::CalcTextSize(i.key().c_str()).x);
        }
    }
    if (node_metadata.contains("outputs"))
    {
        for (auto o = node_metadata["outputs"].begin(); o != node_metadata["outputs"].end(); o++)
            _outputs.push_back(OutputPort(*this, o.key(), o.value()));
    }
    if (node_metadata.contains("properties"))
    {
        for (auto o = node_metadata["properties"].begin(); o != node_metadata["properties"].end(); o++)
            _properties.push_back(Property(*this, o.key(), o.value()));
    }
}

void Node::render()
{
    ImNodes::BeginNode(_id_int);

    ImNodes::BeginNodeTitleBar();

    ImGui::TextUnformatted(_name.c_str());
    ImNodes::EndNodeTitleBar();

    ImVec2 cell_padding(10, 3);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);
    if (ImGui::BeginTable(_name.c_str(), 2))
    {
        ImGui::TableSetupColumn("inputs", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("outputs", ImGuiTableColumnFlags_WidthFixed);

        auto input_it = _inputs.begin();
        auto output_it = _outputs.begin();
        for (auto i = 0; i < max(_inputs.size(), _outputs.size()); i++)
        {
            auto input = input_it != _inputs.end() ? *input_it++: InputPort::empty();
            auto output = output_it != _outputs.end() ? *output_it++: OutputPort::empty();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            input.render();
            
            ImGui::TableNextColumn();
            output.render();
        }

        ImGui::EndTable();
    }
    ImGui::PopStyleVar();

    ImNodes::EndNode();
}

void Node::renderProperties()
{
    ImVec2 cell_padding(10, 3);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);
    if (ImGui::BeginTable(_name.c_str(), 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable))
    {
        ImGui::TableSetupColumn("name");
        ImGui::TableSetupColumn("value");
        ImGui::TableHeadersRow();

        for (auto& property: _properties)
            property.render();

        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}