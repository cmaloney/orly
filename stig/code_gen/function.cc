/* <stig/code_gen/function.cc>

   Implements <stig/code_gen/function.h>

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

#include <stig/code_gen/function.h>

#include <algorithm>

#include <base/split.h>
#include <stig/code_gen/builder.h>
#include <stig/code_gen/context.h>
#include <stig/code_gen/inline_func.h>
#include <stig/code_gen/inner_func.h>
#include <stig/expr/util.h>
#include <stig/expr/walker.h>
#include <stig/expr/where.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::CodeGen;

void TFunction::TArg::TRef::WriteExpr(TCppPrinter &out) const {
  out << Arg->GetId();
}

TFunction::TArg::TRef::TRef(const L0::TPackage *package, const TArg *arg) : TInline(package, arg->GetType()), Arg(arg) {}

TFunction::TArg::TPtr TFunction::TArg::New(const L0::TPackage *package, TId<TIdKind::Arg> &&id, const Type::TType &type) {
  return TPtr(new TArg(package, std::move(id), type));
}


const TId<TIdKind::Arg> &TFunction::TArg::GetId() const {
  assert(this);
  return Id;
}

const TFunction::TArg::TRef::TPtr &TFunction::TArg::GetRef() const {
  assert(this);

  return Ref;
}

Type::TType TFunction::TArg::GetType() const {
  assert(this);
  return Type;
}

TFunction::TArg::TArg(const L0::TPackage *package, TId<TIdKind::Arg> &&id, const Type::TType &type) : Id(id), Type(type), Ref(new TRef(package, this)) {}

void TFunction::AddChild(const std::shared_ptr<TInlineFunc> &func) {
  ChildFuncs.insert(func);
}

void TFunction::Build() {
  //Only build if we haven't already built.
  if(Body) {
    return;
  }

  assert(this);
  TFunctionCtx ctx(this);
  //Build up all of our sub functions
  for(auto &it: ChildFuncs) {
    it->Build();
  }

  //TODO: Common sub expression elimination for inline scopes, cross-function subexpression elimination.
  if(Implicit) {
    Body = CodeGen::Build(Package, Expr, KeepMutable);
  } else {
    Body = BuildInline(Package, Expr, KeepMutable);
  }

  assert(Body);
}


TFunction::TArg::TRef::TPtr TFunction::GetArg(const std::string &name) const {
  assert(this);
  auto arg = Args.find(name);
  assert(arg != Args.end());
  return arg->second->GetRef();
}

const TFunction::TArgs &TFunction::GetArgs() const {
  assert(this);

  return Args;
}


TCodeScope *TFunction::GetCodeScope() {
  assert(this);

  return &CodeScope;
}

bool TFunction::HasArgs() const {
  assert(this);

  return Args.size();
}

void TFunction::WriteArgs(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  Join(", ", Args, [](const TArgs::value_type &arg, TCppPrinter &out) {
    //TODO: The double GetId is fugly.
    out << "const " << arg.second->GetType() << " &" << arg.second->GetId() << "/* " << arg.first << " */";
  }, out);
}

void TFunction::WriteBody(TCppPrinter &out) const {
  assert(this);
  assert(&out);

  assert(Body); // NOTE: If this assertion fails, it means that Build() function probably wasn't called.

  out << " {" << Eol;

  /* Indented output */ {
    TIndent indent(out);

    vector<shared_ptr<TInlineFunc>> ordered_funcs;
    unordered_set<shared_ptr<const TInline>> finished;
    unordered_set<shared_ptr<TInlineFunc>> working_set(ChildFuncs.begin(), ChildFuncs.end());
    unordered_set<shared_ptr<const TInline>> cur_set;
    for (const auto &func : working_set) {
      cur_set.insert(func->Body);
    }
    while (working_set.size() > 0) {
      for (auto &func : working_set) {
        bool can_write = true;
        unordered_set<shared_ptr<const TInline>> depends_on;
        func->Body->AppendDependsOn(depends_on);
        for (const auto &dependency : depends_on) {
          if (dependency && finished.find(dependency) == finished.end() && cur_set.find(dependency) != cur_set.end()) {
            can_write = false;
          }
        }
        if (can_write) {
          ordered_funcs.push_back(func);
          finished.insert(func->Body);
          working_set.erase(func);
          break;
        }
      }
    }

    //Write out inner functions (forward decl followed by definitions).
    for(auto &func: ordered_funcs) {
      func->WriteDecl(out);
      out << ';' << Eol;
    }

    if(!ordered_funcs.empty()) {
      out << Eol;
    }

    for(auto &func: ordered_funcs) {
      func->WriteDef(out);
      out << ';' << Eol;
    }

    if(!ordered_funcs.empty()) {
      out << Eol;
    }

    CodeScope.WriteStart(out);
    out << "return ";
    Body->WriteExpr(out);
    out << ';' << Eol;
  }

  out << "}";
}

TFunction::TFunction(const L0::TPackage *package, const TIdScope::TPtr &id_scope)
      : CodeScope(id_scope), Package(package), KeepMutable(false) {
}

void TFunction::PostCtor(const TNamedArgs &args, const Expr::TExpr::TPtr &expr, bool keep_mutable, bool implicit) {
  assert(this);
  assert(&args);
  assert(Args.empty());
  assert(!Expr);
  assert(&expr);
  assert(expr);
  Expr = expr;
  KeepMutable = keep_mutable;
  Implicit = implicit;

  for(auto &arg: args) {
    //NOTE: It is critical we don't copy, move, etc. the arg or the Ref in it will point to the wrong thing.
    Args.insert(make_pair(arg.first, TArg::New(Package, CodeScope.NewArg(), arg.second)));
  }
  //NOTE: This is copied in test.cc TWith ctor as the local function gather_child_funcs
  //Gather child functions
  Expr::ForEachExpr(Expr, [this](const Expr::TExpr::TPtr &expr) {
    const Expr::TWhere *where = expr->TryAs<Expr::TWhere>();
    if(where) {
      for(auto &func: where->GetFunctions()) {
        ChildFuncs.insert(TInnerFunc::New(Package, func, CodeScope.GetIdScope()));
      }
    }
    return false;
  });
}