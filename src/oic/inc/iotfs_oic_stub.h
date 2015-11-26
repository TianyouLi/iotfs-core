#ifndef __IOTFS__IOTFS_STUB_H
#define __IOTFS__IOTFS_STUB_H

#include <OCPlatform.h>
#include <OCApi.h>
#include <mutex>
#include <condition_variable>

namespace iotfs {

class OICStub {
public:
  OICStub(std::shared_ptr<OC::OCResource> resource);
  ~OICStub();

  void put(OC::OCRepresentation rep, OC::QueryParamsMap query,
           OC::PutCallback callback);

  void get(OC::QueryParamsMap query, OC::GetCallback callback);

  std::shared_ptr<OC::OCResource> resource() { return _resource; };

private:
  void remove();
  void post();

  void wait();
  void resume();

private:
  std::shared_ptr<OC::OCResource> _resource;

  // lock when retrieve information from remote//
  bool _isWaiting;
  std::mutex _m;
  std::condition_variable _cv;
};
}

#endif
