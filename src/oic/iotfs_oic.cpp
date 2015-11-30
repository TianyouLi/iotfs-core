// std lib
#include <functional>

// boost lib
#include <boost/algorithm/string.hpp>

// boost trivial logger
#include <boost/log/trivial.hpp>

// ourselves
#include <inc/iotfs_oic.h>
#include <inc/iotfs_oic_directory.h>

using namespace OC;
using namespace std::placeholders;

namespace iotfs {

static OICFolder &createOICFolder(IoTFolder &parent, const std::string &child,
                                  std::shared_ptr<OC::OCResource> resource) {
  fusekit::entry *e = parent.find(child.c_str());
  if (e == nullptr) {
    return parent.add_directory(child.c_str(), new OICFolder());
  }
  return *dynamic_cast<OICFolder *>(e);
}

OICInfoProvider::OICInfoProvider() : _initialized(false) {
  _cfg = {OC::ServiceType::InProc, OC::ModeType::Client, "0.0.0.0", 0,
          OC::QualityOfService::LowQos};

  _requestURI << OC_MULTICAST_DISCOVERY_URI;
}

OICInfoProvider::~OICInfoProvider() {}

void OICInfoProvider::initialize(iotfs::daemon *daemon) {
  if (_initialized)
    return;

  _daemon = daemon;
  OCPlatform::Configure(_cfg);

  OCPlatform::OCPresenceHandle presenceHandle = nullptr;

  std::ostringstream multicastPresenceURI;
  multicastPresenceURI << "coap://" << OC_MULTICAST_PREFIX;

  auto presence_callback =
      std::bind(&OICInfoProvider::presenceHandler, this, _1, _2, _3);
  OCPlatform::subscribePresence(presenceHandle, multicastPresenceURI.str(),
                                CT_ADAPTER_IP, presence_callback);

  auto discovery_callback =
      std::bind(&OICInfoProvider::foundResource, this, _1);
  OCPlatform::findResource("", _requestURI.str(), CT_ADAPTER_IP,
                           discovery_callback);

  _initialized = true;
}

void OICInfoProvider::createFolderByUri(
    IoTFolder &parent, const std::string &uri,
    std::shared_ptr<OC::OCResource> resource) {
  std::size_t pos = uri.find("/");
  if (std::string::npos == pos) { // not found, create the directory
    OICFolder &folder = createOICFolder(parent, uri, resource);
    folder.createResourceTypeFolder(resource);
  } else {
    std::string stepUri = uri.substr(0, pos);
    IoTFolder &step = parent.makeChildFolder(stepUri);
    createFolderByUri(step, uri.substr(pos + 1), resource);
  }
}

void OICInfoProvider::foundResource(std::shared_ptr<OC::OCResource> resource) {
  try {
    std::lock_guard<std::mutex> lock(_resourceLock);
    if (_resources.find(resource->uniqueIdentifier()) == _resources.end()) {
      _resources[resource->uniqueIdentifier()] = resource;
      std::stringstream ss;
      ss << resource->uniqueIdentifier();
      std::string uri = ss.str();

      createFolderByUri(_daemon->root(), uri, resource);
    }
  } catch (std::exception &e) {
    BOOST_LOG_TRIVIAL(error) << "Error when handling discovery event callback";
  }
}

void OICInfoProvider::presenceHandler(OCStackResult result,
                                      const unsigned int nonce,
                                      const std::string &hostAddress) {
  BOOST_LOG_TRIVIAL(trace) << "Received presence notification from : "
                           << hostAddress;
  BOOST_LOG_TRIVIAL(trace) << "In presenceHandler: ";

  switch (result) {
  case OC_STACK_OK:
    BOOST_LOG_TRIVIAL(trace) << "Nonce# " << nonce;
    break;
  case OC_STACK_PRESENCE_STOPPED:
    BOOST_LOG_TRIVIAL(trace) << "Presence Stopped";
    break;
  case OC_STACK_PRESENCE_TIMEOUT:
    BOOST_LOG_TRIVIAL(trace) << "Presence Timeout";
    break;
  default:
    BOOST_LOG_TRIVIAL(error) << "Error";
    break;
  }

  if (result == OC_STACK_OK) {
    auto callback = std::bind(&OICInfoProvider::foundResource, this, _1);
    OCPlatform::findResource("", _requestURI.str(), CT_ADAPTER_IP, callback);
  }
}
}
