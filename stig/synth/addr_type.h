/* <stig/synth/addr_type.h>

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

#include <vector>

#include <stig/stig.package.cst.h>
#include <stig/synth/addr_dir_visitor.h>
#include <stig/synth/type.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TAddrType
        : public TType {
      NO_COPY_SEMANTICS(TAddrType);
      public:

      /* TODO */
      TAddrType(const Package::Syntax::TAddrType *addr_type);

      /* TODO */
      virtual ~TAddrType();

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      private:

      void Cleanup();

      /* TODO */
      virtual Type::TType ComputeSymbolicType() const;

      /* TODO */
      std::vector<std::pair<TAddrDir, TType *>> Members;

    };  // TType

  }  // Synth

}  // Stig
