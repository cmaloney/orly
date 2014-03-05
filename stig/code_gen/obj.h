/* <stig/code_gen/obj.h>

   Interface for generating object headers.

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

#include <map>
#include <string>

#include <jhm/naming.h>
#include <stig/code_gen/cpp_printer.h>
#include <stig/code_gen/inline.h>
#include <stig/type/impl.h>

namespace Stig {

  namespace CodeGen {

    /* TODO */
    class TGenObjError : public Base::TFinalError<TGenObjError> {
      public:

      /* Constructor. */
      TGenObjError(const Base::TCodeLocation &loc, const char *msg=0) {
        PostCtor(loc, msg);
      }
    };  // TGenObjError

    class TObjCtor : public TInline {
      NO_COPY_SEMANTICS(TObjCtor);
      public:

      typedef std::map<std::string, TInline::TPtr> TArgs;

      TObjCtor(const L0::TPackage *package, const Type::TType &type, TArgs &&args);

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        for (const auto &iter : Args) {
          dependency_set.insert(iter.second);
          iter.second->AppendDependsOn(dependency_set);
        }
      }

      private:
        TArgs Args;
    }; // TObjCtor


    void GenObjComparison(const Type::TType &obj1, const Type::TType &obj2, TCppPrinter &strm);
    void GenObjHeader(const Jhm::TAbsBase &out_dir, const Type::TType &obj_type);
    void GenObjInclude(const Type::TType &obj, TCppPrinter &strm);

  } // CodeGen

} // Stig