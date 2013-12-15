#pragma once

#include <string>
#include <algorithm>
#include <sys/epoll.h>
#include "observer.hpp"
#include "connstate.hpp"
#include "iserver.hpp"

#define REQUEST_SIZE 1024
#define RESPONSE_SIZE 10240

class ConnectionState;

/**
 * GoF pattern "State", member "Context"
 */
class Connection : public Observer
{
    IServer          *_server;
    ConnectionState  *_state;
    int              _event;
    char             _request[REQUEST_SIZE];
    char             _response[RESPONSE_SIZE];

protected:
    int _sockfd;

public:
    enum {READY_READ=EPOLLIN, READY_WRITE=EPOLLOUT, ERROR=EPOLLERR, HUP=EPOLLHUP};

    Connection(IServer *s) : _server(s), _event(0), _sockfd(0)
    {
    }


    void changeState(ConnectionState *state)
    {
        _state = state;
    }


    char* getRequest()
    {
        return _request; 
    }


    bool processRequest()
    {
        return _server->process(this);
    }


    void setResponse(const std::string& r)
    {
        std::copy(r.begin(), r.end(), _response);
        _response[ r.size() ] = '\0';
    }


    char* getResponse()
    {
        return _response;
    }


    void release() 
    {
        _event = 0; 
        _server->release(this);
    }


    IServer* getServer()
    { 
        return _server; 
    }


    void toReadyRead()
    {
        if (_event == 0)
            _server->getMultiplexor()->addEvents(_sockfd, Connection::READY_READ, this);
        else
            _server->getMultiplexor()->modEvents(_sockfd, Connection::READY_READ);
        _event = Connection::READY_READ;
    }


    void toReadyWrite()
    {
        if (_event == 0)
            _server->getMultiplexor()->addEvents(_sockfd, Connection::READY_WRITE, this);
        else
            _server->getMultiplexor()->modEvents(_sockfd, Connection::READY_WRITE);
        _event = Connection::READY_WRITE;
    }


    bool isReadyRead()
    {
        return _event == Connection::READY_READ;
    }


    bool isReadyWrite()
    {
        return _event == Connection::READY_WRITE;
    }


    void setSocket(const int sockfd)
    {
        _sockfd = sockfd;
    }


    int getSocket()
    {
        return _sockfd;
    }


    virtual void handle(const int event)
    {
        _event = event;
        _state->handle(this);
    }

};

