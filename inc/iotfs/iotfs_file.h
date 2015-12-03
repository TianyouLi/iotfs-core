/*
Copyright (c) 2015, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
