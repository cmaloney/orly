/* <stig/sabot/match_prefix_type.cc>

   Implements <stig/sabot/match_prefix_type.h>.

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

#include <stig/sabot/match_prefix_type.h>

using namespace std;
using namespace Stig;
using namespace Stig::Sabot;

/* TODO */
class TMatchPrefixTypeVisitor final
    : public TTypeDoubleVisitor {
  public:

  /* TODO. */
  TMatchPrefixTypeVisitor(TMatchResult &out)
      : Result(out) {
    assert(&out);
  }

  /* Overrides. */
  virtual void operator()(const Type::TInt8 &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TInt8 &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TInt16 &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TInt16 &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TInt32 &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TInt32 &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TInt64 &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TInt64 &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TUInt8 &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TUInt8 &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TUInt16 &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TUInt16 &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TUInt32 &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TUInt32 &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TUInt64 &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TUInt64 &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TBool &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TBool &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TChar &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TChar &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TFloat &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TFloat &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TDouble &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TDouble &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TDuration &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TDuration &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TTimePoint &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TTimePoint &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TUuid &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TUuid &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TBlob &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TBlob &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TStr &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TStr &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TTombstone &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TTombstone &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TVoid &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TVoid &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TDesc &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TDesc &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TFree &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TFree &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TOpt &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TOpt &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TSet &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TSet &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TVector &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TVector &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TMap &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TMap &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TRecord &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TRecord &lhs, const Type::TTuple &rhs    ) const override;

  virtual void operator()(const Type::TTuple &lhs, const Type::TInt8 &rhs     ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TInt16 &rhs    ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TInt32 &rhs    ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TInt64 &rhs    ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TUInt8 &rhs    ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TUInt16 &rhs   ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TUInt32 &rhs   ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TUInt64 &rhs   ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TBool &rhs     ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TChar &rhs     ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TFloat &rhs    ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TDouble &rhs   ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TDuration &rhs ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TTimePoint &rhs) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TUuid &rhs     ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TBlob &rhs     ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TStr &rhs      ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TTombstone &rhs) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TVoid &rhs     ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TDesc &rhs     ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TFree &rhs     ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TOpt &rhs      ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TSet &rhs      ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TVector &rhs   ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TMap &rhs      ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TRecord &rhs   ) const override;
  virtual void operator()(const Type::TTuple &lhs, const Type::TTuple &rhs    ) const override;

  private:

  /* TODO */
  inline void OnUnary(const Type::TUnary &lhs, const Type::TUnary &rhs) const;

  /* TODO */
  inline void OnBinary(const Type::TBinary &lhs, const Type::TBinary &rhs) const;

  /* TODO */
  inline void DidNotMatch() const;

  /* TODO */
  inline void DidNotUnify() const;

  /* TODO */
  inline void OnFree(const Type::TFree &lhs, const Type::TAny &rhs) const;

  /* TODO */
  TMatchResult &Result;

};  // TMatchPrefixTypeVisitor

inline void TMatchPrefixTypeVisitor::OnUnary(const Type::TUnary &lhs, const Type::TUnary &rhs) const {
  void *lhs_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize() * 2);
  void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + Sabot::Type::GetMaxTypePinSize();
  Type::TUnary::TPin::TWrapper
    lhs_pin(lhs.Pin(lhs_pin_alloc)),
    rhs_pin(rhs.Pin(rhs_pin_alloc));
  void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
  void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
  AcceptDouble(*Type::TAny::TWrapper(lhs_pin->NewElem(lhs_type_alloc)),
               *Type::TAny::TWrapper(rhs_pin->NewElem(rhs_type_alloc)),
               *this);
}

inline void TMatchPrefixTypeVisitor::OnBinary(const Type::TBinary &lhs, const Type::TBinary &rhs) const {
  void *lhs_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize() * 2);
  void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + Sabot::Type::GetMaxTypePinSize();
  Type::TBinary::TPin::TWrapper
    lhs_pin(lhs.Pin(lhs_pin_alloc)),
    rhs_pin(rhs.Pin(rhs_pin_alloc));
  void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
  void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
  AcceptDouble(*Type::TAny::TWrapper(lhs_pin->NewLhs(lhs_type_alloc)),
               *Type::TAny::TWrapper(rhs_pin->NewLhs(rhs_type_alloc)),
               *this);
  if (!IsNoMatch(Result)) {
    AcceptDouble(*Type::TAny::TWrapper(lhs_pin->NewRhs(lhs_type_alloc)),
                 *Type::TAny::TWrapper(rhs_pin->NewRhs(rhs_type_alloc)),
                 *this);
  }
}

/* TODO */
inline void TMatchPrefixTypeVisitor::DidNotMatch() const {
  Result = TMatchResult::NoMatch;
}

/* TODO */
inline void TMatchPrefixTypeVisitor::DidNotUnify() const {
  switch (Result) {
    case TMatchResult::NoMatch: {
      break;
    }
    case TMatchResult::PrefixMatch: {
      break;
    }
    case TMatchResult::Unifies: {
      Result = TMatchResult::PrefixMatch;
      break;
    }
  }
}

