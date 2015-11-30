#include <memory>
#include <thread>
#include <chrono>

// boost trivial logger
#include <boost/log/trivial.hpp>

#include <fusekit/daemon.h>

#include <iotfs/iotfs.h>
#include <plugins/plugin_manager.h>

namespace iotfs {

IoTFSInitializer::IoTFSInitializer() {
  try {
    PluginManager *pm = PluginManager::instance();
    for (IoTInfoProvider *provider : *pm) {
      provider->initialize(&g_daemon);
    }
  } catch (...) {
    BOOST_LOG_TRIVIAL(error) << "Error when initialize plugins";
  }
}

IoTFSInitializer::~IoTFSInitializer() {

}

}
