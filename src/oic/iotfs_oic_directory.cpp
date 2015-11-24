#include <inc/iotfs_oic_directory.h>
#include <inc/iotfs_oic_file.h>

using namespace OC;

namespace iotfs {

OICFolder& OICFolder::makeChildFolder(const std::string& name) {
  fusekit::entry* e = this->find(name.c_str());
  if (e == NULL) {
    return this->add_directory(name.c_str(), new OICFolder(this->_stub));
  }
  return *dynamic_cast<OICFolder*>(e);

}

void OICFolder::createResourceTypeFolder() {
  std::shared_ptr<OC::OCResource> resource = _stub->resource();
  
  for (auto& rt : resource->getResourceTypes()) {
    OICFolder& folder = this->makeChildFolder(rt);
    folder.getState();
  }
}
  
void OICFolder::getState() {
  if (_stub == NULL) {
    std::cout << "Error Operation" << std::endl;
    return;
  }

  _stub->get(QueryParamsMap(),
             std::bind(&OICFolder::onGetState, this, _1, _2, _3));
  _stub->wait();
}

void OICFolder::onGetState(const OC::HeaderOptions& headerOptions,
                           const OC::OCRepresentation& rep, const int eCode) {
  OC::OCRepresentation::const_iterator it = rep.begin();
  for (; it != rep.end(); ++it) {
    std::map<std::string, OICFileDescriptor> attributes;

    OICFileDescriptor descriptor;
    descriptor.content = it->getValueToString();
    descriptor.type = it->type();

    attributes.insert(
        std::pair<std::string, OICFileDescriptor>(it->attrname(), descriptor));

    OICFile* file =
      new OICFile(it->attrname(), _stub->getResourceInterfaces(), attributes, _stub);
    this->add_file(it->attrname().c_str(), file);
  }

  _stub->resume();
}
}
