/* <stig/atom/kit2.test.cc>

   Unit test for <stig/atom/kit2.h>.

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

#include <sstream>
#include <string>
#include <unordered_set>

#include <stig/native/all.h>
#include <stig/native/point.h>
#include <stig/sabot/get_hash.h>
#include <stig/sabot/state_dumper.h>
#include <stig/sabot/type_dumper.h>
#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Atom;

/* An extensible arena that does no interning and enforces no ordering. */
class TTestArena final
    : public TCore::TExtensibleArena {
  public:

  /* Conveniences. */
  using TNote   = TCore::TNote;
  using TOffset = TCore::TOffset;

  /* Do-little. */
  TTestArena() : TCore::TArena(false), TCore::TExtensibleArena(false) {}

  /* Destroys all notes when it goes. */
  ~TTestArena() {
    assert(this);
    for (TNote *note: Notes) {
      delete note;
    }
  }

  /* See base class. */
  virtual TOffset Propose(TNote *proposed_note) override {
    assert(this);
    assert(proposed_note);
    try {
      Notes.insert(proposed_note);
    } catch (...) {
      delete proposed_note;
      throw;
    }
    return reinterpret_cast<TOffset>(proposed_note);
  }

  private:

  /* See base class.  Does nothing. */
  virtual void ReleaseNote(const TNote */*note*/, TOffset /*offset*/, void */*data1*/, void */*data2*/, void */*data3*/) override {}

  /* See base class.  Looks up the requested offset in Notes. */
  virtual const TNote *TryAcquireNote(TOffset offset, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) override {
    assert(this);
    auto note = reinterpret_cast<TNote *>(offset);
    return (Notes.find(note) != Notes.end()) ? note : nullptr;
  }

  /* See base class.  Looks up the requested offset in Notes. */
  virtual const TNote *TryAcquireNote(TOffset offset, size_t /*known_size*/, void *&/*data1*/, void *&/*data2*/, void *&/*data3*/) override {
    assert(this);
    auto note = reinterpret_cast<TNote *>(offset);
    return (Notes.find(note) != Notes.end()) ? note : nullptr;
  }

  /* Our notes. */
  std::unordered_set<TNote *> Notes;

};  // TTestArena

template <typename TVal>
static string ToString(const TVal &val) {
  TTestArena arena;
  ostringstream strm;
  void *state_alloc_1 = alloca(Sabot::State::GetMaxStateSize());
  void *state_alloc_2 = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper(TCore(&arena, Sabot::State::TAny::TWrapper(Native::State::New(val, state_alloc_1))).NewState(&arena, state_alloc_2))->Accept(Sabot::TStateDumper(strm));
  return strm.str();
}

FIXTURE(Empties) {
  EXPECT_EQ(ToString(Native::TFree<bool>::Free), "free(bool)");
  EXPECT_EQ(ToString(Native::TTombstone::Tombstone), "tombstone");
  // TODO: Test void.
}

FIXTURE(Int8) {
  EXPECT_EQ(ToString<int8_t>(-101), "-101");
  EXPECT_EQ(ToString<int8_t>(0), "0");
  EXPECT_EQ(ToString<int8_t>(101), "101");
}

FIXTURE(Int16) {
  EXPECT_EQ(ToString<int16_t>(-101), "-101");
  EXPECT_EQ(ToString<int16_t>(0), "0");
  EXPECT_EQ(ToString<int16_t>(101), "101");
}

FIXTURE(Int32) {
  EXPECT_EQ(ToString<int32_t>(-101), "-101");
  EXPECT_EQ(ToString<int32_t>(0), "0");
  EXPECT_EQ(ToString<int32_t>(101), "101");
}

FIXTURE(Int64) {
  EXPECT_EQ(ToString<int64_t>(-101), "-101");
  EXPECT_EQ(ToString<int64_t>(0), "0");
  EXPECT_EQ(ToString<int64_t>(101), "101");
}

FIXTURE(UInt8) {
  EXPECT_EQ(ToString<uint8_t>(0), "0");
  EXPECT_EQ(ToString<uint8_t>(101), "101");
}

FIXTURE(UInt16) {
  EXPECT_EQ(ToString<uint16_t>(0), "0");
  EXPECT_EQ(ToString<uint16_t>(101), "101");
}

FIXTURE(UInt32) {
  EXPECT_EQ(ToString<uint32_t>(0), "0");
  EXPECT_EQ(ToString<uint32_t>(101), "101");
}

FIXTURE(UInt64) {
  EXPECT_EQ(ToString<uint64_t>(0), "0");
  EXPECT_EQ(ToString<uint64_t>(101), "101");
}

FIXTURE(Bool) {
  EXPECT_EQ(ToString(true), "true");
  EXPECT_EQ(ToString(false), "false");
}

FIXTURE(Char) {
  EXPECT_EQ(ToString('x'), "'x'");
}

