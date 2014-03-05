/* <stig/closure.cc>

   Implements <stig/closure.h>.

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

#include <stig/closure.h>

#include <cassert>

#include <stig/atom/transport_arena2.h>
#include <stig/sabot/type_dumper.h>

using namespace std;
using namespace Io;
using namespace Stig;
using namespace Stig::Atom;

Sabot::Type::TAny *TClosure::TType::TPin::NewElem(size_t elem_idx, string &name, void *type_alloc) const {
  assert(this);
  assert(&name);
  auto iter = Walker[elem_idx];
  name = iter->first;
  return iter->second.GetType(Walker.GetClosure()->Arena.get(), type_alloc);
}

Sabot::Type::TAny *TClosure::TType::TPin::NewElem(size_t elem_idx, void *&out_field_name_state, void *field_name_state_alloc, void *type_alloc) const {
  assert(this);
  auto iter = Walker[elem_idx];
  out_field_name_state = Native::State::New(iter->first, field_name_state_alloc);
  return iter->second.GetType(Walker.GetClosure()->Arena.get(), type_alloc);
}

Sabot::Type::TAny *TClosure::TType::TPin::NewElem(size_t elem_idx, void *type_alloc) const {
  assert(this);
  auto iter = Walker[elem_idx];
  return iter->second.GetType(Walker.GetClosure()->Arena.get(), type_alloc);
}

size_t TClosure::TType::GetElemCount() const {
  assert(this);
  return Closure->CoreByName.size();
}

TClosure::TType::TPinBase *TClosure::TType::Pin(void *alloc) const {
  assert(this);
  return new (alloc) TPin(this);
}

Sabot::Type::TRecord *TClosure::TState::GetRecordType(void *type_alloc) const {
  assert(this);
  return new (type_alloc) TType(Closure);
}

Sabot::State::TAny *TClosure::TState::TPin::NewElemInRange(size_t elem_idx, void *state_alloc) const {
  assert(this);
  return Walker[elem_idx]->second.NewState(Walker.GetClosure()->Arena.get(), state_alloc);
}

TClosure::TState::TPinBase *TClosure::TState::Pin(void *alloc) const {
  assert(this);
  return new (alloc) TPin(this);
}

void TClosure::Read(TBinaryInputStream &strm) {
  assert(this);
  assert(&strm);
  Reset();
  strm >> MethodName;
  TCore core;
  unique_ptr<TTransportArena> transport_arena(TTransportArena::Read(strm, core));
  // get the record type from the core
  void *record_type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
  Sabot::Type::TAny::TWrapper type(core.GetType(transport_arena.get(), record_type_alloc));
  auto record_type = dynamic_cast<const Sabot::Type::TRecord *>(type.get());
  if (!record_type) {
    Reset();
    ostringstream strm;
    type->Accept(Sabot::TTypeDumper(strm));
    DEFINE_ERROR(error_t, runtime_error, "reading closure; not a closure type");
    THROW_ERROR(error_t) << "type is \"" << strm.str() << '"';
  }
  // get the record state from the core
  void *record_state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper state(core.NewState(transport_arena.get(), record_state_alloc));
  auto record_state = dynamic_cast<const Sabot::State::TRecord *>(state.get());
  if (!record_state) {
    Reset();
    throw TCore::TBadType();
  }
  // pin the record's type
  void *record_type_pin_alloc = alloca(Sabot::Type::GetMaxTypePinSize());
  Sabot::Type::TRecord::TPin::TWrapper type_pin(record_type->Pin(record_type_pin_alloc));
  // pin the record's state
  void *record_state_pin_alloc = alloca(Sabot::State::GetMaxStatePinSize());
  Sabot::State::TRecord::TPin::TWrapper state_pin(record_state->Pin(record_state_pin_alloc));
  // iterate over the record sabot (type and state, in parallel)
  std::string elem_name;
  void
      *elem_type_alloc  = alloca(Sabot::Type::GetMaxTypeSize()),
      *elem_state_alloc = alloca(Sabot::State::GetMaxStateSize());
  for (size_t elem_idx = 0; elem_idx < type_pin->GetElemCount(); ++elem_idx) {
    Sabot::Type::TAny::TWrapper elem_type(type_pin->NewElem(elem_idx, elem_name, elem_type_alloc));
    Sabot::State::TAny::TWrapper elem_state(state_pin->NewElem(elem_idx, elem_state_alloc));
    CoreByName[elem_name] = TCore(Arena.get(), elem_state.get());
  }
}

bool TClosure::AddCore(const string &name, const TCore &core) {
  assert(this);
  assert(&core);
  return CoreByName.insert(make_pair(name, core)).second;
}

void TClosure::Reset() {
  assert(this);
  Arena = make_shared<TSuprena>();
  MethodName.clear();
}

void TClosure::Write(TBinaryOutputStream &strm) const {
  assert(this);
  assert(&strm);
  strm << MethodName;
  TState state(this);
  TCore core(Arena.get(), &state);
  TTransportArena::Write(strm, Arena.get(), core);
}

TClosure::TClosure(const string &method_name)
    : Arena(make_shared<TSuprena>()), MethodName(method_name) {}

const TCore *TClosure::GetCore(const string &name) const {
  assert(this);
  const TCore *result = TryGetCore(name);
  if (!result) {
    THROW_ERROR(TUnknownArgName) << '"' << name << '"';
  }
  return result;
}

const TCore *TClosure::TryGetCore(const string &name) const {
  assert(this);
  auto iter = CoreByName.find(name);
  return (iter != CoreByName.end()) ? &(iter->second): nullptr;
}
