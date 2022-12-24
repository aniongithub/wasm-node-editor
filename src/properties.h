#pragma once

#include "window.h"

class Properties: public Window
{
    protected:
        void renderContents();
    public:
        Properties():
            Window("Properties")
        {}
};