/* <orly/csv_to_bin/synth.cc>

   Implements <orly/csv_to_bin/synth.h>.

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

#include <orly/csv_to_bin/synth.h>

#include <cassert>

#include <orly/csv_to_bin/sql.table.cst.h>
#include <tools/nycr/error.h>

using namespace std;
using namespace Orly::CsvToBin;

using Tools::Nycr::TError;
using Tools::Nycr::TPosRange;

Symbol::TKey::TOrder TranslateOrder(const Syntax::TOrder *that) {
  struct visitor_t final : public Syntax::TOrder::TVisitor {
    Symbol::TKey::TOrder &Order;
    visitor_t(Symbol::TKey::TOrder &order) : Order(order) {}
    virtual void operator()(const Syntax::TImplicitAsc  *) const override { Order = Symbol::TKey::Asc ; }
    virtual void operator()(const Syntax::TExplicitAsc  *) const override { Order = Symbol::TKey::Asc ; }
    virtual void operator()(const Syntax::TExplicitDesc *) const override { Order = Symbol::TKey::Desc; }
  };
  assert(that);
  Symbol::TKey::TOrder order;
  that->Accept(visitor_t(order));
  return order;
}

Symbol::TKey::TField TranslateColRef(
    const Symbol::TTable *table_symbol, const Syntax::TColRef *that) {
  assert(table_symbol);
  assert(that);
  return {
      table_symbol->FindCol(that->GetName()->GetLexeme().GetText()),
      TranslateOrder(that->GetOrder()) };
}

static vector<Symbol::TKey::TField> TranslateColRefList(
    const Symbol::TTable *table_symbol, const Syntax::TColRefList *that) {
  assert(that);
  vector<Symbol::TKey::TField> fields;
  for (;;) {
    fields.push_back(TranslateColRef(table_symbol, that->GetColRef()));
    const auto *tail = dynamic_cast<const Syntax::TColRefListTail *>(
        that->GetOptColRefListTail());
    if (!tail) {
      break;
    }
    that = tail->GetColRefList();
  }
  return move(fields);
}

static bool TranslateNull(const Syntax::TNull *that) {
  struct visitor_t final : public Syntax::TNull::TVisitor {
    bool &IsNull;
    visitor_t(bool &is_null) : IsNull(is_null) {}
    virtual void operator()(const Syntax::TImplicitNotNull *) const override { IsNull = false; }
    virtual void operator()(const Syntax::TExplicitNull    *) const override { IsNull = true ; }
    virtual void operator()(const Syntax::TExplicitNotNull *) const override { IsNull = false; }
  };
  assert(that);
  bool is_null;
  that->Accept(visitor_t(is_null));
  return is_null;
}

static Symbol::TType TranslateType(const Syntax::TType *that) {
  struct visitor_t final : public Syntax::TType::TVisitor {
    Symbol::TType &TypeSymbol;
    visitor_t(Symbol::TType &type_symbol) : TypeSymbol(type_symbol) {}
    virtual void operator()(const Syntax::TTimePntType  *) const override { TypeSymbol = Symbol::TType::TimePnt ; }
    virtual void operator()(const Syntax::TStrType      *) const override { TypeSymbol = Symbol::TType::Str     ; }
    virtual void operator()(const Syntax::TRealType     *) const override { TypeSymbol = Symbol::TType::Real    ; }
    virtual void operator()(const Syntax::TIntType      *) const override { TypeSymbol = Symbol::TType::Int     ; }
    virtual void operator()(const Syntax::TIdType       *) const override { TypeSymbol = Symbol::TType::Id      ; }
    virtual void operator()(const Syntax::TBoolType     *) const override { TypeSymbol = Symbol::TType::Bool    ; }
  };
  assert(that);
  Symbol::TType type_symbol;
  that->Accept(visitor_t(type_symbol));
  return type_symbol;
}

static void TranslateDef(
    Symbol::TTable *table_symbol, const Syntax::TDef *that) {
  struct visitor_t final : public Syntax::TDef::TVisitor {
    const Syntax::TSemi *&Semi;
    Symbol::TTable *TableSymbol;
    visitor_t(const Syntax::TSemi *&semi, Symbol::TTable *table_symbol)
        : Semi(semi), TableSymbol(table_symbol) {}
    virtual void operator()(
        const Syntax::TPrimaryKeyDef *that) const override {
      Semi = that->GetSemi();
      TableSymbol->SetPrimaryKey(make_unique<Symbol::TPrimaryKey>(
          TranslateColRefList(TableSymbol, that->GetColRefList())));
    }
    virtual void operator()(
        const Syntax::TSecondaryKeyDef *that) const override {
      Semi = that->GetSemi();
      TableSymbol->AddSecondaryKey(make_unique<Symbol::TSecondaryKey>(
          that->GetName()->GetLexeme().GetText(),
          TranslateColRefList(TableSymbol, that->GetColRefList())));
    }
    virtual void operator()(const Syntax::TColDef *that) const override {
      Semi = that->GetSemi();
      TableSymbol->AddCol(make_unique<Symbol::TCol>(
          that->GetName()->GetLexeme().GetText(),
          TranslateType(that->GetType()),
          TranslateNull(that->GetNull())));
    }
    virtual void operator()(const Syntax::TBadDef *) const override {}
  };
  assert(table_symbol);
  assert(that);
  const Syntax::TSemi *semi = nullptr;
  try {
    that->Accept(visitor_t(semi, table_symbol));
  } catch (const exception &ex) {
    TPosRange pos_range;
    if (semi) {
      pos_range = semi->GetLexeme().GetPosRange();
    }
    TError::TBuilder(pos_range) << ex.what();
  }
}

unique_ptr<Symbol::TTable> Orly::CsvToBin::NewTable(
    ostream &strm, const char *src_text) {
  assert(&strm);
  auto symbol = make_unique<Symbol::TTable>();
  try {
    auto syntax = Syntax::TTable::ParseStr(src_text);
    if (syntax) {
      const Syntax::TDefSeq *def_seq = syntax->GetDefSeq();
      do {
        TranslateDef(symbol.get(), def_seq->GetDef());
        def_seq =
            dynamic_cast<const Syntax::TDefSeq *>(def_seq->GetOptDefSeq());
      } while (def_seq);
    }
    symbol->Verify();
  } catch (const exception &ex) {
    TError::TBuilder(TPosRange()) << ex.what();
  }
  if (TError::GetFirstError()) {
    TError::PrintSortedErrors(strm);
    symbol.reset();
  }
  return move(symbol);
}
