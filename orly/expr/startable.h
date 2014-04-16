/* <orly/expr/startable.h>

   TODO

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

#include <base/class_traits.h>
#include <orly/expr/start.h>
#include <orly/type/impl.h>

namespace Orly {

  namespace Expr {

    class TStartable {
      NO_COPY(TStartable);
      public:

      using TPtr = std::shared_ptr<TStartable>;

      virtual ~TStartable() {}

      const TStart::TPtr &GetStart() const {
        assert(this);
        assert(Start);
        return Start;
      }

      const TStart::TPtr &TryGetStart() const {
        assert(this);
        return Start;
      }

      void SetStart(const TStart::TPtr &start) {
        assert(this);
        assert(start);
        assert(!Start);
        Start = start;
      }

      protected:

      TStartable()
          : Start(nullptr) {}

      TStart::TPtr Start;

    };  // TStartable

  }  // Expr

}  // Orly
