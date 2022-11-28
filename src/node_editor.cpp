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

#define INPUT_OUTPUT_SPACING 25

int id = 1;
bool popup = false;
ImVec2 nextNodePos = ImVec2(-1, -1);
std::vector<std::string> nodes;
json nodes_data;
json createNodeMenu_data = {};
ImNodesContext* imnodes_ctx;
IdGenerator<std::string, int> idGen(1);
std::vector<std::pair<int, int>> links;

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
        int id_int;
        idGen.getId(it.key(), id_int);

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

        // reserve ids for all nodes, input and output ports using keys of the form
        // node/category/../here/node_name.port_name
        auto node = nodes_data[it.key()];
        if (node.contains("inputs"))
        {
            for (auto i = node["inputs"].begin(); i != node["inputs"].end(); i++)
            {
                int input_id;
                idGen.getId(it.key() + "." + i.key(), input_id);
            }
        }
        if (node.contains("outputs"))
        {
            for (auto o = node["outputs"].begin(); o != node["outputs"].end(); o++)
            {
                int output_id;
                idGen.getId(it.key() + "." + o.key(), output_id);
            }
        }
    }
    std::cout << createNodeMenu_data.dump(4) << std::endl;
}

static int renderNode(std::string node_id, const std::vector<const char*>& inputs, const std::vector<const char*>& outputs)
{
    int int_id;
    idGen.getId(node_id, int_id);
    ImNodes::BeginNode(int_id);

    ImNodes::BeginNodeTitleBar();

    auto idStream = std::stringstream(node_id);
    std::string id_segment;
    // Get the last part of the full node title
    while (std::getline(idStream, id_segment, '/'));

    ImGui::TextUnformatted(id_segment.c_str());
    ImNodes::EndNodeTitleBar();

    float maxInputTextSize = 0;
    assert(nodes_data.contains(node_id));
    auto& node = nodes_data[node_id];

    if (node.contains("inputs"))
    {
        for (auto it = node["inputs"].begin(); it != node["inputs"].end(); it++)
        {
            int input_id;
            idGen.getId(node_id + "." + it.key(), input_id);
            ImNodes::BeginInputAttribute(input_id);
            ImGui::TextUnformatted(it.key().c_str());
            ImNodes::EndInputAttribute();

            maxInputTextSize = max(maxInputTextSize, ImGui::CalcTextSize(it.key().c_str()).x);
        }
    }

    if (node.contains("outputs"))
    {
        for (auto it = node["outputs"].begin(); it != node["outputs"].end(); it++)
        {
            int output_id;
            idGen.getId(node_id + "." + it.key(), output_id);
            ImNodes::BeginOutputAttribute(output_id);
            ImGui::Indent(maxInputTextSize + INPUT_OUTPUT_SPACING);
            ImGui::TextUnformatted(it.key().c_str());
            ImNodes::EndOutputAttribute();
        }
    }

    ImNodes::EndNode();
    return int_id;
}

void createNode(const char* item, ImVec2 createPos)
{
    nodes.push_back(item);
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

    // ImGui::SetNextWindowPos(ImVec2(0, 0));
    // ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Node editor"); // Replace this with filename

    renderCreateNodeMenu();

    // reset our id counter
    id = 1;

    ImNodes::BeginNodeEditor();

    for (int i = 0; i < nodes.size(); i++)
    {
        auto n = nodes[i];
        auto id = renderNode(n, { "input0", "input1" }, { "output" });
        if ((i == nodes.size() - 1) && (nextNodePos.x != -1) && (nextNodePos.y != -1))
        {
            ImNodes::SetNodeScreenSpacePos(id, nextNodePos);
            nextNodePos = ImVec2(-1, -1);
        }
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
            std::string start_node_name;
            idGen.getKey(start_node_id, start_node_name);
            std::string start_attribute_name;
            idGen.getKey(start_attribute_id, start_attribute_name);
            std::string end_node_name;
            idGen.getKey(end_node_id, end_node_name);
            std::string end_attribute_name;
            idGen.getKey(end_attribute_id, end_attribute_name);

            auto start_attr_parts = StringSplitter<'.'>(start_attribute_name).allTokens();
            auto end_attr_parts = StringSplitter<'.'>(end_attribute_name).allTokens();

            auto start_noderef = nodes_data[start_attr_parts[0]];
            auto start_attribute_type = start_noderef["outputs"][start_attr_parts[1]].get<std::string>();
            auto end_noderef = nodes_data[end_attr_parts[0]];
            auto end_attribute_type = end_noderef["inputs"][end_attr_parts[1]].get<std::string>();

            if (start_attribute_type == end_attribute_type)
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