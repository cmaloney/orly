/* <stig/type/get_prec.cc>

   Implements <stig/type/get_prec.h>.

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

#include <stig/type/get_prec.h>

#include <base/impossible_error.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Type;

TPrec Stig::Type::GetPrec(const Type::TType &type) {
  class TPrecVisitor : public TType::TVisitor {
    NO_COPY_SEMANTICS(TPrecVisitor);
    public:
    TPrecVisitor(TPrec &prec) : Prec(prec) {}
    virtual void operator()(const TAddr *) const {
      Prec = TPrec::Addr;
    }
    virtual void operator()(const TAny *) const {throw TImpossibleError(HERE);}
    virtual void operator()(const TBool *) const {
      Prec = TPrec::Bool;
    }
    virtual void operator()(const TDict *) const {
      Prec = TPrec::Dict;
    }
    virtual void operator()(const TErr *) const {throw TImpossibleError(HERE);}
    virtual void operator()(const TFunc *) const {throw TImpossibleError(HERE);}
    virtual void operator()(const TId *) const {
      Prec = TPrec::Id;
    }
    virtual void operator()(const TInt *) const {
      Prec = TPrec::Int;
    }
    virtual void operator()(const TList *) const {
      Prec = TPrec::List;
    }
    virtual void operator()(const TMutable *) const {throw TImpossibleError(HERE);}
    virtual void operator()(const TObj *) const {
      Prec = TPrec::Obj;
    }
    virtual void operator()(const TOpt *) const {throw TImpossibleError(HERE);}
    virtual void operator()(const TReal *) const {
      Prec = TPrec::Real;
    }
    virtual void operator()(const TSeq *) const {throw TImpossibleError(HERE);}
    virtual void operator()(const TSet *) const {
      Prec = TPrec::Set;
    }
    virtual void operator()(const TStr *) const {
      Prec = TPrec::Str;
    }
    virtual void operator()(const TTimeDiff *) const {
      Prec = TPrec::TimeDiff;
    }
    virtual void operator()(const TTimePnt *) const {
      Prec = TPrec::TimePnt;
    }
    private:
    TPrec &Prec;
  };  // TPrecVisitor
  TPrec prec;
  type.Accept(TPrecVisitor(prec));
  return prec;
}