FIXTURE(TFloat) {
  EXPECT_EQ(ToString<float>(-98.6), "-98.6");
  EXPECT_EQ(ToString<float>(0), "0");
  EXPECT_EQ(ToString<float>(98.6), "98.6");
}

FIXTURE(TDouble) {
  EXPECT_EQ(ToString<double>(-98.6), "-98.6");
  EXPECT_EQ(ToString<double>(0), "0");
  EXPECT_EQ(ToString<double>(98.6), "98.6");
}

FIXTURE(TDuration) {
  EXPECT_EQ(ToString(TCore::TStdDuration(0)), "0ms");
  EXPECT_EQ(ToString(TCore::TStdDuration(1234)), "1234ms");
}

FIXTURE(TTimePoint) {
  EXPECT_EQ(ToString(TCore::TStdTimePoint()), "1970:01:01:00:00:00utc");
}

FIXTURE(TUuid) {
  const char *str = "1b4e28ba-2fa1-11d2-883f-b9a761bde3fb";
  EXPECT_EQ(ToString(TCore::TUuid(str)), str);
}

FIXTURE(Blob) {
  uint8_t data[100];
  for (int i = 0; i < 100; ++i) {
    data[i] = 101 + i;
  }
  EXPECT_EQ(ToString(Native::TBlob(data,  3)), "{{ 101, 102, 103 }}");
  EXPECT_EQ(ToString(Native::TBlob(data, 30)), "{{ 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130 }}");
}

FIXTURE(String) {
  EXPECT_EQ(ToString<string>("Mofo the Psychic Gorilla is in your base, shooting your dudes."), "\"Mofo the Psychic Gorilla is in your base, shooting your dudes.\"");
  EXPECT_EQ(ToString<string>("shortness"), "\"shortness\"");
}

FIXTURE(Desc) {
  EXPECT_EQ(ToString(TDesc<int>(101)), "desc(101)");
}

FIXTURE(Opt) {
  EXPECT_EQ(ToString(Base::TOpt<int>(101)), "opt(101)");
  EXPECT_EQ(ToString(Base::TOpt<bool>()), "unknown opt(bool)");
}

FIXTURE(Set) {
  EXPECT_EQ(ToString(set<int>({ 101, 102, 103 })), "set(101, 102, 103)");
  EXPECT_EQ(ToString(set<bool>()), "empty set(bool)");
}

FIXTURE(Vector) {
  EXPECT_EQ(ToString(vector<int>({ 101, 102, 103 })), "vector(101, 102, 103)");
  EXPECT_EQ(ToString(vector<bool>()), "empty vector(bool)");
}

FIXTURE(Map) {
  EXPECT_EQ(ToString(map<int, string>({ { 101, "hello"}, { 102, "doctor"} })), "map(101: \"hello\", 102: \"doctor\")");
  EXPECT_EQ(ToString(map<double, bool>()), "empty map(double, bool)");
}

FIXTURE(Record) {
  EXPECT_EQ(ToString(TPoint(1.5, 2.5)), "record(X: 1.5, Y: 2.5)");
}

FIXTURE(Tuple) {
  EXPECT_EQ((ToString(tuple<bool, int, double>(true, 101, 98.6))), "tuple(true, 101, 98.6)");
  EXPECT_EQ(ToString(tuple<>()), "tuple()");
}

template <typename TVal>
static string ToString() {
  TTestArena arena;
  ostringstream strm;
  void *type_alloc_1 = alloca(Sabot::Type::GetMaxTypeSize());
  void *type_alloc_2 = alloca(Sabot::Type::GetMaxTypeSize());
  Sabot::Type::TAny::TWrapper(TCore(&arena, *Sabot::Type::TAny::TWrapper(Native::Type::For<TVal>::GetType(type_alloc_1))).GetType(&arena, type_alloc_2))->Accept(Sabot::TTypeDumper(strm));
  return strm.str();
}

FIXTURE(Typical) {
  EXPECT_EQ(ToString<int8_t>(), "int8");
  EXPECT_EQ(ToString<int16_t>(), "int16");
  EXPECT_EQ(ToString<int32_t>(), "int32");
  EXPECT_EQ(ToString<int64_t>(), "int64");
  EXPECT_EQ(ToString<uint8_t>(), "uint8");
  EXPECT_EQ(ToString<uint16_t>(), "uint16");
  EXPECT_EQ(ToString<uint32_t>(), "uint32");
  EXPECT_EQ(ToString<uint64_t>(), "uint64");
  EXPECT_EQ(ToString<bool>(), "bool");
  EXPECT_EQ(ToString<char>(), "char");
  EXPECT_EQ(ToString<float>(), "float");
  EXPECT_EQ(ToString<double>(), "double");
  EXPECT_EQ(ToString<TCore::TStdDuration>(), "duration");
  EXPECT_EQ(ToString<TCore::TStdTimePoint>(), "time_point");
  EXPECT_EQ(ToString<TCore::TUuid>(), "uuid");
  EXPECT_EQ(ToString<Native::TBlob>(), "blob");
  EXPECT_EQ(ToString<string>(), "str");
  EXPECT_EQ(ToString<Native::TTombstone>(), "tombstone");
  EXPECT_EQ(ToString<void>(), "void");
  EXPECT_EQ(ToString<Native::TFree<bool>>(), "free(bool)");
}

