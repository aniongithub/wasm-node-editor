#pragma once

#include <memory>
#include <vector>

#include "node.h"
#include "commands.h"
#include "window.h"

class Graph: public Window
{
    private:
        std::string _id;
        std::vector<std::shared_ptr<Command>> _commands;

        static json nodes_data;
        static json createNodeMenu_data;

        void renderCreateNodeMenu(json category_data);
    protected:
        void renderContents() override;
    public:
        Graph(std::string id, std::string json_graph);
        void createNode(std::string node_id);
        static void registerNodes(std::string json_node_data);
};