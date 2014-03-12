/* <strm/bin/round_trip.test.cc>

   Unit test for <strm/bin/in.h> and <strm/bin/out.h>.

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

#include <strm/bin/in.h>
#include <strm/bin/out.h>

#include <test/kit.h>
#include <strm/mem/static_in.h>
#include <strm/mem/static_out.h>

using namespace std;
using namespace chrono;
using namespace Strm;

/* Enumerated types we'll try to round-trip. */
enum TFlavor { Chocolate, Vanilla };
enum class TSyrup : int { HotFudge, Butterscotch };
enum class TTopping : char { Nuts = 'u', Sprinkles = 'k' };

/* Make a time point from a ticks-since-epoch integer. */
static steady_clock::time_point GetTimePoint(
    steady_clock::duration::rep count) {
  return steady_clock::time_point(steady_clock::duration(count));
}

/* Compare elements of a given type. */
template <typename TVal>
struct For final {
  static bool Cmp(const TVal &lhs, const TVal &rhs) {
    return lhs == rhs;
  }
};  // For<TVal>

/* Specialization for comparing static arrays. */
template <typename TElem, size_t Size>
struct For<TElem[Size]> final {
  static bool Cmp(const TElem (&lhs)[Size], const TElem (&rhs)[Size]) {
    bool success = true;
    for (size_t i = 0; success && i < Size; ++i) {
      success = For<TElem>::Cmp(lhs[i], rhs[i]);
    }
    return success;
  }
};  // For<TElem[Size]>

/* Round-trip a value to binary stream from and back again, and return
   true iff. the value comes back intact. */
template <typename TVal>
static bool RoundTrip(const TVal &expected) {
  Mem::TStaticOutDefault cons;
  /* extra */ {
    Bin::TOut strm(&cons);
    strm << expected;
  }
  TVal actual;
  /* extra */ {
    Mem::TStaticIn prod(cons);
    Bin::TIn strm(&prod);
    strm >> actual;
  }
  return For<TVal>::Cmp(actual, expected);
}

/* Round-trip an integer value to binary stream in NBO form and back again.
   Return true iff. the value comes back intact and its streamed form is the
   correct size. */
template <
    typename TVal,
    typename = typename std::enable_if<
        std::is_integral<TVal>::value
    >::type
>
static bool RoundTripNbo(TVal expected) {
  Mem::TStaticOutDefault cons;
  /* extra */ {
    Bin::TOut strm(&cons);
    strm << Bin::Nbo(expected);
  }
  /* Confirm we wrote a number of bytes equal to the physical size of the
     expected value. */
  if (!cons.GetSize() == sizeof(TVal)) {
    return false;
  }
  TVal actual;
  /* extra */ {
    Mem::TStaticIn prod(cons);
    Bin::TIn strm(&prod);
    strm >> Bin::Nbo(&actual);
  }
  return For<TVal>::Cmp(actual, expected);
}

