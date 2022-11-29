#include "node.h"

#include <map>

#include "utils.h"
#include "id_generator.h"

#include <imgui.h>
#include <imnodes.h>

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
}

void Node::render()
{
    ImNodes::BeginNode(_id_int);

    ImNodes::BeginNodeTitleBar();

    ImGui::TextUnformatted(_name.c_str());
    ImNodes::EndNodeTitleBar();

    auto input_it = _inputs.begin();
    auto output_it = _outputs.begin();
    for (auto i = 0; i < max(_inputs.size(), _outputs.size()); i++)
    {
        auto input = input_it != _inputs.end() ? *input_it: InputPort::empty();
        auto output = output_it != _outputs.end() ? *output_it: OutputPort::empty();
        
        ImGui::Unindent(_output_ports_offset);
        input.render();
        
        ImGui::Indent(_output_ports_offset);
        output.render();

        input_it++;
        output_it++;
    }

    ImNodes::EndNode();
}
