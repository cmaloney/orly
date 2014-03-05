/* <stig/code_gen/effect.cc>

   Implements <stig/code_gen/effect.h>

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

#include <stig/code_gen/effect.h>

#include <base/impossible_error.h>
#include <stig/code_gen/context.h>
#include <stig/code_gen/inline.h>
#include <stig/code_gen/scope.h>
#include <stig/type/mutable.h>
#include <stig/type/seq.h>
#include <stig/type/unwrap.h>
#include <stig/type/util.h>

using namespace Stig;
using namespace Stig::CodeGen;

TPtrC<TMutation> TMutation::New(const TPtrC<TInline> &mutable_, TMutator mutation, const TPtrC<TInline> &rhs) {
  return TPtrC<TMutation>(new TMutation(mutable_, mutation, rhs));
}

void TMutation::Write(TCppPrinter &out) const {
  out << "Var::TMutation::New(TMutator::";
  switch(Mutation) {
    case TMutator::Add: out << "Add";
      break;
    case TMutator::And: out << "And";
      break;
    case TMutator::Assign: out << "Assign";
      break;
    case TMutator::Div: out << "Div";
      break;
    case TMutator::Exp: out << "Exp";
      break;
    case TMutator::Intersection: out << "Intersection";
      break;
    case TMutator::Mod: out << "Mod";
      break;
    case TMutator::Mult: out << "Mult";
      break;
    case TMutator::Or: out << "Or";
      break;
    case TMutator::SymmetricDiff: out << "SymmetricDiff";
      break;
    case TMutator::Sub: out << "Sub";
      break;
    case TMutator::Union: out << "Union";
      break;
    case TMutator::Xor: out << "Xor";
      break;
  };
  out << ", " << Rhs << ")";
}

TMutation::TMutation(const TPtrC<TInline> &mutable_, TMutator mutation, const TPtrC<TInline> &rhs)
    : Mutable(mutable_), Mutation(mutation), Rhs(rhs) {}

Type::TType TMutation::GetValType() const {
  Type::TType ret_type = Mutable->GetReturnType();
  const Type::TMutable *m_type = ret_type.As<Type::TMutable>();
  return m_type->GetSrcAtAddr();
  throw std::logic_error("TODO: TMutation::GetValType()");
}

TPtrC<TDelete> TDelete::New(const Type::TType &val_type) {
  return TPtrC<TDelete>(new TDelete(val_type));
}

void TDelete::Write(TCppPrinter &out) const {
  out << "Var::TDelete::New()";
}

Type::TType TDelete::GetValType() const {
  return ValType;
}

TPtrC<TNew> TNew::New(const TPtrC<TInline> &var) {
  return TPtrC<TNew>(new TNew(var));
}

Type::TType TNew::GetValType() const {
  return Val->GetReturnType();
}

void TNew::Write(TCppPrinter &out) const {
  assert(this);

  out << "Var::TNew::New(Var::ToVar(*Sabot::State::TAny::TWrapper(Native::State::New(" << Val << ", alloca(Sabot::State::GetMaxStateSize())))))";
}

TNew::TNew(const TPtrC<TInline> &val) : Val(val) {}

void TMutate::Write(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  if(Mutable->GetReturnType().Is<Type::TSeq>()) {
    out << "for(auto it = " << Mutable << "->NewCursor(); it; ++it) {" << Eol
        << "  ctx.AddEffect((*it)";
  } else {
    out << "ctx.AddEffect(" << Mutable;
  }

  if(Type::UnwrapSequence(Mutable->GetReturnType()).Is<Type::TMutable>()) {
    //TODO: Check that the address is known and throw if not before calling GetVal on the optional.
    out << ".GetAddr()"; //NOTE: This keeps us from putting mutables in as databse keys.
  }

  if(Type::UnwrapSequence(Mutable->GetReturnType()).Is<Type::TOpt>()) {
    out << ".GetVal()";
  }

  out << ", ";

  /* this is where we write out the index id */ {
    Type::TType addr_type = Type::UnwrapSequence(Mutable->GetReturnType());
    if (addr_type.Is<Type::TMutable>()) {
      addr_type = addr_type.As<Type::TMutable>()->GetAddr();
    }
    addr_type = Type::UnwrapOptional(addr_type);
    Type::TType val_type = Change->GetValType();
    const Base::TUuid &index_id = Package->GetIndexIdFor(addr_type, val_type);
    char uuid[37];
    index_id.FormatUnderscore(uuid);
    out << TStigNamespace(Package->GetNamespace()) << "::My" << uuid << " ,";
  }

  /* mutable */
  if(Mutable->GetReturnType().Is<Type::TMutable>()) {
    class TVisitor : public Type::TPart::TVisitor {
      public:

      TVisitor(TCppPrinter &out, const TPtrC<TInline> &mutable_, uint32_t &mutable_var_count)
          : Mutable(mutable_), MutableVarCount(mutable_var_count), Out(out) {}

      virtual void operator()(const Type::TAddrMember *that) const {
        Write("AddrChange", that->GetIndex());
      }
      virtual void operator()(const Type::TListIndex *) const {
        Write("ListChange", Type::TInt::Get());
      }
      virtual void operator()(const Type::TDictMember *that) const {
        Write("DictChange", that->GetKeyType());
      }
      virtual void operator()(const Type::TObjMember  *that) const {
        Write("ObjChange", that->GetName());
      }

      private:

      /* TODO: Would be nice to be able to use a template for these first two... */
      void Write(const char *name, int arg) const {
        Out << "Var::T" << name << "::New(" << arg << ", ";
      }

      void Write(const char *name, const std::string &arg) const {
        Out << "Var::T" << name << "::New(\"" << arg << "\", ";
      }

      void Write(const char *name, const Type::TType &type) const {
        Out << "Var::T" << name << "::New(Stig::Var::TVar::TDt<" << type << ">::As(" << Mutable << ".GetParts().at("
            << MutableVarCount << ")), ";
        ++MutableVarCount;
      }


      const TPtrC<TInline> &Mutable;
      uint32_t &MutableVarCount;
      TCppPrinter &Out;
    }; // TVisitor

    auto type = Mutable->GetReturnType();
    const auto &parts = type.As<Type::TMutable>()->GetParts();
    //Write out starts
    /* visitor */ {
      uint32_t MutableVarCount = 0;
      TVisitor vis(out, Mutable, MutableVarCount);
      for(auto &part: parts) {
        part->Accept(vis);
      }
    }

    Change->Write(out);

    for(size_t i = 0; i < parts.size(); ++i) {
      out << ")";
    }
  } else {
    Change->Write(out);
  }

  out << ");";
  if(Mutable->GetReturnType().Is<Type::TSeq>()) {
    out << Eol << '}';
  }
}

