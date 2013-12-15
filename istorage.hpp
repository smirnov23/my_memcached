#pragma once

#include <string>

struct IStorage
{
    virtual bool exists(const std::string& key) = 0;
    virtual void add(const std::string& key, const std::string& value) = 0;
    virtual void set(const std::string& key, const std::string& value) = 0;
    virtual std::string& get(const std::string& key) = 0;
    virtual std::string& incr(const std::string& key) = 0;
    virtual std::string& decr(const std::string& key) = 0;
    virtual void del(const std::string& key) = 0;
    virtual long size() = 0;
    virtual void flush_all() = 0;
};
