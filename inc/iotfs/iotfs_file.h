#ifndef __IOTFS__IOTFS_ROOT_FILE_H
#define __IOTFS__IOTFS_ROOT_FILE_H

#include <iotfs/iotfs.h>
#include <sstream>

using namespace fusekit;
using namespace std::placeholders;

namespace iotfs {
const int MAX_SIZE = 4096;
const char DELIMITER = '\n';

//!
/*!
  IoTFile is the base class for all files in IoTFS. Each standard/protocol
  plugin should inherited from this class for its own file class structure.
 */
struct IoTFile
    : public iostream_function_file<MAX_SIZE, DELIMITER, default_time,
                                    default_file_permissions> {
public:
  IoTFile()
      : iostream_function_file<MAX_SIZE, DELIMITER, default_time,
                               default_file_permissions>(
            std::bind(&IoTFile::read, this, _1),
            std::bind(&IoTFile::write, this, _1)) {}

  virtual ~IoTFile() {}

  //!
  /*!
    The read call will be invoked once a file read operation performed.

    \param os value should be serialized into os thus can be passed to
    application level.
    \return <ReturnValue> 0 for success, non-zero for error
  */
  virtual int read(std::ostream &os) = 0;

  //!
  /*!
    The write call will be invoked once a file being write with some
    content.

    \param is the write value could be retrieved from the is.
    \return <ReturnValue> 0 for success, non-zero for error
  */
  virtual int write(std::istream &is) = 0;
};
}

#endif
