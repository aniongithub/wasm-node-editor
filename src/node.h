#pragma once

#include <memory>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Port;
class InputPort;
class OutputPort;
class Property;
class PropertyEditor;

// From https://stackoverflow.com/questions/33069674/is-this-trick-to-make-calling-shared-from-this-in-the-constructor-just-work
#define ENABLE_SHARED_FROM_THIS_IN_CTOR(type) auto ptr = std::shared_ptr<type>( this, [](type*){} )

class Node: public std::enable_shared_from_this<Node>
{
    private:
        std::string _id;
        int _render_id;
        json _node_metadata;
        std::map<std::string, std::shared_ptr<InputPort>> _inputs;
        std::map<std::string, std::shared_ptr<OutputPort>> _outputs;
        std::map<std::string, std::shared_ptr<Property>> _properties;
        std::shared_ptr<PropertyEditor> _stringEditor;

    public:
        Node() = delete;
        Node(const Node&) = delete;
        Node(std::string id, json node_metadata);
        virtual void render();
        virtual void renderProperties();

        const std::string& id() { return _id; }
        static std::shared_ptr<Node> empty()
        {
            static auto _empty = std::make_shared<Node>("", json());
            return _empty;
        }

        std::map<std::string, std::shared_ptr<InputPort>>& inputs() { return _inputs; }
        std::map<std::string, std::shared_ptr<OutputPort>>& outputs() { return _outputs; }
        std::map<std::string, std::shared_ptr<Property>>& properties() { return _properties; }

        std::string name();
        int render_id() { return _render_id; }
};

class Port: public std::enable_shared_from_this<Port>
{
    protected:
        std::shared_ptr<Node> _parent;
        std::string _name;
        std::string _type;
        int _render_id;
    public:
        Port() = delete;
        Port(const Port&) = delete;
        Port(std::shared_ptr<Node> parent, std::string name, std::string type);

        virtual void render() = 0;

        const std::string& type() { return _type; }
        int render_id() { return _render_id; }
};

class InputPort: public Port
{
    public:
        InputPort() = delete;
        InputPort(const InputPort&) = delete;
        InputPort(std::shared_ptr<Node> parent, std::string name, std::string type):
            Port(parent, name, type)
        {
            ENABLE_SHARED_FROM_THIS_IN_CTOR(InputPort);
        }
        void render();
        static std::shared_ptr<InputPort> empty()
        {
            static auto _empty = std::make_shared<InputPort>(Node::empty(), "", "");
            return _empty;
        }
};

class OutputPort: public Port
{
    public:
        OutputPort() = delete;
        OutputPort(const OutputPort&) = delete;
        OutputPort(std::shared_ptr<Node> parent, std::string name, std::string type):
            Port(parent, name, type)
        {
            ENABLE_SHARED_FROM_THIS_IN_CTOR(OutputPort);
        }
        void render();
        static std::shared_ptr<OutputPort> empty()
        {
            static auto _empty = std::make_shared<OutputPort>(Node::empty(), "", "");
            return _empty;
        }
};

class Property: public std::enable_shared_from_this<Property>
{
    private:
        std::shared_ptr<Node> _parent;
        std::string _name;
        json _type;
        std::vector<uint8_t> _data;
        std::shared_ptr<PropertyEditor> _editor;
        int _render_id;
    public:
        Property() = delete;
        Property(const Property&) = delete;
        Property(std::shared_ptr<Node> parent, std::string name, json type);

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

class PropertyEditor: public std::enable_shared_from_this<PropertyEditor>
{
    protected:
        std::shared_ptr<Property> _parent;
    public:
        PropertyEditor() = delete;
        PropertyEditor(const PropertyEditor&) = delete;
        PropertyEditor(std::shared_ptr<Property> parent): 
            _parent(parent)
        {
            ENABLE_SHARED_FROM_THIS_IN_CTOR(PropertyEditor);
        }

        virtual void render() {}
        std::string fullname();
        std::shared_ptr<Property> parent() { return _parent; }
};

#define REGISTER_PROPERTYEDITOR(id, type) class register##type  \
{  \
    public:  \
    register##type()  \
    {  \
        Factory<PropertyEditor, std::shared_ptr<Property>>::instance().register_class<type>( \
            id, \
            [](std::shared_ptr<Property> parent) -> std::unique_ptr<PropertyEditor> \
            { \
                return std::make_unique<type>(parent); \
            } \
        ); \
    }  \
}; \
static register##type type##Registration;

#define CREATE_PROPERTYEDITOR(id, parent) Factory<PropertyEditor, std::shared_ptr<Property>>::instance().create(id, parent)