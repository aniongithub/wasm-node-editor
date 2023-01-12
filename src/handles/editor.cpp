#include "editor.h"

#include <sstream>

#include <handles/graph.h>
#include <handles/node.h>

json Editor_t::nodes_data = {};
json Editor_t::createNode_data = {};

Editor_t::Editor_t(EditorCallbacks& callbacks, EditorFlags flags):
    _callbacks(callbacks),
    _flags(flags)
{
    if (_callbacks.enumerateGraphs)
        _callbacks.enumerateGraphs(_callbacks.context, this);
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

EditorResult Editor_t::renderMainMenu()
{
    bool drawNewGraphPopup = false;
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Graph"))
        {
            if (ImGui::MenuItem("New...", "CTRL+N")) 
            {
                drawNewGraphPopup = true;
            }
            if (ImGui::MenuItem("Open...", "CTRL+O")) {}
            if (ImGui::MenuItem("Open recent"))
            {
                // TODO: Load and render recents menu here...
                // https://github.com/ocornut/imgui/issues/2564#issuecomment-493766526
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z", false, false)) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X", false, false)) {}
            if (ImGui::MenuItem("Copy", "CTRL+C", false, false)) {}
            if (ImGui::MenuItem("Paste", "CTRL+V", false, false)) {}
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
        constexpr int bufSize = 256;
        std::string id;
        id.resize(bufSize);
        
        ImGui::Text("Id: ");
        ImGui::SameLine();
        bool enterPressed = ImGui::InputText(" ",
            &id[0], bufSize, ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_EnterReturnsTrue, 
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
            &id);
        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("OK") || enterPressed)
        {
            ImGui::CloseCurrentPopup(); 
            GraphCallbacks callbacks = {0};
            Graph graph;
            auto result = editGraph(id, "", callbacks, &graph);
            if (result != RESULT_OK)
                return result;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) 
            ImGui::CloseCurrentPopup(); 
        ImGui::EndPopup();
    }
    return RESULT_OK;
}

EditorResult Editor_t::render()
{
    ImGui::DockSpaceOverViewport();

    renderMainMenu();

    _selectedNodes.clear();

    for (auto graph: _graphs)
    {
        auto result = graph->render();
        if (result != RESULT_OK)
            return result;
    }

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

    _graphs.push_back(*graphHdl);
    return result;
}

EditorResult Editor_t::closeGraph(Graph graphHdl)
{
    if (!graphHdl)
        return RESULT_INVALID_ARGS;
    
    auto it = std::find(_graphs.begin(), _graphs.end(), graphHdl);
    if (it == _graphs.end())
        return RESULT_INVALID_ARGS;

    graphHdl->shutdown();
    _graphs.erase(it);
    
    delete graphHdl;
    return RESULT_OK;
}