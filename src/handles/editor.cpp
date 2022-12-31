#include "editor.h"

#include <handles/graph.h>
#include <handles/node.h>

json Editor_t::nodes_data = {};
json Editor_t::createNode_data = {};

Editor_t::Editor_t(EditorCallbacks& callbacks, EditorFlags flags):
    _callbacks(callbacks),
    _flags(flags)
{
}

EditorResult Editor_t::registerNodes(std::string json_data)
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

EditorResult Editor_t::render()
{
    ImGui::DockSpaceOverViewport();

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