FIXTURE(Typical) {
  EXPECT_TRUE(RoundTrip(true));
  EXPECT_TRUE(RoundTrip(false));
  EXPECT_TRUE(RoundTrip<float>(0.0));
  EXPECT_TRUE(RoundTrip<float>(98.6));
  EXPECT_TRUE(RoundTrip<float>(-98.6));
  EXPECT_TRUE(RoundTrip<double>(0.0));
  EXPECT_TRUE(RoundTrip<double>(98.6));
  EXPECT_TRUE(RoundTrip<double>(-98.6));
  EXPECT_TRUE(RoundTrip<int8_t>(0));
  EXPECT_TRUE(RoundTrip<int8_t>(0x7F));
  EXPECT_TRUE(RoundTrip<int8_t>(-0x80));
  EXPECT_TRUE(RoundTrip<int16_t>(0));
  EXPECT_TRUE(RoundTrip<int16_t>(0x7FFF));
  EXPECT_TRUE(RoundTrip<int16_t>(-0x8000));
  EXPECT_TRUE(RoundTrip<int32_t>(0));
  EXPECT_TRUE(RoundTrip<int32_t>(0x7FFFFFFF));
  EXPECT_TRUE(RoundTrip<int32_t>(-0x80000000));
  EXPECT_TRUE(RoundTrip<int64_t>(0));
  EXPECT_TRUE(RoundTrip<int64_t>(0x7FFFFFFFFFFFFFFF));
  EXPECT_TRUE(RoundTrip<int64_t>(-0x8000000000000000));
  EXPECT_TRUE(RoundTrip<uint8_t>(0));
  EXPECT_TRUE(RoundTrip<uint8_t>(0xFF));
  EXPECT_TRUE(RoundTrip<uint16_t>(0));
  EXPECT_TRUE(RoundTrip<uint16_t>(0xFFFF));
  EXPECT_TRUE(RoundTrip<uint32_t>(0));
  EXPECT_TRUE(RoundTrip<uint32_t>(0xFFFFFFFF));
  EXPECT_TRUE(RoundTrip<uint64_t>(0));
  EXPECT_TRUE(RoundTrip<uint64_t>(0xFFFFFFFFFFFFFFFF));
  EXPECT_TRUE(RoundTrip<string>(""));
  EXPECT_TRUE(RoundTrip<string>("Mofo the Psychic Gorilla"));
  EXPECT_TRUE(RoundTrip(seconds(0)));
  EXPECT_TRUE(RoundTrip(seconds(101)));
  EXPECT_TRUE(RoundTrip(seconds(-101)));
  EXPECT_TRUE(RoundTrip(GetTimePoint(0)));
  EXPECT_TRUE(RoundTrip(GetTimePoint(1390755769110637L)));
  EXPECT_TRUE(RoundTrip(Chocolate));
  EXPECT_TRUE(RoundTrip(Vanilla));
  EXPECT_TRUE(RoundTrip(TSyrup::HotFudge));
  EXPECT_TRUE(RoundTrip(TSyrup::Butterscotch));
  EXPECT_TRUE(RoundTrip(TTopping::Nuts));
  EXPECT_TRUE(RoundTrip(TTopping::Sprinkles));
  EXPECT_TRUE(RoundTrip(make_pair(true, 101)));
  EXPECT_TRUE(RoundTrip(tuple<>()));
  EXPECT_TRUE(RoundTrip(make_tuple(true, 101, 98.6, string("hello"))));
  EXPECT_TRUE(RoundTrip<int[3]>({ 101, 202, 303 }));
  EXPECT_TRUE(RoundTrip<list<int>>({ 101, 202, 303 }));
  EXPECT_TRUE((RoundTrip<map<int, string>>(
      { { 101, "hello" }, { 202, "doctor" }, { 303, "name" } })));
  EXPECT_TRUE(RoundTrip<set<int>>({ 101, 202, 303 }));
  EXPECT_TRUE(RoundTrip<multiset<int>>({ 101, 101, 303 }));
  EXPECT_TRUE((RoundTrip<multimap<int, string>>(
      { { 101, "hello" }, { 101, "doctor" }, { 303, "name" } })));
  EXPECT_TRUE((RoundTrip<unordered_map<int, string>>(
      { { 101, "hello" }, { 202, "doctor" }, { 303, "name" } })));
  EXPECT_TRUE((RoundTrip<unordered_multimap<int, string>>(
      { { 101, "hello" }, { 101, "doctor" }, { 303, "name" } })));
  EXPECT_TRUE(RoundTrip<unordered_set<int>>({ 101, 202, 303 }));
  EXPECT_TRUE(RoundTrip<unordered_multiset<int>>({ 101, 101, 303 }));
  EXPECT_TRUE(RoundTrip<vector<int>>({ 101, 202, 303 }));
}

FIXTURE(Nbo) {
  EXPECT_TRUE(RoundTripNbo<int8_t>(0));
  EXPECT_TRUE(RoundTripNbo<int8_t>(0x7F));
  EXPECT_TRUE(RoundTripNbo<int8_t>(-0x80));
  EXPECT_TRUE(RoundTripNbo<int16_t>(0));
  EXPECT_TRUE(RoundTripNbo<int16_t>(0x7FFF));
  EXPECT_TRUE(RoundTripNbo<int16_t>(-0x8000));
  EXPECT_TRUE(RoundTripNbo<int32_t>(0));
  EXPECT_TRUE(RoundTripNbo<int32_t>(0x7FFFFFFF));
  EXPECT_TRUE(RoundTripNbo<int32_t>(-0x80000000));
  EXPECT_TRUE(RoundTripNbo<int64_t>(0));
  EXPECT_TRUE(RoundTripNbo<int64_t>(0x7FFFFFFFFFFFFFFF));
  EXPECT_TRUE(RoundTripNbo<int64_t>(-0x8000000000000000));
  EXPECT_TRUE(RoundTripNbo<uint8_t>(0));
  EXPECT_TRUE(RoundTripNbo<uint8_t>(0xFF));
  EXPECT_TRUE(RoundTripNbo<uint16_t>(0));
  EXPECT_TRUE(RoundTripNbo<uint16_t>(0xFFFF));
  EXPECT_TRUE(RoundTripNbo<uint32_t>(0));
  EXPECT_TRUE(RoundTripNbo<uint32_t>(0xFFFFFFFF));
  EXPECT_TRUE(RoundTripNbo<uint64_t>(0));
  EXPECT_TRUE(RoundTripNbo<uint64_t>(0xFFFFFFFFFFFFFFFF));
}
