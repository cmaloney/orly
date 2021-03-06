/* <orly/type/gen_code.cc>

   Implements <orly/type/gen_code.h>.

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

#include <orly/type/gen_code.h>

#include <base/not_implemented.h>
#include <base/split.h>

using namespace std;
using namespace Base;
using namespace Orly;
using namespace Orly::Type;

template <size_t N>
using TTypeArray = array<TType, N>;

class TCodeGenVisitor : public TType::TVisitor {
  public:

  TCodeGenVisitor(ostream &strm) : Strm(strm) {}
  virtual ~TCodeGenVisitor() {}

  private:
  virtual void operator()(const TAddr *that) const {
    Strm
      << "Orly::Type::TAddr::Get(Orly::Type::TAddr::TElems{"
      << Base::Join(that->GetElems(),
                    ", ",
                    [this](ostream &strm, const pair<TAddrDir, TType> &elem) {
                      strm << '{';
                      WriteCppType(strm, elem.first) << ", ";
                      elem.second.Accept(*this);
                      strm << '}';
                    })
      << "})";
  }

  virtual void operator()(const TAny *) const {NOT_IMPLEMENTED();}
  virtual void operator()(const TBool *) const {
    Write("TBool");
  }
  virtual void operator()(const TDict *that) const {
    Write("TDict", that->GetKey(), that->GetVal());
  }
  virtual void operator()(const TErr *that) const {
    Write("TErr", that->GetElem());
  }
  virtual void operator()(const TFunc *that) const {
    Write("TFunc", that->GetParamObject(), that->GetReturnType());
  }
  virtual void operator()(const TId *) const {
    Write("TId");
  }
  virtual void operator()(const TInt *) const {
    Write("TInt");
  }
  virtual void operator()(const TList *that) const {
    Write("TList", {that->GetElem()});
  }
  virtual void operator()(const TMutable *that) const {
    Write("TMutable", that->GetAddr(), that->GetVal());
  }
  virtual void operator()(const TObj *that) const {
  const TObj::TElems &elem_map = that->GetElems();
    Strm
      << "Orly::Type::TObj::Get(std::map<std::string, Orly::Type::TType>{"
      << Base::Join(elem_map,
                    ", ",
                    [this](ostream &strm, const TObj::TElems::value_type &that) {
                      strm << "{std::string(\"" << that.first << "\"), ";
                      that.second.Accept(*this);
                      strm << '}';
                    })
      << "})";
  }
  virtual void operator()(const TOpt *that) const {
    Write("TOpt", that->GetElem());
  }
  virtual void operator()(const TReal *) const {
    Write("TReal");
  }
  virtual void operator()(const TSeq *that) const {
    Write("TSeq", that->GetElem());
  }
  virtual void operator()(const TSet *that) const {
    Write("TSet", that->GetElem());
  }
  virtual void operator()(const TStr *) const {
    Write("TStr");
  }
  virtual void operator()(const TTimeDiff *) const {
    Write("TTimeDiff");
  }
  virtual void operator()(const TTimePnt *) const {
    Write("TTimePnt");
  }

  void Write(const char *name) const {
    Write<0>(name, TTypeArray<0>());
  }
  void Write(const char *name, const TType &type) const {
    Write<1>(name, {{type}});
  }

  void Write(const char *name, const TType &t1, const TType &t2) const {
    Write<2>(name, {{t1, t2}});
  }

  template <size_t N>
  void Write(const char *name, const TTypeArray<N> &elems) const {
    Strm
      << "Orly::Type::" << name << "::Get("
      << Base::Join(elems,
                    ", ",
                    [this](ostream &, const TType &type) {
                      type.Accept(*this);
                    })
      << ')';
  }

  ostream &Strm;
};

void Orly::Type::GenCode(ostream &strm, const TType &type) {
  type.Accept(TCodeGenVisitor(strm));
}
