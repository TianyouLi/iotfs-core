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

class OICFile : public IoTFile {
public:
  OICFile(const std::string &resourceTypeName,
          const std::vector<std::string> &resourceIntefaces,
          const OICFileAttributes &attributes, std::shared_ptr<OICStub> stub);
  virtual ~OICFile();

  virtual int read(std::ostream &os);
  virtual int write(std::istream &is);

private:
  std::string _fileName;
  std::string _resourceURI;
  int _allowedOperation;
  OICFileAttributes _values;
  std::shared_ptr<OICStub> _stub;

  void setAllowedOperation(const std::vector<std::string> &interfaces);

  void update();
  void didUpdate(const OC::HeaderOptions &headerOptions,
                 const OC::OCRepresentation &rep, const int eCode);

  void retrieve();
  void didRetrieve(const OC::HeaderOptions &headerOptions,
                   const OC::OCRepresentation &rep, const int eCode);
};
}

#endif
