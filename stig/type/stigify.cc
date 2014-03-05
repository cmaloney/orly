/* <stig/type/stigify.cc>

   Implements <stig/type/stigify.h>.

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

#include <stig/type/stigify.h>

#include <base/split.h>
#include <stig/type.h>

using namespace std;
using namespace Stig;
using namespace Stig::Type;

void Stig::Type::Stigify(ostream &strm, const TType &type) {
  class TVisitor : public TType::TVisitor {
    NO_COPY_SEMANTICS(TVisitor);
    public:
    TVisitor(ostream &strm) : Strm(strm) {}
    virtual void operator()(const TAddr *that) const {
      Strm << "<[";
      Base::Join(',', that->GetElems(), [this] (const std::pair<TAddr::TDir, TType> &elem, std::ostream &strm) {
        strm << elem.first << ' ';
        elem.second.Accept(*this);
      }, Strm);
      Strm << "]>";
    }
    virtual void operator()(const TAny *) const {throw Base::TImpossibleError(HERE);}
    virtual void operator()(const TBool *) const {
      Strm << "bool";
    }
    virtual void operator()(const TDict *that) const {
      Strm << "{";
      that->GetKey().Accept(*this);
      Strm << ":";
      that->GetVal().Accept(*this);
      Strm << "}";
    }
    virtual void operator()(const TErr *that) const {
      that->GetElem().Accept(*this);
      Strm << '!';
    }
    virtual void operator()(const TFunc *that) const {
      that->GetParamObject().Accept(*this);
      Strm << " -> ";
      that->GetReturnType().Accept(*this);
    }
    virtual void operator()(const TId *) const {
      Strm << "id";
    }
    virtual void operator()(const TInt *) const {
      Strm << "int";
    }
    virtual void operator()(const TList *that) const {
      Strm << "[";
      that->GetElem().Accept(*this);
      Strm << "]";
    }
    virtual void operator()(const TMutable *that) const {
      //TODO: We could print out the actual address
      Strm << "mutable @";
      that->GetAddr().Accept(*this);
      Strm << ' ';
      that->GetVal().Accept(*this);
    }
    virtual void operator()(const TObj *that) const {
      Strm << "<{";
      Base::Join(", ", that->GetElems(), [this](const TObj::TElems::value_type &elem, std::ostream &out) {
        out << '.' << elem.first << ':';
        elem.second.Accept(*this);
      }, Strm) << "}>";
    }
    virtual void operator()(const TOpt *that) const {
      that->GetElem().Accept(*this);
      Strm << "?";
    }
    virtual void operator()(const TReal *) const {
      Strm << "real";
    }
    virtual void operator()(const TSeq *that) const {
      that->GetElem().Accept(*this);
      Strm << '*';
    }
    virtual void operator()(const TSet *that) const {
      Strm << "{";
      that->GetElem().Accept(*this);
      Strm << "}";
    }
    virtual void operator()(const TStr *) const {
      Strm << "str";
    }
    virtual void operator()(const TTimeDiff *) const {
      Strm << "time_diff";
    }
    virtual void operator()(const TTimePnt *) const {
      Strm << "time_pnt";
    }
    private:
    std::ostream &Strm;
  }; // TVisitor
  type.Accept(TVisitor(strm));
}