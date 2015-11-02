// std lib
#include <functional>

// boost lib
#include <boost/algorithm/string.hpp>

// ourselves
#include <iotfs/oic/iotfs_oic.h>
#include <iotfs/oic/iotfs_oic_directory.h>

using namespace OC;
using namespace std::placeholders;

namespace iotfs {

static OICFolder& createOICFolder(IOTFolder& parent, const std::string& child,
                                  std::shared_ptr<OC::OCResource> resource) {
  fusekit::entry* e = parent.find(child.c_str());
  if (e == NULL) {
    return parent.add_directory(child.c_str(), new OICFolder(resource));
  }
  return *dynamic_cast<OICFolder*>(e);
}

  
OICInfoProvider::OICInfoProvider(iotfs::daemon& daemon) : _daemon(daemon) {
  _cfg = {
    OC::ServiceType::InProc,
    OC::ModeType::Client,
    "0.0.0.0",
    0,
    OC::QualityOfService::LowQos
  };

  _requestURI << OC_MULTICAST_DISCOVERY_URI;
}

OICInfoProvider::~OICInfoProvider() {}

void OICInfoProvider::initialize() {
  OCPlatform::Configure(_cfg);
  
  OCPlatform::OCPresenceHandle presenceHandle = nullptr;

  std::ostringstream multicastPresenceURI;
  multicastPresenceURI << "coap://" << OC_MULTICAST_PREFIX;

  auto presence_callback =
    std::bind(&OICInfoProvider::presenceHandler, this, _1, _2, _3);
  OCPlatform::subscribePresence(presenceHandle, multicastPresenceURI.str(),
                                CT_ADAPTER_IP, presence_callback);

  auto discovery_callback = std::bind(&OICInfoProvider::foundResource, this, _1);
  OCPlatform::findResource("", _requestURI.str(), CT_ADAPTER_IP, discovery_callback);
}


void OICInfoProvider::createFolderByUri(
    IOTFolder& parent, const std::string& uri,
    std::shared_ptr<OC::OCResource> resource) {
  std::size_t pos = uri.find("/");
  if (std::string::npos == pos) {  // not found, create the directory
    OICFolder& folder = createOICFolder(parent, uri, resource);
    folder.createResourceTypeFolder();
  } else {
    std::string stepUri = uri.substr(0, pos);
    IOTFolder& step = parent.makeChildFolder(stepUri);
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

      createFolderByUri(_daemon.root(), uri, resource);
    }
  } catch (std::exception& e) {
    // error handling
  }
}

void OICInfoProvider::presenceHandler(OCStackResult result,
                                      const unsigned int nonce,
                                      const std::string& hostAddress) {
  std::cout << "Received presence notification from : " << hostAddress
            << std::endl;
  std::cout << "In presenceHandler: ";

  switch (result) {
    case OC_STACK_OK:
      std::cout << "Nonce# " << nonce << std::endl;
      break;
    case OC_STACK_PRESENCE_STOPPED:
      std::cout << "Presence Stopped\n";
      break;
    case OC_STACK_PRESENCE_TIMEOUT:
      std::cout << "Presence Timeout\n";
      break;
    default:
      std::cout << "Error\n";
      break;
  }

  if (result == OC_STACK_OK) {
    auto callback = std::bind(&OICInfoProvider::foundResource, this, _1);
    OCPlatform::findResource("", _requestURI.str(), CT_ADAPTER_IP, callback);
  }
}
}
