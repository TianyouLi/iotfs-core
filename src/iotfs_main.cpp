#include <iotfs/iotfs.h>

int main(int argc, char* argv[])
{
  return iotfs::g_daemon.run(argc, argv);
}
