#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <imgui.h>

#include <api.h>
#include <utils/factory.hpp>

struct PropertyEditor_t;
typedef struct PropertyEditor_t* PropertyEditor;

struct Property_t
{
    private:
        Node _parent;
        std::string _name;
        json _type_metadata;
        PropertyEditor _editor;
        ImGuiID _renderId;

        std::vector<uint8_t> _data;
    public:
        Property_t() = delete;
        Property_t(const Property_t&) = delete;
        Property_t(Node parent, std::string name, json type_metadata):
            _parent(parent),
            _name(name),
            _type_metadata(type_metadata),
            _renderId(ImGui::GetID(this)),
            _editor(nullptr)
        {
        }

        EditorResult prepare();

        EditorResult render();

        EditorResult shutdown();

        Node parent() { return _parent; }
        PropertyEditor editor() { return _editor; }

        template <typename T>
        T* getData() { return (T*)&_data[0]; }
        
        template <typename T>
        void setData(const T& data);

        size_t dataSizeBytes() { return _data.size(); }
        void allocateData(size_t dataSizeBytes) { _data.resize(dataSizeBytes); }
        json type_metadata() { return _type_metadata; }
};

typedef struct Property_t* Property;

struct PropertyEditor_t
{
    private:
        Property _parent;
        ImGuiID _renderId;
    public:
        PropertyEditor_t() = delete;
        PropertyEditor_t(const PropertyEditor&) = delete;
        PropertyEditor_t(Property parent):
            _parent(parent),
            _renderId(ImGui::GetID(this))
        {
        }

        virtual EditorResult render() 
        {
            return RESULT_NOT_IMPLEMENTED;
        }

        Property parent() { return _parent; }
        ImGuiID renderId() { return _renderId; }
};
typedef struct PropertyEditor_t* PropertyEditor;

#define REGISTER_PROPERTYEDITOR(id, type) class register##type  \
{  \
    public:  \
    register##type()  \
    {  \
        Factory<PropertyEditor_t, Property>::instance().register_class<type>( \
            id, \
            [](Property parent) -> PropertyEditor_t* \
            { \
                return new type(parent); \
            } \
        ); \
    }  \
}; \
static register##type type##Registration;

#define CREATE_PROPERTYEDITOR(id, parent) Factory<PropertyEditor_t, Property>::instance().create(id, parent)