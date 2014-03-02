/* <stig/rpc/var/dump.cc> 

   Implements <stig/rpc/var/dump.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rpc/var/dump.h>

#include <cassert>
#include <ctime>
#include <sstream>

#include <stig/rpc/all_vars.h>
#include <stig/rpc/type/dump.h>

using namespace std;
using namespace Stig::Rpc;

ostream &Stig::Rpc::operator<<(ostream &strm, const TVar &var) {
  struct visitor_t final : public TVar::TVisitor {
    ostream &Strm;
    visitor_t(ostream &strm) : Strm(strm) {}
    virtual void operator()(const Var::TBool *var) const override {
      Strm << boolalpha << var->GetVal();
    }
    virtual void operator()(const Var::TId *var) const override {
      Strm << '{' << var->GetVal() << '}';
    }
    virtual void operator()(const Var::TInt *var) const override {
      Strm << var->GetVal();
    }
    virtual void operator()(const Var::TReal *var) const override {
      Strm << var->GetVal();
    }
    virtual void operator()(const Var::TStr *var) const override {
      Strm << '"' << var->GetVal() << '"';
    }
    virtual void operator()(const Var::TTimeDiff *var) const override {
      Strm << "time_diff(" << var->GetVal().count() << ')';
    }
    virtual void operator()(const Var::TTimePnt *var) const override {
      auto time = TTimePnt::clock::to_time_t(
          TTimePnt::clock::time_point(chrono::duration_cast<TTimePnt::clock::time_point::duration>(var->GetVal().time_since_epoch()))
      );
      Strm << "time_pnt(\"" << ctime(&time) << "\")";
    }
    virtual void operator()(const Var::TList *var) const override {
      const auto &elems = var->GetElems();
      if (!elems.empty()) {
        Strm << "list(";
        bool sep = false;
        for (const auto &elem: var->GetElems()) {
          if (sep) {
            Strm << ", ";
          } else {
            sep = true;
          }
          elem->Accept(*this);
        }
        Strm << ')';
      } else {
        Strm << "empty_list(";
        Strm << *var->GetElemType();
        Strm << ')';
      }
    }
    virtual void operator()(const Var::TOpt *var) const override {
      const TVar *elem = var->TryGetElem();
      if (elem) {
        Strm << "opt(";
        elem->Accept(*this);
        Strm << ')';
      } else {
        Strm << "empty_opt(";
        Strm << *var->GetElemType();
        Strm << ')';
      }
    }
    virtual void operator()(const Var::TSet *var) const override {
      const auto &elems = var->GetElems();
      if (!elems.empty()) {
        Strm << "set(";
        bool sep = false;
        for (const auto &elem: var->GetElems()) {
          if (sep) {
            Strm << ", ";
          } else {
            sep = true;
          }
          elem->Accept(*this);
        }
        Strm << ')';
      } else {
        Strm << "empty_set(";
        Strm << *var->GetElemType();
        Strm << ')';
      }
    }
    virtual void operator()(const Var::TDict *var) const override {
      const auto &elems = var->GetElems();
      if (!elems.empty()) {
        Strm << "dict(";
        bool sep = false;
        for (const auto &elem: var->GetElems()) {
          if (sep) {
            Strm << ", ";
          } else {
            sep = true;
          }
          elem.first->Accept(*this);
          Strm << ": ";
          elem.second->Accept(*this);
        }
        Strm << ')';
      } else {
        Strm << "empty_dict(";
        Strm << *var->GetKeyType();
        Strm << ", ";
        Strm << *var->GetValType();
        Strm << ')';
      }
    }
    virtual void operator()(const Var::TAddr *var) const override {
      Strm << "addr(";
      bool sep = false;
      for (const auto &field: var->GetFields()) {
        if (sep) {
          Strm << ", ";
        } else {
          sep = true;
        }
        Strm << GetDirName(field.first) << ' ';
        field.second->Accept(*this);
      }
      Strm << ')';
    }
    virtual void operator()(const Var::TObj *var) const override {
      Strm << "obj(";
      bool sep = false;
      for (const auto &field: var->GetFields()) {
        if (sep) {
          Strm << ", ";
        } else {
          sep = true;
        }
        Strm << field.first << ": ";
        field.second->Accept(*this);
      }
      Strm << ')';
    }
  };
  assert(&strm);
  assert(&var);
  var.Accept(visitor_t(strm));
  return strm;
}

string ToString(const TVar &var) {
  ostringstream strm;
  strm << var;
  return strm.str();
}

