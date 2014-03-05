/* <stig/code_gen/reduce.h>

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

#include <stig/code_gen/inline.h>

namespace Stig {

  namespace CodeGen {

    class TImplicitFunc;

    class TReduce : public TInline {
      NO_COPY_SEMANTICS(TReduce);
      public:

      typedef std::shared_ptr<TReduce> TPtr;
      typedef std::shared_ptr<TImplicitFunc> TFuncPtr;

      static TPtr New(const L0::TPackage *package,
                      const Type::TType &ret_type,
                      const TInline::TPtr &seq,
                      const TInline::TPtr &start,
                      const TFuncPtr &reduce_func);

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override;

      private:
      TReduce(const L0::TPackage *package,
              const Type::TType &ret_type,
              const TInline::TPtr &seq,
              const TInline::TPtr &start,
              const TFuncPtr &reduce_func);

      TFuncPtr Func;
      TInline::TPtr Seq, Start;
    }; // TReduce


  } // CodeGen

} // Stig