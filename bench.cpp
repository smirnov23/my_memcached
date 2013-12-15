#include <iostream>
#include <fstream>
#include <cstdio>
#include <chrono>
#include <cstring>
#include <algorithm>
#include "inet.hpp"

#define BUFFER_SIZE 1024
#define COUNT 1000000

using namespace std;
using namespace std::chrono;

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        cout << argv[0] << " <server> <port> <text>" << endl;
        return EXIT_FAILURE;
    }

    int i, n;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in addr = inet::createAddr(argv[1], atoi(argv[2]));
    high_resolution_clock::time_point start, finish;
    double seconds[COUNT];

    int sockfd = inet::socket(SOCK_STREAM);
    inet::reuseaddr(sockfd);
    inet::connect(sockfd, addr);
    for (i=0; i<COUNT; i++)
    {
        n = sprintf(buffer,"add key%d %s", i, argv[3]);
//        n = sprintf(buffer,"set key%d %s", i, argv[3]);
//        n = sprintf(buffer,"get key%d", i);
//        n = sprintf(buffer,"incr key%d", i);
//        n = sprintf(buffer,"decr key%d", i);
//        n = sprintf(buffer,"delete key%d", i);

        start = high_resolution_clock::now();
        inet::send(sockfd, buffer, n);
        n = inet::recv(sockfd, buffer, BUFFER_SIZE-1);
        finish = high_resolution_clock::now();

        buffer[n] = '\0';
        seconds[i] = duration_cast<duration<double>>(finish-start).count();
    }
    inet::close(sockfd);

    // output times
    sort(seconds, seconds+COUNT, greater<double>());
    double sum = 0.0L;
    for(double s : seconds) sum += s;
    sprintf(buffer, "./%lu.txt", time(NULL));
    ofstream fout(buffer);
    if (!fout.is_open())
    {
        perror("Cant open stats file");
        return EXIT_FAILURE;
    }
    fout.setf(ios_base::fixed, ios_base::floatfield);
    fout.precision(6);
    fout << "max: " << *(max_element(seconds, seconds+COUNT)) << " s\n";
    fout << "min: " << *(min_element(seconds, seconds+COUNT)) << " s\n";
    fout << "avg: " << (sum/long(COUNT)) << " s\n";
    for(double s : seconds) fout << s << " s\n";

    return EXIT_SUCCESS;
}
