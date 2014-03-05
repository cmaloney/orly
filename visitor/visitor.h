/* <visitor/visitor.h>

   The visitor pattern.

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

#include <base/no_construction.h>
#include <visitor/applier.h>
#include <visitor/double.h>
#include <visitor/computer.h>
#include <visitor/single.h>
#include <visitor/pass.h>

namespace Visitor {

  namespace Alias {

    /* Visitors that handle the sum of a set of types. */
    template <typename TVisitor>
    struct Single {
      NO_CONSTRUCTION(Single);

      /* A visitor that returns TResult.  See <visitor/computer.h> for details. */
      template <typename TResult>
      using TComputer = Visitor::TComputer<TResult, TVisitor>;

      /* A visitor that invokes TVerb::operator().  See <visitor/applier.h> for details. */
      template <typename TVerb>
      using TApplier = Visitor::Single::TApplier<TVerb, TVisitor>;

    };  // Single

    /* Visitors that handle the cartesian product of a set of types. If only TLhsVisitor and TRhsVisitor is specified,
       Visitor::Double::TVisitor template is used to generate the abstract double visitor. If the double visitor template
       instantiation is too slow, write a manual one and specify it as the third template argument. */
    template <typename TLhsVisitor,
              typename TRhsVisitor,
              typename TDoubleVisitor = Visitor::Double::TVisitor<TLhsVisitor, TRhsVisitor>>
    struct Double {
      NO_CONSTRUCTION(Double);

      /* A visitor that returns TResult.  See <visitor/computer.h> for details. */
      template <typename TResult>
      using TComputer = Visitor::TComputer<TResult, TDoubleVisitor>;

      /* A visitor that invokes TVerb::operator().  See <visitor/applier.h> for details. */
      template <typename TVerb>
      using TApplier = Visitor::Double::TApplier<TVerb, TDoubleVisitor>;

    };  // Double

  }  // Alias

}  // Visitor
