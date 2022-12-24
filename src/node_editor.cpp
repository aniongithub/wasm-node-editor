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
#include "properties.h"
#include "link.h"

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
std::string focusedWindow = "";

std::vector<std::shared_ptr<Window>> _windows { 
    std::make_shared<Graph>("misc/new_graph", "{}"), 
    std::make_shared<Graph>("misc/foobar", "{}"),
    std::make_shared<Properties>() };

void initializeNodeEditor()
{
    imnodes_ctx = ImNodes::CreateContext();
    ImNodes::SetNodeGridSpacePos(1, ImVec2(200.0f, 200.0f));
}

void registerNodes(const char* json_data)
{
    Graph::registerNodes(json_data);
}

void editGraph(const char* id, const char* json_graph)
{
    _windows.push_back(std::make_shared<Graph>(id, json_graph));
}

// void createNode(const char* id, ImVec2 createPos)
// {
//     nodes.push_back(std::make_shared<Node>(id, nodes_data[id]));
//     nextNodePos = createPos;
//     popup = false;
// }

void createNode(const Graph& graph, const char* node_id)
{
    
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
    popup = ((ImGui::IsMouseDown(ImGuiMouseButton_Right) || popup) && 
            !ImGui::IsMouseDown(ImGuiMouseButton_Left) || 
            ImGui::IsPopupOpen("createNodeMenu")) &&
            focusedWindow == "Editor";
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

void renderMainMenuBar()
{
    bool drawNewGraphPopup = false;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "CTRL+N")) 
            {
                drawNewGraphPopup = true;
            }
            if (ImGui::MenuItem("Open", "CTRL+O")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
        
    if (drawNewGraphPopup)
    {
        ImGui::OpenPopup("Name");
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }
    if (ImGui::BeginPopupModal("Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        constexpr int bufSize = 1024;
        std::string id(bufSize, '\0');
        ImGui::Text("Id: ");
        ImGui::SameLine();
        ImGui::InputText(" ",
            &id[0], bufSize, 0, 
            [](ImGuiInputTextCallbackData* data) -> int {
                auto str = static_cast<std::string*>(data->UserData);
                str->resize(data->BufSize);
                data->Buf = &(*str)[0];
                return 0;
            }, 
            &id);
        if (ImGui::Button("OK")) 
            ImGui::CloseCurrentPopup(); 
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) 
            ImGui::CloseCurrentPopup(); 
        ImGui::EndPopup();
    }
}

bool renderNodeEditor()
{
    ImGui::DockSpaceOverViewport();
    renderMainMenuBar();

    for (auto& window: _windows)
        window->render();

    // focusedWindow = "";

    // // TODO: Add log output here...
    // ImGui::Begin("Log");
    // if (ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy | ImGuiFocusedFlags_RootAndChildWindows))
    //     focusedWindow = "Log";
    // ImGui::End();

    // ImGui::Begin("Editor"); // Replace this with node id, ensure we can have multiple

    // if (ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy | ImGuiFocusedFlags_RootAndChildWindows))
    //     focusedWindow = "Editor";

    // renderCreateNodeMenu();

    // // reset our id counter
    // id = 1;

    // // Clear selected nodes per frame
    // selectedNodes.clear();

    // ImNodes::BeginNodeEditor();

    // for (int i = 0; i < nodes.size(); i++)
    // {
    //     auto node = nodes[i];
       
    //     node->render();
    //     if ((i == nodes.size() - 1) && (nextNodePos.x != -1) && (nextNodePos.y != -1))
    //     {
    //         ImNodes::SetNodeScreenSpacePos(node->render_id(), nextNodePos);
    //         nextNodePos = ImVec2(-1, -1);
    //     }
    //     if (ImNodes::IsNodeSelected(node->render_id()))
    //         selectedNodes.push_back(node);
    // }

    // // Render property editor
    // {
    //     ImGui::Begin("Properties");
    //     if (ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy | ImGuiFocusedFlags_RootAndChildWindows))
    //         focusedWindow = "Properties";
        
    //     // TODO: Deal with multiple nodes case later
    //     if (selectedNodes.size() == 1) 
    //         for (auto& node: selectedNodes)
    //             node->renderProperties();
    //     ImGui::End();
    // }

    // // Draw links
    // {
    //     for (auto& link: links)
    //         ImNodes::Link(link.render_id(), link.start_port()->render_id(), link.end_port()->render_id());
    // }

    // ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_TopRight);
    // ImNodes::EndNodeEditor();

    // // Handle links
    // {
    //     int start_node_id, start_attribute_id, end_node_id, end_attribute_id;
    //     bool created_from_snap;
    //     if (ImNodes::IsLinkCreated(
    //         &start_node_id, &start_attribute_id, 
    //         &end_node_id, &end_attribute_id, 
    //         &created_from_snap))
    //     {
    //         auto start_nodeRef = Node::empty();
    //         auto end_nodeRef = Node::empty();
    //         for (auto& node: nodes)
    //         {
    //             if (node->render_id() == start_node_id)
    //                 start_nodeRef = node;
    //             if (node->render_id() == end_node_id)
    //                 end_nodeRef = node;
                
    //             if ((start_nodeRef != Node::empty()) && 
    //                 (end_nodeRef != Node::empty()))
    //                 break;
    //         }
    //         assert(start_nodeRef != Node::empty() &&
    //             end_nodeRef != Node::empty());

    //         auto start_portRef = OutputPort::empty();
            
    //         for (auto& iter: start_nodeRef->outputs())
    //             if (iter.second->render_id() == start_attribute_id)
    //             {
    //                 start_portRef = iter.second;
    //                 break;
    //             }
    //         auto end_portRef = InputPort::empty();
    //         for (auto& iter: end_nodeRef->inputs())
    //             if (iter.second->render_id() == end_attribute_id)
    //             {
    //                 end_portRef = iter.second;
    //                 break;
    //             }

    //         assert(start_portRef != OutputPort::empty() &&
    //             end_portRef != InputPort::empty());
            
    //         // TODO: The types may be implicitly convertible?
    //         if (start_portRef->type() == end_portRef->type())
    //             links.push_back(std::move(Link(start_nodeRef, start_portRef, end_nodeRef, end_portRef)));
    //     }
    // }

    // ImGui::End();

    return true;
}

void shutdownNodeEditor()
{
    // Nothing to do
}