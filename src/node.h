#pragma once

#include <string>
#include <string_view>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Port;
class InputPort;
class OutputPort;
class Property;
class PropertyEditor;

class Node
{
    private:
        std::string _id;
        json _node_metadata;
        std::map<std::string, InputPort> _inputs;
        std::map<std::string, OutputPort> _outputs;
        std::map<std::string, Property> _properties;
        std::shared_ptr<PropertyEditor> _stringEditor;

    protected:
        Node() {}
    public:
        Node(std::string id, json node_metadata);
        virtual void render();
        virtual void renderProperties();

        const std::string& id() { return _id; }
        static Node& empty()
        {
            static Node _empty;
            return _empty;
        }

        std::map<std::string, InputPort>& inputs() { return _inputs; }
        std::map<std::string, OutputPort>& outputs() { return _outputs; }
        std::map<std::string, Property>& properties() { return _properties; }

        std::string name();
};

class Port
{
    protected:
        Node& _parent;
        std::string _name;
        std::string _type;
        Port():
            _parent(Node::empty()) {}
    public:
        Port(Node& parent, std::string name, std::string type):
            _parent(parent),
            _name(name),
            _type(type)
        {}

        virtual void render() = 0;

        std::string fullName();
        const std::string& type() { return _type; }
};

class InputPort: public Port
{
    public:
        InputPort() {}
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
    public:
        OutputPort() {}
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
        Node& _parent;
        std::string _name;
        json _type;
        std::vector<uint8_t> _data;
        std::shared_ptr<PropertyEditor> _editor;
    public:
        Property():
            _parent(Node::empty()) {}
        Property(Node& parent, std::string name, json type);
        void render();

        const json& type() { return _type; }
        
        template <typename T>
        T* getData() { return (T*)&_data[0]; }
        
        template <typename T>
        void setData(T& data);

        size_t dataSizeBytes() { return _data.size(); }
        void allocateData(size_t dataSizeBytes) { _data.resize(dataSizeBytes); }

        std::string fullname();
};

class PropertyEditor
{
    protected:
        Property& _parent;
    public:
        PropertyEditor(Property& parent): 
            _parent(parent)
        {}
        virtual void render() {}
        std::string fullname();
};

#define REGISTER_PROPERTYEDITOR(id, type) class register##type  \
{  \
    public:  \
    register##type()  \
    {  \
        Factory<PropertyEditor, Property&>::instance().register_class<type>( \
            id, \
            [](Property& parent) -> std::unique_ptr<PropertyEditor> \
            { \
                return std::make_unique<type>(parent); \
            } \
        ); \
    }  \
}; \
static register##type type##Registration;

#define CREATE_PROPERTYEDITOR(id, parent) Factory<PropertyEditor, Property&>::instance().create(id, parent)