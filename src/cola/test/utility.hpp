#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cassert>
#include <sstream>

template<typename T> 
inline std::string to_string(const T& value)
{
    std::stringstream ss;
    ss << value; 
    return ss.str();
}

template<typename T> 
inline std::string to_string(const std::vector<T>& value)
{
    std::stringstream ss;
    ss << "[";
    for(typename std::vector<T>::const_iterator it = value.begin(); it != value.end(); ++it) {
        ss << *it << ((it == value.end()-1)? "" : ", ");
    }
    ss << "]";
    return ss.str();
}

inline void success(const std::string& msg)
{
    std::cout << "\033[34m" << msg << "\033[m" << std::endl;
}

#endif // UTILITY_HPP