FIXTURE(PinnedLongStr) {
  const char *expected = "This is too long to store directly, so we'll store it indirectly.";
  const size_t expected_size = strlen(expected);
  TTestArena arena;
  void *state_alloc_1 = alloca(Sabot::State::GetMaxStateSize());
  void *state_alloc_2 = alloca(Sabot::State::GetMaxStateSize());
  Sabot::State::TAny::TWrapper tpl_state(TCore(make_tuple(expected), &arena, state_alloc_1).NewState(&arena, state_alloc_2));
  auto tpl = dynamic_cast<Sabot::State::TTuple *>(tpl_state.get());
  if (EXPECT_TRUE(tpl)) {
    void *state_pin_alloc_1 = alloca(Sabot::State::GetMaxStatePinSize());
    Sabot::State::TTuple::TPin::TWrapper tpl_pin(tpl->Pin(state_pin_alloc_1));
    {
      Sabot::State::TAny::TWrapper str_state(tpl_pin->NewElem(0, state_alloc_1));
      auto str = dynamic_cast<Sabot::State::TStr *>(str_state.get());
      if (EXPECT_TRUE(str)) {
        void *state_pin_alloc_2 = alloca(Sabot::State::GetMaxStatePinSize());
        EXPECT_EQ(str->GetSize(), expected_size);
        Sabot::State::TStr::TPin::TWrapper pin(str->Pin(state_pin_alloc_2));
        EXPECT_EQ(pin->GetSize(), expected_size);
        const char
            *start = pin->GetStart(),
            *limit = pin->GetLimit();
        EXPECT_LE(start, limit);
        EXPECT_FALSE(*limit);
        size_t actual_size = limit - start;
        EXPECT_EQ(actual_size, expected_size);
        EXPECT_FALSE(strcmp(start, expected));
      }
    }
  }
}

FIXTURE(ConcatTuple) {
  TTestArena arena;
  void *state_alloc_1 = alloca(Sabot::State::GetMaxStateSize());
  void *state_alloc_2 = alloca(Sabot::State::GetMaxStateSize());
  const TCore final_tuple(&arena,
                          Sabot::State::TAny::TWrapper(Native::State::New(make_tuple(1, 3.14, true), state_alloc_1)),
                          Sabot::State::TAny::TWrapper(Native::State::New(make_tuple(set<int>{7, 1}, string("Hello")), state_alloc_2)));
  ostringstream strm;
  Sabot::State::TAny::TWrapper(final_tuple.NewState(&arena, state_alloc_1))->Accept(Sabot::TStateDumper(strm));
  EXPECT_EQ(strm.str(), "tuple(1, 3.14, true, set(1, 7), \"Hello\")");
}

FIXTURE(SplitTuple) {
  TTestArena arena;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  const TCore big_tuple(make_tuple(1, 3.14, true, set<int>{7, 1}, string("Hello")), &arena, state_alloc);
  TCore lhs_core, rhs_core;
  if (EXPECT_TRUE(big_tuple.TrySplit(&arena, 3, &arena, lhs_core, &arena, rhs_core))) {
    /* check left */ {
      ostringstream strm;
      Sabot::State::TAny::TWrapper(lhs_core.NewState(&arena, state_alloc))->Accept(Sabot::TStateDumper(strm));
      EXPECT_EQ(strm.str(), "tuple(1, 3.14, true)");
    }
    /* check right */ {
      ostringstream strm;
      Sabot::State::TAny::TWrapper(rhs_core.NewState(&arena, state_alloc))->Accept(Sabot::TStateDumper(strm));
      EXPECT_EQ(strm.str(), "tuple(set(1, 7), \"Hello\")");
    }
  }
}

FIXTURE(StoredHash) {
  TTestArena arena;
  void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
  TCore big_tuple(make_tuple(1, 3.14, true, set<int>{7, 1}, string("Hello")), &arena, state_alloc);
  size_t out_hash = 0UL;
  size_t stored_hash = Sabot::GetHash(*Sabot::State::TAny::TWrapper(big_tuple.NewState(&arena, state_alloc)));
  EXPECT_FALSE(big_tuple.TryGetStoredHash(out_hash));
  EXPECT_TRUE(big_tuple.TrySetStoredHash(stored_hash));
  EXPECT_TRUE(big_tuple.TryGetStoredHash(out_hash));
  EXPECT_EQ(stored_hash, out_hash);
  TCore direct_int64(64L, &arena, state_alloc);
  EXPECT_FALSE(direct_int64.TryGetStoredHash(out_hash));
  EXPECT_FALSE(direct_int64.TrySetStoredHash(stored_hash));
}