#ifndef __IOTFS__IOTFS_H
#define __IOTFS__IOTFS_H

#include <fusekit/daemon.h>
#include <fusekit/stream_function_file.h>
#include <fusekit/stream_object_file.h>

#include <iotfs/iotfs_directory.h>

namespace iotfs {

//-
// The item type indicates which type of the item belongs to.
//-
enum IoTInfoItemType { SERVER, RESOURCE, DEVICE };

//-
// The provider will provide iot info items.
//-
class IoTInfoProvider {
 public:
  virtual ~IoTInfoProvider(){};

 public:
  virtual void initialize() = 0;
};

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
}

#endif
