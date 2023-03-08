#include "editor.h"

#include <sstream>
#include <vector>
#include <regex>

#include <api.h>
#include <handles/graph.h>
#include <handles/node.h>

json Editor_t::nodes_data = {};
json Editor_t::createNode_data = {};

Editor_t::Editor_t(EditorCallbacks& callbacks, EditorFlags flags):
    _callbacks(callbacks),
    _flags(flags)
{
    if (_callbacks.initialize)
        _callbacks.initialize(_callbacks.context, this);
}

EditorResult Editor_t::registerGraphs(std::string json_data)
{
    // TODO: Validate against a schema
    auto stream = std::stringstream(json_data);
    auto nodes = json::parse(stream, nullptr, false);
    if (nodes.is_discarded())
        return RESULT_INVALID_ARGS;

    for (auto it = nodes.begin(); it != nodes.end(); it++)
    {
        nodes_data[it.key()] = it.value();

        // collapse node ids into a category tree
        std::stringstream id(it.key());
        std::string id_segment;
        json* curr = &createNode_data;
        
        while (std::getline(id, id_segment, '/'))
        {
            if (!curr->contains(id_segment))
                (*curr)[id_segment] = {};
            curr = &((*curr)[id_segment]);
        }
        *curr = it.key();
    }
    return RESULT_OK;
}

bool Editor_t::node_exists(std::string id)
{
    for (auto it = Editor_t::nodes_data.begin(); it != Editor_t::nodes_data.end(); it++)
    {
        if (strcmp(it.key().c_str(), id.c_str()) == 0)
            return true;
    }
    
    return false;
}

EditorResult Editor_t::renderProperties()
{
    ImGui::Begin("Properties");
    
    // TODO: Deal with multiple nodes case later
    if (_selectedNodes.size() == 1) 
        for (auto& node: _selectedNodes)
            node->renderProperties();
    ImGui::End();
    
    return RESULT_OK;
}

EditorResult Editor_t::renderGraphTree(json nodeData, std::string currPath, std::string filter)
{
    for (auto it = nodeData.begin(); it != nodeData.end(); it++)
    {
        if (it.value().is_string())            
        {
            auto id = currPath == "" ? it.key() : currPath + "/" + it.key();
            if (id.find(filter) != std::string::npos)
            {
                ImGui::Selectable(it.key().c_str());
                if (ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload(id.c_str(), nullptr, 0);
                    ImGui::Text("%s", id.c_str());
                    ImGui::EndDragDropSource();
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    auto editable = nodes_data[id].value("editable", false);
                    if (editable)
                    {
                        auto it = _graphs.find(id);
                        if (it != _graphs.end())
                        {
                            _focusedWindow = id;
                        }
                        else
                        {
                            printf("Graph %s is not open, open it here\n", id.c_str());
                            // TODO: Continue here, we need to figure out what callbacks need to be invoked because we may have instructions for it
                            // editGraph(id, )
                        }
                    }
                    else
                    {
                        printf("Graph %s is not editable!\n", id.c_str());
                    }
                }
            }
        }
        
        if (!it.value().is_string())
        {
            ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
            if (ImGui::TreeNode(it.key().c_str()))
            {
                auto saveCurrPath = currPath;
                currPath = currPath == "" ? it.key() : currPath + "/" + it.key();
                renderGraphTree(it.value(), currPath, filter);
                ImGui::TreePop();
                currPath = saveCurrPath;
            }
        }
    }
    return RESULT_OK;
}

EditorResult Editor_t::renderGraphWindow()
{
    ImGui::Begin("Graphs");

    ImGui::PushItemWidth(-1);
    
    ImGui::InputText("##search",
        &_graphFilter[0], 256, ImGuiInputTextFlags_CallbackResize, 
        [](ImGuiInputTextCallbackData* data) -> int {
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
            {
                auto str = static_cast<std::string*>(data->UserData);
                if (data->BufSize > str->size())
                    str->resize(data->BufSize);
                data->Buf = &(*str)[0];
            }
            return 0;
        },
        &_graphFilter);
    
    auto newClicked = ImGui::Button("New...");
    renderNewGraphDialog(newClicked);

    renderGraphTree(createNode_data, "", _graphFilter.c_str());
    
    ImGui::PopItemWidth();

    ImGui::End();

    return RESULT_OK;
}

