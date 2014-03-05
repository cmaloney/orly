/* <stig/atom/kit2.cc>

   Implements <stig/atom/kit2.h>.

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

#include <stig/atom/kit2.h>

#include <stig/sabot/order_states.h>

#include <memory>

using namespace std;
using namespace Stig::Atom;

/*
 *    TCore's public members.
 */

TCore::TBadType::TBadType()
    : runtime_error("core or note is of incorrect type") {}

TCore::TCorrupt::TCorrupt()
    : runtime_error("core or note is corrupt") {}

bool TCore::TOrderedArenaCompare::operator()(const TCore &lhs, const TCore &rhs) const {
  const TOffset *lhs_offset = lhs.TryGetOffset();
  const TOffset *rhs_offset = rhs.TryGetOffset();
  if (lhs_offset && rhs_offset) { /* both are indirections */
    return *lhs_offset < *rhs_offset;
  } else if (!lhs_offset && !rhs_offset) { /* neither is an indirection */
    switch (lhs.Tycon) {
      case TTycon::Int8: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<int8_t>() < rhs.ForceAs<int8_t>();
      }
      case TTycon::Int16: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<int16_t>() < rhs.ForceAs<int16_t>();
      }
      case TTycon::Int32: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<int32_t>() < rhs.ForceAs<int32_t>();
      }
      case TTycon::Int64: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<int64_t>() < rhs.ForceAs<int64_t>();
      }
      case TTycon::UInt8: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<uint8_t>() < rhs.ForceAs<uint8_t>();
      }
      case TTycon::UInt16: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<uint16_t>() < rhs.ForceAs<uint16_t>();
      }
      case TTycon::UInt32: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<uint32_t>() < rhs.ForceAs<uint32_t>();
      }
      case TTycon::UInt64: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<uint64_t>() < rhs.ForceAs<uint64_t>();
      }
      case TTycon::Bool: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<bool>() < rhs.ForceAs<bool>();
      }
      case TTycon::Char: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<char>() < rhs.ForceAs<char>();
      }
      case TTycon::Float: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<float>() < rhs.ForceAs<float>();
      }
      case TTycon::Double: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<double>() < rhs.ForceAs<double>();
      }
      case TTycon::Duration: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<TStdDuration>().count() < rhs.ForceAs<TStdDuration>().count();
      }
      case TTycon::TimePoint: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<TStdTimePoint>().time_since_epoch().count() < rhs.ForceAs<TStdTimePoint>().time_since_epoch().count();
      }
      case TTycon::Uuid: {
        assert(lhs.Tycon == rhs.Tycon);
        return lhs.ForceAs<Base::TUuid>() < rhs.ForceAs<Base::TUuid>();
      }
      default: {
        assert(lhs.Tycon != TTycon::Str);
        assert(lhs.Tycon != TTycon::Blob);
        assert(rhs.Tycon != TTycon::Str);
        assert(rhs.Tycon != TTycon::Blob);
        if (lhs.Tycon >= TTycon::MinDirectStr && lhs.Tycon <= TTycon::MaxDirectStr) { /* they should both be direct strings */
          assert(rhs.Tycon >= TTycon::MinDirectStr && rhs.Tycon <= TTycon::MaxDirectStr);
          const char *lhs_start, *lhs_limit,
                     *rhs_start, *rhs_limit;
          lhs_start = lhs.DirectStr;
          lhs_limit = lhs_start + MaxDirectSize - static_cast<TTyconNumeric>(lhs.Tycon);
          rhs_start = rhs.DirectStr;
          rhs_limit = rhs_start + MaxDirectSize - static_cast<TTyconNumeric>(rhs.Tycon);
          return Atom::IsLt(QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start)));
        }
        if (lhs.Tycon >= TTycon::MinDirectBlob && lhs.Tycon <= TTycon::MaxDirectBlob) { /* they should both be direct blobs */
          assert(rhs.Tycon >= TTycon::MinDirectBlob && rhs.Tycon <= TTycon::MaxDirectBlob);
          const uint8_t *lhs_start, *lhs_limit,
                        *rhs_start, *rhs_limit;
          lhs_start = lhs.DirectBlob;
          lhs_limit = lhs_start + static_cast<TTyconNumeric>(TTycon::MaxDirectBlob) - static_cast<TTyconNumeric>(lhs.Tycon);
          rhs_start = rhs.DirectBlob;
          rhs_limit = rhs_start + static_cast<TTyconNumeric>(TTycon::MaxDirectBlob) - static_cast<TTyconNumeric>(rhs.Tycon);
          return Atom::IsLt(QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start)));
        }
        throw TCorrupt();
      }
    }
  } else if (lhs_offset) { /* left is an indirection */
    if (rhs.Tycon >= TTycon::MinDirectStr && rhs.Tycon <= TTycon::MaxDirectStr) { /* they should both be strings */
      assert(lhs.Tycon == TTycon::Str);
      const char *lhs_start, *lhs_limit,
                 *rhs_start, *rhs_limit;
      void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin));
      assert(Arena);
      TArena::TFinalPin::TWrapper lhs_pin(Arena->Pin(lhs.IndirectScalarArray.Offset,
                                                     sizeof(Atom::TCore::TNote) + lhs.IndirectScalarArray.Size,
                                                     lhs_pin_alloc));
      lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
      rhs_start = rhs.DirectStr;
      rhs_limit = rhs_start + MaxDirectSize - static_cast<TTyconNumeric>(rhs.Tycon);
      return Atom::IsLt(QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start)));
    } else if (rhs.Tycon >= TTycon::MinDirectBlob && rhs.Tycon <= TTycon::MaxDirectBlob) { /* they should both be blobs */
      assert(lhs.Tycon == TTycon::Blob);
      const uint8_t *lhs_start, *lhs_limit,
                    *rhs_start, *rhs_limit;
      void *lhs_pin_alloc = alloca(sizeof(TArena::TFinalPin));
      assert(Arena);
      TArena::TFinalPin::TWrapper lhs_pin(Arena->Pin(lhs.IndirectScalarArray.Offset,
                                                     sizeof(Atom::TCore::TNote) + lhs.IndirectScalarArray.Size,
                                                     lhs_pin_alloc));
      lhs_pin->GetNote()->Get(lhs_start, lhs_limit);
      rhs_start = rhs.DirectBlob;
      rhs_limit = rhs_start + static_cast<TTyconNumeric>(TTycon::MaxDirectBlob) - static_cast<TTyconNumeric>(rhs.Tycon);
      return Atom::IsLt(QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start)));
    }
    throw TCorrupt();
  } else { /* right is an indirection */
    if (lhs.Tycon >= TTycon::MinDirectStr && lhs.Tycon <= TTycon::MaxDirectStr) { /* they should both be strings */
      assert(rhs.Tycon == TTycon::Str);
      const char *lhs_start, *lhs_limit,
                 *rhs_start, *rhs_limit;
      void *rhs_pin_alloc = alloca(sizeof(TArena::TFinalPin));
      assert(Arena);
      TArena::TFinalPin::TWrapper rhs_pin(Arena->Pin(rhs.IndirectScalarArray.Offset,
                                                     sizeof(Atom::TCore::TNote) + rhs.IndirectScalarArray.Size,
                                                     rhs_pin_alloc));
      rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
      lhs_start = lhs.DirectStr;
      lhs_limit = lhs_start + MaxDirectSize - static_cast<TTyconNumeric>(lhs.Tycon);
      return Atom::IsLt(QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start)));
    } else if (lhs.Tycon >= TTycon::MinDirectBlob && lhs.Tycon <= TTycon::MaxDirectBlob) { /* they should both be blobs */
      assert(rhs.Tycon == TTycon::Blob);
      const uint8_t *lhs_start, *lhs_limit,
                    *rhs_start, *rhs_limit;
      void *rhs_pin_alloc = alloca(sizeof(TArena::TFinalPin));
      assert(Arena);
      TArena::TFinalPin::TWrapper rhs_pin(Arena->Pin(rhs.IndirectScalarArray.Offset,
                                                     sizeof(Atom::TCore::TNote) + rhs.IndirectScalarArray.Size,
                                                     rhs_pin_alloc));
      rhs_pin->GetNote()->Get(rhs_start, rhs_limit);
      lhs_start = lhs.DirectBlob;
      lhs_limit = lhs_start + static_cast<TTyconNumeric>(TTycon::MaxDirectBlob) - static_cast<TTyconNumeric>(lhs.Tycon);
      return Atom::IsLt(QuickCompareMem(lhs_start, rhs_start, (lhs_limit - lhs_start), (rhs_limit - rhs_start)));
    }
    throw TCorrupt();
  }
}

