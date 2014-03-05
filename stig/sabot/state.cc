/* <stig/sabot/state.cc>

   Implements <stig/sabot/state.h>.

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

#include <stig/sabot/state.h>

#include <cassert>

using namespace std;
using namespace Stig::Sabot;

size_t State::GetStaticElemCount(const Sabot::Type::TAny &type) {
  class visitor_t final : public TTypeVisitor {
    public:
    visitor_t(size_t &result) : Result(result) {}
    virtual void operator()(const Type::TInt8 &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TInt16 &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TInt32 &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TInt64 &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TUInt8 &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TUInt16 &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TUInt32 &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TUInt64 &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TBool &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TChar &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TFloat &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TDouble &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TDuration &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TTimePoint &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TUuid &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TBlob &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TStr &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TTombstone &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TVoid &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TDesc &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TFree &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TOpt &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TSet &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TVector &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TMap &) const override { throw TUnreachable(); }
    virtual void operator()(const Type::TRecord &type) const override { Result = type.GetElemCount(); }
    virtual void operator()(const Type::TTuple &type) const override { Result = type.GetElemCount(); }
    private:
    size_t &Result;
  };
  size_t result;
  type.Accept(visitor_t(result));
  return result;
}

#define ACCEPT_AND_GET_TYPE(name)  \
  void State::T##name::Accept(const TStateVisitor &visitor) const {  \
    assert(this);  \
    assert(&visitor);  \
    visitor(*this);  \
  }  \
  Type::TAny *State::T##name::GetType(void *type_alloc) const {  \
    assert(this);  \
    return Get##name##Type(type_alloc);  \
  }
ACCEPT_AND_GET_TYPE(Free)
ACCEPT_AND_GET_TYPE(Tombstone)
ACCEPT_AND_GET_TYPE(Void)
ACCEPT_AND_GET_TYPE(Int8)
ACCEPT_AND_GET_TYPE(Int16)
ACCEPT_AND_GET_TYPE(Int32)
ACCEPT_AND_GET_TYPE(Int64)
ACCEPT_AND_GET_TYPE(UInt8)
ACCEPT_AND_GET_TYPE(UInt16)
ACCEPT_AND_GET_TYPE(UInt32)
ACCEPT_AND_GET_TYPE(UInt64)
ACCEPT_AND_GET_TYPE(Bool)
ACCEPT_AND_GET_TYPE(Char)
ACCEPT_AND_GET_TYPE(Float)
ACCEPT_AND_GET_TYPE(Double)
ACCEPT_AND_GET_TYPE(Duration)
ACCEPT_AND_GET_TYPE(TimePoint)
ACCEPT_AND_GET_TYPE(Uuid)
ACCEPT_AND_GET_TYPE(Blob)
ACCEPT_AND_GET_TYPE(Str)
ACCEPT_AND_GET_TYPE(Desc)
ACCEPT_AND_GET_TYPE(Opt)
ACCEPT_AND_GET_TYPE(Set)
ACCEPT_AND_GET_TYPE(Vector)
ACCEPT_AND_GET_TYPE(Map)
ACCEPT_AND_GET_TYPE(Record)
ACCEPT_AND_GET_TYPE(Tuple)
#undef ACCEPT_AND_GET_TYPE

namespace Stig {

  namespace Sabot {

    /* TODO */
    template <typename TLhs>
    class TRhsVisitor
        : public TStateVisitor {
      public:

      /* TODO */
      TRhsVisitor(const TLhs &lhs, const TStateDoubleVisitor &double_visitor)
          : Lhs(lhs), DoubleVisitor(double_visitor) {}

      /* TODO */
      virtual void operator()(const State::TFree &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TTombstone &rhs ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TVoid &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TInt8 &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TInt16 &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TInt32 &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TInt64 &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TUInt8 &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TUInt16 &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TUInt32 &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TUInt64 &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TBool &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TChar &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TFloat &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TDouble &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TDuration &rhs  ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TTimePoint &rhs ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TUuid &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TBlob &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TStr &rhs       ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TDesc &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TOpt &rhs       ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TSet &rhs       ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TVector &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TMap &rhs       ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TRecord &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const State::TTuple &rhs     ) const { DoubleVisitor(Lhs, rhs); }

      private:

      /* TODO */
      const TLhs &Lhs;

      /* TODO */
      const TStateDoubleVisitor &DoubleVisitor;

    };  // TRhsVisitor<TLhs>

    /* TODO */
    class TLhsVisitor
        : public TStateVisitor {
      public:

      /* TODO */
      TLhsVisitor(const State::TAny &rhs, const TStateDoubleVisitor &double_visitor)
          : Rhs(rhs), DoubleVisitor(double_visitor) {}

      virtual ~TLhsVisitor() {}

      /* TODO */
      virtual void operator()(const State::TFree &lhs      ) const { Rhs.Accept(TRhsVisitor<State::TFree     >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TTombstone &lhs ) const { Rhs.Accept(TRhsVisitor<State::TTombstone>(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TVoid &lhs      ) const { Rhs.Accept(TRhsVisitor<State::TVoid     >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TInt8 &lhs      ) const { Rhs.Accept(TRhsVisitor<State::TInt8     >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TInt16 &lhs     ) const { Rhs.Accept(TRhsVisitor<State::TInt16    >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TInt32 &lhs     ) const { Rhs.Accept(TRhsVisitor<State::TInt32    >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TInt64 &lhs     ) const { Rhs.Accept(TRhsVisitor<State::TInt64    >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TUInt8 &lhs     ) const { Rhs.Accept(TRhsVisitor<State::TUInt8    >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TUInt16 &lhs    ) const { Rhs.Accept(TRhsVisitor<State::TUInt16   >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TUInt32 &lhs    ) const { Rhs.Accept(TRhsVisitor<State::TUInt32   >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TUInt64 &lhs    ) const { Rhs.Accept(TRhsVisitor<State::TUInt64   >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TBool &lhs      ) const { Rhs.Accept(TRhsVisitor<State::TBool     >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TChar &lhs      ) const { Rhs.Accept(TRhsVisitor<State::TChar     >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TFloat &lhs     ) const { Rhs.Accept(TRhsVisitor<State::TFloat    >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TDouble &lhs    ) const { Rhs.Accept(TRhsVisitor<State::TDouble   >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TDuration &lhs  ) const { Rhs.Accept(TRhsVisitor<State::TDuration >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TTimePoint &lhs ) const { Rhs.Accept(TRhsVisitor<State::TTimePoint>(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TUuid &lhs      ) const { Rhs.Accept(TRhsVisitor<State::TUuid     >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TBlob &lhs      ) const { Rhs.Accept(TRhsVisitor<State::TBlob     >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TStr &lhs       ) const { Rhs.Accept(TRhsVisitor<State::TStr      >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TDesc &lhs      ) const { Rhs.Accept(TRhsVisitor<State::TDesc     >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TOpt &lhs       ) const { Rhs.Accept(TRhsVisitor<State::TOpt      >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TSet &lhs       ) const { Rhs.Accept(TRhsVisitor<State::TSet      >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TVector &lhs    ) const { Rhs.Accept(TRhsVisitor<State::TVector   >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TMap &lhs       ) const { Rhs.Accept(TRhsVisitor<State::TMap      >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TRecord &lhs    ) const { Rhs.Accept(TRhsVisitor<State::TRecord   >(lhs, DoubleVisitor)); }
      virtual void operator()(const State::TTuple &lhs     ) const { Rhs.Accept(TRhsVisitor<State::TTuple    >(lhs, DoubleVisitor)); }

      private:

      /* TODO */
      const State::TAny &Rhs;

      /* TODO */
      const TStateDoubleVisitor &DoubleVisitor;

    };  // TLhsVisitor

  }  // Sabot

}  // Stig

void Stig::Sabot::AcceptDouble(const State::TAny &lhs, const State::TAny &rhs, const TStateDoubleVisitor &double_visitor) {
  lhs.Accept(TLhsVisitor(rhs, double_visitor));
}