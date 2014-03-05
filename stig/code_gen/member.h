/* <stig/code_gen/member.h>

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

    class TMutableRewrap : public TInline {
      NO_COPY_SEMANTICS(TMutableRewrap);
      public:

      virtual void WriteExpr(TCppPrinter &out) const;

      virtual void WritePartId(TCppPrinter &out) const = 0;
      virtual void WritePostfixOp(TCppPrinter &out) const = 0;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &dependency_set) const override {
        assert(this);
        dependency_set.insert(Src);
        Src->AppendDependsOn(dependency_set);
      }

      protected:
      TMutableRewrap(const L0::TPackage *package, const Type::TType &return_type, const TInline::TPtr &src);

      const TInline::TPtr Src;
    }; // TMutableRewrap


    class TAddrMember : public TMutableRewrap  {
      NO_COPY_SEMANTICS(TAddrMember);
      public:

      TAddrMember(const L0::TPackage *package, const Type::TType &return_type, const TInline::TPtr &addr, size_t index);

      virtual void WriteExpr(TCppPrinter &out) const;
      void WritePartId(TCppPrinter &out) const final;
      void WritePostfixOp(TCppPrinter &out) const;

      private:
      size_t Index;
    }; // TAddrMember

    class TObjMember : public TMutableRewrap {
      NO_COPY_SEMANTICS(TObjMember);

      public:

      TObjMember(const L0::TPackage *package, const Type::TType &return_type, const TInline::TPtr &obj, const std::string &name);

      void WritePartId(TCppPrinter &out) const final;
      void WritePostfixOp(TCppPrinter &out) const;

      private:
      std::string Name;
    }; // TObjMember

  } // CodeGen

} // Stig