TStmtBlock::TStmtBlock() {}

void TStmtBlock::Add(const TPtrC<TStmt> &stmt) {
  assert(this);
  assert(&stmt);
  Stmts.push_back(stmt);
}

void TStmtBlock::Add(const L0::TPackage *package, const TPtrC<TInline> &key, TMutator mutation, const TPtrC<TInline> &rhs) {
  assert(this);
  assert(&key);
  assert(&rhs);
  assert(key);
  assert(rhs);

  /* TODO: We should really find the source read inline here and make a map from those to the partial changes, then
     code gen large mutation blocks as giant initializer lists with conflict detection. However, that takes a lot more
     work, and for now we're going for the stupid simple route. */

  //We're going to refer to it at least twice, so it should be a local.
  Context::GetScope()->AddLocal(key);

  //Add the mutation, building out the change
  Stmts.push_back(TPtrC<TStmt>(new TMutate(package, key, TMutation::New(key, mutation, rhs))));
}

void TStmtBlock::AddDelete(const L0::TPackage *package, const TPtrC<TInline> &key, const Type::TType &val_type) {
  assert(&key);
  assert(key);

  Stmts.push_back(TPtrC<TStmt>(new TMutate(package, key, TDelete::New(val_type))));
}

void TStmtBlock::AddNew(const L0::TPackage *package, const TPtrC<TInline> &key, const TPtrC<TInline> &val) {
  assert(&key);
  assert(&val);
  assert(key);
  assert(val);

  Stmts.push_back(TPtrC<TStmt>(new TMutate(package, key, TNew::New(val))));
}

bool TStmtBlock::IsEmpty() const {
  assert(this);
  return Stmts.empty();
}

void TStmtBlock::Write(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  for(auto &it: Stmts) {
    it->Write(out);
    out << Eol;
  }
}

TPredicatedBlock::TPredicatedBlock(const TPtrC<TInline> &condition) : Condition(condition) {}

TStmtBlock &TPredicatedBlock::GetStmts() {
  assert(this);
  return Stmts;
}

void TPredicatedBlock::Write(TCppPrinter &out) const {
  assert(this);
  out << "if (" << Condition << ") {" << Eol;
  /* indent */ {
    TIndent indent(out);
    Stmts.Write(out);
  }
  out << '}';
}

TIf::TIf(const L0::TPackage *package, TPredicatedBlocks &&if_clauses)
    : TStmt(package),
      IfClauses(std::move(if_clauses)) {}

void TIf::SetOptElseClause() {
  assert(this);
  assert(!OptElseClause);
  OptElseClause = std::unique_ptr<TStmtBlock>(new TStmtBlock);
}

TStmtBlock &TIf::GetElseClause() {
  assert(this);
  assert(OptElseClause);
  return *OptElseClause;
}

void TIf::Write(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  Base::Join(" else ", IfClauses, [](TPredicatedBlocks::const_reference it, TCppPrinter &out) {
    it->Write(out);
  }, out);

  if(OptElseClause) {
    out << " else {" << Eol;
    /* indent */ {
      TIndent indent(out);
      OptElseClause->Write(out);
    }
    out << '}' << Eol;
  }

}