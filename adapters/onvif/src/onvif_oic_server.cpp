//******************************************************************
//
// Copyright 2014 Intel Mobile Communications GmbH All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

///
/// This sample provides steps to define an interface for a resource
/// (properties and methods) and host this resource on the server.
///

#include <functional>
#include <string>
#include <vector>

#include <pthread.h>
#include <mutex>
#include <condition_variable>

#include <boost/thread/thread.hpp>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "./gen-cpp/OnvifAdapter.h"

#include "OCPlatform.h"
#include "OCApi.h"

using namespace OC;
using namespace std;
using namespace boost;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace PH = std::placeholders;

int gObservation = 0;
void * ChangeLightRepresentation (void *param);
void * handleSlowResponse (void *param, std::shared_ptr<OCResourceRequest> pRequest);

// Specifies where to notify all observers or list of observers
// false: notifies all observers
// true: notifies list of observers
bool isListOfObservers = false;

// Specifies secure or non-secure
// false: non-secure resource
// true: secure resource
bool isSecure = false;

/// Specifies whether Entity handler is going to do slow response or not
bool isSlowResponse = false;


/// Onvif Thrift Server IP Address
const char* thriftServerAddr = "127.0.0.1";

// Forward declaring the entityHandler

/// This class represents a single resource named 'OnvifResource'. This resource has
class OnvifResource
{

public:
  /// Access this property from a TB client
  std::string m_name;
  std::string m_resourceUri;

  
  std::vector<Property> m_properties;
  
  bool m_refreshing;
  boost::thread m_refresh_thread;
  OCResourceHandle m_resourceHandle;
  OCRepresentation m_resourceRep;
  ObservationIds m_interestedObservers;


  
  boost::mutex thrift_client_mutex;
  boost::shared_ptr<TTransport> socket;
  boost::shared_ptr<TTransport> transport;
  boost::shared_ptr<TProtocol> protocol;
  OnvifAdapterClient client;
  
public:
  /// Constructor
  OnvifResource()
    :m_name("IP Cammera"), m_resourceUri("/wto/cso/sto/cam"),
     m_refreshing(true),
     m_refresh_thread(std::bind(&OnvifResource::threadFunc, this)),
     m_resourceHandle(nullptr),
     socket(new TSocket(thriftServerAddr, 6140)),
     transport(new TBufferedTransport(socket)),
     protocol(new TBinaryProtocol(transport)),
     client(protocol)
  {
    // Initialize representation
    m_resourceRep.setUri(m_resourceUri);
    m_resourceRep.setValue("name", m_name);

    cout << "connecting to [" << thriftServerAddr << "]" << endl;
    
    transport->open();
  }

  ~OnvifResource() {
    m_refreshing = false;
    m_refresh_thread.join();
    transport->close();
  }
  /* Note that this does not need to be a member function: for classes you do not have
     access to, you can accomplish this with a free function: */

  /// This function internally calls registerResource API.
  void createResource()
  {
    std::string resourceURI = m_resourceUri; //URI of the resource
    std::string resourceTypeName = "core.ipcam"; //resource type name. In this case, it is light
    std::string resourceInterface = DEFAULT_INTERFACE; // resource interface.

    // OCResourceProperty is defined ocstack.h
    uint8_t resourceProperty;
    if(isSecure)
      {
        resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE | OC_SECURE;
      }
    else
      {
        resourceProperty = OC_DISCOVERABLE | OC_OBSERVABLE;
      }
    EntityHandler cb = std::bind(&OnvifResource::entityHandler, this,PH::_1);

    // This will internally create and register the resource.
    OCStackResult result = OCPlatform::registerResource(
                             m_resourceHandle, resourceURI, resourceTypeName,
                             resourceInterface, cb, resourceProperty);

    if (OC_STACK_OK != result)
      {
        cout << "Resource creation was unsuccessful\n";
      }
  }

  void threadFunc() {
    while(m_refreshing) {
      refreshRTSPUri();
      sleep(10);
    }
  }

  void refreshRTSPUri() {
    std::string uri;
    try {
      boost::mutex::scoped_lock lock(thrift_client_mutex);
      client.getProperties(m_properties);
      for ( Property p : m_properties ) {
        cout << "name: " << p.name << endl;
        cout << "value: " << p.value << endl;
      }
    } catch (...) {
      // do nothing right now
      cout << "exception when getting rtsp uri" << endl;
    }
  }

  void pan(int i) {
    try {
      boost::mutex::scoped_lock lock(thrift_client_mutex);
      client.moveX(i);    
    } catch (...) {
      // do nothing right now
      cout << "exception when pan" << endl;
    }
  }

  void tilt(int i) {
    try {
      boost::mutex::scoped_lock lock(thrift_client_mutex);
      client.moveY(i);
    } catch (...) {
      // do nothing right now
      cout << "exception when tilt" << endl;
    }
  }

  void stop() {
    try {
      boost::mutex::scoped_lock lock(thrift_client_mutex);
      client.stopXY();
    } catch (...) {
      // do nothing right now
      cout << "exception when stop" << endl;
    }
  }
  
