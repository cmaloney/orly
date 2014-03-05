/* <stig/code_gen/take.h>

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

#include <stig/code_gen/function.h>
#include <stig/code_gen/inline.h>

namespace Stig {

  namespace CodeGen {

    class TTake
        : public TInline {
      NO_COPY_SEMANTICS(TTake);
      public:

      typedef std::shared_ptr<TTake> TPtr;

      static TPtr New(
          const L0::TPackage *package,
          const Type::TType &ret_type,
          const TInline::TPtr &seq,
          const TInline::TPtr &count);

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        dependency_set.insert(Count);
        Count->AppendDependsOn(dependency_set);
        dependency_set.insert(Seq);
        Seq->AppendDependsOn(dependency_set);
      }

      private:

      TTake(const L0::TPackage *package,
            const Type::TType &ret_type,
            const TInline::TPtr &seq,
            const TInline::TPtr &count);

      TInline::TPtr Count;

      TInline::TPtr Seq;

    };  // TTake

  }  // CodeGen

}  // Stig