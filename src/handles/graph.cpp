#include "graph.h"

#include <imnodes.h>

#include <handles/editor.h>
#include <handles/node.h>
#include <commands/createNode.h>

Graph_t::Graph_t(Editor parent, std::string id, std::string json_graph_data, GraphCallbacks callbacks):
    _parent(parent),
    _id(id),
    _json_graph_data(json_graph_data),
    _callbacks(callbacks),
    _allowClose(true),
    _closed(false),
    _popup(false),
    _windowFlags(ImGuiWindowFlags_None)
{
    static ImNodesContext* imnodes_ctx = ImNodes::CreateContext();
    ImNodes::SetNodeGridSpacePos(1, ImVec2(200.0f, 200.0f));
}

EditorResult Graph_t::prepare()
{
    // TODO: Validate against a schema
    auto stream = std::stringstream(_json_graph_data);
    auto commands = json::parse(stream, nullptr, false);
    
    // TODO: Test this
    /*
    {
        "createNode": {
            "id": "foo.bar",
            "metadata": {
                "pos_2D": [ 100.0, 100.0 ]
            }
        }
    }
    */

    if (!commands.is_discarded())
        for (auto it = commands.begin(); it != commands.end(); it++)
        {
            if (it.key() == CREATENODE_COMMAND)
            {
                auto createNodeCommand = it.value().get<CreateNodeCommand>();
                continue;
            }
        }
    
    return RESULT_OK;
}

EditorResult Graph_t::renderAddNodeMenu(json createNodeData)
{
    for (auto it = createNodeData.begin(); it != createNodeData.end(); it++)
    {
        if (it.value().is_string())
            if (ImGui::Selectable(it.key().c_str()))
            {
                auto mousePos = ImGui::GetMousePos();
                auto metadata = json({
                    {"pos_x", mousePos.x}, {"pos_y", mousePos.y}
                }).dump();
                if (_callbacks.nodeCreated)
                {
                    Node node;
                    auto result = _callbacks.nodeCreated(_callbacks.context, this, it.key().c_str(), it.key().size() + 1,
                        metadata.c_str(), metadata.size() + 1, &node);
                    if (result != RESULT_OK)
                        return result;
                }
            }
        
        if (!it.value().is_string())
        {
            if (ImGui::BeginMenu(it.key().c_str()))
            {
                renderAddNodeMenu(it.value());
                ImGui::EndMenu();
            }
        }
    }
    return RESULT_OK;
}

EditorResult Graph_t::renderContents()
{
    #pragma region Render the popup menu
    
    _popup = ImGui::IsPopupOpen("graph_contextMenu") ||
        (ImGui::IsWindowFocused() &&
        (ImGui::IsMouseDown(ImGuiMouseButton_Right)));
    if (_popup)
    {
        ImGui::OpenPopup("graph_contextMenu");
        if (ImGui::BeginPopup("graph_contextMenu"))
        {            
            // Render the add node menu
            if (ImGui::BeginMenu("Add", parent()->selectedNodes().size() == 0))
            {            
                auto result = renderAddNodeMenu(parent()->getCreateNodeData());
                if (result != RESULT_OK)
                    return result;

                ImGui::EndMenu();
            }
            
            ImGui::EndPopup();
        }
        else
            _popup = false;
    }

    #pragma endregion

    #pragma region Render nodes

    for (auto& node: _nodes)
        node->render();

    #pragma endregion    

    return RESULT_OK;
}

EditorResult Graph_t::render()
{
    ImGui::Begin(_id.c_str(), _allowClose? &_closed : nullptr, _windowFlags);

    ImNodes::BeginNodeEditor();
    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_TopRight);

    auto result = renderContents();
    if (result != RESULT_OK)
        return result;

    ImNodes::EndNodeEditor();
    ImGui::End();
    
    return RESULT_OK;
}

EditorResult Graph_t::createNode(std::string id, std::string json_node_metadata, Node* nodeHdl)
{
    auto node = new Node_t(this, id, json::parse(json_node_metadata));
    auto result = node->prepare();
    if (result == RESULT_OK)
        _nodes.push_back(node);
    else
        return result;
    
    return RESULT_OK;
}

EditorResult Graph_t::shutdown()
{
    return RESULT_NOT_IMPLEMENTED;    
}