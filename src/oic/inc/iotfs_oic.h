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
  virtual void initialize(iotfs::daemon *daemon);

private:
  void foundResource(std::shared_ptr<OC::OCResource> resource);

  void createFolderByUri(IoTFolder &parent, const std::string &uri,
                         std::shared_ptr<OC::OCResource> resource);

  void presenceHandler(OCStackResult result, const unsigned int nonce,
                       const std::string &hostAddress);

private:
  OC::PlatformConfig _cfg;
  std::ostringstream _requestURI;
  std::mutex _resourceLock;
  DiscoveredResourceMap _resources;
  iotfs::daemon *_daemon;
  bool _initialized;
};
}

#endif
