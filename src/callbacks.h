#pragma once

#include <api.h>

EditorResult editorInitialize(void* context, Editor editorHdl);
EditorResult editorShutdown(void* context, Editor editorHdl);

EditorResult editorOpenGraph(void* context, Editor editorHdl, const char* id, size_t idSizeBytes);
EditorResult editorCloseGraph(void* context, Editor editorHdl, const char* id, size_t idSizeBytes, Graph graph);

EditorResult graphCreateNode(void* context, Graph graphHdl, const char* id, size_t idSizeBytes, const char* json_node_metadata, size_t json_node_medataSizeBytes, Node* nodeHdl);