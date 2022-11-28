#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "imgui.h"

template <typename T>
T max(T a, T b)
{
    return a > b ? a : b;
}

template <>
ImVec2 max(ImVec2 a, ImVec2 b)
{
    return ImVec2(max(a.x, b.x), max(a.y, b.y));
}

template <char splitchar>
class StringSplitter
{
    private:
        std::stringstream _data;
    public:
        StringSplitter() = delete;
        StringSplitter(const StringSplitter& other) = delete;
        StringSplitter(std::string data)
            : _data(std::stringstream(data))
        {
        }

        std::vector<std::string> allTokens()
        {
            std::vector<std::string> tokens;
            std::string token;
            while (std::getline(_data, token, splitchar))
                tokens.push_back(token);

            return tokens;
        }
};