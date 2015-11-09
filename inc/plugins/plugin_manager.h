#ifndef __PLUGIN_MANAGER
#define __PLUGIN_MANAGER

// std
#include <vector>

// boost property tree
#include <boost/property_tree/ptree.hpp>

// micro service
#include <usModuleActivator.h>
#include <usModuleContext.h>

// iotfs interface
#include <iotfs/iotfs.h>

using boost::property_tree::ptree;

US_USE_NAMESPACE

namespace iotfs {

class PluginManager {

public:
  static PluginManager* instance();
  ~PluginManager();
  
private:
  PluginManager();

private:
  void init();
  
private:
  // singleton
  static PluginManager* _instance;


private:
  // ptree object
  ptree                 _pt;

  // list of iot info providers
  std::vector<IoTInfoProvider*> _providers;
};
  
}

#endif
