/* <stig/synth/scope_and_def.cc>

   Implements <stig/synth/scope_and_def.h>.

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

#include <stig/synth/scope_and_def.h>

#include <base/assert_true.h>
#include <base/no_default_case.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TScope::~TScope() {
  assert(this);
  Defs.DeleteEachMember();
}

void TScope::Bind() const {
  assert(this);
  ForEachControlledRef([this](TAnyRef &ref){ ref.Bind(this); });
  Defs.ForEachUniqueMember([](TDef *def) -> bool {
    def->BindEachRef();
    return true;
  });
}

TAction TScope::BuildEachDef(int pass) const {
  assert(this);
  return TDef::BuildEachDef(pass, [this](const std::function<bool (TDef *)> &cb) { Defs.ForEachUniqueMember(cb); });
}

TScope::TScope(TScope *parent)
    : Parent(parent), Defs(this) {}

void TScope::ForEachControlledRef(const std::function<void (TAnyRef &)> &) const {}

TDef *TScope::TryGetAnyDef(const TName &name) const {
  assert(this);
  assert(&name);
  TDef *def;
  if (name) {
    const TScope *scope = this;
    for (;;) {
      def = scope->Defs.TryGetFirstMember(name);
      if (def) {
        if (def->GetScopeMembership()->TryGetNextMemberWithSameKey()) {
          Tools::Nycr::TError::TBuilder(name.GetPosRange())
            << '"' << name.GetText() << "\" is declared multiple times";
          def = nullptr;
        }
        break;
      }
      scope = scope->Parent;
      if (!scope) {
        Tools::Nycr::TError::TBuilder(name.GetPosRange())
          << '"' << name.GetText() << "\" is not declared";
        break;
      }
    }
  } else {
    def = nullptr;
  }
  return def;
}

void TScope::AddError(const TName &name, const char *expected_type) {
  assert(&name);
  assert(expected_type);
  Tools::Nycr::TError::TBuilder(name.GetPosRange())
    << '"' << name.GetText() << "\" does not refer to " << expected_type;
}

TDef::~TDef() {}

void TDef::BindEachRef() {
  assert(this);
  TScope *scope = GetOuterScope();
  ForEachRef([scope](TAnyRef &ref){ ref.Bind(scope); });
  ForEachInnerScope([](TScope *scope){ scope->Bind(); });
}

TAction TDef::BuildEachDef(int pass, const std::function<void (const std::function<bool (TDef *)> &)> &generate) {
  assert(pass > 0);
  assert(&generate);
  TAction action = Finish;
  generate([pass, &action](TDef *def){
    bool keep_going;
    switch (def ? def->BuildHelper(pass) : Finish) {
      case Continue: {
        action = Continue;
        keep_going = true;
        break;
      }
      case Finish: {
        keep_going = true;
        break;
      }
      case Fail: {
        action = Fail;
        keep_going = false;
        break;
      }
      NO_DEFAULT_CASE;
    }
    return keep_going;
  });
  return action;
}

TDef::TDef(TScope *outer_scope, const TName &name)
    : ScopeMembership(this, name, Base::AssertTrue(outer_scope)->GetDefs()),
      Readiness(0), State(Ready) {
  auto prev = ScopeMembership.TryGetPrevMemberWithSameKey();
  if (prev) {
    Tools::Nycr::TError::TBuilder(name.GetPosRange())
        << "duplicate name \"" << name.GetText()
        << "\" previously declared at " << prev->GetName().GetPosRange();
  }
}

void TDef::ForEachRef(const std::function<void (TAnyRef &)> &) {}

void TDef::ForEachPred(int, const std::function<bool (TDef *)> &) {}

void TDef::ForEachInnerScope(const std::function<void (TScope *)> &) {}

TAction TDef::BuildHelper(int pass) {
  assert(this);
  TAction action;
  if (Readiness < pass) {
    switch (State) {
      case Ready: {
        // Change state so we can detect cycles.
        State = InProgress;
        // Build our predecessors.
        action = BuildEachDef(pass, [this, pass](const std::function<bool (TDef *)> &cb){ ForEachPred(pass, cb); });
        if (action != Fail) {
          // Preds all went ok, so build ourself now.
          action = Build(pass);
        }
        // Change state to reflect the result of this step.
        switch (action) {
          case Continue: {
            State = Ready;
            break;
          }
          case Finish: {
            State = Shiny;
            break;
          }
          case Fail: {
            State = Cruddy;
            break;
          }
          NO_DEFAULT_CASE;
        }
        // We are now up-to-date with this pass.
        Readiness = pass;
        // If we built ok, step into the inner scopes, if we have any.
        if (action != Fail) {
          ForEachInnerScope([pass](TScope *scope){
              if (!scope->HasSymbol()) {
                assert(pass == 1); // We should be building scopes in pass 1
                scope->BuildSymbol();
              }
              scope->BuildEachDef(pass);
          });
        }
        break;
      }
      case InProgress: {
        // We have encountered ourself during a predecessor walk.  This means we have a cycle.
        const TName &name = GetName();
        Tools::Nycr::TError::TBuilder(name.GetPosRange())
            << '"' << name.GetText() << "\" has a cycle in its build dependencies";
        action = Fail;
        /* TODO: Provide a mechanism to customize this error message so that we can describe the pass in which the cycle was found. */
        break;
      }
      case Shiny: {
        Readiness = pass;
        action = Finish;
        break;
      }
      case Cruddy: {
        action = Fail;
        break;
      }
      NO_DEFAULT_CASE;
    }
  } else {
    action = Finish;
  }
  return action;
}
