#ifndef __IOTFS__IOTFS_ROOT_FILE_H
#define __IOTFS__IOTFS_ROOT_FILE_H

#include <iotfs/iotfs.h>
#include <sstream>

using namespace fusekit;
using namespace std::placeholders;

namespace iotfs {
const int MAX_SIZE = 4096;
const char DELIMITER = '\n';

// OICResource is an abstract model, it may
// contains multiple resourceType.
// Hence we represent each resourceType by one OICFile
struct IOTFile
    : public iostream_function_file<MAX_SIZE, DELIMITER, default_time,
                                    default_file_permissions> {
 public:
  IOTFile()
      : iostream_function_file<MAX_SIZE, DELIMITER, default_time,
                               default_file_permissions>(
            std::bind(&IOTFile::read, this, _1),
            std::bind(&IOTFile::write, this, _1)) {}

  virtual ~IOTFile() {}

  // make it as an pure abstract class
  virtual int read(std::ostream& os) = 0;
  virtual int write(std::istream& is) = 0;
};
}

#endif
