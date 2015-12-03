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

#include <string>
#include <set>
#include <cstdlib>

#include <plugins/plugin_manager.h>

// boost trivial logger
#include <boost/log/trivial.hpp>

// boost property tree xml seder
#include <boost/property_tree/xml_parser.hpp>

// boost filesystem
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// boost algorithms
#include <boost/foreach.hpp>

// micro service
#include <usModule.h>
#include <usModuleRegistry.h>
#include <usSharedLibrary.h>

// local headers
#include <iotfs/utils.h>

using boost::property_tree::ptree;
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

US_USE_NAMESPACE

namespace iotfs {

PluginManager *PluginManager::_instance = nullptr;

PluginManager::PluginManager() {}

PluginManager::~PluginManager() {}

PluginManager *PluginManager::instance() {
  if (_instance == nullptr) {
    _instance = new PluginManager();
    _instance->init();
  }

  return _instance;
}

void PluginManager::init() {
  static const std::string plugin_config_file("./plugins.xml");
  std::string current_path = directory_conf();

  std::string full_path = current_path + "/" + plugin_config_file;

  BOOST_LOG_TRIVIAL(trace) << "Loading plugin configuration file from "
                           << full_path;
  try {
    read_xml(full_path, _pt);
  } catch (...) {
    BOOST_LOG_TRIVIAL(error) << "Can not load configuration file from "
                             << full_path;
    return;
  }

  // iterate configuration for configured plugins
  BOOST_LOG_TRIVIAL(trace) << "Target load plugins...";
  for (pt::ptree::value_type &node : _pt.get_child("plugins")) {
    if (node.first == "plugin") {
      std::string name(node.second.get<std::string>("name"));
      std::string path(current_path + "/" +
                       node.second.get<std::string>("path"));
      BOOST_LOG_TRIVIAL(trace) << "Module Name: " << name
                               << " Module Path: " << path;

      SharedLibrary libHandle(path, name);
      BOOST_LOG_TRIVIAL(trace) << "Loading library " << libHandle.GetFilePath();

      try {
        libHandle.Load();
      } catch (...) {
        BOOST_LOG_TRIVIAL(error) << "Error when loading plugin library " << path
                                 << name;
      }
    }
  }

  // iterate to every module and get service references
  std::vector<Module *> modules = ModuleRegistry::GetModules();

  BOOST_LOG_TRIVIAL(trace) << "Print existing plugins... ";
  for (Module *module : modules) {
    BOOST_LOG_TRIVIAL(trace)
        << "Module Name: " << module->GetName()
        << " Module ID: " << module->GetModuleId()
        << " Status: " << (module->IsLoaded() ? "LOADED" : "UNLOADED");

    if (module->IsLoaded()) {
      ModuleContext *context = module->GetModuleContext();
      std::vector<ServiceReference<IoTInfoProvider>> services =
          context->GetServiceReferences<IoTInfoProvider>(
              "(type=IoTInfoProvider)");

      BOOST_LOG_TRIVIAL(trace) << "Find  " << services.size()
                               << " provider references";
      for (ServiceReference<IoTInfoProvider> &value : services) {
        IoTInfoProvider *provider = context->GetService<IoTInfoProvider>(value);
        if (std::find(this->begin(), this->end(), provider) == this->end()) {
          this->push_back(provider);
        }
      }
    }
  }

  BOOST_LOG_TRIVIAL(trace) << "Total " << this->size() << " providers loaded";
}
}
