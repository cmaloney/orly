/* <stig/synth/type.h>

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

#include <cassert>
#include <functional>

#include <base/no_copy_semantics.h>
#include <stig/type.h>
#include <stig/synth/scope_and_def.h>

namespace Stig {

  namespace Synth {

    /* TODO */
    class TType {
      NO_COPY_SEMANTICS(TType);
      public:

      /* TODO */
      virtual ~TType();

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb) = 0;

      /* TODO */
      const Type::TType &GetSymbolicType() const;

      protected:

      /* TODO */
      TType();

      /* TODO */
      virtual Type::TType ComputeSymbolicType() const = 0;

      private:

      /* TODO */
      mutable Type::TType CachedSymbolicType;

    };  // TType

  }  // Synth

}  // Stig
