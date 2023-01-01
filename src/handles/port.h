#pragma once

#include <string>

#include <imgui.h>

#include <handles/node.h>

struct Port_t
{
    private:
        Node _parent;
        ImGuiID _renderId;
        std::string _name;
        std::string _type;
    public:
        Port_t() = delete;
        Port_t(const Port_t&) = delete;
        Port_t(Node parent, std::string name, std::string type):
            _parent(parent),
            _renderId(ImGui::GetID(this)),
            _name(name),
            _type(type)
        {
        }

        EditorResult prepare() { return RESULT_OK; }

        virtual EditorResult render() { return RESULT_NOT_IMPLEMENTED; }

        EditorResult shutdown() { return RESULT_OK; }

        Node parent() { return _parent; }
        std::string type() { return _type; }
        std::string name() { return _name; }
        ImGuiID renderId() { return _renderId; }
};

// TODO: Continue here...
struct InputPort_t: Port_t
{
    public:
        InputPort_t() = delete;
        InputPort_t(const InputPort_t&) = delete;
        InputPort_t(Node parent, std::string name, std::string type):
            Port_t(parent, name, type)
        {
        }

        EditorResult render() override;

        static InputPort empty();
};
typedef struct InputPort_t* InputPort;

struct OutputPort_t: Port_t
{
    public:
        OutputPort_t() = delete;
        OutputPort_t(const OutputPort_t&) = delete;
        OutputPort_t(Node parent, std::string name, std::string type):
            Port_t(parent, name, type)
        {
        }

        EditorResult render() override;

        static OutputPort empty();
};
typedef struct OutputPort_t* OutputPort;