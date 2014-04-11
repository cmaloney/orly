/* <orly/code_gen/effect.h>

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
#include <unordered_map>

#include <orly/code_gen/cpp_printer.h>
#include <orly/code_gen/package_base.h>
#include <orly/error.h>
#include <orly/shared_enum.h>
#include <orly/type/part.h>

namespace Orly {

  namespace CodeGen {

    //TODO: Promote this to some common place, make use of it everywhere.
    template <typename TTarget>
    using TPtrC = std::shared_ptr<const TTarget>;

    template <typename TTarget>
    using TPtr = std::shared_ptr<TTarget>;

    class TInline;

    struct TMutablePart {
      uint32_t Index;
      TPtrC<TInline> Mutable;
    };

  } // CodeGen

} // Orly

namespace std {
  template <>
  struct hash<Orly::CodeGen::TMutablePart> {
    typedef size_t result_type;
    typedef Orly::CodeGen::TMutablePart argument_type;

    result_type operator()(const argument_type &that) const {
      //TODO: Use a better hash
      return std::hash<Orly::CodeGen::TPtrC<Orly::CodeGen::TInline>>()(that.Mutable) ^ that.Index;
    }
  }; //hash<Orly::CodeGen::TMutablePart>
}

namespace Orly {

  namespace CodeGen {

    //NOTE: There is a class hierarchy in <orly/var/mutation.h> which is almost identical to this and should be changed
    //      in parallel.
    class TChange {
      NO_COPY_SEMANTICS(TChange);
      public:

      virtual ~TChange() {}

      virtual void Write(TCppPrinter &out) const = 0;

      virtual Type::TType GetValType() const = 0;

      protected:
      TChange() {}
    }; // TChange

    class TMutation : public TChange {
      NO_COPY_SEMANTICS(TMutation);
      public:

      static TPtrC<TMutation> New(const TPtrC<TInline> &mutable_, TMutator mutation, const TPtrC<TInline> &rhs);

      void Write(TCppPrinter &out) const final;

      virtual Type::TType GetValType() const override final;

      private:
      TMutation(const TPtrC<TInline> &mutable_, TMutator mutation, const TPtrC<TInline> &rhs);

      TPtrC<TInline> Mutable;
      TMutator Mutation;
      TPtrC<TInline> Rhs;
    }; // TMutation

    class TDelete : public TChange {
      public:

      static TPtrC<TDelete> New(const Type::TType &val_type);

      void Write(TCppPrinter &out) const final;

      virtual Type::TType GetValType() const override final;

      private:
      TDelete(const Type::TType &val_type) : ValType(val_type) {}

      const Type::TType ValType;

    }; // TDelete

    class TNew : public TChange {
      public:

      static TPtrC<TNew> New(const TPtrC<TInline> &val);

      void Write(TCppPrinter &out) const final;

      virtual Type::TType GetValType() const override final;

      private:
      TNew(const TPtrC<TInline> &val);

      TPtrC<TInline> Val;
    }; // TNew

    class TStmt {
      NO_COPY_SEMANTICS(TStmt);
      public:

      virtual ~TStmt() {}

      virtual void Write(TCppPrinter &out) const = 0;

      protected:
      TStmt(const L0::TPackage *package) : Package(package) {}

      const L0::TPackage *Package;

    }; // TStmt

    struct TMutate : public TStmt {
      TMutate(const L0::TPackage *package, const TPtrC<TInline> &mutable_, const TPtrC<TChange> &change)
          : TStmt(package), Mutable(mutable_), Change(change) {}
      TPtrC<TInline> Mutable;
      TPtrC<TChange> Change;
      void Write(TCppPrinter &out) const final;
    }; // TMutate

    class TStmtBlock {
      NO_COPY_SEMANTICS(TStmtBlock);
      public:
      TStmtBlock();

      void Add(const TPtrC<TStmt> &stmt);
      void Add(const L0::TPackage *package, const TPtrC<TInline> &key, TMutator mutation, const TPtrC<TInline> &rhs);
      void AddDelete(const L0::TPackage *package, const TPtrC<TInline> &key, const Type::TType &val_type);
      void AddNew(const L0::TPackage *package, const TPtrC<TInline> &key, const TPtrC<TInline> &rhs);

      bool IsEmpty() const;

      void Write(TCppPrinter &out) const;

      private:
      std::vector<TPtrC<TStmt>> Stmts;
    }; // TStmtBlock

    class TPredicatedBlock {
      NO_COPY_SEMANTICS(TPredicatedBlock);
      public:

      TPredicatedBlock(const TPtrC<TInline> &condition);

      TStmtBlock &GetStmts();

      void Write(TCppPrinter &out) const;

      private:
      TPtrC<TInline> Condition;
      TStmtBlock Stmts;
    }; // TPredicatedBlock

    class TIf : public TStmt {
      NO_COPY_SEMANTICS(TIf);
      public:

      typedef std::vector<TPtrC<TPredicatedBlock>> TPredicatedBlocks;

      TIf(const L0::TPackage *package, TPredicatedBlocks &&if_clauses);

      void SetOptElseClause();
      TStmtBlock &GetElseClause();

      void Write(TCppPrinter &out) const final;

      private:
      TPredicatedBlocks IfClauses;
      std::unique_ptr<TStmtBlock> OptElseClause;
    }; // TIf

  } // CodeGen

} // Orly