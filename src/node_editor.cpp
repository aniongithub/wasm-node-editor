#include "node_editor.h"

#include <vector>
#include <string>
#include <map>

#include <imnodes.h>
#include <imgui.h>
#include <sstream>

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
json createNodeMenu_data;
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
    for (auto it = nodes_data.begin(); it < nodes_data.end(); it++)
    {
        std::stringstream id(it.key());
        std::string id_segment;
        auto curr = createNodeMenu_data;
        
        while (std::getline(id, id_segment, '/'))
        {
            curr[id_segment] = {};
            curr = curr[id_segment];
        }
    }
}

static int renderNode(const char* title, const std::vector<const char*>& inputs, const std::vector<const char*>& outputs)
{
    auto node_id = id++;
    ImNodes::BeginNode(node_id);

    ImNodes::BeginNodeTitleBar();

    ImGui::TextUnformatted(title);
    ImNodes::EndNodeTitleBar();

    float maxInputTextSize = 0;
    for (auto input: inputs)
    {
        ImNodes::BeginInputAttribute(id++);
        ImGui::TextUnformatted(input);
        ImNodes::EndInputAttribute();

        maxInputTextSize = max(maxInputTextSize, ImGui::CalcTextSize(input).x);
    }

    for (auto output: outputs)
    {
        ImNodes::BeginOutputAttribute(id++);
        ImGui::Indent(maxInputTextSize + INPUT_OUTPUT_SPACING);
        ImGui::TextUnformatted(output);
        ImNodes::EndOutputAttribute();
    }

    ImNodes::EndNode();
    return node_id;
}

void createNode(const char* item, ImVec2 createPos)
{
    nodes.push_back(item);
    nextNodePos = createPos;
    popup = false;
}

void renderCreateNodeMenu()
{    
    popup = (ImGui::IsMouseDown(ImGuiMouseButton_Right) || popup) && !ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsPopupOpen("foobar");
    if (popup)
    {
        ImGui::OpenPopup("createNodeMenu");
        popup = true;
        if (ImGui::BeginPopup("createNodeMenu"))
        {            
            if (ImGui::Selectable("Foo")) createNode("Foo", ImGui::GetMousePos());
            if (ImGui::Selectable("Bar")) createNode("Bar", ImGui::GetMousePos());
            if (ImGui::BeginMenu("Baz"))
            {
                if (ImGui::Selectable("FooBaz")) createNode("FooBaz", ImGui::GetMousePos());
                if (ImGui::Selectable("BarBaz")) createNode("BarBaz", ImGui::GetMousePos());
                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }
    }
}

bool renderNodeEditor()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Node editor"); // Replace this with filename

    popup = (ImGui::IsMouseDown(ImGuiMouseButton_Right) || popup) && !ImGui::IsMouseDown(ImGuiMouseButton_Left) || ImGui::IsPopupOpen("foobar");
    if (popup)
    {
        ImGui::OpenPopup("foobar");
        popup = true;
        if (ImGui::BeginPopup("foobar"))
        {            
            if (ImGui::Selectable("Foo")) createNode("Foo", ImGui::GetMousePos());
            if (ImGui::Selectable("Bar")) createNode("Bar", ImGui::GetMousePos());
            if (ImGui::BeginMenu("Baz"))
            {
                if (ImGui::Selectable("FooBaz")) createNode("FooBaz", ImGui::GetMousePos());
                if (ImGui::Selectable("BarBaz")) createNode("BarBaz", ImGui::GetMousePos());
                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }
    }

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