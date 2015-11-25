// boost trivial logger
#include <boost/log/trivial.hpp>

#include <inc/iotfs_oic_file.h>
#include <iotfs/utils.h>

using namespace std::placeholders;
using namespace OC;

namespace iotfs {
OICFile::OICFile(const std::string& resourceTypeName,
                 const std::vector<std::string>& resourceIntefaces,
                 const OICFileAttributes& attributes,
                 std::shared_ptr<OICStub> stub)
  :_fileName(resourceTypeName),
   _values(attributes),
   _stub(stub)
{
  setAllowedOperation(resourceIntefaces);
}

OICFile::~OICFile() {}

void OICFile::setAllowedOperation(const std::vector<std::string>& interfaces) {
  for (auto& interface : interfaces) {
    if (interface == OC::DEFAULT_INTERFACE) {
      _allowedOperation |= OICFileOperationRead;
      _allowedOperation |= OICFileOperationWrite;
    } else if (interface == OC::LINK_INTERFACE) {
      _allowedOperation |= OICFileOperationRead;
    } else if (interface == OC::BATCH_INTERFACE) {
      _allowedOperation |= OICFileOperationRead;
      _allowedOperation |= OICFileOperationWrite;
    } else if (interface == OC::GROUP_INTERFACE) {
      _allowedOperation |= OICFileOperationRead;
      _allowedOperation |= OICFileOperationWrite;
    }
  }
}

void OICFile::update() {
  if (!(_allowedOperation & OICFileOperationWrite)) {
    BOOST_LOG_TRIVIAL(error) << "Write Operation Is Not Allowed For This File" ;
    return;
  }

  if (_stub == nullptr) {
    BOOST_LOG_TRIVIAL(error) << "File Operation Error" ;
    return;
  }

  OCRepresentation rep;
  
  for (auto& it : _values) {
    if (it.second.type == AttributeType::Integer) {
      int value = boost::lexical_cast<int>(it.second.content);
      rep.setValue(it.first, value);
    } else if (it.second.type == AttributeType::String) {
      rep.setValue(it.first, it.second.content);
    } else if (it.second.type == AttributeType::Boolean) {
      bool value = boost::lexical_cast<bool>(it.second.content);
      rep.setValue(it.first, value);
    }
  }

  _stub->put(rep, QueryParamsMap(),
             std::bind(&OICFile::didUpdate, this, _1, _2, _3));
  _stub->wait();
}

void OICFile::didUpdate(const OC::HeaderOptions& headerOptions,
                        const OC::OCRepresentation& rep, const int eCode) {
  _stub->resume();
}

void OICFile::retrieve() {
  if (!(_allowedOperation & OICFileOperationRead)) {
    BOOST_LOG_TRIVIAL(error) << "Read Operation Is Not Allowed For This File";
    return;
  }

  if (_stub == nullptr) {
    BOOST_LOG_TRIVIAL(error) << "File Operation Error";
    return;
  }

  _stub->get(QueryParamsMap(),
             std::bind(&OICFile::didRetrieve, this, _1, _2, _3));
  _stub->wait();
}

void OICFile::didRetrieve(const OC::HeaderOptions& headerOptions,
                          const OC::OCRepresentation& rep, const int eCode) {
  if (eCode == OC_STACK_OK) {
    for (auto& it : rep) {
      std::string attrName = it.attrname();

      if (_values.find(attrName) != _values.end()) {
        _values[attrName].content = it.getValueToString();
      }
    }
  }

  _stub->resume();
}

int OICFile::read(std::ostream& os) {
  if (_stub == nullptr) {
    BOOST_LOG_TRIVIAL(error) << "Read File Error" ;
    return -1;
  }

  retrieve();

  for (auto& it : _values) {
    os << it.second.content;
  }

  return 0;
}

// This must be called with a '\n' !!!!
int OICFile::write(std::istream& is) {
  if (_stub == nullptr) {
    BOOST_LOG_TRIVIAL(error) << "Write File Error";
    return -1;
  }

  std::string result;
  is >> result;

  for (auto& it : _values) {
    it.second.content = result;
  }

  update();

  return 0;
}
}
