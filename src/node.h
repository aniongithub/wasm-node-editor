#pragma once

#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "property_editor.h"

class Port;
class InputPort;
class OutputPort;
class Property;

class Node
{
    private:
        std::string _id;
        std::string _name;
        int _id_int;
        float _output_ports_offset;
        json _node_metadata;
        std::vector<InputPort> _inputs;
        std::vector<OutputPort> _outputs;
        std::vector<Property> _properties;
    protected:
        Node(){}
    public:
        Node(std::string id, json node_metadata);
        virtual void render();
        virtual void renderProperties();

        std::string& name() { return _name; }
        const std::string& id() { return _id; }
        const int int_id() { return _id_int; }
        static Node& empty()
        {
            static Node _empty;
            return _empty;
        }

        std::vector<InputPort>& inputs() { return _inputs; }
        std::vector<OutputPort>& outputs() { return _outputs; }
        std::vector<Property>& properties() { return _properties; }
};

class Port
{
    protected:
        int _id_int;
        Node& _parent;
        std::string _name;
        std::string _type;
        Port():
            _parent(Node::empty()) {}
    public:
        Port(Node& parent, std::string name, std::string type);
        virtual void render() = 0;

        const std::string& name() { return _name; }
        const std::string& type() { return _type; }
        const int int_id() { return _id_int; }
};

class InputPort: public Port
{
    private:
        InputPort() {}
    public:
        InputPort(Node& parent, std::string name, std::string type):
            Port(parent, name, type)
        {}
        
        void render();
        static InputPort& empty()
        {
            static InputPort _empty;
            return _empty;
        }
};

class OutputPort: public Port
{
    private:
        OutputPort() {}
    public:
        OutputPort(Node& parent, std::string name, std::string type):
            Port(parent, name, type)
        {}
        void render();
        static OutputPort& empty()
        {
            static OutputPort _empty;
            return _empty;
        }
};

class Property
{
    private:
        int _id_int;
        Node& _parent;
        std::string _name;
        json _type;
        std::vector<uint8_t> _data;
        std::shared_ptr<PropertyEditor> _editor;
        Property():
            _parent(Node::empty()) {}
    public:
        Property(Node& parent, std::string name, json type);
        void render();

        const std::string& name() { return _name; }
        const json& type() { return _type; }
        const int int_id() { return _id_int; }
};