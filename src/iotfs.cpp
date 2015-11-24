#include <memory>
#include <thread>
#include <chrono>

#include <fusekit/daemon.h>
#include <iotfs/iotfs.h>
#include <plugins/plugin_manager.h>

namespace iotfs {

void initialize() {
  PluginManager* pm = PluginManager::instance();
  for (IoTInfoProvider* provider : *pm) {
    provider->initialize(&g_daemon);
  }
}

}

