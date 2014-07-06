/* <tools/nycr/context.h>

   Nycr parsing context (aggregates errors)

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

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <ostream>
#include <tuple>
#include <vector>

#include <base/class_traits.h>
#include <tools/nycr/pos_range.h>

namespace Tools {
  namespace Nycr {

    // TODO: Stable insertion sort the errors so users don't have to manually call SortErrors?
    /* A context which aggregates errors which occur during translation / construction. */
    class TContext {
      public:
      MOVE_ONLY(TContext);
      TContext() = default;

      using TError = std::tuple<TPosRange, std::string>;

      void ClearErrors();
      bool HasErrors() const;
      void AddError(TPosRange pos, std::string &&msg);
      void SortErrors();
      bool ForEachError(const std::function<bool (const TPosRange &pos, const std::string &msg)> &cb) const;
      void PrintErrors(std::ostream &out) const;

      private:
      std::vector<TError> Errors;
    };

    /* Wraps the given type so that the type can only be gotten out if there haven't been any errors. */
    template<typename TTarget>
    class TContextBuilt : public TContext {
      public:
      MOVE_ONLY(TContextBuilt);
      TContextBuilt() = default;

      TTarget *Get() {
        assert(!HasErrors());
        assert(Target);
        return Target.get();
      }

      TTarget *Get() const {
        assert(!HasErrors());
        assert(Target);
        return Target.get();
      }

      void Set(TTarget *target) {
        Target = std::unique_ptr<TTarget>(target);
      }

      private:
      std::unique_ptr<TTarget> Target;
    };


  } // Nycr
} // Toosl