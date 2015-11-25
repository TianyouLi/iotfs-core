// boost trivial logger
#include <boost/log/trivial.hpp>

#include <inc/iotfs_oic_directory.h>
#include <inc/iotfs_oic_file.h>

using namespace OC;

namespace iotfs {

OICFolder& OICFolder::makeChildFolder(const std::string& name) {
  fusekit::entry* e = this->find(name.c_str());
  if (e == nullptr) {
    return this->add_directory(name.c_str(), new OICFolder());
  }
  return *dynamic_cast<OICFolder*>(e);
}

void OICFolder::createResourceTypeFolder(std::shared_ptr<OC::OCResource> resource) {
  for (auto& rt : resource->getResourceTypes()) {
    fusekit::entry* e = this->find(rt.c_str());
    OICRTFolder* folder = nullptr;
    if (e == nullptr) {
      folder = new OICRTFolder(resource);
      this->add_directory(rt.c_str(), folder);
    } else {
      folder =  dynamic_cast<OICRTFolder*>(e);
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
  _stub->wait();
}

void OICRTFolder::onGetState(const OC::HeaderOptions& headerOptions,
                           const OC::OCRepresentation& rep, const int eCode) {
  OC::OCRepresentation::const_iterator it = rep.begin();
  for (; it != rep.end(); ++it) {
    std::map<std::string, OICFileDescriptor> attributes;

    OICFileDescriptor descriptor;
    descriptor.content = it->getValueToString();
    descriptor.type = it->type();

    attributes.insert(
        std::pair<std::string, OICFileDescriptor>(it->attrname(), descriptor));

    OICFile* file = new OICFile(it->attrname(), _stub->getResourceInterfaces(),
                                attributes, _stub);
    this->add_file(it->attrname().c_str(), file);
  }

  _stub->resume();
}
}
