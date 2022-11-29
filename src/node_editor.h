#pragma once

#include <string>

void initializeNodeEditor();
void registerNodes(std::string json_data);
bool renderNodeEditor();
void shutdownNodeEditor();