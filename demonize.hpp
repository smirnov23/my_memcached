#pragma once

#include "connection.hpp"
#include "iserver.hpp"
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>


/**
 * GoF pattern "Decorator"
 */
class DemonizeDecorator : public IServer
{
    IServer* _server;

public:
    DemonizeDecorator(IServer* s) : _server(s)
    {
    }

    virtual void run(const int port)
    {
        int fd = ::open("/dev/null", O_RDWR);
        if (fd < 0)
        {
            ::perror("/dev/null");
            ::exit(EXIT_FAILURE);
        }

        switch(::fork())
        {
            case 0:
                std::cout << "Смотри syslog\n";
                ::setsid();
                ::umask(0);
                ::close(0);
                ::close(1);
                ::close(2);
                ::dup2(fd, 0);
                ::dup2(fd, 1);
                ::dup2(fd, 2);
                close(fd);
                openlog("Simple memcached", LOG_CONS, LOG_DAEMON);
                syslog(LOG_INFO, "Started");
                _server->run(port);
                break;

            case -1:
                ::perror("fork()");
                ::exit(EXIT_FAILURE);

            default:
                ::exit(EXIT_SUCCESS);
        }
    }

    virtual Connection* accept(const int sockfd)
    {
        return _server->accept(sockfd);
    }

    virtual void release(Connection *conn)
    {
        _server->release(conn);
    }

    virtual IMultiplexor* getMultiplexor()
    {
        return _server->getMultiplexor();
    }

    virtual bool process(Connection* conn)
    {
        return _server->process(conn);
    }

    virtual int countActiveConn()
    {
        return _server->countActiveConn();
    }

    virtual int sizeConnPoll()
    {
        return _server->sizeConnPoll();
    }
};

