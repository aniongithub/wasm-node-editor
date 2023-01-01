#include "property.h"

#include <string>
#include <algorithm>

#include <handles/node.h>

template <typename T>
void Property_t::setData(const T& data)
{
    allocateData(sizeof(data));
    memcpy(getData<T>(), &data, sizeof(data));
}

// Specializations for specific types
template <>
void Property_t::setData(const std::string& value)
{
    allocateData(value.size() + 1);
    memcpy(getData<char>(), value.c_str(), value.size() + 1);
}

EditorResult Property_t::prepare()
{
    // Deal with simple typenames
    if (type_metadata().is_string())
    {
        auto type = type_metadata().get<std::string>();
        _type_metadata = json({ {"type", type} });
    }
    
    auto type_name = type_metadata().value("type", "unknown");
    if (type_name == "unknown")
        return RESULT_UNKNOWN_TYPE;

    auto editor_name = type_metadata().value("editor", type_name);

    _editor = CREATE_PROPERTYEDITOR(editor_name.c_str(), this);
    return RESULT_OK;
}

EditorResult Property_t::render()
{
    if (_parent == Node_t::empty())
        return RESULT_OK;

    ImGui::TableNextRow();

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(_name.c_str());
    
    ImGui::TableNextColumn();
    ImGui::PushItemWidth(-1);
    if (_editor)
        _editor->render();
    ImGui::PopItemWidth();
    
    return RESULT_OK;
}

EditorResult Property_t::shutdown()
{
    if (_editor)
    {
        delete _editor;
        _editor = nullptr;
    }
    return RESULT_OK;
}