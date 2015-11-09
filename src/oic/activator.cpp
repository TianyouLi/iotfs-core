// boost trivial logger
#include <boost/log/trivial.hpp>

#include <usModuleActivator.h>
#include <usModuleContext.h>
#include <usServiceProperties.h>

#include <inc/iotfs_oic.h>

US_USE_NAMESPACE

namespace iotfs {
class US_ABI_LOCAL Activator : public ModuleActivator {
 private:
    std::auto_ptr<OICInfoProvider> _oic_provider;
    
 public:
  /**
   * Implements ModuleActivator::Load(). Registers an
   * instance of a dictionary service using the module context;
   * attaches properties to the service that can be queried
   * when performing a service look-up.
   * @param context the context for the module.
   */
  void Load(ModuleContext* context)
  {
    _oic_provider.reset(new OICInfoProvider());
    ServiceProperties props;
    props["root"] = std::string("oic");
    context->RegisterService<IoTInfoProvider>(_oic_provider.get(), props);

    BOOST_LOG_TRIVIAL(trace) << "OIC provider registered";    
  }
  
  /**
   * Implements ModuleActivator::Unload(). Does nothing since
   * the C++ Micro Services library will automatically unregister any registered services.
   * @param context the context for the module.
   */
  void Unload(ModuleContext* /*context*/)
  {
    // NOTE: The service is automatically unregistered
  }
  
};
}

US_EXPORT_MODULE_ACTIVATOR(plugin_iotfs_oic, iotfs::Activator)
