/* <stig/sabot/get_depth.cc>

   Implements <stig/sabot/get_depth.h>.

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

#include <stig/sabot/get_depth.h>

using namespace std;
using namespace Stig;
using namespace Stig::Sabot;

/* Return the depth of a sabot. */
class TDepthVisitor final
    : public TTypeVisitor {
  public:

  /* TODO. */
  TDepthVisitor(size_t &out_depth)
      : Depth(out_depth) {
    assert(&out_depth);
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

  /* Handler for binary types. */
  void OnBinaryType(const Type::TBinary &binary) const;

  /* Handler for unary types. */
  void OnUnaryType(const Type::TUnary &unary) const;

  /* The depth which we set. */
  size_t &Depth;

};  // TDepthVisitor

size_t Stig::Sabot::GetDepth(const Type::TAny &type) {
  size_t ret = 0;
  type.Accept(TDepthVisitor(ret));
  return ret;
}

void TDepthVisitor::operator()(const Type::TInt8 &      ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TInt16 &     ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TInt32 &     ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TInt64 &     ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TUInt8 &     ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TUInt16 &    ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TUInt32 &    ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TUInt64 &    ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TBool &      ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TChar &      ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TFloat &     ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TDouble &    ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TDuration &  ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TTimePoint & ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TUuid &      ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TBlob &      ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TStr &       ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TTombstone & ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TVoid &      ) const { Depth = 0UL; }
void TDepthVisitor::operator()(const Type::TDesc &type  ) const { OnUnaryType(type); }
void TDepthVisitor::operator()(const Type::TFree &type  ) const { OnUnaryType(type); }
void TDepthVisitor::operator()(const Type::TOpt &type   ) const { OnUnaryType(type); }
void TDepthVisitor::operator()(const Type::TSet &type   ) const { OnUnaryType(type); }
void TDepthVisitor::operator()(const Type::TVector &type) const { OnUnaryType(type); }
void TDepthVisitor::operator()(const Type::TMap &type   ) const { OnBinaryType(type); }
void TDepthVisitor::operator()(const Type::TRecord &type) const {
  assert(this);
  assert(&type);
  size_t max_child_depth = 0UL;
  size_t elem_count = type.GetElemCount();
  void *pin_alloc = alloca(Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TRecord::TPin::TWrapper pin(type.Pin(pin_alloc));
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    Type::TAny::TWrapper(pin->NewElem(elem_idx, type_alloc))->Accept(*this);
    max_child_depth = std::max(Depth, max_child_depth);
  }
  Depth = max_child_depth + 1UL;
}
void TDepthVisitor::operator()(const Type::TTuple &type ) const {
  assert(this);
  assert(&type);
  size_t max_child_depth = 0UL;
  size_t elem_count = type.GetElemCount();
  void *pin_alloc = alloca(Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TTuple::TPin::TWrapper pin(type.Pin(pin_alloc));
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    Type::TAny::TWrapper(pin->NewElem(elem_idx, type_alloc))->Accept(*this);
    max_child_depth = std::max(Depth, max_child_depth);
  }
  Depth = max_child_depth + 1UL;
}

void TDepthVisitor::OnBinaryType(const Type::TBinary &binary) const {
  assert(this);
  assert(&binary);
  void *pin_alloc = alloca(Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TBinary::TPin::TWrapper pin(binary.Pin(pin_alloc));
  Type::TAny::TWrapper(pin->NewLhs(type_alloc))->Accept(*this);
  size_t lhs_depth = Depth;
  Type::TAny::TWrapper(pin->NewRhs(type_alloc))->Accept(*this);
  size_t rhs_depth = Depth;
  Depth = std::max(lhs_depth, rhs_depth) + 1UL;
}

void TDepthVisitor::OnUnaryType(const Type::TUnary &unary) const {
  assert(this);
  assert(&unary);
  void *pin_alloc = alloca(Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TUnary::TPin::TWrapper pin(unary.Pin(pin_alloc));
  Type::TAny::TWrapper(pin->NewElem(type_alloc))->Accept(*this);
  ++Depth;
}
