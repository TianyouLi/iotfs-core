#ifndef __IOTFS__IOTFS_ROOT_DIRECTORY_H
#define __IOTFS__IOTFS_ROOT_DIRECTORY_H

#include <fusekit/daemon.h>
using namespace fusekit;

namespace iotfs {

//!
/*!
  Base class for all folder object whatever standard/protocol plugin
  implements. This class could be used for further implementation of
  security features and others.
 */
class IOTFolder : public default_directory<>::type {
public:
  virtual ~IOTFolder(){};

  //!
  /*!
    The generator function basically will make a child folder of
    same type. For example, OIC folder will create another OIC folder
    by using this call.

    \param name	the name of child folder will be created
  */
  virtual IOTFolder &makeChildFolder(const std::string &name);
};
}

#endif
