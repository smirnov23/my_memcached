#pragma once

#include <cctype>
#include <string>
#include <algorithm>


inline std::string& ltrim(std::string& s)
{
    using namespace std;
    s.erase(s.begin(), find_if_not(s.begin(), s.end(), ::isspace));
    return s;
}


inline std::string& rtrim(std::string& s)
{
    using namespace std;
    s.erase(find_if_not(s.rbegin(), s.rend(), ::isspace).base(), s.end());
    return s;
}


inline std::string& trim(std::string& s)
{
   return ltrim(rtrim(s));
}

