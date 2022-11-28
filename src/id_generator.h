#pragma once

#include <map>

template <typename TKey, typename TId>
class IdGenerator
{
    private:
        TId _currId;
        std::map<TKey, TId> _left;
        std::map<TId, TKey> _right;    
    public:
        IdGenerator(TId idStart) 
            :_currId(idStart)
        {
        }
        IdGenerator(IdGenerator const&) = delete;
        void operator=(IdGenerator const&) = delete;

        bool getId(TKey key, TId& id)
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

        bool getKey(TId id, TKey& key)
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