#include <sys/socket.h>
#include <algorithm>
#include <cstring>
#include "connection.hpp"
#include "connstate.hpp"
#include "inet.hpp"


ConnectionState *AcceptState::_instance = nullptr;
ConnectionState *RecvState::_instance = nullptr;
ConnectionState *SendState::_instance = nullptr;


void RecvState::handle(Connection *conn)
{
    if (!conn->isReadyRead())
    {
        conn->release();
        inet::close(conn->getSocket());
        return;
    }

    char* request = conn->getRequest();
    std::fill(request, request+REQUEST_SIZE, 0);
    ssize_t n = inet::recv(conn->getSocket(), request, REQUEST_SIZE); 
    if (n > 0 && n < REQUEST_SIZE)
    {
        if ( conn->processRequest() )
        {
            conn->changeState(SendState::getInstance());
            conn->toReadyWrite();
        }
        else
        {
            conn->release();
            inet::close(conn->getSocket());
        }
    }
    else if (n == 0 || n == REQUEST_SIZE)
    {
        conn->release();
        inet::close(conn->getSocket());
    }
}


void SendState::handle(Connection *conn)
{
    if (!conn->isReadyWrite())
    {
        conn->release();
        inet::close(conn->getSocket());
        return;
    }

    char* response = conn->getResponse();
    ssize_t n = inet::send(conn->getSocket(), response, ::strlen(response)); 
    if (n > 0)
    {
        conn->changeState(RecvState::getInstance());
        conn->toReadyRead();
    }
    else if (n == 0)
    {
        conn->release();
        inet::close(conn->getSocket());
    }
} 


void AcceptState::handle(Connection *conn)
{
    if (!conn->isReadyRead())
    {
        conn->release();
        inet::close(conn->getSocket());
        return;
    }

    struct ::sockaddr_in clientAddr;
    int clientSock = inet::accept(conn->getSocket(), clientAddr);
    Connection *clientConn;
    if (clientSock > 0)
    {
        inet::nonblock(clientSock);
        clientConn = conn->getServer()->accept(clientSock);
        clientConn->changeState(RecvState::getInstance());
    }
}
