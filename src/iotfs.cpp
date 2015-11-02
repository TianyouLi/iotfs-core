#include <memory>
#include <thread>
#include <chrono>

#include <fusekit/daemon.h>
#include <iotfs/oic/iotfs_oic.h>

iotfs::daemon& g_daemon = iotfs::daemon::instance();

namespace iotfs {
CDtor::CDtor() {
  IoTInfoProvider* provider = new OICInfoProvider(g_daemon);
  provider->initialize();
  _data = provider;
}

CDtor::~CDtor() {
  IoTInfoProvider* provider = static_cast<IoTInfoProvider*>(_data);
  delete provider;
}
}

int main(int argc, char* argv[]) { return g_daemon.run(argc, argv); }
