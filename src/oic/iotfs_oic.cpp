/*
Copyright (c) 2015, Intel Corporation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// std lib
#include <functional>

// ourselves
#include <inc/iotfs_oic.h>
#include <inc/iotfs_oic_directory.h>

// boost lib
#include <boost/algorithm/string.hpp>

// boost trivial logger
#include <boost/log/trivial.hpp>

// local headers
#include <iotfs/iotfs.h>


using namespace OC;
namespace ph = std::placeholders;


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
  _requestURI << OC_MULTICAST_DISCOVERY_URI;
}

OICInfoProvider::~OICInfoProvider() {}


static std::map<std::string, OCConnectivityType> map_connectivity =
  {
    {"default", CT_DEFAULT},
    {"adapter_ip", CT_ADAPTER_IP},
    {"adapter_gatt_btle", CT_ADAPTER_GATT_BTLE},
    {"adapter_rfcomm_btedr", CT_ADAPTER_RFCOMM_BTEDR},
    {"flag_secure", CT_FLAG_SECURE},
    {"ip_use_v6", CT_IP_USE_V6},
    {"ip_use_v4", CT_IP_USE_V4},
    {"scope_interface", CT_SCOPE_INTERFACE},
    {"scope_link", CT_SCOPE_LINK},
    {"scope_realm", CT_SCOPE_REALM},
    {"scope_admin", CT_SCOPE_ADMIN},
    {"scope_site", CT_SCOPE_SITE},
    {"scope_org", CT_SCOPE_ORG},
    {"scope_global", CT_SCOPE_GLOBAL}
  };

static std::map<std::string, QualityOfService> map_qos =
  {
    {"low", QualityOfService::LowQos},
    {"mid", QualityOfService::MidQos},
    {"HighQos", QualityOfService::HighQos},
    {"NaQos", QualityOfService::NaQos}
  };

  
void OICInfoProvider::overwriteDefaultConfigure(pt::ptree& properties) {
  try {
    std::string server_connectivity = properties.get<std::string>
      ("PlatformConfig.ServerConnectivity");
    auto result = map_connectivity.find(server_connectivity);
    if (result != map_connectivity.end()) {
      _cfg.serverConnectivity = result->second;
    }
  } catch (...) {
    BOOST_LOG_TRIVIAL(trace) << "Can not read server connectivity config.";
  }

  
  try {
    std::string client_connectivity = properties.get<std::string>
      ("PlatformConfig.ClientConnectivity");
    auto result = map_connectivity.find(client_connectivity);
    if (result != map_connectivity.end()) {
      _cfg.clientConnectivity = result->second;
    }
  } catch (...) {
    BOOST_LOG_TRIVIAL(trace) << "Can not read client connectivity config.";
  }

  
  try {
    std::string ip = properties.get<std::string>("PlatformConfig.IP");
    _cfg.ipAddress = ip;
  } catch (...) {
    BOOST_LOG_TRIVIAL(trace) << "Can not read ip address config.";
  }


  try {
    uint16_t port = properties.get<uint16_t>("PlatformConfig.Port");
    _cfg.port = port;
  } catch (...) {
    BOOST_LOG_TRIVIAL(trace) << "Can not read port config.";
  }


  try {
    std::string qos = properties.get<std::string>("PlatformConfig.QoS");
    auto result = map_qos.find(qos);
    if (result != map_qos.end()) {
      _cfg.QoS = result->second;
    }
  } catch (...) {
    BOOST_LOG_TRIVIAL(trace) << "Can not read QoS config.";
  }
}
  
void OICInfoProvider::loadPlatformConfiguration() {
  static const std::string oc_config_file("./OCPlatformConfig.xml");
  
  // get configuration directory
  std::string current_path = directory_conf();
  
  // get full configuration file pathname
  std::string full_path = current_path + "/" + oc_config_file;

  BOOST_LOG_TRIVIAL(trace) << "Loading OIC configuration file from "
                           << full_path;

  // provide default initialization value
  _cfg = {OC::ServiceType::InProc, OC::ModeType::Client, "0.0.0.0", 0,
          OC::QualityOfService::LowQos};

  // load configurations
  pt::ptree properties;
  try {
    pt::read_xml(full_path, properties);
  } catch (...) {
    BOOST_LOG_TRIVIAL(error) << "Can not load OIC configuration file from "
                             << full_path;
  }

  // overwrite default configuration values
  overwriteDefaultConfigure(properties);
}

  
void OICInfoProvider::initialize(iotfs::daemon *daemon) {
  if (_initialized)
    return;

  _daemon = daemon;

  loadPlatformConfiguration();
  
  OCPlatform::Configure(_cfg);

  OCPlatform::OCPresenceHandle presenceHandle = nullptr;

  std::ostringstream multicastPresenceURI;
  multicastPresenceURI << "coap://" << OC_MULTICAST_PREFIX;

  auto presence_callback =
    std::bind(&OICInfoProvider::presenceHandler, this, ph::_1, ph::_2, ph::_3);
  OCPlatform::subscribePresence(presenceHandle, multicastPresenceURI.str(),
                                CT_ADAPTER_IP, presence_callback);

  auto discovery_callback =
    std::bind(&OICInfoProvider::foundResource, this, ph::_1);
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
    auto callback = std::bind(&OICInfoProvider::foundResource, this, ph::_1);
    OCPlatform::findResource("", _requestURI.str(), CT_ADAPTER_IP, callback);
  }
}
}
