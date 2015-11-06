#include <memory>
#include <thread>
#include <chrono>

#include <fusekit/daemon.h>
#include <iotfs/iotfs.h>
#include <plugins/plugin_manager.h>

iotfs::daemon& g_daemon = iotfs::daemon::instance();

namespace iotfs {
CDtor::CDtor() {
  PluginManager* pm = PluginManager::instance();
  // IoTInfoProvider* provider = new OICInfoProvider(g_daemon);
  // provider->initialize();
  // _data = provider;
}

CDtor::~CDtor() {
  // IoTInfoProvider* provider = static_cast<IoTInfoProvider*>(_data);
  // delete provider;
}
}

int main(int argc, char* argv[]) { return g_daemon.run(argc, argv); }
