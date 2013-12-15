#pragma once

#include <string>
#include <stdexcept>
#include <cstring>
#include <cerrno>
#include <sys/epoll.h>
#include <unistd.h>

#include "observer.hpp"
#include "connection.hpp"
#include "imultiplexor.hpp"


class Epoll : public IMultiplexor, public Subject
{
    int           _epollfd;
    int           _eventsSize;
    ::epoll_event *_events;


public:

    Epoll(const int eventsSize = 1024) : Subject(), _epollfd(0), _eventsSize(eventsSize)
    {
        _epollfd = ::epoll_create(_eventsSize);

        if (_epollfd < 0)
            throw std::runtime_error(std::string("epoll_create: ") + std::strerror(errno));

        _events = new epoll_event[_eventsSize];
    }


    virtual ~Epoll()
    {
        ::close(_epollfd);
        delete [] _events;
    }


    virtual void addEvents(const int sockfd, const int eventVals, Observer *o)
    {
        epoll_event e;
        e.data.fd = sockfd;
        e.events = eventVals;

        if (::epoll_ctl(_epollfd, EPOLL_CTL_ADD, sockfd, &e) < 0)
            throw std::runtime_error(std::string("epoll_ctl") + std::strerror(errno));

        attach(sockfd, o);
    }


    virtual void modEvents(const int sockfd, const int eventVals)
    {
        epoll_event e;
        e.data.fd = sockfd;
        e.events = eventVals;

        if (::epoll_ctl(_epollfd, EPOLL_CTL_MOD, sockfd, &e) < 0)
            throw std::runtime_error(std::string("epoll_ctl") + std::strerror(errno));
    }


    virtual void clearEvents(const int sockfd)
    {
        if (::epoll_ctl(_epollfd, EPOLL_CTL_DEL, sockfd, NULL) < 0)
            throw std::runtime_error(std::string("epoll_ctl") + std::strerror(errno));

        detach(sockfd);
    }


    virtual void run(const int timeout)
    {
        int eventsQty = ::epoll_wait(_epollfd, _events, _eventsSize, timeout);

        if (eventsQty < 0)
            throw std::runtime_error(std::string("epoll_wait: ") + std::strerror(errno));

        for (int i=0; i < eventsQty; i++)
        {
            notify(_events[i].data.fd, _events[i].events);
        }
    }
};
