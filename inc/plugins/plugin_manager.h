#ifndef __PLUGIN_MANAGER
#define __PLUGIN_MANAGER

// boost property tree
#include <boost/property_tree/ptree.hpp>


// 


using boost::property_tree::ptree;

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
  // ptree object
  ptree                 _pt;
};
  
}

#endif