TCore::TCore() {
  Init(TTycon::Void);
}

TCore::TCore(TOffset offset, const TNote *note) {
  assert(note);
  Init(note->Tycon);
  switch (note->Tycon) {
    case TTycon::Blob:
    case TTycon::Str: {
      IndirectScalarArray.Offset = offset;
      IndirectScalarArray.Size = note->GetSize();
      break;
    }
    case TTycon::Desc:
    case TTycon::Free:
    case TTycon::Opt:
    case TTycon::Set:
    case TTycon::Vector:
    case TTycon::Map:
    case TTycon::Record:
    case TTycon::Tuple: {
      IndirectCoreArray.Offset = offset;
      IndirectCoreArray.ElemCount = note->GetSize();
      IndirectCoreArray.IsExemplar = note->IsExemplar();
      IndirectCoreArray.UsesFullNote = true;
      IndirectCoreArray.StoresHash = false;
      IndirectCoreArray.HashVal = 0UL;
      break;
    }
    default: {
      throw TCorrupt();
    }
  }
}

TCore::TCore(TExtensibleArena *arena, const Type::TAny &type) {
  class visitor_t final : public Stig::Sabot::TTypeVisitor {
    public:
    visitor_t(TExtensibleArena *arena, TCore *core) : Arena(arena), Core(core) {}
    virtual void operator()(const Type::TInt8      &    ) const override { Core->InitInt8     (); }
    virtual void operator()(const Type::TInt16     &    ) const override { Core->InitInt16    (); }
    virtual void operator()(const Type::TInt32     &    ) const override { Core->InitInt32    (); }
    virtual void operator()(const Type::TInt64     &    ) const override { Core->InitInt64    (); }
    virtual void operator()(const Type::TUInt8     &    ) const override { Core->InitUInt8    (); }
    virtual void operator()(const Type::TUInt16    &    ) const override { Core->InitUInt16   (); }
    virtual void operator()(const Type::TUInt32    &    ) const override { Core->InitUInt32   (); }
    virtual void operator()(const Type::TUInt64    &    ) const override { Core->InitUInt64   (); }
    virtual void operator()(const Type::TBool      &    ) const override { Core->InitBool     (); }
    virtual void operator()(const Type::TChar      &    ) const override { Core->InitChar     (); }
    virtual void operator()(const Type::TFloat     &    ) const override { Core->InitFloat    (); }
    virtual void operator()(const Type::TDouble    &    ) const override { Core->InitDouble   (); }
    virtual void operator()(const Type::TDuration  &    ) const override { Core->InitDuration (); }
    virtual void operator()(const Type::TTimePoint &    ) const override { Core->InitTimePoint(); }
    virtual void operator()(const Type::TUuid      &    ) const override { Core->InitUuid     (); }
    virtual void operator()(const Type::TBlob      &    ) const override { Core->InitBlob(Arena, nullptr, nullptr); }
    virtual void operator()(const Type::TStr       &    ) const override { Core->InitStr (Arena, nullptr, nullptr); }
    virtual void operator()(const Type::TTombstone &    ) const override { Core->Init(TTycon::Tombstone); }
    virtual void operator()(const Type::TVoid      &    ) const override { Core->Init(TTycon::Void); }
    virtual void operator()(const Type::TFree      &type) const override { Core->InitIndirectCoreArray(TTycon::Free,   Arena, type, true ); }
    virtual void operator()(const Type::TDesc      &type) const override { Core->InitIndirectCoreArray(TTycon::Desc,   Arena, type, false); }
    virtual void operator()(const Type::TOpt       &type) const override { Core->InitIndirectCoreArray(TTycon::Opt,    Arena, type, true ); }
    virtual void operator()(const Type::TSet       &type) const override { Core->InitIndirectCoreArray(TTycon::Set,    Arena, type, true ); }
    virtual void operator()(const Type::TVector    &type) const override { Core->InitIndirectCoreArray(TTycon::Vector, Arena, type, true ); }
    virtual void operator()(const Type::TMap       &type) const override { Core->InitIndirectCoreArray(TTycon::Map,    Arena, type, true ); }
    virtual void operator()(const Type::TRecord    &type) const override { Core->InitIndirectCoreArray(Arena, type, false); }
    virtual void operator()(const Type::TTuple     &type) const override { Core->InitIndirectCoreArray(Arena, type, false); }
    private:
    TExtensibleArena *Arena;
    TCore *Core;
  };
  assert(&type);
  type.Accept(visitor_t(arena, this));
}

