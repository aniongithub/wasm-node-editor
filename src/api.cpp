#include <string>

#include <api.h>
#include <handles/editor.h>
#include <handles/graph.h>

EditorResult initializeEditor(EditorCallbacks callbacks, EditorFlags flags, Editor* editorHdl) 
{
    if (!editorHdl)
        return RESULT_INVALID_ARGS;

    *editorHdl = new Editor_t(callbacks, EDITORFLAGS_NONE);
    return RESULT_OK;
}

EditorResult registerGraphs(Editor editorHdl, const char* json_data, size_t json_dataSizeBytes)
{
    if (!editorHdl ||
        !json_data ||
        (json_dataSizeBytes <= 0))
        return RESULT_INVALID_ARGS;

    return editorHdl->registerGraphs(std::string().assign(json_data, json_dataSizeBytes));
}

EditorResult renderEditor(Editor editorHdl) 
{
    if (!editorHdl)
        return RESULT_INVALID_ARGS;

    return editorHdl->render();
}

EditorResult shutdownEditor(Editor editorHdl) 
{
    if (!editorHdl)
        return RESULT_INVALID_ARGS;
    
    delete editorHdl;
    return RESULT_OK;
}

EditorResult editGraph(Editor editorHdl, const char* id, size_t idSizeBytes, const char* json_graph_data, size_t json_graph_dataSizeBytes, GraphCallbacks callbacks, Graph* graphHdl) 
{ 
    if (!editorHdl ||
        !id ||
        (idSizeBytes <= 0) ||
        !graphHdl)
        return RESULT_INVALID_ARGS;
    
    return editorHdl->editGraph(std::string().assign(id, idSizeBytes), 
        std::string().assign(json_graph_data, json_graph_dataSizeBytes), 
        callbacks, graphHdl);
}

EditorResult closeGraph(Editor editorHdl, Graph graphHdl) 
{ 
    if (!editorHdl)
        return RESULT_INVALID_ARGS;
    
    return editorHdl->closeGraph(graphHdl);
}

EditorResult createNode(Graph graphHdl, const char* id, size_t idSizeBytes, const char* json_node_metadata, size_t json_node_medataSizeBytes, Node* nodeHdl)
{
    if (!id ||
        (idSizeBytes == 0)||
        !nodeHdl)
        return RESULT_INVALID_ARGS;
    
    return graphHdl->createNode(std::string().assign(id, idSizeBytes), 
        std::string().assign(json_node_metadata, json_node_medataSizeBytes),
        nodeHdl);
}