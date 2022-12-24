#include "graph.h"

#include <imnodes.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "id_generator.h"
#include "utils.h"
#include "node_editor.h"

void Graph::registerNodes(std::string json_node_data)
{
    auto stream = std::stringstream(json_node_data);
    auto nodes = json::parse(stream);
    for (auto it = nodes.begin(); it != nodes.end(); it++)
    {
        nodes_data[it.key()] = it.value();

        // collapse node ids into a category tree
        std::stringstream id(it.key());
        std::string id_segment;
        json* curr = &createNodeMenu_data;
        
        while (std::getline(id, id_segment, '/'))
        {
            if (!curr->contains(id_segment))
                (*curr)[id_segment] = {};
            curr = &((*curr)[id_segment]);
        }
        *curr = it.key();
    }    
}

Graph::Graph(std::string id, std::string json_graph):
    Window(id),
    _id(id)
{
    static ImNodesContext* imnodes_ctx = ImNodes::CreateContext();
    ImNodes::SetNodeGridSpacePos(1, ImVec2(200.0f, 200.0f));
}

void Graph::renderCreateNodeMenu(json category_data)
{
    for (auto it = category_data.begin(); it != category_data.end(); it++)
    {
        if (it.value().is_string())
            if (ImGui::Selectable(it.key().c_str()))
                createNode(*this, it.value().get<std::string>().c_str());
        
        if (!it.value().is_string())
        {
            if (ImGui::BeginMenu(it.key().c_str()))
            {
                renderCreateNodeMenu(it.value());
                ImGui::EndMenu();
            }
        }
    }
}

void Graph::renderContents()
{
    ImNodes::BeginNodeEditor();

    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_TopRight);
    ImNodes::EndNodeEditor();
}