TCore::TCore(TExtensibleArena *arena, const State::TAny *state) {
  class visitor_t final : public Stig::Sabot::TStateVisitor {
    public:
    visitor_t(TExtensibleArena *arena, TCore *core) : Arena(arena), Core(core) {}
    virtual void operator()(const State::TInt8      &state) const override { Core->InitInt8     (state.Get()); }
    virtual void operator()(const State::TInt16     &state) const override { Core->InitInt16    (state.Get()); }
    virtual void operator()(const State::TInt32     &state) const override { Core->InitInt32    (state.Get()); }
    virtual void operator()(const State::TInt64     &state) const override { Core->InitInt64    (state.Get()); }
    virtual void operator()(const State::TUInt8     &state) const override { Core->InitUInt8    (state.Get()); }
    virtual void operator()(const State::TUInt16    &state) const override { Core->InitUInt16   (state.Get()); }
    virtual void operator()(const State::TUInt32    &state) const override { Core->InitUInt32   (state.Get()); }
    virtual void operator()(const State::TUInt64    &state) const override { Core->InitUInt64   (state.Get()); }
    virtual void operator()(const State::TBool      &state) const override { Core->InitBool     (state.Get()); }
    virtual void operator()(const State::TChar      &state) const override { Core->InitChar     (state.Get()); }
    virtual void operator()(const State::TFloat     &state) const override { Core->InitFloat    (state.Get()); }
    virtual void operator()(const State::TDouble    &state) const override { Core->InitDouble   (state.Get()); }
    virtual void operator()(const State::TDuration  &state) const override { Core->InitDuration (state.Get()); }
    virtual void operator()(const State::TTimePoint &state) const override { Core->InitTimePoint(state.Get()); }
    virtual void operator()(const State::TUuid      &state) const override { Core->InitUuid     (state.Get()); }
    virtual void operator()(const State::TBlob      &state) const override {
      void *pin_alloc = alloca(State::GetMaxStatePinSize());
      State::TBlob::TPin::TWrapper pin(state.Pin(pin_alloc));
      Core->InitBlob(Arena, pin->GetStart(), pin->GetLimit());
    }
    virtual void operator()(const State::TStr       &state) const override {
      void *pin_alloc = alloca(State::GetMaxStatePinSize());
      State::TStr::TPin::TWrapper pin(state.Pin(pin_alloc));
      Core->InitStr (Arena, pin->GetStart(), pin->GetLimit());
    }
    virtual void operator()(const State::TTombstone &     ) const override { Core->Init(TTycon::Tombstone); }
    virtual void operator()(const State::TVoid      &     ) const override { Core->Init(TTycon::Void); }
    virtual void operator()(const State::TFree      &state) const override {
      void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      Core->InitIndirectCoreArray(TTycon::Free, Arena, *Sabot::Type::TUnary::TWrapper(state.GetFreeType(type_alloc)), true );
    }
    virtual void operator()(const State::TDesc      &state) const override {
      void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      Core->InitIndirectCoreArray(TTycon::Desc, Arena, *Sabot::Type::TUnary::TWrapper(state.GetDescType(type_alloc)), state);
    }
    virtual void operator()(const State::TOpt       &state) const override {
      void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      Core->InitIndirectCoreArray(TTycon::Opt, Arena, *Sabot::Type::TUnary::TWrapper(state.GetOptType(type_alloc)), state);
    }
    virtual void operator()(const State::TSet       &state) const override {
      void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      Core->InitIndirectCoreArray(TTycon::Set, Arena, *Sabot::Type::TUnary::TWrapper(state.GetSetType(type_alloc)), state);
    }
    virtual void operator()(const State::TVector    &state) const override {
      void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      Core->InitIndirectCoreArray(TTycon::Vector, Arena, *Sabot::Type::TUnary::TWrapper(state.GetVectorType(type_alloc)), state);
    }
    virtual void operator()(const State::TMap       &state) const override {
      void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      Core->InitIndirectCoreArray(TTycon::Map, Arena, *Sabot::Type::TBinary::TWrapper(state.GetMapType(type_alloc)), state);
    }
    virtual void operator()(const State::TRecord &state) const override {
      void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      const Sabot::Type::TRecord::TWrapper type(state.GetRecordType(type_alloc));

      //const auto &type = state.GetRecordType();

      size_t elem_count = state.GetElemCount();
      if (elem_count) {
        void *pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TArrayOfSingleStates::TPin::TWrapper pin(state.Pin(pin_alloc));
        TInit2 init2 =
            [this, &type, &pin](size_t elem_idx, void *lhs, void *rhs) {
              void *type_pin_alloc = alloca(Type::GetMaxTypePinSize());
              Type::TRecord::TPin::TWrapper type_pin(type->Pin(type_pin_alloc));
              void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
              string field_name;
              const Type::TAny::TWrapper elem(type_pin->NewElem(elem_idx, field_name, type_alloc));

              new (lhs) TCore(Arena, field_name.c_str());
              void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
              new (rhs) TCore(Arena, State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc)));
            };
        Core->InitIndirectCoreArray(TTycon::Record, Arena->Propose(TNote::New(TTycon::Record, elem_count, false, init2)), elem_count, false);
      } else {
        Core->InitIndirectCoreArray(Arena, *type, false);
      }
    }
    virtual void operator()(const State::TTuple &state) const override {
      void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      if (!Core->TryInitIndirectCoreArray(TTycon::Tuple, Arena, state)) {
        Core->InitIndirectCoreArray(Arena, *Sabot::Type::TTuple::TWrapper(state.GetTupleType(type_alloc)), false);
      }
    }
    private:
    TExtensibleArena *Arena;
    TCore *Core;
  };
  assert(state);
  state->Accept(visitor_t(arena, this));
}

TCore::TCore(TExtensibleArena *arena, const State::TAny *lhs_state, const State::TAny *rhs_state) {
  assert(lhs_state);
  assert(rhs_state);
  const State::TTuple *lhs_tuple = dynamic_cast<const State::TTuple *>(lhs_state);
  const State::TTuple *rhs_tuple = dynamic_cast<const State::TTuple *>(rhs_state);
  assert(lhs_tuple);
  assert(rhs_tuple);
  if (!TryInitIndirectCoreArray(TTycon::Tuple, arena, *lhs_tuple, *rhs_tuple)) {
    assert(false);
    throw;
  }
}

