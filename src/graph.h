#pragma once

#include "node.h"

class Link {
    private:
        Node& _start_node;
        OutputPort& _start_port;
        Node& _end_node;
        InputPort& _end_port;
    public:
        Link(Node& start_node, OutputPort& start_port, Node& end_node, InputPort& end_port):
            _start_node(start_node),
            _start_port(start_port),
            _end_node(end_node),
            _end_port(end_port)
        {
        }

        Node& start_node() { return _start_node; }
        OutputPort& start_port() { return _start_port; }
        Node& end_node() { return _end_node; }
        InputPort& end_port() { return _end_port; }

        std::string fullName();
};