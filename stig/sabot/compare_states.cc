/* <stig/sabot/compare_states.cc>

   Implements <stig/sabot/compare_states.h>.

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

#include <stig/sabot/compare_states.h>

using namespace std;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Sabot;

void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TFree &/*rhs*/      ) const { Comparison = TComparison::Eq; }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFree &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { Comparison = TComparison::Eq; }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTombstone &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TVoid &/*rhs*/      ) const { Comparison = TComparison::Eq; }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVoid &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &lhs    , const State::TInt8 &rhs          ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt8 &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &lhs    , const State::TInt16 &rhs         ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt16 &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &lhs    , const State::TInt32 &rhs         ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt32 &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &lhs    , const State::TInt64 &rhs         ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TInt64 &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &lhs    , const State::TUInt8 &rhs         ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt8 &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &lhs    , const State::TUInt16 &rhs        ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt16 &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &lhs    , const State::TUInt32 &rhs        ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt32 &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &lhs    , const State::TUInt64 &rhs        ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUInt64 &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &lhs    , const State::TBool &rhs          ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBool &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &lhs    , const State::TChar &rhs          ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TChar &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &lhs    , const State::TFloat &rhs         ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TFloat &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &lhs    , const State::TDouble &rhs        ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDouble &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &lhs    , const State::TDuration &rhs      ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDuration &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &lhs    , const State::TTimePoint &rhs     ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTimePoint &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &lhs    , const State::TUuid &rhs          ) const {  OnScalar(lhs, rhs);  }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TUuid &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &lhs    , const State::TBlob &rhs          ) const { OnArrayOfScalars(lhs, rhs); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TBlob &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &lhs    , const State::TStr &rhs           ) const { OnArrayOfScalars(lhs, rhs); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TStr &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &lhs    , const State::TDesc &rhs          ) const {
  OnArrayOfSingleStates(lhs, rhs);
  switch (Comparison) {
    case Atom::TComparison::Lt : {
      Comparison = Atom::TComparison::Gt;
      break;
    }
    case Atom::TComparison::Eq : {
      break;
    }
    case Atom::TComparison::Gt : {
      Comparison = Atom::TComparison::Lt;
      break;
    }
    case Atom::TComparison::Ne : {
      break;
    }
  }
}
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TDesc &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &lhs    , const State::TOpt &rhs           ) const { OnArrayOfSingleStates(lhs, rhs); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TOpt &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &lhs    , const State::TSet &rhs           ) const { OnArrayOfSingleStates(lhs, rhs); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TSet &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &lhs    , const State::TVector &rhs        ) const { OnArrayOfSingleStates(lhs, rhs); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TVector &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &lhs    , const State::TMap &rhs           ) const { OnArrayOfPairsOfStates(lhs, rhs); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TMap &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TRecord &lhs    , const State::TRecord &rhs        ) const { OnArrayOfSingleStates(lhs, rhs); }
void TCompareStatesVisitor::operator()(const State::TRecord &/*lhs*/, const State::TTuple &/*rhs*/     ) const { throw TUnreachable(); }


void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TFree &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TTombstone &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TVoid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TInt8 &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TInt16 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TInt32 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TInt64 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TUInt8 &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TUInt16 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TUInt32 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TUInt64 &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TBool &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TChar &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TFloat &/*rhs*/     ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TDouble &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TDuration &/*rhs*/  ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TTimePoint &/*rhs*/ ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TUuid &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TBlob &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TStr &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TDesc &/*rhs*/      ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TOpt &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TSet &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TVector &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TMap &/*rhs*/       ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &/*lhs*/, const State::TRecord &/*rhs*/    ) const { throw TUnreachable(); }
void TCompareStatesVisitor::operator()(const State::TTuple &lhs    , const State::TTuple &rhs         ) const { OnArrayOfSingleStates(lhs, rhs); }
