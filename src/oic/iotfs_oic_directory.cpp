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

// boost trivial logger
#include <boost/log/trivial.hpp>

#include <inc/iotfs_oic_directory.h>
#include <inc/iotfs_oic_file.h>

using namespace OC;

namespace iotfs {

OICFolder &OICFolder::makeChildFolder(const std::string &name) {
  fusekit::entry *e = this->find(name.c_str());
  if (e == nullptr) {
    return this->add_directory(name.c_str(), new OICFolder());
  }
  return *dynamic_cast<OICFolder *>(e);
}

void OICFolder::createResourceTypeFolder(
    std::shared_ptr<OC::OCResource> resource) {
  for (auto &rt : resource->getResourceTypes()) {
    fusekit::entry *e = this->find(rt.c_str());
    OICRTFolder *folder = nullptr;
    if (e == nullptr) {
      folder = new OICRTFolder(resource);
      this->add_directory(rt.c_str(), folder);
    } else {
      folder = dynamic_cast<OICRTFolder *>(e);
    }

    folder->getState();
  }
}

void OICRTFolder::getState() {
  if (_stub == nullptr) {
    BOOST_LOG_TRIVIAL(error) << "Error Operation";
    return;
  }

  _stub->get(QueryParamsMap(),
             std::bind(&OICRTFolder::onGetState, this, _1, _2, _3));
}

void OICRTFolder::onGetState(const OC::HeaderOptions &headerOptions,
                             const OC::OCRepresentation &rep, const int eCode) {
  for (auto &it : rep) {
    std::map<std::string, OICFileDescriptor> attributes;
    OICFileDescriptor descriptor;

    descriptor.content = it.getValueToString();
    descriptor.type = it.type();

    attributes.insert(
        std::pair<std::string, OICFileDescriptor>(it.attrname(), descriptor));

    OICFile *file =
        new OICFile(it.attrname(), _stub->resource()->getResourceInterfaces(),
                    attributes, _stub);
    this->add_file(it.attrname().c_str(), file);
  }
}
}
