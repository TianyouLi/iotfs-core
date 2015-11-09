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

//-
// CDtor was used for initialize/cleanup during filesystem mount/umount
//-
class CDtor {
 public:
  CDtor();
  virtual ~CDtor();

 private:
  void* _data;
};

  
//-
// define a daemon type that use CDtor as initialize/cleanup
//-
typedef fusekit::daemon<CDtor, iotfs::IOTFolder, fusekit::no_lock> daemon;

  
//-
// The item type indicates which type of the item belongs to.
//-
enum IoTInfoItemType { SERVER, RESOURCE, DEVICE };

//-
// The provider will provide iot info items.
//-
class IOTINFOPROVIDER_EXPORT IoTInfoProvider {
 public:
  virtual ~IoTInfoProvider(){};

 public:
  virtual void initialize(iotfs::daemon* daemon) = 0;
};
  
}

US_DECLARE_SERVICE_INTERFACE(iotfs::IoTInfoProvider, "IoTInfoProvider/1.0");

#endif
