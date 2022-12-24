#include "window.h"

void Window::render()
{
    ImGui::Begin(_title.c_str(), _allowClose? &_closed : nullptr, _flags);

    this->renderContents();

    ImGui::End();
}

void Window::renderContents()
{
    // Nothing to do here
}