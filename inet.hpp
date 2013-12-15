#pragma once

#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>


namespace inet
{

    inline std::string errormsg(const char *prefix)
    {
        return std::string(prefix) + std::strerror(errno);
    }


    /**
    * Create  socket
    */
    inline int socket(const int type)
    {
        int sockfd = ::socket(AF_INET, type, 0);

        if (sockfd < 0)
            throw std::runtime_error(errormsg("SOCKET: "));

        return sockfd;
    }


    /**
    * Make reusable socket
    */
    inline void reuseaddr(const int sockfd)
    {
        int val = 1;
        if (::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
            throw std::runtime_error(errormsg("SETSOCKOPT: "));
    }


    /**
    * Bind address to socket
    */
    inline void bind(const int sockfd, const int port)
    {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);

        if (::bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
            throw std::runtime_error(errormsg("BIND: "));
    }


    /**
    * Make listening socket
    */
    inline void listen(const int sockfd)
    {
        if (::listen(sockfd, SOMAXCONN) < 0)
            throw std::runtime_error(errormsg("LISTEN: "));
    }


    /**
    * Close socket
    */
    inline void close(const int& sockfd)
    {
        ::close(sockfd);
    }


    /**
    * Accept connections from clients
    */
    inline int accept(const int sockfd, struct sockaddr_in& clientaddr)
    {
        socklen_t addrlen = sizeof(clientaddr);
        int clientfd = ::accept(sockfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clientfd < 0 && errno != EWOULDBLOCK && errno != ECONNABORTED)
            throw std::runtime_error(errormsg("ACCEPT: "));

        return clientfd;
    }


    /**
    * Recv
    */
    inline ssize_t recv(const int sockfd, char *buf, const size_t len)
    {
        ssize_t n = ::recv(sockfd, buf, len, 0);

        if (n < 0 && errno != EWOULDBLOCK && errno != EINTR)
            throw std::runtime_error(errormsg("RECV: "));

        return n;
    }


    /**
    * Send
    */
    inline ssize_t send(const int sockfd, const char *buf, size_t len)
    {
        ssize_t n = ::send(sockfd, buf, len, 0);

        if (n < 0 && errno != EWOULDBLOCK && errno != EINTR)
            throw std::runtime_error(errormsg("SEND: "));

        return n;
    }


    inline void nonblock(const int sockfd)
    {
        int val = ::fcntl(sockfd, F_GETFL, 0);
        if (val < 0)
            throw std::runtime_error(errormsg("FCNTL: "));

        val = ::fcntl(sockfd, F_SETFL, val | O_NONBLOCK);
        if (val < 0)
            throw std::runtime_error(errormsg("FCNTL: "));
    }


    inline struct sockaddr_in createAddr(const char* hostname, const int port)
    {
        struct sockaddr_in addr;
        u_long iaddr = inet_addr(hostname);
        if (iaddr != INADDR_NONE)
        { 
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = iaddr;
        }
        else
        {
            hostent* h = gethostbyname(hostname);
            if (h == 0 || h->h_length < 1)
                throw std::runtime_error(errormsg("CREATE_ADDR: "));

            addr.sin_family = h->h_length;
            addr.sin_addr.s_addr = *(u_long *) h->h_addr_list[0];
        }

        addr.sin_port = htons(port);

        return addr;
    }


    inline void connect(const int sockfd, struct sockaddr_in& addr)
    {
        if ( ::connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) )
            throw std::runtime_error(errormsg("CONNECT: "));
    }
}

