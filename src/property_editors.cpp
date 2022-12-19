#include "node.h"

#include <imgui.h>

#include "factory.hpp"
#include "id_generator.h"

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

        static int resizeCallback(ImGuiInputTextCallbackData* data)
        {
            auto stringEditor = static_cast<StringEditor*>(data->UserData);
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
            {
                stringEditor->parent()->allocateData(data->BufSize);
                data->Buf = stringEditor->parent()->getData<char>();
            }
            return 0;
        }
    public:
        StringEditor(std::shared_ptr<Property> parent): PropertyEditor(parent)
        {
            _length = _parent->type().value("length", _length);
            _parent->allocateData(_length);
        }
        void render()
        {
            auto oldName = std::string(_parent->getData<char>());
            if (ImGui::InputText(fullname().c_str(), _parent->getData<char>(), _length, ImGuiInputTextFlags_CallbackResize, 
                resizeCallback, this))
            {
                name_to_id::instance().renameKey(oldName, std::string(_parent->getData<char>()));
            }
        }
};

REGISTER_PROPERTYEDITOR("string", StringEditor);

class NameEditor: public PropertyEditor
{
    private:
        int _length = 256;
        int _lines = 1;
        std::string _tempName;
        bool _name_initialized = false;

        static int resizeCallback(ImGuiInputTextCallbackData* data)
        {
            auto nameEditor = static_cast<NameEditor*>(data->UserData);
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
            {
                nameEditor->_tempName.resize(data->BufSize);
                data->Buf = &nameEditor->_tempName[0];
            }
            return 0;
        }
    public:
        NameEditor(std::shared_ptr<Property> parent): PropertyEditor(parent)
        {
            _length = _parent->type().value("length", _length);
            _parent->allocateData(_length);
        }
        void render()
        {
            if (!_name_initialized)
            {
                _tempName = std::string(this->parent()->getData<char>());
                _name_initialized = true;
            }
            
            if (ImGui::InputText(fullname().c_str(), &_tempName[0], _length, ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_EnterReturnsTrue, 
                resizeCallback, this))
            {
                // printf("Old name was: %s, new name is %s\n", this->parent()->getData<char>(), this->_tempName.c_str());
                auto currName = std::string(this->parent()->getData<char>());
                this->parent()->setData(_tempName);
                name_to_id::instance().renameKey(currName, _tempName);
            }
        }
};

REGISTER_PROPERTYEDITOR("name_editor", NameEditor);