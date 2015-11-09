#ifndef __IOTFS__IOTFS_OIC_H
#define __IOTFS__IOTFS_OIC_H

#include <sstream>
#include <map>
//#include <pair>
#include <vector>
#include <mutex>
#include <string>
#include <mutex>
#include <condition_variable>

#include <iotfs/iotfs.h>
#include <inc/iotfs_oic_directory.h>
#include <inc/iotfs_oic_stub.h>

#include <OCPlatform.h>
#include <OCApi.h>

namespace iotfs {
typedef std::map<OC::OCResourceIdentifier, std::shared_ptr<OC::OCResource>>
    DiscoveredResourceMap;

class OICInfoProvider : public IoTInfoProvider {
 public:
  OICInfoProvider();
  virtual ~OICInfoProvider();
  virtual void initialize(iotfs::daemon* daemon);

 private:
  void foundResource(std::shared_ptr<OC::OCResource> resource);

  void createFolderByUri(IOTFolder& parent,
                         const std::string& uri,
                         std::shared_ptr<OC::OCResource> resource);

  void presenceHandler(OCStackResult result, const unsigned int nonce,
                       const std::string& hostAddress);

 private:
  OC::PlatformConfig _cfg;
  std::ostringstream _requestURI;
  std::mutex _resourceLock;
  DiscoveredResourceMap _resources;
  iotfs::daemon* _daemon;
};
}

#endif
