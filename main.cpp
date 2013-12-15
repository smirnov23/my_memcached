#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "epoll.hpp"
#include "server.hpp"
#include "memcache.hpp"
#include "demonize.hpp"


int main(int argc, char **argv)
try
{
    if (argc != 2)
    {
        std::cout << argv[0] << " <portno>" << std::endl;
        return 0;
    }

    Epoll    mux;
    Memcache mc;
    Server   server(&mux, &mc);
    DemonizeDecorator daemon(&server);
    daemon.run(atoi(argv[1]));
}
catch (std::exception& e)
{
    std::cout << e.what() << std::endl;
}

