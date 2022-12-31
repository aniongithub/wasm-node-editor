#pragma once

#include <functional>
#include <map>
#include <string>
#include <memory>

template <typename TBase, typename... Args>
class Factory
{
    private:
        std::map<std::string, std::function<TBase*(Args... args)>> _registered;

        Factory() {}
    public:
        static Factory<TBase, Args...>& instance()
        {
            static Factory<TBase, Args...> instance;
            return instance;
        }

        Factory(Factory const& other) = delete;
        void operator=(Factory const&) = delete;
        
        template <typename TDerived>
        void register_class(std::string id, std::function<TBase*(Args... args)> createFunc)
        {
            _registered[id] = createFunc;
        }

        TBase* create(std::string id, Args... args)
        {
            auto it = _registered.find(id);
            if (it != _registered.end())
                return it->second(args...);
            else
                return new TBase(args...);
        }
};