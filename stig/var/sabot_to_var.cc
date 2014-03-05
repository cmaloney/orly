/* <stig/var/sabot_to_var.cc>

   Implements <stig/var/sabot_to_var.h>.

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

#include <stig/var/sabot_to_var.h>

#include <stig/type/sabot_to_type.h>

#include <cassert>

using namespace std;
using namespace Stig;

Var::TVarTranslationError::TVarTranslationError()
    : logic_error("could not translate from sabot state to stig var") {}

void Var::TToVarVisitor::operator()(const Sabot::State::TFree &state) const          {
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  void *type_alloc_2 = alloca(Sabot::Type::GetMaxTypeSize());
  Sabot::Type::TAny::TWrapper type_sabot(state.GetType(type_alloc));
  Sabot::Type::TFree *free_type = dynamic_cast<Sabot::Type::TFree *>(type_sabot.get());
  assert(free_type);
  void *pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  Sabot::Type::TFree::TPin::TWrapper pin(free_type->Pin(pin_alloc));
  Sabot::Type::TAny::TWrapper sub_type(pin->NewElem(type_alloc_2));
  Sabot::Type::TDesc *desc_type = dynamic_cast<Sabot::Type::TDesc *>(sub_type.get());
  if (desc_type) {
    void *type_alloc_3 = alloca(Sabot::Type::GetMaxTypeSize());
    void *desc_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
    Sabot::Type::TFree::TPin::TWrapper pin(desc_type->Pin(desc_pin_alloc));
    Sabot::Type::TAny::TWrapper desc_sub_type(pin->NewElem(type_alloc_3));
    Var = Var::TVar::Free(Type::ToType(*desc_sub_type));
  } else {
    Var = Var::TVar::Free(Type::ToType(*sub_type));
  }
}
void Var::TToVarVisitor::operator()(const Sabot::State::TTombstone &/*state*/) const { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TVoid &/*state*/) const      { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TInt8 &/*state*/) const      { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TInt16 &/*state*/) const     { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TInt32 &/*state*/) const     { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TInt64 &state) const         { Var = Var::TVar(state.Get()); }
void Var::TToVarVisitor::operator()(const Sabot::State::TUInt8 &/*state*/) const     { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TUInt16 &/*state*/) const    { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TUInt32 &/*state*/) const    { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TUInt64 &/*state*/) const    { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TBool &state) const          { Var = Var::TVar(state.Get()); }
void Var::TToVarVisitor::operator()(const Sabot::State::TChar &/*state*/) const      { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TFloat &/*state*/) const     { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TDouble &state) const        { Var = Var::TVar(state.Get()); }
void Var::TToVarVisitor::operator()(const Sabot::State::TDuration &state) const      {
  Var = Var::TVar(Base::Chrono::TTimeDiff(std::chrono::duration_cast<Base::Chrono::TTimeDiff>(state.Get())));
}
void Var::TToVarVisitor::operator()(const Sabot::State::TTimePoint &state) const     {
  Var = Var::TVar(Base::Chrono::TTimePnt(std::chrono::duration_cast<Base::Chrono::TTimeDiff>(state.Get().time_since_epoch())));
}
void Var::TToVarVisitor::operator()(const Sabot::State::TUuid &state) const          { Var = Var::TVar(TUUID(state.Get().GetRaw())); }
void Var::TToVarVisitor::operator()(const Sabot::State::TBlob &/*state*/) const      { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TStr &state) const           {
  void *pin_alloc = alloca(Sabot::State::GetMaxStatePinSize());
  Sabot::State::TStr::TPin::TWrapper pin(state.Pin(pin_alloc));
  Var = Var::TVar(string(pin->GetStart(), pin->GetSize()));
}
void Var::TToVarVisitor::operator()(const Sabot::State::TDesc &/*state*/) const      { throw TVarTranslationError(); }
void Var::TToVarVisitor::operator()(const Sabot::State::TOpt &state) const           {
  void *pin_alloc = alloca(Sabot::State::GetMaxStatePinSize());
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TOpt::TPin::TWrapper pin(state.Pin(pin_alloc));
  const size_t elem_count = pin->GetElemCount();
  Rt::TOpt<Var::TVar> opt;
  if (elem_count) {
    opt = Rt::TOpt<Var::TVar>(ToVar(*Sabot::State::TAny::TWrapper(pin->NewElem(0, state_alloc))));
  }
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  const Sabot::Type::TAny::TWrapper elem_type(state.GetType(type_alloc));
  const Sabot::Type::TUnary *unary_type = dynamic_cast<const Sabot::Type::TUnary *>(elem_type.get());
  assert(unary_type);
  void *type_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  Sabot::Type::TUnary::TPin::TWrapper type_pin(unary_type->Pin(type_pin_alloc));
  Var = Var::TVar::Opt(opt, Type::ToType(*Sabot::Type::TAny::TWrapper(type_pin->NewElem(type_alloc))));
}
void Var::TToVarVisitor::operator()(const Sabot::State::TSet &state) const           {
  void *pin_alloc = alloca(Sabot::State::GetMaxStatePinSize());
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TSet::TPin::TWrapper pin(state.Pin(pin_alloc));
  const size_t elem_count = pin->GetElemCount();
  Rt::TSet<Var::TVar> state_set;
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    state_set.insert(ToVar(*Sabot::State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc))));
  }
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  const Sabot::Type::TAny::TWrapper elem_type(state.GetType(type_alloc));
  const Sabot::Type::TUnary *unary_type = dynamic_cast<const Sabot::Type::TUnary *>(elem_type.get());
  assert(unary_type);
  void *type_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  Sabot::Type::TUnary::TPin::TWrapper type_pin(unary_type->Pin(type_pin_alloc));
  Var = Var::TVar::Set(state_set, Type::ToType(*Sabot::Type::TAny::TWrapper(type_pin->NewElem(type_alloc))));
}
void Var::TToVarVisitor::operator()(const Sabot::State::TVector &state) const        {
  void *pin_alloc = alloca(Sabot::State::GetMaxStatePinSize());
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TVector::TPin::TWrapper pin(state.Pin(pin_alloc));
  const size_t elem_count = pin->GetElemCount();
  std::vector<Var::TVar> state_vec(elem_count);
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    state_vec[elem_idx] = ToVar(*Sabot::State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc)));
  }
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  const Sabot::Type::TAny::TWrapper elem_type(state.GetType(type_alloc));
  const Sabot::Type::TUnary *unary_type = dynamic_cast<const Sabot::Type::TUnary *>(elem_type.get());
  assert(unary_type);
  void *type_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  Sabot::Type::TUnary::TPin::TWrapper type_pin(unary_type->Pin(type_pin_alloc));
  Var = Var::TVar::List(state_vec, Type::ToType(*Sabot::Type::TAny::TWrapper(type_pin->NewElem(type_alloc))));
}
void Var::TToVarVisitor::operator()(const Sabot::State::TMap &state) const           {
  void *pin_alloc = alloca(Sabot::State::GetMaxStatePinSize());
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TMap::TPin::TWrapper pin(state.Pin(pin_alloc));
  const size_t elem_count = pin->GetElemCount();
  Rt::TDict<Var::TVar, Var::TVar> state_map;
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    state_map[ToVar(*Sabot::State::TAny::TWrapper(pin->NewLhs(elem_idx, state_alloc)))] = ToVar(*Sabot::State::TAny::TWrapper(pin->NewRhs(elem_idx, state_alloc)));
  }
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  const Sabot::Type::TAny::TWrapper elem_type(state.GetType(type_alloc));
  const Sabot::Type::TBinary *binary_type = dynamic_cast<const Sabot::Type::TBinary *>(elem_type.get());
  assert(binary_type);
  void *type_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  Sabot::Type::TBinary::TPin::TWrapper type_pin(binary_type->Pin(type_pin_alloc));
  Var = Var::TVar::Dict(state_map,
                       Type::ToType(*Sabot::Type::TAny::TWrapper(type_pin->NewLhs(type_alloc))),
                       Type::ToType(*Sabot::Type::TAny::TWrapper(type_pin->NewRhs(type_alloc))));
}
void Var::TToVarVisitor::operator()(const Sabot::State::TRecord &state) const        {
  void *pin_alloc = alloca(Sabot::State::GetMaxStatePinSize());
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  void *type_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  void *elem_type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  Sabot::State::TRecord::TPin::TWrapper pin(state.Pin(pin_alloc));
  Sabot::Type::TRecord::TWrapper record_type(state.GetRecordType(type_alloc));
  Sabot::Type::TRecord::TPin::TWrapper type_pin(record_type->Pin(type_pin_alloc));
  const size_t elem_count = pin->GetElemCount();
  std::unordered_map<std::string, TVar> state_map;
  string field_name;
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    Sabot::Type::TAny::TWrapper(type_pin->NewElem(elem_idx, field_name, elem_type_alloc));
    const Sabot::State::TAny::TWrapper elem_state(pin->NewElem(elem_idx, state_alloc));
    state_map[field_name] = ToVar(*elem_state);
  }
  Var = Var::TVar::Obj(state_map);
}
void Var::TToVarVisitor::operator()(const Sabot::State::TTuple &state) const         {
  void *pin_alloc = alloca(Sabot::State::GetMaxStatePinSize());
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TTuple::TPin::TWrapper pin(state.Pin(pin_alloc));
  const size_t elem_count = pin->GetElemCount();
  std::vector<std::pair<TAddrDir, TVar>> state_vec(elem_count);
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    const Sabot::State::TAny::TWrapper elem_state(pin->NewElem(elem_idx, state_alloc));
    const Sabot::State::TDesc *desc = dynamic_cast<const Sabot::State::TDesc *>(elem_state.get());
    if (desc) {
      void *desc_pin_alloc = alloca(Sabot::State::GetMaxStatePinSize());
      Sabot::State::TDesc::TPin::TWrapper desc_pin(desc->Pin(desc_pin_alloc));
      state_vec[elem_idx] = make_pair(TAddrDir::Desc, ToVar(*Sabot::State::TAny::TWrapper(desc_pin->NewElem(0, state_alloc))));
    } else {
      /* it might be free and then desc. */
      const Sabot::State::TFree *free_state = dynamic_cast<const Sabot::State::TFree *>(elem_state.get());
      if (free_state) {
        void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
        void *type_alloc_2 = alloca(Sabot::Type::GetMaxTypeSize());
        Sabot::Type::TAny::TWrapper type_sabot(free_state->GetType(type_alloc));
        Sabot::Type::TFree *free_type = dynamic_cast<Sabot::Type::TFree *>(type_sabot.get());
        assert(free_type);
        void *sub_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
        Sabot::Type::TFree::TPin::TWrapper sub_pin(free_type->Pin(sub_pin_alloc));
        Sabot::Type::TAny::TWrapper sub_type(sub_pin->NewElem(type_alloc_2));
        Sabot::Type::TDesc *desc_type = dynamic_cast<Sabot::Type::TDesc *>(sub_type.get());
        if (desc_type) {
          state_vec[elem_idx] = make_pair(TAddrDir::Desc, ToVar(*Sabot::State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc))));
        } else {
          state_vec[elem_idx] = make_pair(TAddrDir::Asc, ToVar(*Sabot::State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc))));
        }
      } else {
        state_vec[elem_idx] = make_pair(TAddrDir::Asc, ToVar(*Sabot::State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc))));
      }
    }
  }
  Var = Var::TVar::Addr(state_vec);
}