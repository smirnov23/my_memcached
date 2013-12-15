#pragma once

#include <string>
#include <unordered_map>
#include <unistd.h>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include "istorage.hpp"


class HashTable : public IStorage
{
    std::unordered_map<std::string, std::string> container;

public:

    virtual bool exists(const std::string& key)
    {
        return container.count(key) > 0;
    }


    virtual void add(const std::string& key, const std::string& value)
    {
        container.insert({key, value});
    }


    virtual void set(const std::string& key, const std::string& value)
    {
        container[key] = value;
    }


    virtual std::string& get(const std::string& key)
    {
        return container.at(key);
    }


    virtual std::string& incr(const std::string& key)
    {
        std::string& value = container[key];
        std::string::iterator it = value.begin();

        if (*it == '+' || *it == '-') ++it;

        if (std::find_if_not(it, value.end(), ::isdigit) == value.end())
        {
            long long number = atoll(value.c_str());
            if (number!=0 || errno==0)
            {
                char buffer[sizeof(number)+1];
                ++number;
                sprintf(buffer, "%lld", number);
                value = buffer;
            }
        }

        return value;
    }


    virtual std::string& decr(const std::string& key)
    {
        std::string& value = container[key];
        std::string::iterator it = value.begin();
        if (*it == '+' || *it == '-') ++it;

        if (std::find_if_not(it, value.end(), ::isdigit) == value.end())
        {
            long long number = atoll(value.c_str());
            if (number!=0 || errno==0)
            {
                char buffer[sizeof(number)+1];
                --number;
                sprintf(buffer, "%lld", number);
                value = buffer;
            }
        }

        return value;
    }


    virtual void del(const std::string& key)
    {
        container.erase(key);
    }


    virtual long size()
    {
        return container.size();
    }


    virtual void flush_all()
    {
        container.clear();
    }

};
