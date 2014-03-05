/* <stig/sabot/state_dumper.cc>

   Implements <stig/sabot/state_dumper.h>.

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

#include <stig/sabot/state_dumper.h>
#include <stig/sabot/type_dumper.h>

using namespace std;
using namespace Stig::Sabot;

void TStateDumper::operator()(const State::TFree &state) const {
  assert(this);
  assert(&state);
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TAny::TWrapper(state.GetType(type_alloc))->Accept(TTypeDumper(Strm));
}

void TStateDumper::operator()(const State::TTombstone &state) const {
  assert(this);
  assert(&state);
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TAny::TWrapper(state.GetType(type_alloc))->Accept(TTypeDumper(Strm));
}

void TStateDumper::operator()(const State::TVoid &state) const {
  assert(this);
  assert(&state);
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TAny::TWrapper(state.GetType(type_alloc))->Accept(TTypeDumper(Strm));
}

void TStateDumper::operator()(const State::TInt8 &state) const {
  assert(this);
  assert(&state);
  Strm << static_cast<int>(state.Get());
}

void TStateDumper::operator()(const State::TInt16 &state) const {
  assert(this);
  assert(&state);
  Strm << state.Get();
}

void TStateDumper::operator()(const State::TInt32 &state) const {
  assert(this);
  assert(&state);
  Strm << state.Get();
}

void TStateDumper::operator()(const State::TInt64 &state) const {
  assert(this);
  assert(&state);
  Strm << state.Get();
}

void TStateDumper::operator()(const State::TUInt8 &state) const {
  assert(this);
  assert(&state);
  Strm << static_cast<unsigned>(state.Get());
}

void TStateDumper::operator()(const State::TUInt16 &state) const {
  assert(this);
  assert(&state);
  Strm << state.Get();
}

void TStateDumper::operator()(const State::TUInt32 &state) const {
  assert(this);
  assert(&state);
  Strm << state.Get();
}

void TStateDumper::operator()(const State::TUInt64 &state) const {
  assert(this);
  assert(&state);
  Strm << state.Get();
}

void TStateDumper::operator()(const State::TBool &state) const {
  assert(this);
  assert(&state);
  Strm << (state.Get() ? "true" : "false");
}

void TStateDumper::operator()(const State::TChar &state) const {
  assert(this);
  assert(&state);
  Strm << '\'' << state.Get() << '\'';
}

void TStateDumper::operator()(const State::TFloat &state) const {
  assert(this);
  assert(&state);
  Strm << state.Get();
}

void TStateDumper::operator()(const State::TDouble &state) const {
  assert(this);
  assert(&state);
  Strm << state.Get();
}

void TStateDumper::operator()(const State::TDuration &state) const {
  assert(this);
  assert(&state);
  Strm << state.Get().count() << "ms";
}

void TStateDumper::operator()(const State::TTimePoint &state) const {
  assert(this);
  assert(&state);
  auto sys_tick = TStdTimePoint::clock::to_time_t(std::chrono::time_point_cast<std::chrono::seconds>(state.Get()));
  char buf[20];
  strftime(buf, sizeof(buf), "%Y:%m:%d:%H:%M:%S", gmtime(&sys_tick));
  Strm << buf << "utc";
}

void TStateDumper::operator()(const State::TUuid &state) const {
  assert(this);
  assert(&state);
  Strm << state.Get();
}

void TStateDumper::operator()(const State::TBlob &state) const {
  assert(this);
  assert(&state);
  Strm << "{{ ";
  void *pin_alloc = alloca(State::GetMaxStatePinSize());
  State::TBlob::TPin::TWrapper pin(state.Pin(pin_alloc));
  #if 0
  auto pin = state.Pin();
  #endif
  const auto
      *start = pin->GetStart(),
      *limit = pin->GetLimit();
  for (const auto *csr = start; csr < limit; ++csr) {
    if (csr > start) {
      Strm << ", ";
    }
    Strm << static_cast<unsigned>(*csr);
  }
  Strm << " }}";
}

void TStateDumper::operator()(const State::TStr &state) const {
  assert(this);
  assert(&state);
  Strm << '"';
  void *pin_alloc = alloca(State::GetMaxStatePinSize());
  State::TStr::TPin::TWrapper pin(state.Pin(pin_alloc));
  #if 0
  auto pin = state.Pin();
  #endif
  const auto
      *start = pin->GetStart(),
      *limit = pin->GetLimit();
  for (const auto *csr = start; csr < limit; ++csr) {
    auto c = *csr;
    switch (c) {
      case '\\': {
        Strm << "\\\\";
        break;
      }
      case '\n': {
        Strm << "\\n";
        break;
      }
      case '\r': {
        Strm << "\\r";
        break;
      }
      case '\t': {
        Strm << "\\t";
        break;
      }
      default: {
        if (isprint(c)) {
          Strm << c;
        } else {
          Strm << "\\x" << static_cast<unsigned>(c);
        }
      }
    }
  }
  Strm << '"';
}

void TStateDumper::operator()(const State::TDesc &state) const {
  assert(this);
  OnArrayOfSingleStates("desc", "hosed-up", state);
}

void TStateDumper::operator()(const State::TOpt &state) const {
  assert(this);
  OnArrayOfSingleStates("opt", "unknown", state);
}

void TStateDumper::operator()(const State::TSet &state) const {
  assert(this);
  OnContainerOfSingleStates("set", state);
}

void TStateDumper::operator()(const State::TVector &state) const {
  assert(this);
  OnContainerOfSingleStates("vector", state);
}

void TStateDumper::operator()(const State::TMap &state) const {
  assert(this);
  OnArrayOfPairsOfStates("map", "empty", state);
}

void TStateDumper::operator()(const State::TRecord &state) const {
  assert(this);
  assert(&state);
  size_t elem_count = state.GetElemCount();
  Strm << "record(";
  void *pin_alloc = alloca(State::GetMaxStatePinSize());
  State::TArrayOfSingleStates::TPin::TWrapper pin(state.Pin(pin_alloc));
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TRecord::TWrapper type(state.GetRecordType(type_alloc));
  void *type_pin_alloc = alloca(Type::GetMaxTypePinSize());
  void *sub_type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TRecord::TPin::TWrapper type_pin(type->Pin(type_pin_alloc));
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    if (elem_idx) {
      Strm << ", ";
    }
    string field_name;
    Type::TAny::TWrapper(type_pin->NewElem(elem_idx, field_name, sub_type_alloc));
    Strm << field_name << ": ";
    State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc))->Accept(*this);
  }
  Strm << ')';
}

void TStateDumper::operator()(const State::TTuple &state) const {
  assert(this);
  assert(&state);
  OnArrayOfSingleStates("tuple", nullptr, state);
}

void TStateDumper::OnArrayOfPairsOfStates(const char *name, const char *kwd_for_zero_size, const State::TArrayOfPairsOfStates &state) const {
  assert(this);
  assert(name);
  assert(kwd_for_zero_size);
  assert(&state);
  size_t elem_count = state.GetElemCount();
  if (elem_count) {
    Strm << name << '(';
    void *pin_alloc = alloca(State::GetMaxStatePinSize());
    State::TArrayOfPairsOfStates::TPin::TWrapper pin(state.Pin(pin_alloc));
    void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
    void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
    for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
      if (elem_idx) {
        Strm << ", ";
      }
      State::TAny::TWrapper(pin->NewLhs(elem_idx, lhs_state_alloc))->Accept(*this);
      Strm << ": ";
      State::TAny::TWrapper(pin->NewRhs(elem_idx, rhs_state_alloc))->Accept(*this);
    }
    Strm << ')';
  } else {
    Strm << kwd_for_zero_size << ' ';
    void *type_alloc = alloca(Type::GetMaxTypeSize());
    Type::TAny::TWrapper(state.GetType(type_alloc))->Accept(TTypeDumper(Strm));
  }
}

void TStateDumper::OnArrayOfSingleStates(const char *name, const char *kwd_for_zero_size, const State::TArrayOfSingleStates &state) const {
  assert(this);
  assert(name);
  assert(&state);
  size_t elem_count = state.GetElemCount();
  if (elem_count || !kwd_for_zero_size) {
    Strm << name << '(';
    void *pin_alloc = alloca(State::GetMaxStatePinSize());
    State::TArrayOfSingleStates::TPin::TWrapper pin(state.Pin(pin_alloc));
    void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
    for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
      if (elem_idx) {
        Strm << ", ";
      }
      State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc))->Accept(*this);
    }
    Strm << ')';
  } else {
    Strm << kwd_for_zero_size << ' ';
    void *type_alloc = alloca(Type::GetMaxTypeSize());
    Type::TAny::TWrapper(state.GetType(type_alloc))->Accept(TTypeDumper(Strm));
  }
}

void TStateDumper::OnContainerOfSingleStates(const char *name, const State::TArrayOfSingleStates &state) const {
  assert(this);
  OnArrayOfSingleStates(name, "empty", state);
}
