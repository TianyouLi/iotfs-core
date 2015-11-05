#include <inc/iotfs_oic_stub.h>

namespace iotfs {
OICStub::OICStub(std::shared_ptr<OC::OCResource> resource)
    : _resource(resource), _isWaiting(false) {}

OICStub::~OICStub() {}

void OICStub::put(OC::OCRepresentation rep, OC::QueryParamsMap query,
                  OC::PutCallback callback) {
  _isWaiting = true;
  _resource->put(rep, query, callback);
}

void OICStub::get(OC::QueryParamsMap query, OC::GetCallback callback) {
  _isWaiting = true;
  _resource->get(query, callback);
}

void OICStub::post() {
  // currently do nothing
}

void OICStub::remove() {
  // currently do nothing
}

void OICStub::wait() {
  std::unique_lock<std::mutex> lock(_m);

  while (_isWaiting) {
    _cv.wait(lock);
  }
}

void OICStub::resume() {
  _isWaiting = false;
  _cv.notify_one();
}
}
