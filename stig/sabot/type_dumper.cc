/* <stig/sabot/type_dumper.cc>

   Implements <stig/sabot/type_dumper.h>.

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

#include <stig/sabot/type_dumper.h>

using namespace std;
using namespace Stig::Sabot;

void TTypeDumper::operator()(const Type::TInt8 &) const {
  assert(this);
  Strm << "int8";
}

void TTypeDumper::operator()(const Type::TInt16 &) const {
  assert(this);
  Strm << "int16";
}

void TTypeDumper::operator()(const Type::TInt32 &) const {
  assert(this);
  Strm << "int32";
}

void TTypeDumper::operator()(const Type::TInt64 &) const {
  assert(this);
  Strm << "int64";
}

void TTypeDumper::operator()(const Type::TUInt8 &) const {
  assert(this);
  Strm << "uint8";
}

void TTypeDumper::operator()(const Type::TUInt16 &) const {
  assert(this);
  Strm << "uint16";
}

void TTypeDumper::operator()(const Type::TUInt32 &) const {
  assert(this);
  Strm << "uint32";
}

void TTypeDumper::operator()(const Type::TUInt64 &) const {
  assert(this);
  Strm << "uint64";
}

void TTypeDumper::operator()(const Type::TBool &) const {
  assert(this);
  Strm << "bool";
}

void TTypeDumper::operator()(const Type::TChar &) const {
  assert(this);
  Strm << "char";
}

void TTypeDumper::operator()(const Type::TFloat &) const {
  assert(this);
  Strm << "float";
}

void TTypeDumper::operator()(const Type::TDouble &) const {
  assert(this);
  Strm << "double";
}

void TTypeDumper::operator()(const Type::TDuration &) const {
  assert(this);
  Strm << "duration";
}

void TTypeDumper::operator()(const Type::TTimePoint &) const {
  assert(this);
  Strm << "time_point";
}

void TTypeDumper::operator()(const Type::TUuid &) const {
  assert(this);
  Strm << "uuid";
}

void TTypeDumper::operator()(const Type::TBlob &) const {
  assert(this);
  Strm << "blob";
}

void TTypeDumper::operator()(const Type::TStr &) const {
  assert(this);
  Strm << "str";
}

void TTypeDumper::operator()(const Type::TTombstone &) const {
  assert(this);
  Strm << "tombstone";
}

void TTypeDumper::operator()(const Type::TVoid &) const {
  assert(this);
  Strm << "void";
}

void TTypeDumper::operator()(const Type::TDesc &type) const {
  assert(this);
  OnUnaryType("desc", type);
}

void TTypeDumper::operator()(const Type::TFree &type) const {
  assert(this);
  OnUnaryType("free", type);
}

void TTypeDumper::operator()(const Type::TOpt &type) const {
  assert(this);
  OnUnaryType("opt", type);
}

void TTypeDumper::operator()(const Type::TSet &type) const {
  assert(this);
  OnUnaryType("set", type);
}

void TTypeDumper::operator()(const Type::TVector &type) const {
  assert(this);
  OnUnaryType("vector", type);
}

void TTypeDumper::operator()(const Type::TMap &type) const {
  assert(this);
  OnBinaryType("map", type);
}

void TTypeDumper::operator()(const Type::TRecord &type) const {
  assert(this);
  assert(&type);
  Strm << "record(";
  size_t elem_count = type.GetElemCount();
  void *pin_alloc = alloca(Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TRecord::TPin::TWrapper pin(type.Pin(pin_alloc));
  string field_name;
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    if (elem_idx) {
      Strm << ", ";
    }
    Type::TAny::TWrapper field(pin->NewElem(elem_idx, field_name, type_alloc));
    Strm << field_name << ": ";
    field->Accept(*this);
  }
  Strm << ')';
}

void TTypeDumper::operator()(const Type::TTuple &type) const {
  assert(this);
  assert(&type);
  Strm << "tuple(";
  size_t elem_count = type.GetElemCount();
  void *pin_alloc = alloca(Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TTuple::TPin::TWrapper pin(type.Pin(pin_alloc));
  for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
    if (elem_idx) {
      Strm << ", ";
    }
    Type::TAny::TWrapper(pin->NewElem(elem_idx, type_alloc))->Accept(*this);
  }
  Strm << ')';
}

void TTypeDumper::OnBinaryType(const char *name, const Type::TBinary &binary) const {
  assert(this);
  assert(&binary);
  void *pin_alloc = alloca(Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TBinary::TPin::TWrapper pin(binary.Pin(pin_alloc));
  Strm << name << '(';
  Type::TAny::TWrapper(pin->NewLhs(type_alloc))->Accept(*this);
  Strm << ", ";
  Type::TAny::TWrapper(pin->NewRhs(type_alloc))->Accept(*this);
  Strm << ')';
}

void TTypeDumper::OnUnaryType(const char *name, const Type::TUnary &unary) const {
  assert(this);
  assert(&unary);
  void *pin_alloc = alloca(Type::GetMaxTypePinSize());
  void *type_alloc = alloca(Type::GetMaxTypeSize());
  Type::TUnary::TPin::TWrapper pin(unary.Pin(pin_alloc));
  Strm << name << '(';
  Type::TAny::TWrapper(pin->NewElem(type_alloc))->Accept(*this);
  Strm << ')';
}
