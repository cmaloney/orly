/* <stig/sabot/order_states.h>

   TODO

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

#pragma once

#include <cassert>
#include <ostream>

#include <stig/atom/comparison.h>
#include <stig/sabot/order_types.h>
#include <stig/sabot/state.h>

namespace Stig {

  namespace Sabot {

    /* TODO */
    class TOrderStatesVisitor final
        : public TStateDoubleVisitor {
      public:

      /* TODO. */
      TOrderStatesVisitor(Atom::TComparison &out)
          : Comparison(out) {
        assert(&out);
      }

      /* Overrides. */
      virtual void operator()(const State::TFree &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TFree &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TTombstone &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TTombstone &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TVoid &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TVoid &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TInt8 &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TInt8 &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TInt16 &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TInt16 &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TInt32 &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TInt32 &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TInt64 &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TInt64 &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TUInt8 &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TUInt8 &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TUInt16 &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TUInt16 &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TUInt32 &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TUInt32 &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TUInt64 &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TUInt64 &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TBool &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TBool &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TChar &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TChar &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TFloat &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TFloat &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TDouble &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TDouble &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TDuration &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TDuration &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TTimePoint &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TTimePoint &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TUuid &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TUuid &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TBlob &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TBlob &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TStr &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TStr &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TDesc &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TDesc &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TOpt &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TOpt &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TSet &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TSet &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TVector &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TVector &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TMap &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TMap &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TRecord &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TRecord &lhs, const State::TTuple &rhs     ) const override;

      virtual void operator()(const State::TTuple &lhs, const State::TFree &rhs      ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TTombstone &rhs ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TVoid &rhs      ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TInt8 &rhs      ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TInt16 &rhs     ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TInt32 &rhs     ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TInt64 &rhs     ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TUInt8 &rhs     ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TUInt16 &rhs    ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TUInt32 &rhs    ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TUInt64 &rhs    ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TBool &rhs      ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TChar &rhs      ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TFloat &rhs     ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TDouble &rhs    ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TDuration &rhs  ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TTimePoint &rhs ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TUuid &rhs      ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TBlob &rhs      ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TStr &rhs       ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TDesc &rhs      ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TOpt &rhs       ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TSet &rhs       ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TVector &rhs    ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TMap &rhs       ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TRecord &rhs    ) const override;
      virtual void operator()(const State::TTuple &lhs, const State::TTuple &rhs     ) const override;

      private:

      /* TODO */
      template <typename TVal>
      inline void OnScalar(const State::TScalar<TVal> &lhs, const State::TScalar<TVal> &rhs) const;

      /* TODO */
      template <typename TVal>
      inline void OnArrayOfScalars(const State::TArrayOfScalars<TVal> &lhs, const State::TArrayOfScalars<TVal> &rhs) const;

      /* TODO */
      inline void OnArrayOfSingleStates(const State::TArrayOfSingleStates &lhs, const State::TArrayOfSingleStates &rhs) const;

      /* TODO */
      inline void OnArrayOfPairsOfStates(const State::TArrayOfPairsOfStates &lhs, const State::TArrayOfPairsOfStates &rhs) const;

      template <typename TVal>
      static inline Atom::TComparison Compare(const TVal &lhs, const TVal &rhs);

      /* TODO */
      Atom::TComparison &Comparison;

    };  // TOrderStatesVisitor

    /* TODO */
    template <typename TVal>
    inline Atom::TComparison TOrderStatesVisitor::Compare(const TVal &lhs, const TVal &rhs) {
      return lhs == rhs ? Atom::TComparison::Eq : (lhs < rhs ? Atom::TComparison::Lt : Atom::TComparison::Gt);
    }

    /* TODO */
    template <typename TVal>
    inline void TOrderStatesVisitor::OnScalar(const State::TScalar<TVal> &lhs, const State::TScalar<TVal> &rhs) const {
      Comparison = Compare(lhs.Get(), rhs.Get());
    }

    /* TODO */
    template <typename TVal>
    inline void TOrderStatesVisitor::OnArrayOfScalars(const State::TArrayOfScalars<TVal> &lhs, const State::TArrayOfScalars<TVal> &rhs) const {
      void *lhs_alloc = alloca(State::GetMaxStatePinSize() * 2);
      void *rhs_alloc = reinterpret_cast<uint8_t *>(lhs_alloc) + State::GetMaxStatePinSize();
      typename State::TArrayOfScalars<TVal>::TPin::TWrapper lhs_pin(lhs.Pin(lhs_alloc));
      typename State::TArrayOfScalars<TVal>::TPin::TWrapper rhs_pin(rhs.Pin(rhs_alloc));
      int comp = memcmp(lhs_pin->GetStart(), rhs_pin->GetStart(), std::min(lhs_pin->GetSize(), rhs_pin->GetSize()));
      if (comp == 0) {  // min size bytes are equal
        Comparison = Compare(lhs_pin->GetSize(), rhs_pin->GetSize());
      } else if (comp < 0) { // lhs is less
        Comparison = Atom::TComparison::Lt;
      } else { // lhs is more
        assert(comp > 0);
        Comparison = Atom::TComparison::Gt;
      }
    }

    /* TODO */
    inline void TOrderStatesVisitor::OnArrayOfSingleStates(const State::TArrayOfSingleStates &lhs, const State::TArrayOfSingleStates &rhs) const {
      void *lhs_alloc = alloca(State::GetMaxStatePinSize() * 2);
      void *rhs_alloc = reinterpret_cast<uint8_t *>(lhs_alloc) + State::GetMaxStatePinSize();
      State::TArrayOfSingleStates::TPin::TWrapper lhs_pin(lhs.Pin(lhs_alloc));
      State::TArrayOfSingleStates::TPin::TWrapper rhs_pin(rhs.Pin(rhs_alloc));
      void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
      void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
      for (size_t lhs_idx = 0, rhs_idx = 0; lhs_idx < lhs_pin->GetElemCount() && rhs_idx < rhs_pin->GetElemCount(); ++lhs_idx, ++rhs_idx) {
        Sabot::State::TAny::TWrapper
          lhs_state(lhs_pin->NewElem(lhs_idx, lhs_state_alloc)),
          rhs_state(rhs_pin->NewElem(rhs_idx, rhs_state_alloc));
        AcceptDouble(*lhs_state, *rhs_state, *this);
        if (IsNe(Comparison)) {
          return;
        }
      }
      if (IsEq(Comparison)) {
        Comparison = Compare(lhs_pin->GetElemCount(), rhs_pin->GetElemCount());
      }
    }

    /* TODO */
    inline void TOrderStatesVisitor::OnArrayOfPairsOfStates(const State::TArrayOfPairsOfStates &lhs, const State::TArrayOfPairsOfStates &rhs) const {
      void *lhs_alloc = alloca(State::GetMaxStatePinSize() * 2);
      void *rhs_alloc = reinterpret_cast<uint8_t *>(lhs_alloc) + State::GetMaxStatePinSize();
      State::TArrayOfPairsOfStates::TPin::TWrapper lhs_pin(lhs.Pin(lhs_alloc));
      State::TArrayOfPairsOfStates::TPin::TWrapper rhs_pin(rhs.Pin(rhs_alloc));
      void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
      void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
      for (size_t lhs_idx = 0, rhs_idx = 0; lhs_idx < lhs_pin->GetElemCount() && rhs_idx < rhs_pin->GetElemCount(); ++lhs_idx, ++rhs_idx) {
        Sabot::State::TAny::TWrapper
          lhs_key(lhs_pin->NewLhs(lhs_idx, lhs_state_alloc)),
          rhs_key(rhs_pin->NewLhs(rhs_idx, rhs_state_alloc));
        AcceptDouble(*lhs_key, *rhs_key, *this);
        if (IsNe(Comparison)) {
          return;
        }
        Sabot::State::TAny::TWrapper
          lhs_val(lhs_pin->NewRhs(lhs_idx, lhs_state_alloc)),
          rhs_val(rhs_pin->NewRhs(rhs_idx, rhs_state_alloc));
        AcceptDouble(*lhs_val, *rhs_val, *this);
        if (IsNe(Comparison)) {
          return;
        }
      }
      if (IsEq(Comparison)) {
        Comparison = Compare(lhs_pin->GetElemCount(), rhs_pin->GetElemCount());
      }
    }

    /* TODO */
    inline Atom::TComparison OrderStates(const State::TAny &lhs, const State::TAny &rhs) {
      void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
      void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
      Atom::TComparison comp = OrderTypes(*Sabot::Type::TAny::TWrapper(lhs.GetType(lhs_type_alloc)),
                                          *Sabot::Type::TAny::TWrapper(rhs.GetType(rhs_type_alloc)));
      if (IsEq(comp)) {
        AcceptDouble(lhs, rhs, TOrderStatesVisitor(comp));
      }
      return comp;
    }

  }  // Sabot

}  // Stig