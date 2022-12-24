#pragma once

#include <string>
#include <memory>

#include <imgui.h>

class Window: std::enable_shared_from_this<Window>
{
    private:
        std::string _title;
        bool _allowClose;
        bool _closed;
        ImGuiWindowFlags _flags;
    protected:
        virtual void renderContents();
    public:
        Window() = delete;
        Window(std::string title, bool allowClose = true, ImGuiWindowFlags flags = 0):
            _closed(false),
            _title(title),
            _flags(flags)
        {}

        void render();

        std::string& title() { return _title; }
        ImGuiWindowFlags& windowFlags() { return _flags; }
        bool& closed() { return _closed; }
        bool& allowClose() { return _allowClose; }
};