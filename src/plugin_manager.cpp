#include <string>
#include <set>

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

using boost::property_tree::ptree;
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

US_USE_NAMESPACE

namespace iotfs {

PluginManager* PluginManager::_instance = NULL;
  
PluginManager::PluginManager()
{
  
}

PluginManager::~PluginManager()
{

}

PluginManager* PluginManager::instance()
{
  if (_instance == NULL) {
    _instance = new PluginManager();
    _instance->init();
  }

  return _instance;
}

void PluginManager::init()
{
  static const std::string plugin_config_file("./plugins.xml");
  fs::path current_path(fs::current_path());

  std::string full_path = current_path.generic_string() + "/" + plugin_config_file;

  BOOST_LOG_TRIVIAL(trace) << "Loading plugin configuration file from "
                           << full_path ;
  try {
    read_xml(full_path, _pt) ;
  } catch (...) {
    BOOST_LOG_TRIVIAL(error) << "Can not load configuration file from "
                             << full_path ;
    return;
  }

  // iterate configuration for configured plugins
  BOOST_LOG_TRIVIAL(trace) << "Target load plugins...";    
  BOOST_FOREACH(pt::ptree::value_type &node, _pt.get_child("plugins")) {
    if (node.first == "plugin") {
      std::string name(node.second.get<std::string>("name"));
      std::string path(current_path.generic_string() + "/" + node.second.get<std::string>("path"));
      BOOST_LOG_TRIVIAL(trace) << "Module Name: " << name
                               << " Module Path: " << path;

      SharedLibrary libHandle(path, name);
      BOOST_LOG_TRIVIAL(trace) << "Loading library " << libHandle.GetFilePath();
      libHandle.Load();
    }
  }

  // iterate to every module and get service references
  std::vector<Module*> modules = ModuleRegistry::GetModules();

  BOOST_LOG_TRIVIAL(trace) << "Print existing plugins... ";
  for (std::vector<Module*>::const_iterator moduleIter = modules.begin();
       moduleIter != modules.end(); ++moduleIter) {
    BOOST_LOG_TRIVIAL(trace) << "Module Name: " << (*moduleIter)->GetName()
                             << " Module ID: " << (*moduleIter)->GetModuleId()
                             << " Status: "
                             << ((*moduleIter)->IsLoaded() ? "LOADED" : "UNLOADED");

    if ((*moduleIter)->IsLoaded()) {
      ModuleContext* context = (*moduleIter)->GetModuleContext();
      std::vector<ServiceReference<IoTInfoProvider> > services =
        context->GetServiceReferences<IoTInfoProvider>("(type=IoTInfoProvider)");

      BOOST_LOG_TRIVIAL(trace) << "Find  " << services.size() << " provider references";
      BOOST_FOREACH(ServiceReference<IoTInfoProvider>& value, services) {
        IoTInfoProvider* provider = context->GetService<IoTInfoProvider>(value);
        if (std::find(this->begin(), this->end(), provider) == this->end()) {
          this->push_back(provider);
        }
      }
    }
  }

  BOOST_LOG_TRIVIAL(trace) << "Total " << this->size() << " providers loaded";
}


}
