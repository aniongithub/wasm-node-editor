#include "node_editor.h"

#include <vector>
#include <string>
#include <string_view>
#include <map>

#include <imnodes.h>
#include <imgui.h>
#include <sstream>
#include <iostream>
#include <regex>
#include <iterator>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "id_generator.h"
#include "utils.h"
#include "node.h"

#define INPUT_OUTPUT_SPACING 25

int id = 1;
bool popup = false;
ImVec2 nextNodePos = ImVec2(-1, -1);
std::vector<Node> nodes;
json nodes_data;
json createNodeMenu_data = {};
ImNodesContext* imnodes_ctx;
std::vector<std::pair<int, int>> links;
std::vector<Node> selectedNodes;

void initializeNodeEditor()
{
    imnodes_ctx = ImNodes::CreateContext();
    ImNodes::SetNodeGridSpacePos(1, ImVec2(200.0f, 200.0f));
}

void registerNodes(std::string json_data)
{
    auto stream = std::stringstream(json_data);
    nodes_data = json::parse(stream);
    for (auto it = nodes_data.begin(); it != nodes_data.end(); it++)
    {
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

void createNode(const char* id, ImVec2 createPos)
{
    Node node(id, nodes_data[id]);
    nodes.push_back(node);
    nextNodePos = createPos;
    popup = false;
}

void renderCreateNodeMenuItems(json data)
{
    for (auto it = data.begin(); it != data.end(); it++)
    {
        if (it.value().is_string())
            if (ImGui::Selectable(it.key().c_str()))
                createNode(it.value().get<std::string>().c_str(), ImGui::GetMousePos());
        
        if (!it.value().is_string())
        {
            if (ImGui::BeginMenu(it.key().c_str()))
            {
                renderCreateNodeMenuItems(it.value());
                ImGui::EndMenu();
            }
        }
    }
}

void renderCreateNodeMenu()
{    
    popup = (ImGui::IsMouseDown(ImGuiMouseButton_Right) || popup) && !ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsPopupOpen("createNodeMenu");
    if (popup)
    {
        ImGui::OpenPopup("createNodeMenu");
        popup = true;
        if (ImGui::BeginPopup("createNodeMenu"))
        {            
            renderCreateNodeMenuItems(createNodeMenu_data);
            ImGui::EndPopup();
        }
    }
}

bool renderNodeEditor()
{
    ImGui::DockSpaceOverViewport();

    // TODO: Add property editor here...
    ImGui::Begin("Properties");
    ImGui::End();

    // TODO: Add log output here...
    ImGui::Begin("Log");
    ImGui::End();

    ImGui::Begin("Node editor"); // Replace this with filename

    renderCreateNodeMenu();

    // reset our id counter
    id = 1;

    selectedNodes.clear();

    ImNodes::BeginNodeEditor();

    for (int i = 0; i < nodes.size(); i++)
    {
        auto node = nodes[i];
        int node_id = node.int_id();        
        node.render();
        if ((i == nodes.size() - 1) && (nextNodePos.x != -1) && (nextNodePos.y != -1))
        {
            ImNodes::SetNodeScreenSpacePos(node_id, nextNodePos);
            nextNodePos = ImVec2(-1, -1);
        }
        if (ImNodes::IsNodeSelected(node_id))
            selectedNodes.push_back(node);
    }

    // Draw links
    {
        for (const auto& link: links)
            ImNodes::Link(id++, link.first, link.second);
    }

    ImNodes::EndNodeEditor();

    // Handle links
    {
        int start_node_id, start_attribute_id, end_node_id, end_attribute_id;
        bool created_from_snap;
        if (ImNodes::IsLinkCreated(
            &start_node_id, &start_attribute_id, 
            &end_node_id, &end_attribute_id, 
            &created_from_snap))
        {
            auto start_noderef = std::find_if(nodes.begin(), nodes.end(), 
            [&](Node& node) {
                return node.int_id() == start_node_id;
            });
            assert(start_noderef != nodes.end());
            
            auto end_noderef = std::find_if(nodes.begin(), nodes.end(), [&](Node node) {
                return node.int_id() == end_node_id;
            });
            assert(end_noderef != nodes.end());

            auto start_portref = std::find_if(start_noderef->outputs().begin(), start_noderef->outputs().end(), 
            [&](OutputPort& port){
                return port.int_id() == start_attribute_id;
            });
            auto end_portref = std::find_if(end_noderef->inputs().begin(), end_noderef->inputs().end(), 
            [&](InputPort& port){
                return port.int_id() == end_attribute_id;
            });

            if (start_portref->type() == end_portref->type())
                links.push_back(std::make_pair(start_attribute_id, end_attribute_id));
        }
    }

    ImGui::End();

    return true;
}

void shutdownNodeEditor()
{
    // Nothing to do
}