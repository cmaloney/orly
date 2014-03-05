/* <stig/code_gen/typed_leaf.h>

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
#include <stig/shared_enum.h>

namespace Stig {

  namespace CodeGen {

    class TTypedLeaf : public TInline {
      NO_COPY_SEMANTICS(TTypedLeaf);
      public:

      typedef std::shared_ptr<const TTypedLeaf> TPtr;

      enum TKind { Unknown, Free };

      TTypedLeaf(const L0::TPackage *package, TKind kind, const Type::TType &type, TAddrDir addr_dir /* only for free */);

      void WriteExpr(TCppPrinter &out) const;

      /* Dependency graph */
      virtual void AppendDependsOn(std::unordered_set<TInline::TPtr> &/*dependency_set*/) const override {
        assert(this);
      }

      virtual bool IsFree() const {
        return Kind == Free;
      }

      private:
      TKind Kind;
      TAddrDir AddrDir;
    }; // TTypedLeaf

  } // CodeGen

} // Stig

namespace std {

  template <>
  struct hash<Stig::CodeGen::TTypedLeaf::TKind> {

    typedef size_t result_type;
    typedef Stig::CodeGen::TTypedLeaf::TKind argument_type;

    result_type operator()(const argument_type &that) {
      return static_cast<result_type>(that);
    }

  };  // hash<Stig::CodeGen::TTypedLeaf::TKind>

}  // std
