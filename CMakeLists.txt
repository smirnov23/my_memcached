SET(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++11")

add_executable(memcached main.cpp connstate.cpp)
add_executable(bench bench.cpp)
