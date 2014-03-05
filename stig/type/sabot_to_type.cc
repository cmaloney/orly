/* <stig/type/sabot_to_type.cc>

   Implements <stig/type/sabot_to_type.h>.

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

#include <stig/type/sabot_to_type.h>

#include <cassert>

using namespace std;
using namespace Stig;

Type::TSabotToTypeTranslationError::TSabotToTypeTranslationError()
    : logic_error("could not translate from sabot state to stig type") {}

void Type::TToTypeVisitor::operator()(const Sabot::Type::TInt8 &/*type*/) const       { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TInt16 &/*type*/) const      { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TInt32 &/*type*/) const      { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TInt64 &/*type*/) const      { Type = Type::TInt::Get(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TUInt8 &/*type*/) const      { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TUInt16 &/*type*/) const     { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TUInt32 &/*type*/) const     { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TUInt64 &/*type*/) const     { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TBool &/*type*/) const       { Type = Type::TBool::Get(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TChar &/*type*/) const       { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TFloat &/*type*/) const      { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TDouble &/*type*/) const     { Type = Type::TReal::Get(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TDuration &/*type*/) const   { Type = Type::TTimeDiff::Get(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TTimePoint &/*type*/) const  { Type = Type::TTimePnt::Get(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TUuid &/*type*/) const       { Type = Type::TId::Get(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TBlob &/*type*/) const       { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TStr &/*type*/) const        { Type = Type::TStr::Get(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TTombstone &/*type*/) const  { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TVoid &/*type*/) const       { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TDesc &/*type*/) const       { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TFree &/*type*/) const       { throw TSabotToTypeTranslationError(); }
void Type::TToTypeVisitor::operator()(const Sabot::Type::TOpt &type) const            {
  OnUnary(type);
  Type = Type::TOpt::Get(Type);
}
void Type::TToTypeVisitor::operator()(const Sabot::Type::TSet &type) const            {
  OnUnary(type);
  Type = Type::TSet::Get(Type);
}
void Type::TToTypeVisitor::operator()(const Sabot::Type::TVector &type) const         {
  OnUnary(type);
  Type = Type::TList::Get(Type);
}
void Type::TToTypeVisitor::operator()(const Sabot::Type::TMap &type) const            {
  void *pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  Sabot::Type::TBinary::TPin::TWrapper pin(type.Pin(pin_alloc));
  Type::TType temp_type;
  Sabot::Type::TAny::TWrapper(pin->NewLhs(type_alloc))->Accept(*this);
  temp_type = Type;
  Sabot::Type::TAny::TWrapper(pin->NewRhs(type_alloc))->Accept(*this);
  Type = Type::TDict::Get(temp_type, Type);
}
void Type::TToTypeVisitor::operator()(const Sabot::Type::TRecord &type) const         {
  size_t elem_count = type.GetElemCount();
  void *pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  Sabot::Type::TRecord::TPin::TWrapper pin(type.Pin(pin_alloc));
  Type::TObjElems obj_elem_map;
  std::string field_name;
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    Sabot::Type::TAny::TWrapper(pin->NewElem(elem_idx, field_name, type_alloc))->Accept(*this);
    obj_elem_map[field_name] = Type;
  }
  Type = TObj::Get(obj_elem_map);
}
void Type::TToTypeVisitor::operator()(const Sabot::Type::TTuple &type) const          {
  size_t elem_count = type.GetElemCount();
  void *pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  Sabot::Type::TTuple::TPin::TWrapper pin(type.Pin(pin_alloc));
  Type::TAddrElems addr_elem_vec(elem_count);
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    const Sabot::Type::TAny::TWrapper elem_type(pin->NewElem(elem_idx, type_alloc));
    const Sabot::Type::TDesc *desc = dynamic_cast<const Sabot::Type::TDesc *>(elem_type.get());
    if (desc) {
      void *desc_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
      void *desc_type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      Sabot::Type::TDesc::TPin::TWrapper desc_pin(desc->Pin(desc_pin_alloc));
      Sabot::Type::TAny::TWrapper(desc_pin->NewElem(desc_type_alloc))->Accept(*this);
      addr_elem_vec[elem_idx] = make_pair(TAddrDir::Desc, Type);
    } else {
      elem_type->Accept(*this);
      addr_elem_vec[elem_idx] = make_pair(TAddrDir::Asc, Type);
    }
  }
  Type = TAddr::Get(addr_elem_vec);
}

void Type::TToTypeVisitor::OnUnary(const Sabot::Type::TUnary &unary) const {
  void *pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  Sabot::Type::TUnary::TPin::TWrapper pin(unary.Pin(pin_alloc));
  Sabot::Type::TAny::TWrapper(pin->NewElem(type_alloc))->Accept(*this);
}