#pragma once

#include <iostream>
#include <unordered_set>
#include <stack>
#include "connection.hpp"
#include "connstate.hpp"
#include "inet.hpp"
#include "imultiplexor.hpp"
#include "iservice.hpp"
#include "iserver.hpp"


class Server : public IServer, public Connection
{
    std::stack<Connection*>         _poll;
    std::unordered_set<Connection*> _active;
    IMultiplexor                    *_multiplexor;
    IService                        *_service;

public:
    Server(IMultiplexor *m, IService *s) : Connection(this), _multiplexor(m), _service(s)
    {
    }


    virtual void run(const int port)
    {
        _sockfd = inet::socket(SOCK_STREAM);
        inet::reuseaddr(_sockfd);
        inet::bind(_sockfd, port);
        inet::listen(_sockfd);
        inet::nonblock(_sockfd);
        toReadyRead();
        changeState(AcceptState::getInstance());
        while(true) _multiplexor->run(-1);
    }


    virtual Connection* accept(const int sockfd)
    {
        Connection *conn;

        if (_poll.empty())
        {
            conn = new Connection(this);
            conn->setSocket(sockfd);
        }
        else
        {
            conn = _poll.top();
            _poll.pop();
       }

        _active.insert(conn);
        conn->setSocket(sockfd);
        conn->toReadyRead();
 
        return conn;
    }


    virtual void release(Connection *conn)
    {
        _active.erase(conn);
        _poll.push(conn);
        _multiplexor->clearEvents(conn->getSocket());
    }


    virtual IMultiplexor* getMultiplexor()
    {
        return _multiplexor;
    }


    virtual bool process(Connection *conn)
    {
        return _service->process(conn);
    }


    virtual int countActiveConn()
    {
        return _active.size();
    }


    virtual int sizeConnPoll()
    {
        return _poll.size();
    }
};
