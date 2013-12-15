#pragma once

#include "connection.hpp"

struct IService
{
    virtual bool process(Connection *conn) = 0;
};
