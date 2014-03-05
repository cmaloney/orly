/* <stig/code_gen/test.h>

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

#include <memory>

#include <stig/code_gen/top_func.h>
#include <stig/symbol/test/test.h>

namespace Stig {

  namespace CodeGen {

    class TTestBlock;

    class TTestCase : public TTopFunc {
      NO_COPY_SEMANTICS(TTestCase);
      public:

      TTestCase(const L0::TPackage *package, const Symbol::Test::TTestCase::TPtr &symbol, TId<TIdKind::Test>::TGen &id_gen);
      ~TTestCase();

      virtual void WriteCcName(TCppPrinter &out) const;
      const TId<TIdKind::Test> &GetId() const;
      virtual std::string GetName() const;
      virtual Type::TType GetReturnType() const;
      virtual Type::TType GetType() const;
      void Write(TCppPrinter &out) const;

      virtual void WriteName(TCppPrinter&) const;

      private:
      void WriteWrapperFunction(TCppPrinter &out) const;

      //Individual tests (Which may optionally have names, always have pos ranges).
      /* TODO: Pos, Name should actually be a one or the other type speicifcation. */
      TId<TIdKind::Test> Id;
      std::string Name;
      TPosRange Pos;
      TInline::TPtr Test;
      TTestBlock *OptChildren;
    };

    class TTestBlock {
      NO_COPY_SEMANTICS(TTestBlock);
      public:
      TTestBlock(const L0::TPackage *package, const Symbol::Test::TTestCaseBlock::TPtr &symbol, TId<TIdKind::Test>::TGen &id_gen);
      ~TTestBlock();

      void Write(TCppPrinter &out) const;
      void WriteMeta(TCppPrinter &out) const;

      private:
      std::vector<TTestCase*> Children;
    }; // TTestBlock

    class TWith {
      public:

      typedef std::vector<std::pair<TInline::TPtr, TInline::TPtr>> TNews;

      void Write(TCppPrinter &out) const;

      private:
      /* NOTE: Takes ownership of the code scope. */
      TWith(const L0::TPackage *package, const Symbol::Test::TWithClause::TPtr &symbol);

      std::unordered_set<std::shared_ptr<TInlineFunc>> ChildFuncs;
      std::unique_ptr<TCodeScope> CodeScope;
      TNews News;
      const L0::TPackage *Package;

      friend class TTest;
    };

    class TTest {
      NO_COPY_SEMANTICS(TTest);
      public:

      /* TODO: Move to a codegen specific with. */
      TTest(const L0::TPackage *package, const Symbol::Test::TTest::TPtr &symbol, TId<TIdKind::Test>::TGen &id_gen);

      const TId<TIdKind::Test> &GetId() const;
      void Write(TCppPrinter &out) const;

      private:
      void WriteWrapperFunction(TCppPrinter &out) const;

      TId<TIdKind::Test> Id;
      std::unique_ptr<TWith> OptWith;
      TTestBlock Tests;
    }; // TTest

    template <>
    void TCppPrinter::Write(const Tools::Nycr::TPos &pos);

    template <>
    void TCppPrinter::Write(const TPosRange &pos_range);

  } // CodeGen

} // Stig