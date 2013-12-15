#pragma once

#include "connection.hpp"

class Connection;

/**
 * GoF pattern "State", member "State"
 */
struct ConnectionState
{
    virtual void handle(Connection *conn) = 0;
};


/**
 * @class SendState
 */
class SendState : public ConnectionState
{
    static ConnectionState *_instance;
    SendState() {}

public:
    static ConnectionState* getInstance()
    {
        if (_instance == nullptr) _instance = new SendState();
        return _instance;
    }
    virtual void handle(Connection *conn);
};


/**
 * @class RecvState
 */
class RecvState : public ConnectionState
{
    static ConnectionState *_instance;
    RecvState() {}

public:
    static ConnectionState* getInstance()
    {
        if (_instance == nullptr) _instance = new RecvState();
        return _instance;
    }
    virtual void handle(Connection *conn);
};


/**
 * @class AcceptState
 */
class AcceptState : public ConnectionState
{
    static ConnectionState *_instance;
    AcceptState() {}

public:
    static ConnectionState* getInstance()
    {
        if (_instance == nullptr) _instance = new AcceptState();
        return _instance;
    }
    virtual void handle(Connection *conn);
};