TCore::Type::TAny *TCore::GetType(TArena *arena, void *type_alloc) const {
  assert(this);
  Type::TAny *result;
  switch (Tycon) {
    case TTycon::Int8     : { result = new (type_alloc) Sabot::Type::TInt8     ();            break; }
    case TTycon::Int16    : { result = new (type_alloc) Sabot::Type::TInt16    ();            break; }
    case TTycon::Int32    : { result = new (type_alloc) Sabot::Type::TInt32    ();            break; }
    case TTycon::Int64    : { result = new (type_alloc) Sabot::Type::TInt64    ();            break; }
    case TTycon::UInt8    : { result = new (type_alloc) Sabot::Type::TUInt8    ();            break; }
    case TTycon::UInt16   : { result = new (type_alloc) Sabot::Type::TUInt16   ();            break; }
    case TTycon::UInt32   : { result = new (type_alloc) Sabot::Type::TUInt32   ();            break; }
    case TTycon::UInt64   : { result = new (type_alloc) Sabot::Type::TUInt64   ();            break; }
    case TTycon::Bool     : { result = new (type_alloc) Sabot::Type::TBool     ();            break; }
    case TTycon::Char     : { result = new (type_alloc) Sabot::Type::TChar     ();            break; }
    case TTycon::Float    : { result = new (type_alloc) Sabot::Type::TFloat    ();            break; }
    case TTycon::Double   : { result = new (type_alloc) Sabot::Type::TDouble   ();            break; }
    case TTycon::Duration : { result = new (type_alloc) Sabot::Type::TDuration ();            break; }
    case TTycon::TimePoint: { result = new (type_alloc) Sabot::Type::TTimePoint();            break; }
    case TTycon::Uuid     : { result = new (type_alloc) Sabot::Type::TUuid     ();            break; }
    case TTycon::Blob     : { result = new (type_alloc) Sabot::Type::TBlob     ();            break; }
    case TTycon::Str      : { result = new (type_alloc) Sabot::Type::TStr      ();            break; }
    case TTycon::Tombstone: { result = new (type_alloc) Sabot::Type::TTombstone();            break; }
    case TTycon::Void     : { result = new (type_alloc) Sabot::Type::TVoid     ();            break; }
    case TTycon::Desc     : { result = new (type_alloc) ST::TDesc              (arena, this); break; }
    case TTycon::Free     : { result = new (type_alloc) ST::TFree              (arena, this); break; }
    case TTycon::Opt      : { result = new (type_alloc) ST::TOpt               (arena, this); break; }
    case TTycon::Set      : { result = new (type_alloc) ST::TSet               (arena, this); break; }
    case TTycon::Vector   : { result = new (type_alloc) ST::TVector            (arena, this); break; }
    case TTycon::Map      : { result = new (type_alloc) ST::TMap               (arena, this); break; }
    case TTycon::Record: {
      #ifndef NDEBUG
      void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
      TArena::TFinalPin::TWrapper pin(arena->Pin(IndirectCoreArray.Offset,
                                                 sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * 2 * IndirectCoreArray.ElemCount),
                                                 pin_alloc));
      const TNote::TPairOfCores *start, *limit;
      pin->GetNote()->Get(start, limit);
      size_t elem_count = limit - start;
      if (elem_count < IndirectCoreArray.ElemCount) {
        throw TCorrupt();
      }
      #endif
      result = new (type_alloc) ST::TRecord(arena, this);
      break;
    }
    case TTycon::Tuple: {
      #ifndef NDEBUG
      void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
      TArena::TFinalPin::TWrapper pin(arena->Pin(IndirectCoreArray.Offset,
                                                 sizeof(Atom::TCore::TNote) + (sizeof(Atom::TCore) * IndirectCoreArray.ElemCount),
                                                 pin_alloc));
      const TCore *start, *limit;
      pin->GetNote()->Get(start, limit);
      size_t elem_count = limit - start;
      if (elem_count < IndirectCoreArray.ElemCount) {
        throw TCorrupt();
      }
      #endif
      result = new (type_alloc) ST::TTuple(arena, this);
      break;
    }
    default: {
      if (Tycon >= TTycon::MinDirectBlob && Tycon <= TTycon::MaxDirectBlob) {
        result = new (type_alloc) Sabot::Type::TBlob();
      } else if (Tycon >= TTycon::MinDirectStr && Tycon <= TTycon::MaxDirectStr) {
        result = new (type_alloc) Sabot::Type::TStr();
      } else {
        throw TCorrupt();
      }
    }
  }
  return result;
}

TCore::State::TAny *TCore::NewState(TArena *arena, void *state_alloc) const {
  assert(this);
  State::TAny *result;
  switch (Tycon) {
    case TTycon::Int8     : { result = new (state_alloc) SS::TInt8     (this);        break; }
    case TTycon::Int16    : { result = new (state_alloc) SS::TInt16    (this);        break; }
    case TTycon::Int32    : { result = new (state_alloc) SS::TInt32    (this);        break; }
    case TTycon::Int64    : { result = new (state_alloc) SS::TInt64    (this);        break; }
    case TTycon::UInt8    : { result = new (state_alloc) SS::TUInt8    (this);        break; }
    case TTycon::UInt16   : { result = new (state_alloc) SS::TUInt16   (this);        break; }
    case TTycon::UInt32   : { result = new (state_alloc) SS::TUInt32   (this);        break; }
    case TTycon::UInt64   : { result = new (state_alloc) SS::TUInt64   (this);        break; }
    case TTycon::Bool     : { result = new (state_alloc) SS::TBool     (this);        break; }
    case TTycon::Char     : { result = new (state_alloc) SS::TChar     (this);        break; }
    case TTycon::Float    : { result = new (state_alloc) SS::TFloat    (this);        break; }
    case TTycon::Double   : { result = new (state_alloc) SS::TDouble   (this);        break; }
    case TTycon::Duration : { result = new (state_alloc) SS::TDuration (this);        break; }
    case TTycon::TimePoint: { result = new (state_alloc) SS::TTimePoint(this);        break; }
    case TTycon::Uuid     : { result = new (state_alloc) SS::TUuid     (this);        break; }
    case TTycon::Blob     : { result = new (state_alloc) SS::TBlob     (arena, this); break; }
    case TTycon::Str      : { result = new (state_alloc) SS::TStr      (arena, this); break; }
    case TTycon::Tombstone: { result = new (state_alloc) SS::TTombstone();            break; }
    case TTycon::Void     : { result = new (state_alloc) SS::TVoid     ();            break; }
    case TTycon::Free     : { result = new (state_alloc) SS::TFree     (arena, this); break; }
    case TTycon::Desc     : { result = new (state_alloc) SS::TDesc     (arena, this); break; }
    case TTycon::Opt      : { result = new (state_alloc) SS::TOpt      (arena, this); break; }
    case TTycon::Set      : { result = new (state_alloc) SS::TSet      (arena, this); break; }
    case TTycon::Vector   : { result = new (state_alloc) SS::TVector   (arena, this); break; }
    case TTycon::Map      : { result = new (state_alloc) SS::TMap      (arena, this); break; }
    case TTycon::Record   : { result = new (state_alloc) SS::TRecord   (arena, this); break; }
    case TTycon::Tuple    : { result = new (state_alloc) SS::TTuple    (arena, this); break; }
    default: {
      if (Tycon >= TTycon::MinDirectBlob && Tycon <= TTycon::MaxDirectBlob) {
        result = new (state_alloc) SS::TBlob(arena, this);
      } else if (Tycon >= TTycon::MinDirectStr && Tycon <= TTycon::MaxDirectStr) {
        result = new (state_alloc) SS::TStr(arena, this);
      } else {
        throw TCorrupt();
      }
    }
  }
  return result;
}

void TCore::Remap(const TRemap &remap) {
  assert(this);
  switch (Tycon) {
    case TTycon::Int8:
    case TTycon::Int16:
    case TTycon::Int32:
    case TTycon::Int64:
    case TTycon::UInt8:
    case TTycon::UInt16:
    case TTycon::UInt32:
    case TTycon::UInt64:
    case TTycon::Bool:
    case TTycon::Char:
    case TTycon::Float:
    case TTycon::Double:
    case TTycon::Duration:
    case TTycon::TimePoint:
    case TTycon::Uuid:
    case TTycon::Tombstone:
    case TTycon::Void: {
      break;
    }
    case TTycon::Blob:
    case TTycon::Str: {
      IndirectScalarArray.Offset = remap(IndirectScalarArray.Offset);
      break;
    }
    case TTycon::Free:
    case TTycon::Desc:
    case TTycon::Opt:
    case TTycon::Set:
    case TTycon::Vector:
    case TTycon::Map:
    case TTycon::Record:
    case TTycon::Tuple: {
      IndirectCoreArray.Offset = remap(IndirectCoreArray.Offset);
      break;
    }
    default: {
      if (!(Tycon >= TTycon::MinDirectBlob && Tycon <= TTycon::MaxDirectBlob) &&
          !(Tycon >= TTycon::MinDirectStr  && Tycon <= TTycon::MaxDirectStr )) {
        throw TCorrupt();
      }
    }
  }
}

