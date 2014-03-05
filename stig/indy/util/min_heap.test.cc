/* <stig/indy/util/min_heap.test.cc>

   Unit test for <stig/indy/util/min_heap.h>.

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

#include <stig/indy/util/min_heap.h>

#include <stig/atom/suprena.h>
#include <stig/indy/key.h>
#include <stig/sabot/type_dumper.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Atom;
using namespace Stig::Indy;
using namespace Stig::Indy::Util;

class TMyObj {
  public:

  TMyObj(size_t key, size_t val) : Key(key), Val(val) {}

  bool operator <(const TMyObj &that) const {
    return Key < that.Key;
  }

  bool operator >(const TMyObj &that) const {
    return Key > that.Key;
  }

  inline size_t GetKey() const {
    return Key;
  }

  inline size_t GetVal() const {
    return Val;
  }

  //private:

  size_t Key;
  size_t Val;

};

FIXTURE(Typical) {
  const size_t max_elems = 10;
  std::vector<TMyObj *> data;
  TMinHeap<TMyObj, size_t> min_heap(max_elems);
  for(size_t i = 0; i < max_elems; ++i) {
    data.push_back(new TMyObj(i * 10, i));
    min_heap.Insert(*data.back(), i);
  }
  size_t num_seen = 0UL;
  while (min_heap) {
    size_t pos;
    const TMyObj &min_val = min_heap.Pop(pos);
    EXPECT_EQ(pos, num_seen);
    EXPECT_EQ(min_val.GetKey(), num_seen * 10);
    EXPECT_EQ(min_val.GetVal(), num_seen);
    ++num_seen;
  }
  for (auto iter : data) {
    delete iter;
  }
}


FIXTURE(RevTypical) {
  const size_t max_elems = 10;
  std::vector<TMyObj *> data;
  TMinHeap<TMyObj, size_t> min_heap(max_elems);
  for(size_t i = 0; i < max_elems; ++i) {
    data.push_back(new TMyObj((max_elems - i - 1) * 10, i));
    min_heap.Insert(*data.back(), i);
  }
  size_t num_seen = 0UL;
  while (min_heap) {
    size_t pos;
    const TMyObj &min_val = min_heap.Pop(pos);
    EXPECT_EQ(pos, max_elems - num_seen - 1);
    EXPECT_EQ(min_val.GetKey(), num_seen * 10);
    EXPECT_EQ(min_val.GetVal(), max_elems - num_seen - 1);
    ++num_seen;
  }
  for (auto iter : data) {
    delete iter;
  }
}