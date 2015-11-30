#ifndef __IOTFS__IOTFS_OIC_DIRECTORY_H
#define __IOTFS__IOTFS_OIC_DIRECTORY_H

#include <memory>
#include <iotfs/iotfs_directory.h>
#include <inc/iotfs_oic_stub.h>

namespace iotfs {
class OICFolder : public IoTFolder {
public:
  OICFolder() : IoTFolder(){};

  virtual ~OICFolder(){};

  virtual OICFolder &makeChildFolder(const std::string &name);

public:
  void createResourceTypeFolder(std::shared_ptr<OC::OCResource> resource);
};

class OICRTFolder : public OICFolder {
public:
  OICRTFolder(std::shared_ptr<OC::OCResource> resource)
      : OICFolder(), _stub(new OICStub(resource)){};

  ~OICRTFolder(){};

public:
  void getState();
  void onGetState(const OC::HeaderOptions &headerOptions,
                  const OC::OCRepresentation &rep, const int eCode);

private:
  std::shared_ptr<OICStub> _stub;
};
}

#endif