const TCore::TOffset *TCore::TryGetOffset() const {
  assert(this);
  switch (Tycon) {
    case TTycon::Int8:
    case TTycon::Int16:
    case TTycon::Int32:
    case TTycon::Int64:
    case TTycon::UInt8:
    case TTycon::UInt16:
    case TTycon::UInt32:
    case TTycon::UInt64:
    case TTycon::Bool:
    case TTycon::Char:
    case TTycon::Float:
    case TTycon::Double:
    case TTycon::Duration:
    case TTycon::TimePoint:
    case TTycon::Uuid:
    case TTycon::Tombstone:
    case TTycon::Void: {
      break;
    }
    case TTycon::Blob:
    case TTycon::Str: {
      return &IndirectScalarArray.Offset;
      break;
    }
    case TTycon::Free:
    case TTycon::Desc:
    case TTycon::Opt:
    case TTycon::Set:
    case TTycon::Vector:
    case TTycon::Map:
    case TTycon::Record:
    case TTycon::Tuple: {
      return &IndirectCoreArray.Offset;
      break;
    }
    default: {
      if (!(Tycon >= TTycon::MinDirectBlob && Tycon <= TTycon::MaxDirectBlob) &&
          !(Tycon >= TTycon::MinDirectStr  && Tycon <= TTycon::MaxDirectStr )) {
        throw TCorrupt();
      }
    }
  }
  return nullptr;
}

bool TCore::TryTruncateTuple() {
  assert(this);
  assert(Tycon == TTycon::Tuple);
  bool success = (IndirectCoreArray.ElemCount > 1);
  if (success) {
    --(IndirectCoreArray.ElemCount);
    IndirectCoreArray.UsesFullNote = false;
    IndirectCoreArray.StoresHash = false;
    IndirectCoreArray.HashVal = 0UL;
  }
  return success;
}

bool TCore::TrySplit(TArena *arena, size_t lhs_size,
                     TExtensibleArena *lhs_arena, TCore &lhs_core,
                     TExtensibleArena *rhs_arena, TCore &rhs_core) const {
  assert(this);
  assert(arena);
  assert(lhs_arena);
  assert(rhs_arena);
  assert(&lhs_core);
  assert(&rhs_core);
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper my_state(NewState(arena, state_alloc));
  const Sabot::State::TTuple *tuple_state = dynamic_cast<const Sabot::State::TTuple *>(my_state.get());
  assert(tuple_state); /* this must be a tuple to split. */
  size_t elem_count = tuple_state->GetElemCount();
  bool success;
  if ((success = elem_count > lhs_size)) {
    assert(elem_count > lhs_size); /* both sides of the split should have something in them. */
    if (!lhs_core.TryInitIndirectCoreArray(TTycon::Tuple, lhs_arena, *tuple_state, 0, lhs_size)) {
      assert(false);
      throw;
    }
    if (!rhs_core.TryInitIndirectCoreArray(TTycon::Tuple, rhs_arena, *tuple_state, lhs_size, elem_count)) {
      assert(false);
      throw;
    }
  }
  return success;
}

/*
 *    TCore's private members.
 */

TCore::TCore(TExtensibleArena *arena, const char *c_str) {
  InitStr(arena, c_str, c_str + strlen(c_str));
}

void TCore::CopyOut(TArena *arena, string &out) const {
  assert(this);
  assert(&out);
  if (Tycon == TTycon::Str) {
    assert(arena);
    void *pin_alloc = alloca(sizeof(TArena::TFinalPin));
    TArena::TFinalPin::TWrapper pin(arena->Pin(IndirectScalarArray.Offset,
                                               sizeof(Atom::TCore::TNote) + IndirectScalarArray.Size,
                                               pin_alloc));
    const char *start, *limit;
    pin->GetNote()->Get(start, limit);
    out.assign(start, limit);
  } else if (Tycon >= TTycon::MinDirectStr && Tycon <= TTycon::MaxDirectStr) {
    out = DirectStr;
  } else {
    /* We throw 'corrupt' here instead of 'bad type' because this function is only used when accessing the
       elements of a record.  If we expect a string (as an element name) and get something else, that's
       a corruption. */
    throw TCorrupt();
  }
}

void TCore::Init(TTycon tycon) {
  assert(this);
  memset(DirectBlob, 0, MaxDirectSize);
  Tycon = tycon;
}

void TCore::InitBlob(TExtensibleArena *arena, const uint8_t *start, const uint8_t *limit) {
  assert(this);
  assert(start <= limit);
  size_t size = limit - start;
  if (size <= MaxDirectSize) {
    TTyconNumeric remaining_size = MaxDirectSize - size;
    memcpy(DirectBlob, start, size);
    memset(DirectBlob + size, 0, remaining_size);
    remaining_size += static_cast<TTyconNumeric>(TTycon::MinDirectBlob);
    Tycon = reinterpret_cast<TTycon &>(remaining_size);
  } else {
    assert(arena);
    InitIndirectScalarArray(TTycon::Blob, arena->Propose(TNote::New(start, limit, false)), size);
  }
}

void TCore::InitStr(TExtensibleArena *arena, const char *start, const char *limit) {
  assert(this);
  assert(start <= limit);
  assert(!limit || !*limit);
  size_t size = limit - start;
  if (size <= MaxDirectSize) {
    TTyconNumeric remaining_size = MaxDirectSize - size;
    memcpy(DirectStr, start, size);
    memset(DirectStr + size, 0, remaining_size);
    remaining_size += static_cast<TTyconNumeric>(TTycon::MinDirectStr);
    Tycon = reinterpret_cast<TTycon &>(remaining_size);
  } else {
    assert(arena);
    InitIndirectScalarArray(TTycon::Str, arena->Propose(TNote::New(start, limit, false)), size);
  }
}

void TCore::InitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const Type::TUnary &type, const State::TArrayOfSingleStates &state) {
  assert(this);
  assert(arena);
  assert(&type);
  assert(&state);
  if (!TryInitIndirectCoreArray(tycon, arena, state)) {
    InitIndirectCoreArray(tycon, arena, type, true);
  }
}

void TCore::InitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const Type::TUnary &type, bool is_exemplar) {
  assert(this);
  assert(arena);
  assert(&type);
  TInit1 init1 =
      [arena, &type](size_t, void *elem) {
        void *pin_alloc = alloca(Type::GetMaxTypePinSize());
        void *type_alloc = alloca(Type::GetMaxTypeSize());
        Type::TUnary::TPin::TWrapper pin(type.Pin(pin_alloc));
        new (elem) TCore(arena, *Sabot::Type::TAny::TWrapper(pin->NewElem(type_alloc)));
      };
  InitIndirectCoreArray(tycon, arena->Propose(TNote::New(tycon, 1, is_exemplar, init1)), 1, is_exemplar);
}

