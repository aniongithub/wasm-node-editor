#include "node.h"

#include <imgui.h>

#include "factory.hpp"

class FloatEditor: public PropertyEditor
{
    private:
        float _step = 0.01f, _step_fast = 1.0f;
        std::string _format = "%.3f";
    public:
        FloatEditor(std::shared_ptr<Property> parent): PropertyEditor(parent)
        {
            _step = _parent->type().value("min", _step);
            _step_fast = _parent->type().value("max", _step_fast);
            _format = _parent->type().value("format", _format);
            _parent->allocateData(sizeof(float));
        }
        void render() 
        {
            ImGui::InputFloat(fullname().c_str() , _parent->getData<float>(), _step, _step_fast, _format.c_str());
        }
};

REGISTER_PROPERTYEDITOR("float", FloatEditor);

class StringEditor: public PropertyEditor
{
    private:
        int _length = 256;
        int _lines = 1;
    public:
        StringEditor(std::shared_ptr<Property> parent): PropertyEditor(parent)
        {
            _length = _parent->type().value("length", _length) + 1;
            _parent->allocateData(_length);
        }
        void render()
        {
            ImGui::InputText(fullname().c_str(), _parent->getData<char>(), _length + 1);
        }
};

REGISTER_PROPERTYEDITOR("string", StringEditor);