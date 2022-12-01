#include "property_editor.h"

#include "factory.hpp"

class FloatPropertyEditor: public PropertyEditor
{
    public:
        FloatPropertyEditor()
        {
        }
        virtual void render() {}
};

REGISTER_TYPE("float", FloatPropertyEditor, PropertyEditor);