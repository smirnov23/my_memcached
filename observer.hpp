#pragma once

#include <vector>


/**
 * GoF pattern "Observer", member "Subject"
 */
struct Observer
{
    virtual void handle(const int event) = 0;
};


/**
 * GoF pattern "Observer", member "Subject"
 */
class Subject
{
    std::vector<Observer*> _observers;

protected:
    void attach(const int i, Observer *o)
    {
        if (i >= (int)_observers.size()) _observers.resize(i+1);
        _observers[i] = o;
    }


    void detach(const int i)
    {
        _observers[i] = nullptr;
    }


    void notify(const int i, const int event)
    {
        _observers[i]->handle(event);
    }
};
