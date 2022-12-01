#pragma once

#include <functional>
#include <map>
#include <string>
#include <memory>

template <typename TBase>
class Factory
{
    private:
        std::map<std::string, std::function<std::unique_ptr<TBase>()>> _registered;

        Factory() {}
    public:
        static Factory<TBase>& instance()
        {
            static Factory<TBase> instance;
            return instance;
        }

        Factory(Factory const& other) = delete;
        void operator=(Factory const&) = delete;
        
        template <typename TDerived>
        void register_class(std::string id, std::function<std::unique_ptr<TBase>()> createFunc)
        {
            _registered[id] = createFunc;
        }

        std::unique_ptr<TBase> create(std::string id)
        {
            auto it = _registered.find(id);
            if (it != _registered.end())
                return it->second();
            else
                return std::make_unique<TBase>();
        }
};

#define REGISTER_TYPE(id, type, base) class register##type { public: register##type() { Factory<base>::instance().register_class<type>(id, []()->std::unique_ptr<base> { return std::make_unique<type>(); });} }; static register##type global_##type##Registration;