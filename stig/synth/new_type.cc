/* <stig/synth/new_type.cc>

   Implements <stig/synth/new_type.h>.

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

#include <stig/synth/new_type.h>

#include <cassert>
#include <stig/type.h>
#include <stig/synth/addr_type.h>
#include <stig/synth/atomic_type.h>
#include <stig/synth/binary_type.h>
#include <stig/synth/obj_type.h>
#include <stig/synth/ref_type.h>
#include <stig/synth/unary_type.h>

using namespace Stig;
using namespace Stig::Synth;

TType *Stig::Synth::NewType(const Package::Syntax::TType *root) {
  class TTypeVisitor
      : public Package::Syntax::TType::TVisitor {
    public:
    TTypeVisitor(TType *&out)
        : Out(out) {}
    virtual void operator()(const Package::Syntax::TRefType *that) const   { Out = new TRefType(that); }
    virtual void operator()(const Package::Syntax::TParenType *that) const { Out = NewType(that->GetType()); }
    virtual void operator()(const Package::Syntax::TAddrType *that) const  { Out = new TAddrType(that); }
    virtual void operator()(const Package::Syntax::TObjType *that) const   { Out = new TObjType(that); }
    virtual void operator()(const Package::Syntax::TSeqType *that) const   { OnUnary(that->GetType(), Type::TSeq::Get); }
    virtual void operator()(const Package::Syntax::TErrType *that) const   { OnUnary(that->GetType(), Type::TErr::Get); }
    virtual void operator()(const Package::Syntax::TOptType *that) const   { OnUnary(that->GetType(), Type::TOpt::Get); }
    virtual void operator()(const Package::Syntax::TListType *that) const  { OnUnary(that->GetType(), Type::TList::Get); }
    virtual void operator()(const Package::Syntax::TDictType *that) const  { OnBinary(that->GetKey(), that->GetValue(), Type::TDict::Get); }
    virtual void operator()(const Package::Syntax::TSetType *that) const   { OnUnary(that->GetType(), Type::TSet::Get); }
    virtual void operator()(const Package::Syntax::TFuncType *that) const  { OnBinary(that->GetParams(), that->GetResult(), Type::TFunc::Get);}
    virtual void operator()(const Package::Syntax::TMutableType *that) const  {
      // TODO: Make a clean end-user error message when there is a mutable in a mutable. Currently we assertion fail in the mutable type.
      class TOptMutableAtVisitor
          : public Package::Syntax::TOptMutableTypeAt::TVisitor {
        public:
        TOptMutableAtVisitor(const Package::Syntax::TType *&addr_type)
            : AddrType(addr_type) {}
        virtual void operator()(const Package::Syntax::TNoMutableTypeAt *) const { AddrType = nullptr; }
        virtual void operator()(const Package::Syntax::TMutableTypeAt *that) const { AddrType = that->GetType(); }
        private:
        const Package::Syntax::TType *&AddrType;
      };  // TOptMutableAtVisitor
      const Package::Syntax::TType *addr_type;
      that->GetOptMutableTypeAt()->Accept(TOptMutableAtVisitor(addr_type));
      if (addr_type) {
        OnBinary(addr_type, that->GetType(), Type::TMutable::Get);
      } else {
        OnUnary(that->GetType(), [](const Type::TType &that) {
          return Type::TMutable::Get(Type::TAddr::Get({}), that);
        });
      }
    }
    virtual void operator()(const Package::Syntax::TRealType *) const      { OnAtomic(Type::TReal::Get); }
    virtual void operator()(const Package::Syntax::TTimeDiffType *) const  { OnAtomic(Type::TTimeDiff::Get); }
    virtual void operator()(const Package::Syntax::TIntType *) const       { OnAtomic(Type::TInt::Get); }
    virtual void operator()(const Package::Syntax::TTimePntType *) const   { OnAtomic(Type::TTimePnt::Get); }
    virtual void operator()(const Package::Syntax::TStrType *) const       { OnAtomic(Type::TStr::Get); }
    virtual void operator()(const Package::Syntax::TIdType *) const        { OnAtomic(Type::TId::Get); }
    virtual void operator()(const Package::Syntax::TBoolType *) const      { OnAtomic(Type::TBool::Get); }
    private:
    void OnAtomic(TAtomicType::TGet get) const {
      Out = new TAtomicType(get);
    }
    void OnBinary(const Package::Syntax::TType *lhs, const Package::Syntax::TType *rhs, TBinaryType::TGet get) const {
      TType *lhs_type = nullptr;
      TType *rhs_type = nullptr;
      try {
        lhs_type = NewType(lhs);
        rhs_type = NewType(rhs);
        Out = new TBinaryType(lhs_type, rhs_type, get);
      } catch (...) {
        delete lhs_type;
        delete rhs_type;
        throw;
      }
    }
    void OnUnary(const Package::Syntax::TType *that, TUnaryType::TGet get) const {
      TType *type = nullptr;
      try {
        type = NewType(that);
        Out = new TUnaryType(type, get);
      } catch (...) {
        delete type;
        throw;
      }
    }
    TType *&Out;
  };  // TTypeVisitor
  assert(root);
  TType *type = nullptr;
  try {
    root->Accept(TTypeVisitor(type));
  } catch (...) {
    delete type;
    throw;
  }
  return type;
}
