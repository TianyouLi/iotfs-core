#include <iotfs/iotfs_directory.h>

namespace iotfs {

IoTFolder &IoTFolder::makeChildFolder(const std::string &name) {
  fusekit::entry *e = this->find(name.c_str());
  if (e == nullptr) {
    return this->add_directory(name.c_str(), new IoTFolder());
  }
  return *dynamic_cast<IoTFolder *>(e);
}
}
