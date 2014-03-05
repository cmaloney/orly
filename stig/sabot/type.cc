/* <stig/sabot/type.cc>

   Implements <stig/sabot/type.h>

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

#include <stig/sabot/type.h>

#include <cassert>

using namespace Stig::Sabot;

/*
#define NULLARY_TYPE(name)  \
  const Type::T##name Type::T##name::name;
NULLARY_TYPE(Int8)
NULLARY_TYPE(Int16)
NULLARY_TYPE(Int32)
NULLARY_TYPE(Int64)
NULLARY_TYPE(UInt8)
NULLARY_TYPE(UInt16)
NULLARY_TYPE(UInt32)
NULLARY_TYPE(UInt64)
NULLARY_TYPE(Bool)
NULLARY_TYPE(Char)
NULLARY_TYPE(Float)
NULLARY_TYPE(Double)
NULLARY_TYPE(Duration)
NULLARY_TYPE(TimePoint)
NULLARY_TYPE(Uuid)
NULLARY_TYPE(Blob)
NULLARY_TYPE(Str)
NULLARY_TYPE(Tombstone)
NULLARY_TYPE(Void)
#undef NULLARY_TYPE
*/

#define ACCEPT(name)  \
  void Type::T##name::Accept(const TTypeVisitor &visitor) const {  \
    assert(this);  \
    assert(&visitor);  \
    visitor(*this);  \
  }
ACCEPT(Int8)
ACCEPT(Int16)
ACCEPT(Int32)
ACCEPT(Int64)
ACCEPT(UInt8)
ACCEPT(UInt16)
ACCEPT(UInt32)
ACCEPT(UInt64)
ACCEPT(Bool)
ACCEPT(Char)
ACCEPT(Float)
ACCEPT(Double)
ACCEPT(Duration)
ACCEPT(TimePoint)
ACCEPT(Uuid)
ACCEPT(Blob)
ACCEPT(Str)
ACCEPT(Tombstone)
ACCEPT(Void)
ACCEPT(Desc)
ACCEPT(Free)
ACCEPT(Opt)
ACCEPT(Set)
ACCEPT(Vector)
ACCEPT(Map)
ACCEPT(Record)
ACCEPT(Tuple)
#undef ACCEPT

namespace Stig {

  namespace Sabot {

    /* TODO */
    template <typename TLhs>
    class TRhsVisitor
        : public TTypeVisitor {
      public:

      /* TODO */
      TRhsVisitor(const TLhs &lhs, const TTypeDoubleVisitor &double_visitor)
          : Lhs(lhs), DoubleVisitor(double_visitor) {}

      /* TODO */
      virtual void operator()(const Type::TInt8 &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TInt16 &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TInt32 &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TInt64 &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TUInt8 &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TUInt16 &rhs   ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TUInt32 &rhs   ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TUInt64 &rhs   ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TBool &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TChar &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TFloat &rhs    ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TDouble &rhs   ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TDuration &rhs ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TTimePoint &rhs) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TUuid &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TBlob &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TStr &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TTombstone &rhs) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TVoid &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TDesc &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TFree &rhs     ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TOpt &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TSet &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TVector &rhs   ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TMap &rhs      ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TRecord &rhs   ) const { DoubleVisitor(Lhs, rhs); }
      virtual void operator()(const Type::TTuple &rhs    ) const { DoubleVisitor(Lhs, rhs); }

      private:

      /* TODO */
      const TLhs &Lhs;

      /* TODO */
      const TTypeDoubleVisitor &DoubleVisitor;

    };  // TRhsVisitor<TLhs>

    /* TODO */
    class TLhsVisitor
        : public TTypeVisitor {
      public:

      /* TODO */
      TLhsVisitor(const Type::TAny &rhs, const TTypeDoubleVisitor &double_visitor)
          : Rhs(rhs), DoubleVisitor(double_visitor) {}

      virtual ~TLhsVisitor() {}

      /* TODO */
      virtual void operator()(const Type::TInt8 &lhs     ) const { Rhs.Accept(TRhsVisitor<Type::TInt8     >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TInt16 &lhs    ) const { Rhs.Accept(TRhsVisitor<Type::TInt16    >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TInt32 &lhs    ) const { Rhs.Accept(TRhsVisitor<Type::TInt32    >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TInt64 &lhs    ) const { Rhs.Accept(TRhsVisitor<Type::TInt64    >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TUInt8 &lhs    ) const { Rhs.Accept(TRhsVisitor<Type::TUInt8    >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TUInt16 &lhs   ) const { Rhs.Accept(TRhsVisitor<Type::TUInt16   >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TUInt32 &lhs   ) const { Rhs.Accept(TRhsVisitor<Type::TUInt32   >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TUInt64 &lhs   ) const { Rhs.Accept(TRhsVisitor<Type::TUInt64   >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TBool &lhs     ) const { Rhs.Accept(TRhsVisitor<Type::TBool     >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TChar &lhs     ) const { Rhs.Accept(TRhsVisitor<Type::TChar     >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TFloat &lhs    ) const { Rhs.Accept(TRhsVisitor<Type::TFloat    >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TDouble &lhs   ) const { Rhs.Accept(TRhsVisitor<Type::TDouble   >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TDuration &lhs ) const { Rhs.Accept(TRhsVisitor<Type::TDuration >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TTimePoint &lhs) const { Rhs.Accept(TRhsVisitor<Type::TTimePoint>(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TUuid &lhs     ) const { Rhs.Accept(TRhsVisitor<Type::TUuid     >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TBlob &lhs     ) const { Rhs.Accept(TRhsVisitor<Type::TBlob     >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TStr &lhs      ) const { Rhs.Accept(TRhsVisitor<Type::TStr      >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TTombstone &lhs) const { Rhs.Accept(TRhsVisitor<Type::TTombstone>(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TVoid &lhs     ) const { Rhs.Accept(TRhsVisitor<Type::TVoid     >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TDesc &lhs     ) const { Rhs.Accept(TRhsVisitor<Type::TDesc     >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TFree &lhs     ) const { Rhs.Accept(TRhsVisitor<Type::TFree     >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TOpt &lhs      ) const { Rhs.Accept(TRhsVisitor<Type::TOpt      >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TSet &lhs      ) const { Rhs.Accept(TRhsVisitor<Type::TSet      >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TVector &lhs   ) const { Rhs.Accept(TRhsVisitor<Type::TVector   >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TMap &lhs      ) const { Rhs.Accept(TRhsVisitor<Type::TMap      >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TRecord &lhs   ) const { Rhs.Accept(TRhsVisitor<Type::TRecord   >(lhs, DoubleVisitor)); }
      virtual void operator()(const Type::TTuple &lhs    ) const { Rhs.Accept(TRhsVisitor<Type::TTuple    >(lhs, DoubleVisitor)); }

      private:

      /* TODO */
      const Type::TAny &Rhs;

      /* TODO */
      const TTypeDoubleVisitor &DoubleVisitor;

    };  // TLhsVisitor

  }  // Sabot

}  // Stig

void Stig::Sabot::AcceptDouble(const Type::TAny &lhs, const Type::TAny &rhs, const TTypeDoubleVisitor &double_visitor) {
  lhs.Accept(TLhsVisitor(rhs, double_visitor));
}