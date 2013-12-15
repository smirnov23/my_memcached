#pragma once

#include "connection.hpp"
#include "imultiplexor.hpp"

// Надо избавиться от этого класса
struct IServer
{
    virtual void run(const int port) = 0;
    virtual Connection* accept(const int sockfd) = 0;
    virtual void release(Connection *conn) = 0;
    virtual IMultiplexor* getMultiplexor() = 0;
    virtual bool process(Connection* conn) = 0;
    virtual int countActiveConn() = 0;
    virtual int sizeConnPoll() = 0;
};

