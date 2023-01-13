#include "node.h"

#include <imgui.h>
#include <imnodes.h>

#include <handles/graph.h>
#include <handles/editor.h>
#include <handles/property.h>
#include <utils/string_splitter.hpp>

std::string Node_t::getNewName()
{
    auto nameRoot = StringSplitter<'/'>(_typeId).lastToken();
    auto name_suffix = 1;
    
    static std::map<std::string, int> typeNameCounters;
    auto nameCounterIter = typeNameCounters.find(_typeId);
    if (nameCounterIter == typeNameCounters.end())
        typeNameCounters[_typeId] = name_suffix;
    else
        {
            nameCounterIter->second++;
            name_suffix = nameCounterIter->second;
        }

    return nameRoot.c_str() + std::to_string(name_suffix);
}

Node_t::Node_t(Graph parent, std::string typeId, json node_metadata):
    _parent(parent),
    _typeId(typeId),
    _renderId(ImGui::GetID(this)),
    _node_metadata(node_metadata)
{
    if (!parent)
        return;
    
    auto nameProperty = new Property_t(this, "name", json({{"type", "string"}, {"editor", "name_editor"}}));
    assert(nameProperty->prepare() == RESULT_OK);
    
    _properties.insert({"name", nameProperty});
    _properties["name"]->setData<std::string>(getNewName());

    ImVec2 pos(100.0f, 100.0f);
    pos.x = _node_metadata.value("pos_x", pos.x);
    pos.y = _node_metadata.value("pos_y", pos.y);
    ImNodes::SetNodeScreenSpacePos(renderId(), pos);
}

EditorResult Node_t::prepare()
{
    auto it = Editor_t::getNodesData().find(_typeId.c_str());
    if (it == Editor_t::getNodesData().end())
        return RESULT_INVALID_ARGS;

    auto nodeData = it.value();
    
    if (nodeData.contains("inputs"))
        {
            for (auto i = nodeData["inputs"].begin(); i != nodeData["inputs"].end(); i++)
            {
                auto port = new InputPort_t(this, i.key(), i.value());
                auto result = port->prepare();
                if (result != RESULT_OK)
                    return result;
                
                _inputs.insert({i.key(), port});
            }
        }
    
    if (nodeData.contains("outputs"))
    {
        for (auto o = nodeData["outputs"].begin(); o != nodeData["outputs"].end(); o++)
        {
            auto port = new OutputPort_t(this, o.key(), o.value());
            auto result = port->prepare();
            if (result != RESULT_OK)
                return result;
            
            _outputs.insert({o.key(), port});
        }
    }
    if (nodeData.contains("properties"))
    {
        for (auto p = nodeData["properties"].begin(); p != nodeData["properties"].end(); p++)
        {
            auto property = new Property_t(this, p.key(), p.value());
            auto result = property->prepare();
            if (result != RESULT_OK)
                return result;
            _properties.insert({p.key(), property});
        }
    }

    return RESULT_OK;
}

EditorResult Node_t::renderProperties()
{
    auto node_name = std::string(_properties["name"]->getData<char>());
    ImVec2 cell_padding(5, 3);
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

    return RESULT_OK;
}

EditorResult Node_t::render()
{
    ImNodes::BeginNode(renderId());

    std::string name(properties()["name"]->getData<char>());

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(name.c_str());
    ImNodes::EndNodeTitleBar();

    ImVec2 cell_padding(10, 3);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);
    if (ImGui::BeginTable(name.c_str(), 2))
    {
        ImGui::TableSetupColumn("inputs", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("outputs", ImGuiTableColumnFlags_WidthFixed);

        auto input_it = _inputs.begin();
        auto output_it = _outputs.begin();
        for (auto i = 0; i < std::max(_inputs.size(), _outputs.size()); i++)
        {
            auto input = input_it != _inputs.end() ? *input_it++: std::make_pair("", InputPort_t::empty());
            auto output = output_it != _outputs.end() ? *output_it++: std::make_pair("", OutputPort_t::empty());

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            input.second->render();
            
            ImGui::TableNextColumn();
            // Right-align - https://stackoverflow.com/a/58052701/802203
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::CalcTextSize(output.second->name().c_str()).x 
            - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
            output.second->render();
        }

        ImGui::EndTable();
    }
    ImGui::PopStyleVar();

    ImNodes::EndNode();

    if (ImNodes::IsNodeSelected(_renderId))
        if (parent()->focused())
            parent()->parent()->selectedNodes().push_back(this);
        else
            ImNodes::ClearNodeSelection(renderId());

    return RESULT_OK;
}

EditorResult Node_t::shutdown()
{
    return RESULT_OK;
}