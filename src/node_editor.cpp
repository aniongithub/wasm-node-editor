#include "node_editor.h"

#include <vector>
#include <string>
#include <map>

#include <imnodes.h>
#include <imgui.h>
#include <sstream>
#include <iostream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#define INPUT_OUTPUT_SPACING 25

template <typename T>
T max(T a, T b)
{
    return a > b ? a : b;
}

template <>
ImVec2 max(ImVec2 a, ImVec2 b)
{
    return ImVec2(max(a.x, b.x), max(a.y, b.y));
}

int id = 1;
bool popup = false;
ImVec2 nextNodePos = ImVec2(-1, -1);
std::vector<std::string> nodes;
json nodes_data;
json createNodeMenu_data = {};
ImNodesContext* imnodes_ctx;

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
    std::cout << createNodeMenu_data.dump(4) << std::endl;
}

static int renderNode(const char* node_id, const std::vector<const char*>& inputs, const std::vector<const char*>& outputs)
{
    auto int_id = id++;
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
            ImNodes::BeginInputAttribute(id++);
            ImGui::TextUnformatted(it.key().c_str());
            ImNodes::EndInputAttribute();

            maxInputTextSize = max(maxInputTextSize, ImGui::CalcTextSize(it.key().c_str()).x);
        }
    }

    if (node.contains("outputs"))
    {
        for (auto it = node["outputs"].begin(); it != node["outputs"].end(); it++)
        {
            ImNodes::BeginOutputAttribute(id++);
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
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Node editor"); // Replace this with filename

    renderCreateNodeMenu();

    // reset our id counter
    id = 1;

    ImNodes::BeginNodeEditor();

    for (int i = 0; i < nodes.size(); i++)
    {
        auto n = nodes[i];
        auto id = renderNode(n.c_str(), { "input0", "input1" }, { "output" });
        if ((i == nodes.size() - 1) && (nextNodePos.x != -1) && (nextNodePos.y != -1))
        {
            ImNodes::SetNodeScreenSpacePos(id, nextNodePos);
            nextNodePos = ImVec2(-1, -1);
        }
    }

    ImNodes::EndNodeEditor();

    ImGui::End();

    return true;
}

void shutdownNodeEditor()
{
    // Nothing to do
}