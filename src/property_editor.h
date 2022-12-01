#pragma once

#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class PropertyEditor
{
    private:
        json _args;
    public:
        virtual void render() {}

        json& args() { return _args; }
};