void TCore::InitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const Type::TBinary &type, const State::TArrayOfPairsOfStates &state) {
  assert(this);
  assert(arena);
  assert(&type);
  assert(&state);
  size_t elem_count = state.GetElemCount();
  if (elem_count) {
    void *pin_alloc = alloca(State::GetMaxStatePinSize());
    State::TArrayOfPairsOfStates::TPin::TWrapper pin(state.Pin(pin_alloc));
    TInit2 init2 =
        [arena, &pin](size_t elem_idx, void *lhs, void *rhs) {
          void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
          void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
          new (lhs) TCore(arena, State::TAny::TWrapper(pin->NewLhs(elem_idx, lhs_state_alloc)));
          new (rhs) TCore(arena, State::TAny::TWrapper(pin->NewRhs(elem_idx, rhs_state_alloc)));
        };
    InitIndirectCoreArray(tycon, arena->Propose(TNote::New(tycon, elem_count, false, init2)), elem_count, false);
  } else {
    InitIndirectCoreArray(tycon, arena, type, true);
  }
}

void TCore::InitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const Type::TBinary &type, bool is_exemplar) {
  assert(this);
  assert(arena);
  assert(&type);
  TInit2 init2 =
      [arena, &type](size_t, void *lhs, void *rhs) {
        void *pin_alloc = alloca(Type::GetMaxTypePinSize());
        void *type_alloc = alloca(Type::GetMaxTypeSize());
        Type::TBinary::TPin::TWrapper pin(type.Pin(pin_alloc));
        new (lhs) TCore(arena, *Sabot::Type::TAny::TWrapper(pin->NewLhs(type_alloc)));
        new (rhs) TCore(arena, *Sabot::Type::TAny::TWrapper(pin->NewRhs(type_alloc)));
      };
  InitIndirectCoreArray(tycon, arena->Propose(TNote::New(tycon, 1, is_exemplar, init2)), 1, is_exemplar);
}

void TCore::InitIndirectCoreArray(TExtensibleArena *arena, const Type::TTuple &type, bool is_exemplar) {
  assert(this);
  assert(arena);
  assert(&type);
  TInit1 init1 =
      [arena, &type](size_t elem_idx, void *elem) {
        void *pin_alloc = alloca(Type::GetMaxTypePinSize());
        void *type_alloc = alloca(Type::GetMaxTypeSize());
        Type::TTuple::TPin::TWrapper pin(type.Pin(pin_alloc));
        new (elem) TCore(arena, *Sabot::Type::TAny::TWrapper(pin->NewElem(elem_idx, type_alloc)));
      };
  size_t elem_count = type.GetElemCount();
  InitIndirectCoreArray(TTycon::Tuple, arena->Propose(TNote::New(TTycon::Tuple, elem_count, is_exemplar, init1)), elem_count, is_exemplar);
}

void TCore::InitIndirectCoreArray(TExtensibleArena *arena, const Type::TRecord &type, bool is_exemplar) {
  assert(this);
  assert(arena);
  assert(&type);
  TInit2 init2 =
      [arena, &type](size_t elem_idx, void *lhs, void *rhs) {
        string field_name;
        void *pin_alloc = alloca(Type::GetMaxTypePinSize());
        void *type_alloc = alloca(Type::GetMaxTypeSize());
        Type::TRecord::TPin::TWrapper pin(type.Pin(pin_alloc));
        const Sabot::Type::TAny::TWrapper elem(pin->NewElem(elem_idx, field_name, type_alloc));
        new (lhs) TCore(arena, field_name.c_str());
        new (rhs) TCore(arena, *elem);
      };
  size_t elem_count = type.GetElemCount();
  InitIndirectCoreArray(TTycon::Record, arena->Propose(TNote::New(TTycon::Record, elem_count, is_exemplar, init2)), elem_count, is_exemplar);
}

void TCore::InitIndirectCoreArray(TTycon tycon, TOffset offset, size_t elem_count, bool is_exemplar) {
  assert(this);
  Init(tycon);
  IndirectCoreArray.Offset = offset;
  IndirectCoreArray.ElemCount = elem_count;
  IndirectCoreArray.IsExemplar = is_exemplar;
  IndirectCoreArray.UsesFullNote = true;
  IndirectCoreArray.StoresHash = false;
  IndirectCoreArray.HashVal = 0UL;
}

bool TCore::TryInitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const State::TArrayOfSingleStates &state) {
  assert(this);
  size_t elem_count = state.GetElemCount();
  bool success = (elem_count != 0);
  if (success) {
    void *pin_alloc = alloca(State::GetMaxStatePinSize());
    State::TArrayOfSingleStates::TPin::TWrapper pin(state.Pin(pin_alloc));
    TInit1 init1 =
        [arena, &pin](size_t elem_idx, void *elem) {
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          new (elem) TCore(arena, State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc)));
        };
    InitIndirectCoreArray(tycon, arena->Propose(TNote::New(tycon, elem_count, false, init1)), elem_count, false);
  }
  return success;
}

bool TCore::TryInitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const State::TArrayOfSingleStates &lhs_state,
                                     const State::TArrayOfSingleStates &rhs_state) {
  assert(this);
  size_t lhs_size = lhs_state.GetElemCount();
  size_t elem_count = lhs_size + rhs_state.GetElemCount();
  bool success = (elem_count != 0);
  if (success) {
    void *lhs_pin_alloc = alloca(State::GetMaxStatePinSize() * 2);
    void *rhs_pin_alloc = reinterpret_cast<uint8_t *>(lhs_pin_alloc) + State::GetMaxStatePinSize();
    State::TArrayOfSingleStates::TPin::TWrapper
      lhs_pin(lhs_state.Pin(lhs_pin_alloc)),
      rhs_pin(rhs_state.Pin(rhs_pin_alloc));
    TInit1 init1 =
        [arena, &lhs_pin, &rhs_pin, &lhs_size](size_t elem_idx, void *elem) {
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          new (elem) TCore(arena, State::TAny::TWrapper((elem_idx < lhs_size ?
                                                         lhs_pin->NewElem(elem_idx, state_alloc) :
                                                         rhs_pin->NewElem(elem_idx - lhs_size, state_alloc))));
        };
    InitIndirectCoreArray(tycon, arena->Propose(TNote::New(tycon, elem_count, false, init1)), elem_count, false);
  }
  return success;
}

bool TCore::TryInitIndirectCoreArray(TTycon tycon, TExtensibleArena *arena, const State::TArrayOfSingleStates &state, size_t start, size_t limit) {
  assert(this);
  assert(start < limit);
  size_t elem_count = limit - start;
  bool success = (elem_count != 0);
  if (success) {
    void *pin_alloc = alloca(State::GetMaxStatePinSize());
    State::TArrayOfSingleStates::TPin::TWrapper pin(state.Pin(pin_alloc));
    TInit1 init1 =
        [arena, &pin, start](size_t elem_idx, void *elem) {
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          new (elem) TCore(arena, State::TAny::TWrapper(pin->NewElem(elem_idx + start, state_alloc)));
        };
    InitIndirectCoreArray(tycon, arena->Propose(TNote::New(tycon, elem_count, false, init1)), elem_count, false);
  }
  return success;
}

void TCore::InitIndirectScalarArray(TTycon tycon, TOffset offset, size_t size) {
  assert(this);
  Init(tycon);
  IndirectScalarArray.Offset = offset;
  IndirectScalarArray.Size = size;
}

