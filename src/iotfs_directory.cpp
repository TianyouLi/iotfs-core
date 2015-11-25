#include <iotfs/iotfs_directory.h>

namespace iotfs {

IOTFolder &IOTFolder::makeChildFolder(const std::string &name) {
  fusekit::entry *e = this->find(name.c_str());
  if (e == nullptr) {
    return this->add_directory(name.c_str(), new IOTFolder());
  }
  return *dynamic_cast<IOTFolder *>(e);
}
}
