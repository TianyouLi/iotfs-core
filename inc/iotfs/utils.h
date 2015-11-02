#ifndef __IOTFS_UTILS_H__
#define __IOTFS_UTILS_H__

#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>

/**
 * specialized version for bool lexical_cast<bool>("true")
 */
namespace boost {
  template<>
  bool lexical_cast<bool, std::string>(const std::string& arg) {
    std::istringstream ss(arg);
    bool b;
    ss >> std::boolalpha >> b;
    return b;
  }

  template<>
  std::string lexical_cast<std::string, bool>(const bool& b) {
    std::ostringstream ss;
    ss << std::boolalpha << b;
    return ss.str();
  }
}


#endif