/*
 *    TCore::TArena stuff.
 */

TCore::TArena::TNotFound::TNotFound()
    : runtime_error("note was not found") {}

TCore::TArena::TPin::~TPin() {
  assert(this);
  //#ifndef NDEBUG
  if (Arena) {
    Arena->ReleaseNote(Note, Offset, Data1, Data2, Data3);
  }
  //#else
  //Arena->ReleaseNote(Note, Offset, Data);
  //#endif
}

TCore::TArena::TPin::TPin(TArena *arena, TOffset offset)
    : Arena(arena), Offset(offset), Data1(nullptr), Data2(nullptr), Data3(nullptr) {
  assert(arena);
  Note = arena->TryAcquireNote(offset, Data1, Data2, Data3);
  #ifndef NDEBUG
  if (!Note) {
    throw TNotFound();
  }
  #endif
}

TCore::TArena::TPin::TPin(TArena *arena, TOffset offset, size_t known_size)
    : Arena(arena), Offset(offset), Data1(nullptr), Data2(nullptr), Data3(nullptr) {
  assert(arena);
  Note = arena->TryAcquireNote(offset, known_size, Data1, Data2, Data3);
  #ifndef NDEBUG
  if (!Note) {
    throw TNotFound();
  }
  #endif
}

TCore::TArena::TFinalPin *TCore::TArena::Pin(TOffset offset, void *alloc) {
  return new (alloc) TFinalPin(this, offset);
}

TCore::TArena::TFinalPin *TCore::TArena::Pin(TOffset offset, size_t known_size, void *alloc) {
  return new (alloc) TFinalPin(this, offset, known_size);
}

/*
 *    TCore::TNote stuff.
 */

bool TCore::TNote::TOrderedArenaCompare::operator()(const TNote &lhs, const TNote &rhs) const {
  assert(lhs.Tycon == rhs.Tycon); /* we only order things of the same type */
  assert(lhs.Tycon != TTycon::Free); /* we shouldn't be storing free in the ordered arena */
  switch (lhs.Tycon) {
    case TTycon::Blob: {
      const uint8_t *lhs_start, *lhs_limit, *rhs_start, *rhs_limit;
      lhs.Get(lhs_start, lhs_limit);
      rhs.Get(rhs_start, rhs_limit);
      const size_t lhs_size = lhs_limit - lhs_start;
      const size_t rhs_size = rhs_limit - rhs_start;
      int comp = memcmp(lhs_start, rhs_start, std::min(lhs_size, rhs_size));
      return (comp != 0) ? (comp < 0) : (lhs_size < rhs_size);
      break;
    }
    case TTycon::Str: {
      const char *lhs_start, *lhs_limit, *rhs_start, *rhs_limit;
      lhs.Get(lhs_start, lhs_limit);
      rhs.Get(rhs_start, rhs_limit);
      const size_t lhs_size = lhs_limit - lhs_start;
      const size_t rhs_size = rhs_limit - rhs_start;
      int comp = memcmp(lhs_start, rhs_start, std::min(lhs_size, rhs_size));
      return (comp != 0) ? (comp < 0) : (lhs_size < rhs_size);
      break;
    }
    case TTycon::Desc: {
      const TCore *lhs_start, *lhs_limit, *rhs_start, *rhs_limit;
      lhs.Get(lhs_start, lhs_limit);
      rhs.Get(rhs_start, rhs_limit);
      assert(lhs_limit - lhs_start == 1);
      assert(rhs_limit - rhs_start == 1);
      return OrderedCoreCompare(*rhs_start, *lhs_start); /* we flipped lhs, rhs because it's desc. We can't negate OrderedCoreCompare because this is < operator */
      break;
    }
    case TTycon::Opt: {
      const TCore *lhs_start, *lhs_limit, *rhs_start, *rhs_limit;
      lhs.Get(lhs_start, lhs_limit);
      rhs.Get(rhs_start, rhs_limit);
      assert(lhs_limit - lhs_start == 1);
      assert(rhs_limit - rhs_start == 1);
      if (!lhs.Exemplar) {
        return !rhs.Exemplar ? OrderedCoreCompare(*lhs_start, *rhs_start) : false;
      }
      return !rhs.Exemplar;
      break;
    }
    case TTycon::Set:
    case TTycon::Vector: {
      if (!lhs.Exemplar) {
        if (!rhs.Exemplar) {
          const TCore *lhs_start, *lhs_limit, *rhs_start, *rhs_limit;
          lhs.Get(lhs_start, lhs_limit);
          rhs.Get(rhs_start, rhs_limit);
          for (const TCore *lhs_core = lhs_start, *rhs_core = rhs_start; lhs_core < lhs_limit && rhs_core < rhs_limit; ++lhs_core, ++rhs_core) {
            if (OrderedCoreCompare(*lhs_core, *rhs_core)) {
              return true;
            } else if (OrderedCoreCompare(*rhs_core, *lhs_core)) {
              return false;
            }
          }
          return (lhs_limit - lhs_start) < (rhs_limit - rhs_start);
        }
        return false;
      }
      return !rhs.Exemplar;
      break;
    }
    case TTycon::Tuple: {
      const TCore *lhs_start, *lhs_limit, *rhs_start, *rhs_limit;
      lhs.Get(lhs_start, lhs_limit);
      rhs.Get(rhs_start, rhs_limit);
      #ifndef NDEBUG
      const size_t lhs_size = lhs_limit - lhs_start;
      const size_t rhs_size = rhs_limit - rhs_start;
      #endif
      assert(!lhs.Exemplar);
      assert(!rhs.Exemplar);
      assert(lhs_size == rhs_size);
      assert(lhs_size > 0); /* we don't support empty tuple */
      for (const TCore *lhs_core = lhs_start, *rhs_core = rhs_start; lhs_core < lhs_limit && rhs_core < rhs_limit; ++lhs_core, ++rhs_core) {
        if (OrderedCoreCompare(*lhs_core, *rhs_core)) {
          return true;
        } else if (OrderedCoreCompare(*rhs_core, *lhs_core)) {
          return false;
        }
      }
      return false;
      break;
    }
    case TTycon::Record: {
      const TPairOfCores *lhs_start, *lhs_limit, *rhs_start, *rhs_limit;
      lhs.Get(lhs_start, lhs_limit);
      rhs.Get(rhs_start, rhs_limit);
      assert(!lhs.Exemplar);
      assert(!rhs.Exemplar);
      #ifndef NDEBUG
      const size_t lhs_size = lhs_limit - lhs_start;
      const size_t rhs_size = rhs_limit - rhs_start;
      #endif
      assert(lhs_size == rhs_size);
      assert(lhs_size > 0); /* we don't support empty record */
      for (const TPairOfCores *lhs_core = lhs_start, *rhs_core = rhs_start; lhs_core < lhs_limit && rhs_core < rhs_limit; ++lhs_core, ++rhs_core) {
        if (OrderedCoreCompare(lhs_core->first, rhs_core->first)) {
          return true;
        } else if (OrderedCoreCompare(rhs_core->first, lhs_core->first)) {
          return false;
        }
        if (OrderedCoreCompare(lhs_core->second, rhs_core->second)) {
          return true;
        } else if (OrderedCoreCompare(rhs_core->second, lhs_core->second)) {
          return false;
        }
      }
      return false;
    }
    case TTycon::Map: {
      const TPairOfCores *lhs_start, *lhs_limit, *rhs_start, *rhs_limit;
      lhs.Get(lhs_start, lhs_limit);
      rhs.Get(rhs_start, rhs_limit);
      if (!lhs.Exemplar) {
        if (!rhs.Exemplar) {
          for (const TPairOfCores *lhs_core = lhs_start, *rhs_core = rhs_start; lhs_core < lhs_limit && rhs_core < rhs_limit; ++lhs_core, ++rhs_core) {
            if (OrderedCoreCompare(lhs_core->first, rhs_core->first)) {
              return true;
            } else if (OrderedCoreCompare(rhs_core->first, lhs_core->first)) {
              return false;
            }
            if (OrderedCoreCompare(lhs_core->second, rhs_core->second)) {
              return true;
            } else if (OrderedCoreCompare(rhs_core->second, lhs_core->second)) {
              return false;
            }
          }
          return (lhs_limit - lhs_start) < (rhs_limit - rhs_start);
        }
        return false;
      }
      return !rhs.Exemplar;
      break;
    }
    default: {
      throw TCorrupt();
    }
  }
}

