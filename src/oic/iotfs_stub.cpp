#include <inc/iotfs_oic_stub.h>

namespace iotfs {
OICStub::OICStub(std::shared_ptr<OC::OCResource> resource)
    : _resource(resource), _isWaiting(false) {}

OICStub::~OICStub() {}

void OICStub::put(OC::OCRepresentation rep, OC::QueryParamsMap query,
                  OC::PutCallback callback) {
  _isWaiting = true;
  auto wrapped =
      [this, callback](const OC::HeaderOptions &ho,
                       const OC::OCRepresentation &rep, const int ecode) {
        callback(ho, rep, ecode);
        resume();
      };

  _resource->put(rep, query, wrapped);
  wait();
}

void OICStub::get(OC::QueryParamsMap query, OC::GetCallback callback) {
  _isWaiting = true;
  auto wrapped =
      [this, callback](const OC::HeaderOptions &ho,
                       const OC::OCRepresentation &rep, const int ecode) {
        callback(ho, rep, ecode);
        resume();
      };

  _resource->get(query, wrapped);
  wait();
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
