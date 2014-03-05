/* <stig/symbol/package.h>

   TODO

   Copyright 2010-2014 Stig LLC

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

#include <base/no_copy_semantics.h>
#include <jhm/naming.h>
#include <stig/symbol/scope.h>

namespace Stig {

  namespace Symbol {

    /* Top level package. */
    class TPackage
        : public TScope {
      NO_COPY_SEMANTICS(TPackage);
      public:

      /* Convenience typedef for std::shared_ptr<TPackage> */
      typedef std::shared_ptr<TPackage> TPtr;

      /* Returns a std::shared_ptr to a new TPackage instance */
      static TPtr New(const Jhm::TNamespace &ns, unsigned int version);

      /* Return the Package's namespace. */
      const Jhm::TNamespace &GetNamespace() const;

      /* Returns the package version number */
      unsigned int GetVersion() const;

      private:

      /* Do-little */
      TPackage(const Jhm::TNamespace &ns, unsigned int version);

      /* See accessor */
      Jhm::TNamespace Namespace;

      /* See accessor */
      unsigned int Version;

    };  // TPackage

  }  // Symbol

}  // Stig