size_t TCore::TNote::THash::operator()(const TNote *note) const {
  return note ? _Hash_impl::hash(note->GetStart(), note->RawSize) : 0;
}

bool TCore::TNote::TIsEq::operator()(const TNote *lhs, const TNote *rhs) const {
  return
      (!lhs && !rhs) ||
      (lhs && rhs && lhs->RawSize == rhs->RawSize
          && lhs->Tycon == rhs->Tycon
          && lhs->Exemplar == rhs->Exemplar
          && memcmp(lhs->GetStart(), rhs->GetStart(), lhs->RawSize) == 0);
}

void TCore::TNote::SetUnReferenced() {
  assert(this);
  UnReferenced = true;
}

void TCore::TNote::ForOffset(const std::function<void (Atom::TCore::TOffset)> &cb) const {
  assert(this);
  switch (Tycon) {
    case TTycon::Blob:
    case TTycon::Str: {
      break;
    }
    case TTycon::Desc:
    case TTycon::Free:
    case TTycon::Opt:
    case TTycon::Set:
    case TTycon::Vector:
    case TTycon::Tuple: {
      const TCore *csr, *limit;
      Get(csr, limit);
      for (; csr < limit; ++csr) {
        const TOffset *offset = csr->TryGetOffset();
        if (offset) {
          cb(*offset);
        }
      }
      break;
    }
    case TTycon::Map:
    case TTycon::Record: {
      const TPairOfCores *csr, *limit;
      Get(csr, limit);
      for (; csr < limit; ++csr) {
        const TOffset *offset = csr->first.TryGetOffset();
        if (offset) {
          cb(*offset);
        }
        offset = csr->second.TryGetOffset();
        if (offset) {
          cb(*offset);
        }
      }
      break;
    }
    default: {
      throw TCorrupt();
    }
  }
}

void TCore::TNote::Remap(const TRemap &remap) {
  assert(this);
  switch (Tycon) {
    case TTycon::Blob:
    case TTycon::Str: {
      break;
    }
    case TTycon::Desc:
    case TTycon::Free:
    case TTycon::Opt:
    case TTycon::Set:
    case TTycon::Vector:
    case TTycon::Tuple: {
      TCore *csr, *limit;
      Get(csr, limit);
      for (; csr < limit; ++csr) {
        csr->Remap(remap);
      }
      break;
    }
    case TTycon::Map:
    case TTycon::Record: {
      TPairOfCores *csr, *limit;
      Get(csr, limit);
      for (; csr < limit; ++csr) {
        csr->first.Remap(remap);
        csr->second.Remap(remap);
      }
      break;
    }
    default: {
      throw TCorrupt();
    }
  }
}

TCore::TNote *TCore::TNote::New(const uint8_t *start, const uint8_t *limit, bool is_exemplar) {
  assert(start <= limit);
  return NewRawCopy(TTycon::Blob, is_exemplar, false, start, limit - start);
}

TCore::TNote *TCore::TNote::New(const char *start, const char *limit, bool is_exemplar) {
  assert(start <= limit);
  assert(!limit || !*limit);
  return NewRawCopy(TTycon::Str, is_exemplar, false, start, limit - start + 1);
}

TCore::TNote *TCore::TNote::New(const std::string &str, bool is_exemplar) {
  return NewRawCopy(TTycon::Str, is_exemplar, false, str.data(), str.size());
}

TCore::TNote *TCore::TNote::New(TTycon tycon, size_t elem_count, bool is_exemplar, const TInit1 &init1) {
  assert(&init1);
  size_t raw_size = elem_count * sizeof(TCore);
  TNote *note = new (raw_size) TNote(tycon, is_exemplar, false , raw_size);
  try {
    auto *elem = note->GetStart<TCore>();
    for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx, ++elem) {
      init1(elem_idx, elem);
    }
  } catch (...) {
    delete note;
    throw;
  }
  return note;
}

TCore::TNote *TCore::TNote::New(TTycon tycon, size_t elem_count, bool is_exemplar, const TInit2 &init2) {
  assert(&init2);
  size_t raw_size = elem_count * sizeof(TPairOfCores);
  TNote *note = new (raw_size) TNote(tycon, is_exemplar, false, raw_size);
  try {
    auto *elem = note->GetStart<TPairOfCores>();
    for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx, ++elem) {
      init2(elem_idx, &(elem->first), &(elem->second));
    }
  } catch (...) {
    delete note;
    throw;
  }
  return note;
}

TCore::TNote *TCore::TNote::New(const TNote *that) {
  assert(that);
  return NewRawCopy(that->Tycon, that->Exemplar, that->UnReferenced, that->GetStart(), that->RawSize);
}

TCore::TNote::TNote(TTycon tycon, bool is_exemplar, bool is_un_referenced, size_t raw_size)
    : Tycon(tycon), Exemplar(is_exemplar), UnReferenced(is_un_referenced), Pad(0), RawSize(raw_size) {}

void *TCore::TNote::operator new(size_t, size_t extra_size, size_t /*junk*/) {
  auto ptr = malloc(sizeof(TNote) + extra_size);
  if (!ptr) {
    throw bad_alloc();
  }
  #ifndef NDEBUG
  /* for valgrind */
  memset(ptr, 0, sizeof(TNote) + extra_size);
  #endif
  return ptr;
}

void TCore::TNote::operator delete(void *ptr, size_t) {
  free(ptr);
}

TCore::TNote *TCore::TNote::NewRawCopy(TTycon tycon, bool is_exemplar, bool is_un_referenced, const void *raw_data, size_t raw_size) {
  assert(raw_data || !raw_size);
  TNote *note = new (raw_size) TNote(tycon, is_exemplar, is_un_referenced, raw_size);
  memcpy(note->GetStart(), raw_data, raw_size);
  return note;
}
