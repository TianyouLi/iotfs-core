#include <string>

#include <plugins/plugin_manager.h>

// boost trivial logger
#include <boost/log/trivial.hpp>

// boost property tree xml seder
#include <boost/property_tree/xml_parser.hpp>

// boost filesystem
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

using boost::property_tree::ptree;
namespace fs =  boost::filesystem;

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
  }
}


  
}
