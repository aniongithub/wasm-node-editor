#include "port.h"

#include <imnodes.h>

EditorResult InputPort_t::render()
{
    if (!parent())
        return RESULT_OK;

    ImNodes::BeginInputAttribute(renderId());
    ImGui::TextUnformatted(name().c_str());
    ImNodes::EndInputAttribute();    
    
    return RESULT_OK;
}

InputPort InputPort_t::empty()
{
    static auto _empty = std::make_unique<InputPort_t>(nullptr, "", "");
    return _empty.get();
}

EditorResult OutputPort_t::render()
{
    if (!parent())
        return RESULT_OK;

    ImNodes::BeginOutputAttribute(renderId());
    ImGui::TextUnformatted(name().c_str());
    ImNodes::EndOutputAttribute();    
    
    return RESULT_OK;
}

OutputPort OutputPort_t::empty()
{
    static auto _empty = std::make_unique<OutputPort_t>(nullptr, "", "");
    return _empty.get();
}