EditorResult Editor_t::graphNodeCreated(void* context, Graph graphHdl, const char* id, size_t idSizeBytes, const char* json_node_metadata, size_t json_node_medataSizeBytes, Node* nodeHdl)
{
    if (!graphHdl)
        return RESULT_INVALID_ARGS;
    return graphHdl->onNodeCreated(context, id, idSizeBytes, json_node_metadata, json_node_medataSizeBytes, nodeHdl);
}

EditorResult Editor_t::renderNewGraphDialog(bool renderDialog)
{
    if (renderDialog)
    {
        ImGui::OpenPopup("New Graph");
        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }

    bool graphExistsErrorPopup = false;
    std::string new_id;

    if (ImGui::BeginPopupModal("New Graph", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Id: ");
        ImGui::SameLine();
        bool enterPressed = ImGui::InputText(" ",
            &new_id[0], 256, ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_EnterReturnsTrue, 
            [](ImGuiInputTextCallbackData* data) -> int {
                if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                {
                    auto str = reinterpret_cast<std::string*>(data->UserData);
                    if (data->BufSize > str->size())
                        str->resize(data->BufSize);
                    data->Buf = str->data();
                }
                return 0;
            },
            &new_id);
        if (ImGui::Button("OK") || enterPressed)
        {
            ImGui::CloseCurrentPopup(); 
            new_id = std::string(new_id.c_str());

            if (node_exists(new_id))
            {
                graphExistsErrorPopup = true;
            }
            else
            {
                GraphCallbacks callbacks = {0};
                callbacks.nodeCreated = graphNodeCreated;
                Graph graph;
                auto result = editGraph(new_id, "[]", callbacks, &graph);
                if (result != RESULT_OK)
                    return result;
                
                // Add an empty graph 
                json newGraph;
                newGraph[new_id] = "{ \"editable\": true }"_json;
                registerGraphs(newGraph.dump());
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup(); 

        ImGui::EndPopup();
    }
    if (graphExistsErrorPopup)
    {
        ImGui::OpenPopup("Error");
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }
    if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Graph already exists");
        ImGui::TextUnformatted("");
        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("Ok"))
            ImGui::CloseCurrentPopup();
        
        ImGui::EndPopup();
    }
    return RESULT_OK;
}

ImVec2 operator +(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x + b.x, a.y + b.y);
}

ImVec2 operator -(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x - b.x, a.y - b.y);
}


EditorResult Editor_t::render()
{
    ImGui::DockSpaceOverViewport();

    renderGraphWindow();
    
    _selectedNodes.clear();

    std::vector<Graph> closed;

    for (auto it: _graphs)
    {
        auto graph = it.second;

        // Focus the requested graph
        if (strcmp(_focusedWindow.c_str(), graph->id().c_str()) == 0)
        {
            ImGui::SetNextWindowFocus();
            _focusedWindow = "";
        }

        auto result = graph->render();

        if (result != RESULT_OK)
            return result;
        
        // Add any hidden graphs to a vector to be closed
        if (!graph->open())
            closed.push_back(graph);
    }

    for (auto c: closed)
        closeGraph(c);

    renderProperties();

    return RESULT_OK;
}

EditorResult Editor_t::editGraph(std::string id, std::string json_graph_data, GraphCallbacks callbacks, Graph* graphHdl)
{
    if (_callbacks.openGraph)
        _callbacks.openGraph(_callbacks.context, this, id.c_str(), id.size());

    *graphHdl = new Graph_t(this, id, json_graph_data, callbacks);
    auto result = (*graphHdl)->prepare();
    if (result != RESULT_OK)
    {
        delete *graphHdl;
        *graphHdl = nullptr;
    }

    _graphs[id] = *graphHdl;
    return result;
}

EditorResult Editor_t::closeGraph(Graph graphHdl)
{
    if (!graphHdl)
        return RESULT_INVALID_ARGS;
    
    auto it = _graphs.find(graphHdl->id());
    if (it == _graphs.end())
        return RESULT_INVALID_ARGS;

    graphHdl->shutdown();
    _graphs.erase(graphHdl->id());
    
    delete graphHdl;
    return RESULT_OK;
}