  OCResourceHandle getHandle()
  {
    return m_resourceHandle;
  }

  // Puts representation.
  // Gets values from the representation and
  // updates the internal state
  void put(OCRepresentation& rep)
  {
    try {
      string p;
      string t;
      
      rep.getValue("pan", p);
      rep.getValue("tilt", t);

      if (!p.empty()) {
        if (p[0] == '0') {
          stop();
        } else if (p[0] == '-') {
          pan(-1);
        } else {
          pan(1);
        }
        cout << "pan  : " << p << endl;
      }

      if (!t.empty()) {
        if (t[0] == '0') {
          stop();
        } else if (t[0] == '-') {
          tilt(-1);
        } else {
          tilt(1);
        }
        cout << "tilt : " << t << endl;
      }
      
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
    }
  }

  // Post representation.
  // Post can create new resource or simply act like put.
  // Gets values from the representation and
  // updates the internal state
  OCRepresentation post(OCRepresentation& rep)
  {
    put(rep);
    return get();
  }


  // gets the updated representation.
  // Updates the representation with latest internal state before
  // sending out.
  OCRepresentation get()
  {
    for (Property p : m_properties) {
      m_resourceRep.setValue(p.name, p.value);
    }
    
    return m_resourceRep;
  }

  void addType(const std::string& type) const
  {
    OCStackResult result = OCPlatform::bindTypeToResource(m_resourceHandle, type);
    if (OC_STACK_OK != result)
      {
        cout << "Binding TypeName to Resource was unsuccessful\n";
      }
  }

  void addInterface(const std::string& interface) const
  {
    OCStackResult result = OCPlatform::bindInterfaceToResource(m_resourceHandle, interface);
    if (OC_STACK_OK != result)
      {
        cout << "Binding TypeName to Resource was unsuccessful\n";
      }
  }

private:
  // This is just a sample implementation of entity handler.
  // Entity handler can be implemented in several ways by the manufacturer
  OCEntityHandlerResult entityHandler(std::shared_ptr<OCResourceRequest> request)
  {
    cout << "\tIn Server CPP entity handler:\n";
    OCEntityHandlerResult ehResult = OC_EH_ERROR;
    if(request)
      {
        // Get the request type and request flag
        std::string requestType = request->getRequestType();
        int requestFlag = request->getRequestHandlerFlag();

        if(requestFlag & RequestHandlerFlag::RequestFlag)
          {
            cout << "\t\trequestFlag : Request\n";
            auto pResponse = std::make_shared<OC::OCResourceResponse>();
            pResponse->setRequestHandle(request->getRequestHandle());
            pResponse->setResourceHandle(request->getResourceHandle());

            // If the request type is GET
            if(requestType == "GET")
              {
                cout << "\t\t\trequestType : GET\n";
                if(isSlowResponse) // Slow response case
                  {
                    static int startedThread = 0;
                    if(!startedThread)
                      {
                        std::thread t(handleSlowResponse, (void *)this, request);
                        startedThread = 1;
                        t.detach();
                      }
                    ehResult = OC_EH_SLOW;
                  }
                else // normal response case.
                  {
                    pResponse->setErrorCode(200);
                    pResponse->setResponseResult(OC_EH_OK);
                    pResponse->setResourceRepresentation(get());
                    if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                      {
                        ehResult = OC_EH_OK;
                      }
                  }
              }
            else if(requestType == "PUT")
              {
                cout << "\t\t\trequestType : PUT\n";
                OCRepresentation rep = request->getResourceRepresentation();

                // Do related operations related to PUT request
                // Update the lightResource
                // system("./Hello");
                put(rep);
                pResponse->setErrorCode(200);
                pResponse->setResponseResult(OC_EH_OK);
                pResponse->setResourceRepresentation(get());
                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                  {
                    ehResult = OC_EH_OK;
                  }
              }
            else if(requestType == "POST")
              {
                cout << "\t\t\trequestType : POST\n";

                OCRepresentation rep = request->getResourceRepresentation();

                // Do related operations related to POST request
                OCRepresentation rep_post = post(rep);
                pResponse->setResourceRepresentation(rep_post);
                pResponse->setErrorCode(200);
                if(rep_post.hasAttribute("createduri"))
                  {
                    pResponse->setResponseResult(OC_EH_RESOURCE_CREATED);
                    pResponse->setNewResourceUri(rep_post.getValue<std::string>("createduri"));
                  }
                else
                  {
                    pResponse->setResponseResult(OC_EH_OK);
                  }

                if(OC_STACK_OK == OCPlatform::sendResponse(pResponse))
                  {
                    ehResult = OC_EH_OK;
                  }
              }
            else if(requestType == "DELETE")
              {
                // DELETE request operations
              }
          }

        if(requestFlag & RequestHandlerFlag::ObserverFlag)
          {
            ObservationInfo observationInfo = request->getObservationInfo();
            if(ObserveAction::ObserveRegister == observationInfo.action)
              {
                m_interestedObservers.push_back(observationInfo.obsId);
              }
            else if(ObserveAction::ObserveUnregister == observationInfo.action)
              {
                m_interestedObservers.erase(std::remove(
                                                        m_interestedObservers.begin(),
                                                        m_interestedObservers.end(),
                                                        observationInfo.obsId),
                                            m_interestedObservers.end());
              }

            pthread_t threadId;

            cout << "\t\trequestFlag : Observer\n";
            gObservation = 1;
            static int startedThread = 0;

            // Observation happens on a different thread in ChangeLightRepresentation function.
            // If we have not created the thread already, we will create one here.
            if(!startedThread)
              {
                pthread_create (&threadId, NULL, ChangeLightRepresentation, (void *)this);
                startedThread = 1;
              }
            ehResult = OC_EH_OK;
          }
      }
    else
      {
        std::cout << "Request invalid" << std::endl;
      }

    return ehResult;
  }

};

