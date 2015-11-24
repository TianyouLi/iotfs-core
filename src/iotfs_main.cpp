#include <iotfs/iotfs.h>

int main(int argc, char* argv[])
{
  iotfs::initialize();
  return iotfs::g_daemon.run(argc, argv);
}
