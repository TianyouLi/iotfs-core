#ifndef __IOTFS__IOTFS_OIC_DIRECTORY_H
#define __IOTFS__IOTFS_OIC_DIRECTORY_H

#include <memory>
#include <iotfs/iotfs_directory.h>
#include <inc/iotfs_oic_stub.h>

namespace iotfs {
class OICFolder : public IOTFolder {
 public:
  OICFolder(std::shared_ptr<OC::OCResource> resource)
    : IOTFolder(), _stub(new OICStub(resource)) {};

  virtual ~OICFolder(){};

  virtual OICFolder& makeChildFolder(const std::string& name);

 public: 
  void getState();
  void onGetState(const OC::HeaderOptions& headerOptions,
                          const OC::OCRepresentation& rep, const int eCode);
  void createResourceTypeFolder();
  
 protected:
  std::shared_ptr<OICStub> _stub;

 private:
  OICFolder(std::shared_ptr<OICStub> stub)
    : IOTFolder(), _stub(stub) {};
};
}

#endif
