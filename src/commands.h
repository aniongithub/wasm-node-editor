#pragma once

#include <memory>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Base class of all commands
class Command: std::enable_shared_from_this<Command>
{
};

class CreateNodeCommand: public Command
{
    private:
        std::string _id;

    public:
        CreateNodeCommand() = delete;
        CreateNodeCommand(const CreateNodeCommand&) = delete;
        CreateNodeCommand(std::string id, std::string metadata);
};

class DeleteNodeCommand: public Command
{
    
};

class LinkCommand: public Command
{

};