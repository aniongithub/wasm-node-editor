#pragma once

#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <api.h>
#include <commands/command.h>

static const char* DELETENODE_COMMAND = "deleteNode";

struct DeleteNodeCommand: Command
{
    public:
        // std::string id;
        // json json_metadata;
        // NLOHMANN_DEFINE_TYPE_INTRUSIVE(CreateNodeCommand, id, json_metadata);
};