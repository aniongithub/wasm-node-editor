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
    _properties.insert({"name", new Property_t(this, "name", json({{"type", "string"}, {"editor", "name_editor"}}))});
    _properties["name"]->setData<std::string>(getNewName());

    auto x = _node_metadata.dump();
    ImVec2 pos(100.0f, 100.0f);
    pos.x = _node_metadata.value("pos_x", pos.x);
    pos.y = _node_metadata.value("pos_y", pos.y);
    ImNodes::SetNodeScreenSpacePos(renderId(), pos);
}

EditorResult Node_t::prepare()
{
    // Prepare all properties
    for (auto propIter = _properties.begin(); propIter != _properties.end(); propIter++)
    {
        auto result = propIter->second->prepare();
        if (result != RESULT_OK)
            return result;
    }
    
    return RESULT_OK;
}

EditorResult Node_t::renderProperties()
{
    auto node_name = std::string(_properties["name"]->getData<char>());
    ImVec2 cell_padding(5, 0);
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

    // TODO: Continue here...
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(name.c_str());
    ImNodes::EndNodeTitleBar();

    ImNodes::EndNode();

    // TODO: Get nextNodePos from properties when done    
    if (ImNodes::IsNodeSelected(_renderId))
        parent()->parent()->selectedNodes().push_back(this);

    return RESULT_OK;
}

EditorResult Node_t::shutdown()
{
    return RESULT_OK;
}