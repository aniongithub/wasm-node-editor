#include <emscripten.h>
#include <api.h>

EditorResult editorInitialize(void* context, Editor editorHdl)
{
    while (true)
    {
        char* str = (char*)EM_ASM_PTR(
            {
                var jsStr = onEnumerateGraphs();
                if (jsStr == null)
                    return null;
                
                var lengthBytes = lengthBytesUTF8(jsStr) + 1;
                var stringOnWasmHeap = _malloc(lengthBytes);
                stringToUTF8(jsStr, stringOnWasmHeap, lengthBytes);
                return stringOnWasmHeap;
            });
        if (str)
        {
            printf("Got non-null graph with data: %s\n", str);
            registerGraphs(editorHdl, str, strlen(str));
            free(str);
        }
        else
        {
            printf("got null graph, done enumerating nodes\n");
            break;
        }
    }

    return RESULT_OK;
}

EditorResult editorShutdown(void* context, Editor editorHdl)
{
    return RESULT_OK;
}

EditorResult editorOpenGraph(void* context, Editor editorHdl, const char* id, size_t idSizeBytes)
{
    char* str = (char*)EM_ASM_PTR(
        {
            var jsStr = onOpenGraph(UTF8ToString($0));
            var lengthBytes = lengthBytesUTF8(jsStr) + 1;
            var stringOnWasmHeap = _malloc(lengthBytes);
            stringToUTF8(jsStr, stringOnWasmHeap, lengthBytes);
            return stringOnWasmHeap;
        }, id);
    printf("string result from openGraph callback: %s\n", str);
    free(str);

    // TODO: Execute the commands returned by the JS callback here
    return RESULT_OK;
}

EditorResult editorCloseGraph(void* context, Editor editorHdl, const char* id, size_t idSizeBytes, Graph graph)
{
    EM_ASM(
        {
            onCloseGraph(UTF8ToString($0));
        }, id);
    return RESULT_OK;
}

EditorResult graphCreateNode(void* context, Graph graphHdl, const char* id, size_t idSizeBytes, const char* json_node_metadata, size_t json_node_medataSizeBytes, Node* nodeHdl)
{
    EM_ASM(
        {
            onNodeCreated(UTF8ToString($0), UTF8ToString($1), $2);
        }, graphHdl->id().c_str(), id, (*nodeHdl)->renderId());
    return RESULT_OK;
}