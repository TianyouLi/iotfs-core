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
typedef fusekit::daemon<IoTFSInitializer, iotfs::IoTFolder, fusekit::no_lock> daemon;

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

static iotfs::daemon &g_daemon = iotfs::daemon::instance();
}

US_DECLARE_SERVICE_INTERFACE(iotfs::IoTInfoProvider, "IoTInfoProvider/1.0");

#endif
