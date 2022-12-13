#pragma once

#include "node.h"

class Link {
    private:
        std::shared_ptr<Node> _start_node;
        std::shared_ptr<OutputPort> _start_port;
        std::shared_ptr<Node> _end_node;
        std::shared_ptr<InputPort> _end_port;
    public:
        Link(std::shared_ptr<Node> start_node, 
             std::shared_ptr<OutputPort> start_port, 
             std::shared_ptr<Node> end_node, 
             std::shared_ptr<InputPort> end_port):
            _start_node(start_node),
            _start_port(start_port),
            _end_node(end_node),
            _end_port(end_port)
        {
        }

        std::shared_ptr<Node> start_node() { return _start_node; }
        std::shared_ptr<OutputPort> start_port() { return _start_port; }
        std::shared_ptr<Node> end_node() { return _end_node; }
        std::shared_ptr<InputPort> end_port() { return _end_port; }

        std::string fullName();
};