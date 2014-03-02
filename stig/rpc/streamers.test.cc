/* <stig/rpc/streamers.test.cc> 

   Unit test for <stig/rpc/streamers.h>.

   Copyright 2010-2014 Tagged
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
   
     http://www.apache.org/licenses/LICENSE-2.0
   
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/rpc/streamers.h>

#include <sstream>
#include <string>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig::Rpc;

/* Insert the given value into a blob, then extract it again and return it. */
template <typename TThat>
static TThat RoundTrip(const TThat &that) {
  TBufferPool buffer_pool;
  TBlob blob = (TBlob::TWriter(&buffer_pool) << that).DraftBlob();
  TThat result;
  TBlob::TReader(&blob) >> result;
  return move(result);
}

/* Explicit overload for c-string, because a c-string is extracted as a std string. */
static string RoundTrip(const char *that) {
  TBufferPool buffer_pool;
  TBlob blob = (TBlob::TWriter(&buffer_pool) << that).DraftBlob();
  string result;
  TBlob::TReader(&blob) >> result;
  return move(result);
}

FIXTURE(RoundTrips) {
  /* Built-in types. */
  EXPECT_EQ(RoundTrip<bool    >( true),  true);
  EXPECT_EQ(RoundTrip<bool    >(false), false);
  EXPECT_EQ(RoundTrip<char    >(  'a'),   'a');
  EXPECT_EQ(RoundTrip<float   >(    0),   0.0);
  EXPECT_EQ(RoundTrip<double  >(-98.6), -98.6);
  EXPECT_EQ(RoundTrip<double  >(    0),   0.0);
  EXPECT_EQ(RoundTrip<double  >( 98.6),  98.6);
  EXPECT_EQ(RoundTrip<int8_t  >( -101),  -101);
  EXPECT_EQ(RoundTrip<int8_t  >(    0),     0);
  EXPECT_EQ(RoundTrip<int8_t  >(  101),   101);
  EXPECT_EQ(RoundTrip<int16_t >( -101),  -101);
  EXPECT_EQ(RoundTrip<int16_t >(    0),     0);
  EXPECT_EQ(RoundTrip<int16_t >(  101),   101);
  EXPECT_EQ(RoundTrip<int32_t >( -101),  -101);
  EXPECT_EQ(RoundTrip<int32_t >(    0),     0);
  EXPECT_EQ(RoundTrip<int32_t >(  101),   101);
  EXPECT_EQ(RoundTrip<int64_t >( -101),  -101);
  EXPECT_EQ(RoundTrip<int64_t >(    0),    0u);
  EXPECT_EQ(RoundTrip<int64_t >(  101),  101u);
  EXPECT_EQ(RoundTrip<uint8_t >(    0),    0u);
  EXPECT_EQ(RoundTrip<uint8_t >(  101),  101u);
  EXPECT_EQ(RoundTrip<uint16_t>(    0),    0u);
  EXPECT_EQ(RoundTrip<uint16_t>(  101),  101u);
  EXPECT_EQ(RoundTrip<uint32_t>(    0),    0u);
  EXPECT_EQ(RoundTrip<uint32_t>(  101),  101u);
  EXPECT_EQ(RoundTrip<uint64_t>(    0),    0u);
  EXPECT_EQ(RoundTrip<uint64_t>(  101),  101u);
  /* UUIDs. */
  EXPECT_EQ(RoundTrip(TUuid()), TUuid());
  EXPECT_EQ(RoundTrip(TUuid("1b4e28ba-2fa1-11d2-883f-b9a761bde3fb")), TUuid("1b4e28ba-2fa1-11d2-883f-b9a761bde3fb"));
  /* Strings. */
  EXPECT_EQ(RoundTrip(string()), string());
  EXPECT_EQ(RoundTrip(string("Mofo the Psychic Gorilla")), string("Mofo the Psychic Gorilla"));
  EXPECT_EQ(RoundTrip(""), string());
  EXPECT_EQ(RoundTrip("Mofo the Psychic Gorilla"), string("Mofo the Psychic Gorilla"));
  /* Pairs and tuples. */
  EXPECT_TRUE(RoundTrip(make_pair(101, 'a')) == make_pair(101, 'a'));
  EXPECT_TRUE(RoundTrip(make_tuple()) == make_tuple());
  EXPECT_TRUE(RoundTrip(make_tuple(1, 'a', true)) == make_tuple(1, 'a', true));
  /* Chrono types. */
  EXPECT_TRUE(RoundTrip(system_clock::time_point()) == system_clock::time_point());
  EXPECT_TRUE(RoundTrip(seconds(5)) == seconds(5));
  /* Lists, vectors, sets, multisets, maps, and multimaps. */
  EXPECT_TRUE(RoundTrip(list<int>()) == list<int>());
  EXPECT_TRUE(RoundTrip(list<int>({ 1, 2, 3 })) == list<int>({ 1, 2, 3 }));
  EXPECT_TRUE(RoundTrip(vector<int>()) == vector<int>());
  EXPECT_TRUE(RoundTrip(vector<int>({ 1, 2, 3 })) == vector<int>({ 1, 2, 3 }));
  EXPECT_TRUE(RoundTrip(set<int>()) == set<int>());
  EXPECT_TRUE(RoundTrip(set<int>({ 1, 2, 3 })) == set<int>({ 1, 2, 3 }));
  EXPECT_TRUE(RoundTrip(multiset<int>()) == multiset<int>());
  EXPECT_TRUE(RoundTrip(multiset<int>({ 1, 2, 3 })) == multiset<int>({ 1, 2, 3 }));
  EXPECT_TRUE(RoundTrip((map<int, int>()) == map<int, int>()));
  EXPECT_TRUE(RoundTrip((map<int, int>({ { 1, 101}, { 2, 202 }, { 3, 303 } })) == map<int, int>({ { 1, 101}, { 2, 202 }, { 3, 303 } })));
  EXPECT_TRUE(RoundTrip((multimap<int, int>()) == multimap<int, int>()));
  EXPECT_TRUE(RoundTrip((multimap<int, int>({ { 1, 101}, { 2, 202 }, { 3, 303 } })) == multimap<int, int>({ { 1, 101}, { 2, 202 }, { 3, 303 } })));
  /* Unordered sets, unordered multisets, unordered maps, and unordered multimaps. */
  EXPECT_TRUE(RoundTrip(unordered_set<int>()) == unordered_set<int>());
  EXPECT_TRUE(RoundTrip(unordered_set<int>({ 1, 2, 3 })) == unordered_set<int>({ 1, 2, 3 }));
  EXPECT_TRUE(RoundTrip(unordered_multiset<int>()) == unordered_multiset<int>());
  EXPECT_TRUE(RoundTrip(unordered_multiset<int>({ 1, 2, 3 })) == unordered_multiset<int>({ 1, 2, 3 }));
  EXPECT_TRUE(RoundTrip((unordered_map<int, int>()) == unordered_map<int, int>()));
  EXPECT_TRUE(RoundTrip((
      unordered_map<int, int>({ { 1, 101}, { 2, 202 }, { 3, 303 } })) == unordered_map<int, int>({ { 1, 101}, { 2, 202 }, { 3, 303 } })));
  EXPECT_TRUE(RoundTrip((unordered_multimap<int, int>()) == unordered_multimap<int, int>()));
  EXPECT_TRUE(RoundTrip((
      unordered_multimap<int, int>({ { 1, 101}, { 2, 202 }, { 3, 303 } })) == unordered_multimap<int, int>({ { 1, 101}, { 2, 202 }, { 3, 303 } })));
}

