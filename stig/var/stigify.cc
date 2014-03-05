/* <stig/var/stigify.cc>

   Implements <stig/var/stigify.h>.

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

#include <stig/var/stigify.h>

#include <base/chrono.h>
#include <base/split.h>
#include <stig/type/stigify.h>
#include <stig/var.h>
#include <tools/nycr/escape.h>

using namespace std;
using namespace Stig;
using namespace Stig::Var;

void Stig::Var::Stigify(ostream &strm, const TVar &var) {
  class TVisitor : public TVar::TVisitor {
    NO_COPY_SEMANTICS(TVisitor);
    public:
    TVisitor(ostream &strm) : Strm(strm) {}
    private:
    virtual void operator()(const TAddr *that) const {
      Strm << "<[";

      Base::Join(',', that->GetVal(), [this](const pair<TAddrDir, TVar> &elem, ostream &strm) {
        strm << elem.first << ' ';
        elem.second.Accept(*this);
      }, Strm);

      Strm << "]>";
    }
    virtual void operator()(const TBool *that) const {
      Strm << boolalpha <<that->GetVal();
    }
    virtual void operator()(const TDict *that) const {
      if(that->GetVal().empty()) {
        Strm << "empty ";
        Stig::Type::Stigify(Strm, that->GetType());
      } else {
        Strm << "{";
        size_t num_args = 0;
        for (auto iter : that->GetVal()) {
          if (num_args > 0) {
            Strm << ",";
          }
          iter.first.Accept(*this);
          Strm << ":";
          iter.second.Accept(*this);
          ++num_args;
        }
        Strm << "}";
      }
    }
    virtual void operator()(const TErr *) const {throw Base::TNotImplementedError(HERE);}
    virtual void operator()(const TFree *) const {throw Base::TImpossibleError(HERE);}
    virtual void operator()(const TId *that) const {
      Strm << '{' << that->GetVal() << '}';
    }
    virtual void operator()(const TInt *that) const {
      Strm << that->GetVal();
    }
    virtual void operator()(const TList *that) const {
      if(that->GetVal().empty()) {
        Strm << "empty ";
        Stig::Type::Stigify(Strm, that->GetType());
      } else {
        Strm << "[";
        size_t num_args = 0;
        for (auto iter : that->GetVal()) {
          if (num_args > 0) {
            Strm << ",";
          }
          iter.Accept(*this);
          ++num_args;
        }
        Strm << "]";
      }
    }
    virtual void operator()(const TMutable *that) const {
      that->GetVal().Accept(*this);
    }
    virtual void operator()(const TObj *that) const {
      Strm << "<{";
      size_t num_args = 0;
      for (auto iter : that->GetVal()) {
        if (num_args > 0) {
          Strm << ",";
        }
        Strm << "." << iter.first << ":";
        iter.second.Accept(*this);
        ++num_args;
      }
      Strm << "}>";
    }
    virtual void operator()(const TOpt *that) const {
      if (that->GetVal().IsKnown()) {
        that->GetVal().GetVal().Accept(*this);
        Strm << '?';
      } else {
        Strm << "unknown ";
        Type::Stigify(Strm, that->GetInnerType());
      }
    }
    virtual void operator()(const TReal *that) const {
      Strm << showpoint << that->GetVal();
    }
    virtual void operator()(const TSet *that) const {
      if (that->GetVal().empty()) {
        Strm << "empty ";
        Stig::Type::Stigify(Strm, that->GetType());
      } else {
        Strm << "{";
        size_t num_args = 0;
        for (auto iter : that->GetVal()) {
          if (num_args > 0) {
            Strm << ",";
          }
          iter.Accept(*this);
          ++num_args;
        }
        Strm << "}";
      }
    }
    virtual void operator()(const TStr *that) const {
      Strm << Tools::Nycr::TEscape(that->GetVal());
    }
    virtual void operator()(const TTimeDiff *that) const {
      Strm << '{' << Base::Chrono::TTimeDiffInfo(that->GetVal()).AsString() << '}';
    }
    virtual void operator()(const TTimePnt *that) const {
      Strm << '{' << Base::Chrono::TTimePntInfo(that->GetVal()).AsString() << '}';
    }
    ostream &Strm;
  };
  var.Accept(TVisitor(strm));
}