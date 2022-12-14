#pragma once

#include <map>

template <typename TKey>
class IdGenerator
{
    private:
        int _currId;
        
        // TODO: This is efficient enough? Figure it out later
        std::map<TKey, int> _left;
        std::map<int, TKey> _right;    
    public:
        static IdGenerator<TKey>& instance()
        {
            static IdGenerator<TKey> instance;
            return instance;
        }

        IdGenerator(IdGenerator<TKey> const& other) = delete;
        void operator=(IdGenerator<TKey> const&) = delete;
        
        IdGenerator() 
            :_currId(1)
        {
        }

        bool getId(TKey key, int& id)
        {
            auto it = _left.find(key);
            if (it == _left.end())
            {
                id = _currId++;
                _left.insert({key, id});
                _right.insert({id, key});
            }
            else
                id = it->second;
            
            return true;
        }

        bool getKey(int id, TKey& key)
        {
            auto it = _right.find(id);
            if (it == _right.end())
                return false;
            else
            {
                key = it->second;
                return true;
            }
        }
};

using name_to_id = IdGenerator<std::string>;