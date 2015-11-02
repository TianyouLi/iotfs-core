#include <iotfs/oic/iotfs_oic_file.h>
#include <iotfs/utils.h>

using namespace std::placeholders;
using namespace OC;

namespace iotfs {
OICFile::OICFile(const std::string& resourceTypeName,
                 const std::vector<std::string>& resourceIntefaces,
                 const OICFileAttributes& attributes):_fileName(resourceTypeName), _values(attributes)
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
    std::cout << "Write Operation Is Not Allowed For This File" << std::endl;
    return;
  }

  if (_stub == NULL) {
    std::cout << "File Operation Error" << std::endl;
    return;
  }

  OCRepresentation rep;
  std::map<std::string, OICFileDescriptor>::iterator it;
  for (it = _values.begin(); it != _values.end(); ++it) {
    if (it->second.type == AttributeType::Integer) {
      int value = boost::lexical_cast<int>(it->second.content);
      rep.setValue(it->first, value);
    } else if (it->second.type == AttributeType::String) {
      rep.setValue(it->first, it->second.content);
    } else if (it->second.type == AttributeType::Boolean) {
      bool value = boost::lexical_cast<bool>(it->second.content);
      rep.setValue(it->first, value);
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
    std::cout << "Read Operation Is Not Allowed For This File" << std::endl;
    return;
  }

  if (_stub == NULL) {
    std::cout << "File Operation Error" << std::endl;
    return;
  }

  _stub->get(QueryParamsMap(),
             std::bind(&OICFile::didRetrieve, this, _1, _2, _3));
  _stub->wait();
}

void OICFile::didRetrieve(const OC::HeaderOptions& headerOptions,
                          const OC::OCRepresentation& rep, const int eCode) {
  if (eCode == OC_STACK_OK) {
    OCRepresentation::const_iterator it = rep.begin();
    for (; it != rep.end(); ++it) {
      std::string attrName = it->attrname();

      if (_values.find(attrName) != _values.end()) {
        _values[attrName].content = it->getValueToString();
      }
    }
  }

  _stub->resume();
}

int OICFile::read(std::ostream& os) {
  if (_stub == NULL) {
    std::cout << "Read File Error" << std::endl;
    return -1;
  }

  retrieve();

  std::map<std::string, OICFileDescriptor>::iterator it;
  ;
  for (it = _values.begin(); it != _values.end(); ++it) {
    os << it->second.content;
  }

  return 0;
}

// This must be called with a '\n' !!!!
int OICFile::write(std::istream& is) {
  if (_stub == NULL) {
    std::cout << "Write File Error" << std::endl;
    return -1;
  }

  std::string result;
  is >> result;

  std::map<std::string, OICFileDescriptor>::iterator it;
  ;
  for (it = _values.begin(); it != _values.end(); ++it) {
    it->second.content = result;
  }

  update();

  return 0;
}
}
