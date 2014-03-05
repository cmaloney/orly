/* <stig/sabot/get_tuple_size.cc>

   Implements <stig/sabot/get_tuple_size.h>.

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

#include <stig/sabot/get_tuple_size.h>

using namespace std;
using namespace Stig::Sabot;

/* Return the size of the tuple. */
class TGetTupleSizeVisitor final
    : public TTypeVisitor {
  public:

  /* TODO. */
  TGetTupleSizeVisitor(size_t &size)
      : Size(size) {
    assert(&size);
  }

  /* Overrides. */
  virtual void operator()(const Type::TInt8 &type) const override;
  virtual void operator()(const Type::TInt16 &type) const override;
  virtual void operator()(const Type::TInt32 &type) const override;
  virtual void operator()(const Type::TInt64 &type) const override;
  virtual void operator()(const Type::TUInt8 &type) const override;
  virtual void operator()(const Type::TUInt16 &type) const override;
  virtual void operator()(const Type::TUInt32 &type) const override;
  virtual void operator()(const Type::TUInt64 &type) const override;
  virtual void operator()(const Type::TBool &type) const override;
  virtual void operator()(const Type::TChar &type) const override;
  virtual void operator()(const Type::TFloat &type) const override;
  virtual void operator()(const Type::TDouble &type) const override;
  virtual void operator()(const Type::TDuration &type) const override;
  virtual void operator()(const Type::TTimePoint &type) const override;
  virtual void operator()(const Type::TUuid &type) const override;
  virtual void operator()(const Type::TBlob &type) const override;
  virtual void operator()(const Type::TStr &type) const override;
  virtual void operator()(const Type::TTombstone &type) const override;
  virtual void operator()(const Type::TVoid &type) const override;
  virtual void operator()(const Type::TDesc &type) const override;
  virtual void operator()(const Type::TFree &type) const override;
  virtual void operator()(const Type::TOpt &type) const override;
  virtual void operator()(const Type::TSet &type) const override;
  virtual void operator()(const Type::TVector &type) const override;
  virtual void operator()(const Type::TMap &type) const override;
  virtual void operator()(const Type::TRecord &type) const override;
  virtual void operator()(const Type::TTuple &type) const override;

  private:

  size_t &Size;

};  // TGetTupleSizeVisitor

size_t Stig::Sabot::GetTupleSize(const Type::TAny &type) {
  size_t size;
  type.Accept(TGetTupleSizeVisitor(size));
  return size;
}

void TGetTupleSizeVisitor::operator()(const Type::TInt8 &      ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TInt16 &     ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TInt32 &     ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TInt64 &     ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TUInt8 &     ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TUInt16 &    ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TUInt32 &    ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TUInt64 &    ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TBool &      ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TChar &      ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TFloat &     ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TDouble &    ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TDuration &  ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TTimePoint & ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TUuid &      ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TBlob &      ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TStr &       ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TTombstone & ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TVoid &      ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TDesc &      ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TFree &      ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TOpt &       ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TSet &       ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TVector &    ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TMap &       ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TRecord &    ) const { assert(false); }
void TGetTupleSizeVisitor::operator()(const Type::TTuple &that ) const { Size = that.GetElemCount(); }