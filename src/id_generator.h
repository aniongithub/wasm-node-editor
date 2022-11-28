#pragma once

#include <boost/bimap.hpp>

template <typename TKey, typename TId>
class IdGenerator
{
    private:
        TId _currId;
        boost::bimap<TKey, TId> _bimap;
    public:
        IdGenerator(TId idStart) 
            :_currId(idStart)
        {
        }
        IdGenerator(IdGenerator const&) = delete;
        void operator=(IdGenerator const&) = delete;

        bool getId(TKey key, TId& id)
        {
            auto it = _bimap.left.find(key);
            if (it == _bimap.left.end())
            {
                id = _currId++;
                _bimap.insert({key, id});
            }
            else
                id = it->second;
            
            return true;
        }

        bool getKey(TId id, TKey& key)
        {
            auto it = _bimap.right.find(id);
            if (it == _bimap.right.end())
                return false;
            else
            {
                key = it->second;
                return true;
            }
        }
};