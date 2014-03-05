/* <stig/sabot/get_hash.cc>

   Implements <stig/sabot/get_hash.h>.

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

#include <stig/sabot/get_hash.h>

using namespace std;
using namespace Stig;
using namespace Stig::Sabot;

size_t Stig::Sabot::GetHash(const State::TAny &state) {
  size_t ret = 0;
  state.Accept(THashVisitor(ret));
  return ret;
}

void THashVisitor::operator()(const State::TFree &/*state*/)      const { Hash = 0UL; }
void THashVisitor::operator()(const State::TTombstone &/*state*/) const { Hash = 0UL; }
void THashVisitor::operator()(const State::TVoid &/*state*/)      const { Hash = 0UL; }
void THashVisitor::operator()(const State::TInt8 &state)          const { Hash = _Hash_impl::hash(&state.Get(), sizeof(int8_t)); }
void THashVisitor::operator()(const State::TInt16 &state)         const { Hash = _Hash_impl::hash(&state.Get(), sizeof(int16_t)); }
void THashVisitor::operator()(const State::TInt32 &state)         const { Hash = _Hash_impl::hash(&state.Get(), sizeof(int32_t)); }
void THashVisitor::operator()(const State::TInt64 &state)         const { Hash = _Hash_impl::hash(&state.Get(), sizeof(int64_t)); }
void THashVisitor::operator()(const State::TUInt8 &state)         const { Hash = _Hash_impl::hash(&state.Get(), sizeof(uint8_t)); }
void THashVisitor::operator()(const State::TUInt16 &state)        const { Hash = _Hash_impl::hash(&state.Get(), sizeof(uint16_t)); }
void THashVisitor::operator()(const State::TUInt32 &state)        const { Hash = _Hash_impl::hash(&state.Get(), sizeof(uint32_t)); }
void THashVisitor::operator()(const State::TUInt64 &state)        const { Hash = _Hash_impl::hash(&state.Get(), sizeof(uint64_t)); }
void THashVisitor::operator()(const State::TBool &state)          const { Hash = hash<bool>()(state.Get()); }
void THashVisitor::operator()(const State::TChar &state)          const { Hash = hash<char>()(state.Get()); }
void THashVisitor::operator()(const State::TFloat &state)         const { Hash = hash<float>()(state.Get()); }
void THashVisitor::operator()(const State::TDouble &state)        const { Hash = hash<double>()(state.Get()); }
void THashVisitor::operator()(const State::TDuration &state)      const {
  Hash = std::hash<TStdDuration::rep>()(state.Get().count());
}
void THashVisitor::operator()(const State::TTimePoint &state)     const {
  Hash = std::hash<TStdDuration::rep>()(state.Get().time_since_epoch().count());
}
void THashVisitor::operator()(const State::TUuid &state)          const { Hash = hash<Base::TUuid>()(state.Get()); }
void THashVisitor::operator()(const State::TBlob &state)          const {
  void *pin_alloc = alloca(State::GetMaxStatePinSize());
  State::TBlob::TPin::TWrapper pin(state.Pin(pin_alloc));
  Hash = _Hash_impl::hash(pin->GetStart(), pin->GetSize());
}
void THashVisitor::operator()(const State::TStr &state)           const {
  void *pin_alloc = alloca(State::GetMaxStatePinSize());
  State::TStr::TPin::TWrapper pin(state.Pin(pin_alloc));
  Hash = _Hash_impl::hash(pin->GetStart(), pin->GetSize());
}
void THashVisitor::operator()(const State::TDesc &state)          const { OnArrayOfSingle(state); }
void THashVisitor::operator()(const State::TOpt &state)           const { OnArrayOfSingle(state); }
void THashVisitor::operator()(const State::TSet &state)           const { OnArrayOfSingle(state); }
void THashVisitor::operator()(const State::TVector &state)        const { OnArrayOfSingle(state); }
void THashVisitor::operator()(const State::TMap &state)           const { OnArrayOfPairs(state);  }
void THashVisitor::operator()(const State::TRecord &state)        const { OnArrayOfSingle(state); }
void THashVisitor::operator()(const State::TTuple &state)         const { OnArrayOfSingle(state); }

void THashVisitor::OnArrayOfSingle(const State::TArrayOfSingleStates &single) const {
  void *pin_alloc = alloca(State::GetMaxStatePinSize());
  State::TArrayOfSingleStates::TPin::TWrapper pin(single.Pin(pin_alloc));
  size_t total_hash = 0UL;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  for (size_t elem_idx = 0; elem_idx < pin->GetElemCount(); ++elem_idx) {
    Sabot::State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc))->Accept(*this);
    total_hash ^= Base::RotatedRight(Hash, elem_idx * 5);
  }
  Hash = total_hash;
}

void THashVisitor::OnArrayOfPairs(const State::TArrayOfPairsOfStates &that) const {
  void *pin_alloc = alloca(State::GetMaxStatePinSize());
  State::TArrayOfPairsOfStates::TPin::TWrapper pin(that.Pin(pin_alloc));
  size_t total_hash = 0UL;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  for (size_t elem_idx = 0; elem_idx < pin->GetElemCount(); ++elem_idx) {
    Sabot::State::TAny::TWrapper(pin->NewLhs(elem_idx, state_alloc))->Accept(*this);
    total_hash ^= Base::RotatedRight(Hash, elem_idx * 5);
    Sabot::State::TAny::TWrapper(pin->NewRhs(elem_idx, state_alloc))->Accept(*this);
    total_hash ^= Base::RotatedRight(Hash, elem_idx * 7);
  }
  Hash = total_hash;
}