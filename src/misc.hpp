/*
  Utilities for c++
 */

#ifndef _MY_MISC_HEADER
#define _MY_MISC_HEADER

#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <cstring>

#include "cola/cola.hpp"

// Simple debugging output
#define dbg(...) _dbg(#__VA_ARGS__, __VA_ARGS__)
inline void _dbg(std::string) {
  std::cout << std::endl;
}
template<class H,class... T>
inline void _dbg(std::string s, H h, T... t) {
  int l = s.find(',');
  std::cout << s.substr(0, l) << " = " << h << ", ";
  _dbg(s.substr(l+1), t...);
}
template<class T,class U>
inline std::ostream& operator<<(std::ostream &o, const std::pair<T,U> &p) {
  o << "(" << p.first << "," << p.second << ")";
  return o;
}
template<class T>
inline std::ostream& operator<<(std::ostream &o, const std::vector<T> &v) {
  o << "[";
  for (T item : v) {
    o << item << ",";
  }
  o << "]";
  return o;
}
template<class T, class U>
inline std::ostream& operator<<(std::ostream &o, const std::map<T,U> &m) {
  o << "[";
  for (auto item : m) {
    o << item << ",";
  }
  o << "]";
  return o;
}

// Join to string
template <class T>
std::string join(const std::vector<T> &v, const char* delim = "") {
  std::ostringstream os;
  for(auto &i : v){ os << i << delim; }
  std::string str = os.str();
  str.erase(str.size() - strlen(delim));
  return str;
}
template <class T>
std::string join(const std::vector<T> &v, const char delim){
  return join(v, std::string(1, delim).c_str());
}


// Simple coloring in stdout
// http://d.hatena.ne.jp/aki-yam/20081001/1222851138
// TODO consider to use https://github.com/ikalnytskyi/termcolor
namespace ioscc {
  enum TColorCode {red=1, blue, green};
  static const char *_sesc    = "\033[";
  static const char *_sreset  = "0m";
  static const char *_sred    = "31;1m";
  static const char *_sblue   = "34;1m";
  static const char *_sgreen  = "32;1m";
};
struct _colored_stream{std::ostream &os;};
inline _colored_stream operator<< (std::ostream &os, ioscc::TColorCode ccode) {
  switch (ccode){
    case ioscc::red    : os<<ioscc::_sesc<<ioscc::_sred;   break;
    case ioscc::blue   : os<<ioscc::_sesc<<ioscc::_sblue;  break;
    case ioscc::green  : os<<ioscc::_sesc<<ioscc::_sgreen; break;
  }
  _colored_stream colos={os};
  return colos;
}
template <typename T>
inline _colored_stream operator<< (_colored_stream os, const T &rhs) {
  os.os << rhs;
  return os;
};
template <>
inline _colored_stream operator<< (_colored_stream os, const ioscc::TColorCode &ccode) {
  os.os << ioscc::_sesc << ioscc::_sreset;
  os.os << ccode;
  return os;
};
inline _colored_stream operator<< (_colored_stream os, std::ostream& (*__pf)(std::ostream&)) {
  os.os << ioscc::_sesc << ioscc::_sreset;
  __pf(os.os);
  return os;
};


// Simple Timer
namespace mytimer {
  class timer {
  private:
    std::chrono::system_clock::time_point startTime;
    inline std::chrono::system_clock::time_point getTime() {
      return std::chrono::system_clock::now();
    }
  public:
    inline void set() {
      startTime = getTime();
    }
    inline double getMsec() {
      auto dur = getTime() - startTime;
      auto msec = std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count();
      return msec / 1000.0 / 1000.0;
    }
    inline double getSec() {
      return (double)getMsec()/1000.0;
    }
    inline double get() {
      return getSec();
    }
    inline void print(const std::string str= "") {
      double t = get();
      if(t > 1) std::cout << str << " " << t << " sec." << std::endl;
      else    std::cout << str << " " << t*1000.0 << "msec." << std::endl;
    }
    timer() {
      set();
    }
  };
};

#endif // _MY_MISC_HEADER
