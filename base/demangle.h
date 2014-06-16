/* <base/demangle.h>

   Demangle C++ names.

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

#pragma once

#include <memory>
#include <ostream>
#include <typeinfo>

namespace Base {

  /* Wrap a unique_ptr pointing to the name. Note the name may or may not be allocated (The deleter may just noop).
     Also: Don't edit the string under any circumstances (It might be in the RO section of the binary frome the typeinfo
     struct.

     NOTE: We make our own type because the internals are possibly inthe RO section of the binary so const casting this
     string may in some cases SIGSEGV your application.
     */
  class TDemanlgeStr : public std::unique_ptr<const char, void (*)(const char*)> {
    using TParent = std::unique_ptr<const char, void (*)(const char*)>;
    using TParent::TParent;
  };

  TDemanlgeStr Demangle(const std::type_info &t);
  TDemanlgeStr Demangle(const char *name);

  /* Return the type ot TVal demangled. Always returns a value. */
  template<typename TVal>
  TDemanlgeStr Demangle() {
    return Demangle(typeid(TVal));
  }

  std::ostream &operator<<(std::ostream &out, const TDemanlgeStr &ptr);

} // Base