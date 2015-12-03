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
  std::auto_ptr<iotfs::OICInfoProvider> _oic_provider;

public:
  /**
   * Implements ModuleActivator::Load(). Registers an
   * instance of a dictionary service using the module context;
   * attaches properties to the service that can be queried
   * when performing a service look-up.
   * @param context the context for the module.
   */
  void Load(ModuleContext *context) {
    _oic_provider.reset(new iotfs::OICInfoProvider());
    ServiceProperties props;
    props["root"] = std::string("oic");
    props["type"] = std::string("IoTInfoProvider");
    context->RegisterService<iotfs::IoTInfoProvider>(_oic_provider.get(),
                                                     props);

    BOOST_LOG_TRIVIAL(trace) << "OIC provider registered";
  }

  /**
   * Implements ModuleActivator::Unload(). Does nothing since
   * the C++ Micro Services library will automatically unregister any registered
   * services.
   * @param context the context for the module.
   */
  void Unload(ModuleContext * /*context*/) {
    // NOTE: The service is automatically unregistered
  }
};
}

US_EXPORT_MODULE_ACTIVATOR(iotfs_oic, iotfs::Activator)
