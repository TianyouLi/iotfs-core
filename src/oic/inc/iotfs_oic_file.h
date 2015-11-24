#ifndef __IOTFS__IOTFS_OIC_FILE_H
#define __IOTFS__IOTFS_OIC_FILE_H

#include <string>
#include <vector>
#include <map>

#include <AttributeValue.h>
#include <OCPlatform.h>
#include <OCApi.h>

#include <iotfs/iotfs.h>
#include <iotfs/iotfs_file.h>
#include <inc/iotfs_oic_stub.h>

using namespace fusekit;
using namespace std::placeholders;

namespace iotfs {
typedef struct OICFileDescriptor {
  std::string content;
  OC::AttributeType type;
} OICFileDescriptor;

typedef std::map<std::string, OICFileDescriptor> OICFileAttributes;

typedef enum OICFileOpeartion {
  OICFileOperationRead = 1 << 0,
  OICFileOperationWrite = 1 << 1
} OICFileOpeartion;

class OICFile : public IOTFile {
 public:
  OICFile(const std::string& resourceTypeName,
          const std::vector<std::string>& resourceIntefaces,
          const OICFileAttributes& attributes,
          std::shared_ptr<OICStub> stub);
  virtual ~OICFile();

  virtual int read(std::ostream& os);
  virtual int write(std::istream& is);

 private:
  std::string _fileName;
  std::string _resourceURI;
  int _allowedOperation;
  OICFileAttributes _values;
  std::shared_ptr<OICStub> _stub;

  void setAllowedOperation(const std::vector<std::string>& interfaces);

  void update();
  void didUpdate(const OC::HeaderOptions& headerOptions,
                 const OC::OCRepresentation& rep, const int eCode);

  void retrieve();
  void didRetrieve(const OC::HeaderOptions& headerOptions,
                   const OC::OCRepresentation& rep, const int eCode);
};
}

#endif