// ChangeLightRepresentaion is an observation function,
// which notifies any changes to the resource to stack
// via notifyObservers
void * ChangeLightRepresentation (void *param)
{
  OnvifResource* lightPtr = (OnvifResource*) param;

  // This function continuously monitors for the changes
  while (1)
    {
      sleep (3);

      if (gObservation)
        {
          // If under observation if there are any changes to the light resource
          // we call notifyObservors
          //
          // For demostration we are changing the power value and notifying.
          cout << "Notifying observers with resource handle: " << lightPtr->getHandle() << endl;

          OCStackResult result = OC_STACK_OK;

          if(isListOfObservers)
            {
              std::shared_ptr<OCResourceResponse> resourceResponse(new OCResourceResponse());

              resourceResponse->setErrorCode(200);
              resourceResponse->setResourceRepresentation(lightPtr->get(), DEFAULT_INTERFACE);

              result = OCPlatform::notifyListOfObservers(  lightPtr->getHandle(),
                                                           lightPtr->m_interestedObservers,
                                                           resourceResponse);
            }
          else
            {
              result = OCPlatform::notifyAllObservers(lightPtr->getHandle());
            }

          if(OC_STACK_NO_OBSERVERS == result)
            {
              cout << "No More observers, stopping notifications" << endl;
              gObservation = 0;
            }
        }
    }

  return NULL;
}

void * handleSlowResponse (void *param, std::shared_ptr<OCResourceRequest> pRequest)
{
  // This function handles slow response case
  OnvifResource* lightPtr = (OnvifResource*) param;
  // Induce a case for slow response by using sleep
  std::cout << "SLOW response" << std::endl;
  sleep (10);

  auto pResponse = std::make_shared<OC::OCResourceResponse>();
  pResponse->setRequestHandle(pRequest->getRequestHandle());
  pResponse->setResourceHandle(pRequest->getResourceHandle());
  pResponse->setResourceRepresentation(lightPtr->get());
  pResponse->setErrorCode(200);
  pResponse->setResponseResult(OC_EH_OK);

  // Set the slow response flag back to false
  isSlowResponse = false;
  OCPlatform::sendResponse(pResponse);
  return NULL;
}

void PrintUsage()
{
  std::cout << std::endl;
  std::cout << "Usage : onvif_oic_server <ip address>\n";
}


int main(int argc, char* argv[])
{
  PrintUsage();

  isListOfObservers = false;
  isSecure = false;
  if (argc == 2) {
    thriftServerAddr = argv[1];
  }
  
  // Create PlatformConfig object
  PlatformConfig cfg {
    OC::ServiceType::InProc,
      OC::ModeType::Server,
      "0.0.0.0", // By setting to "0.0.0.0", it binds to all available interfaces
      0,         // Uses randomly available port
      OC::QualityOfService::LowQos
      };

  OCPlatform::Configure(cfg);
  try
    {
      // Create the instance of the resource class
      // (in this case instance of class 'OnvifResource').

      OnvifResource myLight;

      // Invoke createResource function of class light.
      OCPlatform::startPresence(30);
      myLight.createResource();
      std::cout << "Created resource." << std::endl;

      // myLight.addType(std::string("core.brightlight"));
      // myLight.addInterface(std::string("oc.mi.ll"));
      // std::cout << "Added Interface and Type" << std::endl;

      // A condition variable will free the mutex it is given, then do a non-
      // intensive block until 'notify' is called on it.  In this case, since we
      // don't ever call cv.notify, this should be a non-processor intensive version
      // of while(true);
      std::mutex blocker;
      std::condition_variable cv;
      std::unique_lock<std::mutex> lock(blocker);
      std::cout <<"Waiting" << std::endl;
      cv.wait(lock, []{return false;});
    }
  catch(OCException &e)
    {
      std::cout << "OCException in main : " << e.what() << endl;
    }

  // No explicit call to stop the platform.
  // When OCPlatform::destructor is invoked, internally we do platform cleanup

  return 0;
}

