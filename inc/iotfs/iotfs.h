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

#ifndef __IOTFS__IOTFS_H
#define __IOTFS__IOTFS_H

#include <fusekit/daemon.h>
#include <fusekit/stream_function_file.h>
#include <fusekit/stream_object_file.h>

#include <iotfs/iotfs_directory.h>

#include <usServiceInterface.h>

US_USE_NAMESPACE

#ifdef IOTINFOPROVIDER_EXPORTS
#define IOTINFOPROVIDER_EXPORT US_ABI_EXPORT
#else
#define IOTINFOPROVIDER_EXPORT US_ABI_IMPORT
#endif

namespace iotfs {

class IoTFSInitializer {
public:
  IoTFSInitializer();
  ~IoTFSInitializer();
};

//!
/*!
  define a daemon type that use iotfs folder template parameter
 */
typedef fusekit::daemon<IoTFSInitializer, iotfs::IoTFolder, fusekit::no_lock>
    daemon;

//!
/*!
  The provider is the interface class for any IoTFS plugins to implement,
  please refer OICInfoProvider for more details.
 */
class IOTINFOPROVIDER_EXPORT IoTInfoProvider {
public:
  virtual ~IoTInfoProvider(){};

public:
  //!
  /*!
    The function initialize will be invoked after library is loaded and
    plugin manager retrieved the references of IoTInfoProvider's instance.

    \param daemon	will be used inside the provider for managing directory
    and file mappings for devices.
  */
  virtual void initialize(iotfs::daemon *daemon) = 0;
};

//! 
/*!
  Get configuration directory

  \param  
  \return <ReturnValue> the string of full path of configuration directory
*/
inline const std::string directory_conf() {
  char* path = std::getenv("IOTFS_BIN_DIR");

  if (path == nullptr) {
    return std::string("");
  }

  return std::string(path);
}


  
//! 
/*!
  Global iotfs::daemon intance, which will be passed to every plugin during
  initialization.
 */
static iotfs::daemon &g_daemon = iotfs::daemon::instance();
}


US_DECLARE_SERVICE_INTERFACE(iotfs::IoTInfoProvider, "IoTInfoProvider/1.0");

#endif
