/* <multi_event/multi_event.test.cc>

   Unit test for <multi_event/multi_event.h>.

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

#include <multi_event/multi_event.h>

#include <thread>

#include <test/kit.h>

using namespace std;
using namespace placeholders;
using namespace Base;
using namespace MultiEvent;

static bool IsEqual(const unordered_set<shared_ptr<TEvent>> &lhs, const unordered_set<shared_ptr<TEvent>> &rhs) {
  size_t size = lhs.size();
  bool success = (size == rhs.size());
  for (auto iter = lhs.begin(); success && iter != lhs.end(); ++iter) {
    success = (rhs.find(*iter) != rhs.end());
  }
  return success;
}

static bool Wait(
    const unordered_set<shared_ptr<TEvent>> &in,
    unordered_set<shared_ptr<TEvent>> &out,
    const TOpt<chrono::milliseconds> &timeout) {
  return TWaiter().Wait(TWaiter::Produce(in), TWaiter::Consume(out), timeout);
}

static void TestWait(
    const unordered_set<shared_ptr<TEvent>> &events,
    const unordered_set<shared_ptr<TEvent>> &expected) {
  unordered_set<shared_ptr<TEvent>> actual;
  bool timed_out = !Wait(events, actual, !expected.empty() ? TOpt<chrono::milliseconds>() : chrono::milliseconds(10));
  EXPECT_EQ(expected.empty(), timed_out);
  EXPECT_TRUE(IsEqual(actual, expected));
}

using TEventSet = unordered_set<shared_ptr<TEvent>>;

FIXTURE(Timeout1) {
  TestWait(TEventSet{ TEvent::New() }, TEventSet{});
}

FIXTURE(Timeout3) {
  TestWait({ TEvent::New(), TEvent::New(), TEvent::New() }, TEventSet{});
}

FIXTURE(Sync1) {
  auto e1 = TEvent::New();
  e1->Fire();
  TestWait({ e1 }, { e1 });
}

FIXTURE(Sync1of3) {
  auto e1 = TEvent::New(), e2 = TEvent::New(), e3 = TEvent::New();
  e2->Fire();
  TestWait({ e1, e2, e3 }, { e2 });
}

FIXTURE(Sync2of3) {
  auto e1 = TEvent::New(), e2 = TEvent::New(), e3 = TEvent::New();
  e2->Fire();
  e3->Fire();
  TestWait({ e1, e2, e3 }, { e2, e3 });
}

FIXTURE(Sync3of3) {
  auto e1 = TEvent::New(), e2 = TEvent::New(), e3 = TEvent::New();
  e1->Fire();
  e2->Fire();
  e3->Fire();
  TestWait({ e1, e2, e3 }, { e1, e2, e3 });
}

FIXTURE(Async) {
  auto e1 = TEvent::New(), e2 = TEvent::New(), e3 = TEvent::New();
  unordered_set<shared_ptr<TEvent>> events({ e1, e2, e3 }), expected({ e2 });
  thread waiter(TestWait, cref(events), cref(expected));
  e2->Fire();
  waiter.join();
}

FIXTURE(ByVal) {
  auto e1 = TEvent::New(), e2 = TEvent::New(), e3 = TEvent::New();
  e2->Fire();
  int actual = 0;
  TWaiter().Wait<int>(
    unordered_map<shared_ptr<TEvent>, int>({ {  e1, 101 }, { e2, 202 }, { e3, 303 } }),
    TOpt<chrono::milliseconds>(),
    bind(
      [](const int &val, int &actual) {
        actual = val;
      },
      _1,
      ref(actual)
    )
  );
  EXPECT_EQ(actual, 202);
}
