#ifndef __IOTFS_UTILS_H__
#define __IOTFS_UTILS_H__

#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

/**
 * specialized version for bool lexical_cast<bool>("true")
 */
namespace boost {
template <> bool lexical_cast<bool, std::string>(const std::string &arg) {

  if (boost::iequals(arg, std::string("true"))) {
    return true;
  }

  if (boost::iequals(arg, std::string("false"))) {
    return false;
  }
  
  throw bad_lexical_cast();  
}

}

#endif
