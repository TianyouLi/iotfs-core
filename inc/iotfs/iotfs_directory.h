#ifndef __IOTFS__IOTFS_ROOT_DIRECTORY_H
#define __IOTFS__IOTFS_ROOT_DIRECTORY_H

#include <fusekit/daemon.h>
using namespace fusekit;

namespace iotfs {
  
class IOTFolder : public default_directory<>::type {
 public:
  virtual ~IOTFolder(){};

  virtual IOTFolder& makeChildFolder(const std::string& name) ;
};
  
}

#endif
