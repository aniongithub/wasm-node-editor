#include "property.h"

#include <api.h>
#include <utils/factory.hpp>
#include <handles/property.h>

#pragma region Float 
struct FloatEditor: PropertyEditor_t
{
    private:
        float _step = 0.01f, _step_fast = 1.0f;
        std::string _format = "%.3f";
    public:
        FloatEditor(Property parent): 
            PropertyEditor_t(parent)
        {
            _step = this->parent()->type_metadata().value("min", _step);
            _step_fast = this->parent()->type_metadata().value("max", _step_fast);
            _format = this->parent()->type_metadata().value("format", _format);
            this->parent()->allocateData(sizeof(float));
        }
        EditorResult render()
        {
            ImGui::InputFloat(label().c_str(), parent()->getData<float>(), _step, _step_fast, _format.c_str());
            return RESULT_OK;
        }
};

REGISTER_PROPERTYEDITOR("float", FloatEditor);

class StringEditor: public PropertyEditor_t
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
        StringEditor(Property parent): 
            PropertyEditor_t(parent)
        {
            _length = this->parent()->type_metadata().value("length", _length);
            this->parent()->allocateData(_length);
        }

        EditorResult render()
        {
            auto oldName = std::string(parent()->getData<char>());
            if (ImGui::InputText(label().c_str(), 
                parent()->getData<char>(), _length, ImGuiInputTextFlags_CallbackResize, 
                resizeCallback, this))
            {
                // Nothing to do here...
            }

            return RESULT_OK;
        }
};

REGISTER_PROPERTYEDITOR("string", StringEditor);

class NameEditor: public PropertyEditor_t
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
        NameEditor(Property parent): 
            PropertyEditor_t(parent)
        {
            _length = this->parent()->type_metadata().value("length", _length);
            this->parent()->allocateData(_length);
        }
        EditorResult render()
        {
            if (!_name_initialized)
            {
                _tempName = std::string(this->parent()->getData<char>());
                _name_initialized = true;
            }
            
            if (ImGui::InputText(label().c_str(), &_tempName[0], _length, ImGuiInputTextFlags_CallbackResize | ImGuiInputTextFlags_EnterReturnsTrue, 
                resizeCallback, this))
            {
                // printf("Old name was: %s, new name is %s\n", this->parent()->getData<char>(), this->_tempName.c_str());
                auto currName = std::string(this->parent()->getData<char>());
                this->parent()->setData(_tempName);
            }
            
            return RESULT_OK;
        }
};

REGISTER_PROPERTYEDITOR("name_editor", NameEditor);