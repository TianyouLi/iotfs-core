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

#include <inc/iotfs_oic_file.h>
#include <iotfs/utils.h>

using namespace std::placeholders;
using namespace OC;

namespace iotfs {
OICFile::OICFile(const std::string &resourceTypeName,
                 const std::vector<std::string> &resourceIntefaces,
                 const OICFileAttributes &attributes,
                 std::shared_ptr<OICStub> stub)
    : _fileName(resourceTypeName), _values(attributes), _stub(stub) {
  setAllowedOperation(resourceIntefaces);
}

OICFile::~OICFile() {}

void OICFile::setAllowedOperation(const std::vector<std::string> &interfaces) {
  for (auto &interface : interfaces) {
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
    BOOST_LOG_TRIVIAL(error) << "Write Operation Is Not Allowed For This File";
    return;
  }

  if (_stub == nullptr) {
    BOOST_LOG_TRIVIAL(error) << "File Operation Error";
    return;
  }

  OCRepresentation rep;

  for (auto &it : _values) {
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
}

void OICFile::didUpdate(const OC::HeaderOptions &headerOptions,
                        const OC::OCRepresentation &rep, const int eCode) {}

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
}

void OICFile::didRetrieve(const OC::HeaderOptions &headerOptions,
                          const OC::OCRepresentation &rep, const int eCode) {
  if (eCode == OC_STACK_OK) {
    for (auto &it : rep) {
      std::string attrName = it.attrname();

      if (_values.find(attrName) != _values.end()) {
        _values[attrName].content = it.getValueToString();
      }
    }
  }
}

int OICFile::read(std::ostream &os) {
  if (_stub == nullptr) {
    BOOST_LOG_TRIVIAL(error) << "Read File Error";
    return -1;
  }

  retrieve();

  for (auto &it : _values) {
    os << it.second.content;
  }

  return 0;
}

// This must be called with a '\n' !!!!
int OICFile::write(std::istream &is) {
  if (_stub == nullptr) {
    BOOST_LOG_TRIVIAL(error) << "Write File Error";
    return -1;
  }

  std::string result;
  is >> result;

  for (auto &it : _values) {
    it.second.content = result;
  }

  update();

  return 0;
}
}
