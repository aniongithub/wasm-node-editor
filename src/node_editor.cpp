#include "node_editor.h"

#include <vector>
#include <string>
#include <map>
#include <memory>

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
#include "graph.h"

#define INPUT_OUTPUT_SPACING 25

int id = 1;
bool popup = false;
ImVec2 nextNodePos = ImVec2(-1, -1);
std::vector<std::shared_ptr<Node>> nodes;
json nodes_data;
json createNodeMenu_data = {};
ImNodesContext* imnodes_ctx;
std::vector<Link> links;
std::vector<std::shared_ptr<Node>> selectedNodes;

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
    nodes.push_back(std::make_shared<Node>(id, nodes_data[id]));
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

    // TODO: Add log output here...
    ImGui::Begin("Log");
    ImGui::End();

    ImGui::Begin("Node editor"); // Replace this with filename

    renderCreateNodeMenu();

    // reset our id counter
    id = 1;

    // Clear selected nodes per frame
    selectedNodes.clear();

    ImNodes::BeginNodeEditor();

    for (int i = 0; i < nodes.size(); i++)
    {
        auto node = nodes[i];
        int node_id;
        name_to_id::instance().getId(node->name(), node_id);
        
        node->render();
        if ((i == nodes.size() - 1) && (nextNodePos.x != -1) && (nextNodePos.y != -1))
        {
            ImNodes::SetNodeScreenSpacePos(node_id, nextNodePos);
            nextNodePos = ImVec2(-1, -1);
        }
        if (ImNodes::IsNodeSelected(node_id))
            selectedNodes.push_back(node);
    }

    // Render property editor
    {
        ImGui::Begin("Properties");
        // TODO: Deal with multiple nodes case later
        if (selectedNodes.size() == 1) 
            for (auto& node: selectedNodes)
                node->renderProperties();
        ImGui::End();
    }

    // Draw links
    {
        for (auto& link: links)
        {
            int start_port_id;
            name_to_id::instance().getId(link.start_port()->fullName(), start_port_id);
            
            int end_port_id;
            name_to_id::instance().getId(link.end_port()->fullName(), end_port_id);

            int link_id;
            name_to_id::instance().getId(link.fullName(), link_id);
            ImNodes::Link(link_id, start_port_id, end_port_id);
        }
    }

    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_TopRight);
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
            std::string start_node_name;
            name_to_id::instance().getKey(start_node_id, start_node_name);
            std::string end_node_name;
            name_to_id::instance().getKey(end_node_id, end_node_name);
            
            auto start_noderef = Node::empty();
            auto end_noderef = Node::empty();
            for (auto& node: nodes)
            {
                if (node->name() == start_node_name)
                    start_noderef = node;
                if (node->name() == end_node_name)
                    end_noderef = node;
                
                if ((start_noderef != Node::empty()) && 
                    (end_noderef != Node::empty()))
                    break;
            }
            
            std::string start_port_fullname;
            name_to_id::instance().getKey(start_attribute_id, start_port_fullname);
            auto start_port_name = StringSplitter<'.'>(start_port_fullname).lastToken();
            auto start_portref = start_noderef->outputs()[start_port_name];
            
            std::string end_port_fullname;
            name_to_id::instance().getKey(end_attribute_id, end_port_fullname);
            auto end_port_name = StringSplitter<'.'>(end_port_fullname).lastToken();
            auto end_portref = end_noderef->inputs()[end_port_name];

            // TODO: The types may be implicitly convertible?
            if (start_portref->type() == end_portref->type())
                links.push_back(std::move(Link(start_noderef, start_portref, end_noderef, end_portref)));
        }
    }

    ImGui::End();

    return true;
}

void shutdownNodeEditor()
{
    // Nothing to do
}