/* <orly/synth/addr_ctor.h>

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

#include <functional>
#include <vector>

#include <base/class_traits.h>
#include <orly/orly.package.cst.h>
#include <orly/synth/addr_dir_visitor.h>
#include <orly/synth/expr.h>
#include <orly/synth/new_expr.h>
#include <orly/synth/scope_and_def.h>

namespace Orly {

  namespace Synth {

    /* TODO */
    class TAddrCtor
        : public TExpr {
      NO_COPY(TAddrCtor);
      public:

      /* TODO */
      TAddrCtor(const TExprFactory *expr_factory, const Package::Syntax::TAddrCtor *addr_ctor);

      /* TODO */
      virtual ~TAddrCtor();

      /* TODO */
      virtual Expr::TExpr::TPtr Build() const;

      /* TODO */
      virtual void ForEachInnerScope(const std::function<void (TScope *)> &cb);

      /* TODO */
      virtual void ForEachRef(const std::function<void (TAnyRef &)> &cb);

      private:

      /* TODO */
      void Cleanup();

      /* TODO */
      const Package::Syntax::TAddrCtor *AddrCtor;

      /* TODO */
      std::vector<std::pair<TAddrDir, TExpr *>> Members;

    };  // TAddrCtor

  }  // Synth

}  // Orly
