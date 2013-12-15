#pragma once

#include "observer.hpp"

struct IMultiplexor
{
    virtual void addEvents(const int sockfd, const int eventVals, Observer *o) = 0;
    virtual void modEvents(const int sockfd, const int eventVals) = 0;
    virtual void clearEvents(const int sockfd) = 0;
    virtual void run(const int timeout) = 0;
};