/* TODO */
inline void TMatchPrefixTypeVisitor::OnFree(const Type::TFree &lhs, const Type::TAny &rhs) const {
  TMatchResult cur = TMatchResult::Unifies;
  void *lhs_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  Type::TUnary::TPin::TWrapper lhs_pin(lhs.Pin(lhs_pin_alloc));
  void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  AcceptDouble(*Type::TAny::TWrapper(lhs_pin->NewElem(lhs_type_alloc)), rhs, TMatchPrefixTypeVisitor(cur));
  switch (cur) {
    case TMatchResult::NoMatch: {
      DidNotUnify();
      break;
    }
    case TMatchResult::PrefixMatch: {
      throw std::runtime_error("We do not currently support Free<Free<T>>");
      break;
    }
    case TMatchResult::Unifies: {
      break;
    }
  }
}

void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt8 &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt16 &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt32 &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TInt64 &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt8 &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt16 &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt32 &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUInt64 &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TBool &/*rhs*/     ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBool &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TChar &/*rhs*/     ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TChar &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFloat &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDouble &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDuration &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTimePoint &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TUuid &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TBlob &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TStr &/*rhs*/      ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TStr &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TTombstone &/*rhs*/) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTombstone &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { /* Matches */  }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVoid &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &lhs,     const Type::TDesc &rhs         ) const { OnUnary(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TDesc &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TFree &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TInt8 &rhs     ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TInt16 &rhs    ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TInt32 &rhs    ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TInt64 &rhs    ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TUInt8 &rhs    ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TUInt16 &rhs   ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TUInt32 &rhs   ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TUInt64 &rhs   ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TBool &rhs     ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TChar &rhs     ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TFloat &rhs    ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TDouble &rhs   ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TDuration &rhs ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TTimePoint &rhs) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TUuid &rhs     ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TBlob &rhs     ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TStr &rhs      ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TTombstone &rhs) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TVoid &rhs     ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TDesc &rhs     ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TOpt &rhs      ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TSet &rhs      ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TVector &rhs   ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TMap &rhs      ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TRecord &rhs   ) const { OnFree(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TFree &lhs, const Type::TTuple &rhs    ) const { OnFree(lhs, rhs); }


void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &lhs,     const Type::TOpt &rhs          ) const { OnUnary(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TOpt &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &lhs,     const Type::TSet &rhs          ) const { OnUnary(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TSet &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &lhs,     const Type::TVector &rhs       ) const { OnUnary(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TVector &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &lhs,     const Type::TMap &rhs          ) const { OnBinary(lhs, rhs); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TMap &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &lhs,     const Type::TRecord &rhs       ) const {
  size_t lhs_count = lhs.GetElemCount();
  size_t rhs_count = rhs.GetElemCount();
  if (lhs_count != rhs_count) {
    DidNotMatch();
  } else {
    void *lhs_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize() * 2);
    void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + Sabot::Type::GetMaxTypePinSize();
    Type::TRecord::TPin::TWrapper
      lhs_pin(lhs.Pin(lhs_pin_alloc)),
      rhs_pin(rhs.Pin(rhs_pin_alloc));
    void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
    void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
    void *lhs_type_alloc_2 = alloca(Sabot::Type::GetMaxTypeSize() * 2);
    void *rhs_type_alloc_2 = reinterpret_cast<uint8_t *>(lhs_type_alloc_2) + Sabot::Type::GetMaxTypeSize();
    Sabot::State::TAny
      *lhs_field_name_state,
      *rhs_field_name_state;
    void *lhs_field_name_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
    void *rhs_field_name_state_alloc = reinterpret_cast<uint8_t *>(lhs_field_name_state_alloc) + Sabot::State::GetMaxStateSize();
    for (size_t elem_idx = 0; elem_idx < lhs_count && elem_idx < rhs_count; ++elem_idx) {
      const Type::TAny::TWrapper lhs_elem(lhs_pin->NewElem(elem_idx, reinterpret_cast<void *&>(lhs_field_name_state), lhs_field_name_state_alloc, lhs_type_alloc));
      const Type::TAny::TWrapper rhs_elem(rhs_pin->NewElem(elem_idx, reinterpret_cast<void *&>(rhs_field_name_state), rhs_field_name_state_alloc, rhs_type_alloc));
      Sabot::State::TAny::TWrapper
      lhs_field_name(lhs_field_name_state),
      rhs_field_name(rhs_field_name_state);
      Atom::TComparison str_comp = CompareStates(*lhs_field_name, *rhs_field_name, lhs_type_alloc_2, rhs_type_alloc_2);
      if (Atom::IsEq(str_comp)) {  // Names match
        AcceptDouble(*lhs_elem, *rhs_elem, *this);
        if (IsNoMatch(Result)) {
          return;
        }
      } else {  // Names don't match
        DidNotMatch();
        return;
      }
    }
  }
}
void TMatchPrefixTypeVisitor::operator()(const Type::TRecord &/*lhs*/, const Type::TTuple &/*rhs*/    ) const { DidNotMatch(); }


void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TFree &/*rhs*/     ) const { throw TUnreachable(); }

void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TInt8 &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TInt16 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TInt32 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TInt64 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TUInt8 &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TUInt16 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TUInt32 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TUInt64 &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TBool &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TChar &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TFloat &/*rhs*/    ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TDouble &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TDuration &/*rhs*/ ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TTimePoint &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TUuid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TBlob &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TStr &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TTombstone &/*rhs*/) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TVoid &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TDesc &/*rhs*/     ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TOpt &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TSet &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TVector &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TMap &/*rhs*/      ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &/*lhs*/, const Type::TRecord &/*rhs*/   ) const { DidNotMatch(); }
void TMatchPrefixTypeVisitor::operator()(const Type::TTuple &lhs,     const Type::TTuple &rhs        ) const {
  size_t lhs_count = lhs.GetElemCount();
  size_t rhs_count = rhs.GetElemCount();
  if (lhs_count != rhs_count) {
    DidNotMatch();
  } else {
    void *lhs_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize() * 2);
    void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + Sabot::Type::GetMaxTypePinSize();
    Type::TTuple::TPin::TWrapper
      lhs_pin(lhs.Pin(lhs_pin_alloc)),
      rhs_pin(rhs.Pin(rhs_pin_alloc));
    void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
    void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
    for (size_t elem_idx = 0; elem_idx < lhs_count && elem_idx < rhs_count; ++elem_idx) {
      AcceptDouble(*Type::TAny::TWrapper(lhs_pin->NewElem(elem_idx, lhs_type_alloc)),
                   *Type::TAny::TWrapper(rhs_pin->NewElem(elem_idx, rhs_type_alloc)),
                   *this);
      if (IsNoMatch(Result)) {
        break;
      }
    }
  }
}

