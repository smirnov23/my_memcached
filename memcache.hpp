#pragma once

#include <string>
#include <cstring>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include <unistd.h>
#include <ctime>
#include <stdexcept>
#include <cctype>

#include "iservice.hpp"
#include "hashtable.hpp"
#include "trim.hpp"


class Memcache : public IService
{
    HashTable _storage;

    void parse(const char *rawRequest, std::string& cmd, std::string& args)
    {
        std::string request(rawRequest);

        trim(request);

        if (request.empty()) return;

        std::istringstream iss(request);
        iss >> cmd;
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        std::getline(iss, args);
    } 


public:
    bool process(Connection *conn)
    {
        std::string cmd, args, key;
        char* response = conn->getResponse();

        parse(conn->getRequest(), cmd, args);
        std::istringstream carg(args);

        if (cmd == "add")
        {
            carg >> key;

            std::string value;
            std::getline(carg, value);
            trim(value);

            if ( _storage.exists(key) )
                ::strcpy(response, "EXISTS\r\n");
            else try
            {
                _storage.add(key,value);
                ::strcpy(response, "STORED\r\n");
            }
            catch (...)
            {
                ::strcpy(response, "NOT_STORED\r\n");
            }

            return true;
        }

        if (cmd == "set")
        {
            carg >> key;

            std::string value;
            std::getline(carg, value);
            trim(value);

            if ( !_storage.exists(key) )
                ::strcpy(response, "NOT_FOUND\r\n");
            else try
            {
                _storage.set(key,value);
                ::strcpy(response, "STORED\r\n");
            }
            catch (...)
            {
                ::strcpy(response, "NOT_STORED\r\n");
            }

            return true;
        }

        if (cmd == "get")
        {
            carg >> key;
            try
            {
                std::string& value = _storage.get(key);
                ::sprintf(response, "VALUE %s\r\n", value.c_str());
            }
            catch(std::out_of_range& e)
            {
                ::strcpy(response, "NOT_FOUND\r\n");
            }

            return true;
        }

        if (cmd == "delete")
        {
            carg >> key;
            _storage.del(key);
            ::strcpy(response, "DELETED\r\n");

            return true;
        }

        if (cmd == "incr")
        {
            carg >> key;
            ::sprintf(response, "VALUE %s\r\n", _storage.incr(key).c_str());
            return true;
        }

        if (cmd == "decr")
        {
            carg >> key;
            ::sprintf(response, "VALUE %s\r\n", _storage.decr(key).c_str());
            return true;
        }

        if (cmd == "stats")
        {
            ::sprintf(response, "pid:\t\t\t%d\r\n"
                                "time:\t\t\t%ld\r\n"
                                "curr_items:\t\t%ld\r\n"
                                "threads:\t\t%d\r\n"
                                "curr_connections:\t%d\r\n"
                                "connection_structures:\t%d\r\n"
                                "END\r\n",
                                ::getpid(), ::time(NULL), _storage.size(), 1, conn->getServer()->countActiveConn(), conn->getServer()->sizeConnPoll());

            return true;
        }

        if (cmd == "flush_all")
        {
            _storage.flush_all();
            ::sprintf(response, "OK\r\n");
            return true;
        }

        return false;
    }
};

