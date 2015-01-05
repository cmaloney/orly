/* <base/demangle.cc>

   Implements <base/demangle.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <base/demangle.h>

#include <cxxabi.h>

using namespace Base;

TDemangleStr Base::Demangle(const std::type_info &t) {
  auto ret = Demangle(t.name());
  if(!ret) {
    ret = TDemangleStr(t.name(), [](const char *) {});
  }
  return ret;
}

TDemangleStr Base::Demangle(const char *name) {
  int status;
  TDemangleStr ret(abi::__cxa_demangle(name, nullptr, nullptr, &status),
                   [](const char *ptr) { free(const_cast<char *>(ptr)); });
  if(status != 0) {
    ret.reset();
  }
  return ret;
}

std::ostream &Base::operator<<(std::ostream &out, const TDemangleStr &ptr) {
  if(ptr) {
    out << ptr.get();
  }
  return out;
}