TMatchResult Stig::Sabot::MatchPrefixType(const Type::TAny &lhs_any, const Type::TAny &rhs_any) {
  TMatchResult result = TMatchResult::Unifies;
  TMatchPrefixTypeVisitor visitor(result);
  //const Type::TTuple *lhs_ptr = dynamic_cast<const Type::TTuple *>(&lhs_any);
  //const Type::TTuple *rhs_ptr = dynamic_cast<const Type::TTuple *>(&rhs_any);
  assert(dynamic_cast<const Type::TTuple *>(&lhs_any));  /* lhs must be a tuple. */
  assert(dynamic_cast<const Type::TTuple *>(&rhs_any));  /* rhs must be a tuple. */
  const Type::TTuple &lhs = *static_cast<const Type::TTuple *>(&lhs_any);
  const Type::TTuple &rhs = *static_cast<const Type::TTuple *>(&rhs_any);
  size_t lhs_count = lhs.GetElemCount();
  size_t rhs_count = rhs.GetElemCount();
  void *lhs_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize() * 2);
  void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + Sabot::Type::GetMaxTypePinSize();
  Type::TTuple::TPin::TWrapper
    lhs_pin(lhs.Pin(lhs_pin_alloc)),
    rhs_pin(rhs.Pin(rhs_pin_alloc));
  void *lhs_type_alloc = alloca(Sabot::Type::GetMaxTypeSize() * 2);
  void *rhs_type_alloc = reinterpret_cast<uint8_t *>(lhs_type_alloc) + Sabot::Type::GetMaxTypeSize();
  size_t elem_idx = 0;
  for (; elem_idx < lhs_count && elem_idx < rhs_count; ++elem_idx) {
    AcceptDouble(*Type::TAny::TWrapper(lhs_pin->NewElem(elem_idx, lhs_type_alloc)),
                 *Type::TAny::TWrapper(rhs_pin->NewElem(elem_idx, rhs_type_alloc)),
                 visitor);
    switch (result) {
      case TMatchResult::NoMatch: {
        return result;
      }
      case TMatchResult::PrefixMatch: {
        /* we didn't unify on one of the free types, so we're not going to be able to unify anymore.
           If we're in the unification phase we're never going to get a NoMatch either */
        return result;
      }
      case TMatchResult::Unifies: {
        break;
      }
    }
  }
  if (IsUnifies(result)) {
    if (lhs_count == rhs_count) {
      /* do nothing */
    } else if (elem_idx < lhs_count) { /* lhs is bigger */
      result = TMatchResult::PrefixMatch;
      for (; elem_idx < lhs_count; ++elem_idx) {
        Type::TAny::TWrapper elem_wrapper(lhs_pin->NewElem(elem_idx, lhs_type_alloc));
        const Type::TFree *free_type = dynamic_cast<const Type::TFree *>(elem_wrapper.get());
        if (free_type == nullptr) {
          result = TMatchResult::NoMatch;
        }
      }
    } else { /* rhs is bigger */
      assert(elem_idx < rhs_count);
      result = TMatchResult::PrefixMatch;
    }
  }
  return result;
}