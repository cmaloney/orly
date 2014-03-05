/* <visitor/shared.test.cc>

   Unit test for <visitor/shared.h>

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

#include <memory>
#include <unordered_set>

#include <visitor/shared.h>

#include <test/kit.h>

using namespace Visitor;

FIXTURE(EmptyShared) {
  TShared<int> empty;
  EXPECT_FALSE(empty);
}

FIXTURE(New) {
  auto ptr = MakeShared<std::string>("hello");
  EXPECT_TRUE(ptr);
}

FIXTURE(Share) {
  /* non-const */ {
    auto f = [](std::string &that) -> TShared<std::string> {
      return Share(that);
    };
    auto lhs = MakeShared<std::string>("world");
    auto rhs = f(*lhs);
    EXPECT_EQ(lhs, rhs);
    EXPECT_EQ(lhs.GetCount(), 2u);
    EXPECT_EQ(rhs.GetCount(), 2u);
  }
  /* const */ {
    auto f = [](const std::string &that) -> TShared<const std::string> {
      return Share(that);
    };
    auto lhs = MakeShared<const std::string>("world");
    auto rhs = f(*lhs);
    EXPECT_EQ(lhs, rhs);
    EXPECT_EQ(lhs.GetCount(), 2u);
    EXPECT_EQ(rhs.GetCount(), 2u);
  }
}

FIXTURE(SharedFromThis) {
  /* std::shared_ptr. */{
    class TObj : public std::enable_shared_from_this<TObj> {
      public:
      std::shared_ptr<TObj> GetThis() {
        assert(this);
        return shared_from_this();
      }
    };  // TObj
    auto lhs = std::make_shared<TObj>();
    auto rhs = lhs->GetThis();
    EXPECT_EQ(lhs, rhs);
    EXPECT_EQ(lhs.use_count(), 2u);
    EXPECT_EQ(rhs.use_count(), 2u);
  }
  /* TShared. */ {
    class TObj {
      public:
      TShared<TObj> GetThis() {
        assert(this);
        return Share(*this);
      }
    };  // TObj
    auto lhs = MakeShared<TObj>();
    auto rhs = lhs->GetThis();
    EXPECT_EQ(lhs, rhs);
    EXPECT_EQ(lhs.GetCount(), 2u);
    EXPECT_EQ(rhs.GetCount(), 2u);
  }
}

FIXTURE(Copy) {
  auto lhs = MakeShared<int>(0);
  TShared<int> rhs(lhs);
  EXPECT_EQ(lhs, rhs);
  EXPECT_EQ(lhs.GetCount(), 2u);
  EXPECT_EQ(rhs.GetCount(), 2u);
}

FIXTURE(Move) {
  auto lhs = MakeShared<int>(0);
  TShared<int> rhs(std::move(lhs));
  EXPECT_FALSE(lhs);
  EXPECT_TRUE(rhs);
  EXPECT_EQ(rhs.GetCount(), 1u);
}

FIXTURE(CopyToVoid) {
  /* non-const */ {
    auto lhs = MakeShared<int>(42);
    TShared<void> rhs(lhs);
    EXPECT_EQ(lhs, rhs);
    EXPECT_EQ(lhs.GetCount(), 2u);
    EXPECT_EQ(rhs.GetCount(), 2u);
  }
  /* const */ {
    auto lhs = MakeShared<const int>(24);
    TShared<const void> rhs(lhs);
    EXPECT_EQ(lhs, rhs);
    EXPECT_EQ(lhs.GetCount(), 2u);
    EXPECT_EQ(rhs.GetCount(), 2u);
  }
}

FIXTURE(MoveToVoid) {
  auto lhs = MakeShared<int>(42);
  TShared<void> rhs(std::move(lhs));
  EXPECT_FALSE(lhs);
  EXPECT_TRUE(rhs);
  EXPECT_EQ(rhs.GetCount(), 1u);
}

FIXTURE(Swap) {
  auto lhs = MakeShared<std::string>("lhs");
  auto rhs = MakeShared<std::string>("rhs");
  std::swap(lhs, rhs);
  EXPECT_TRUE(lhs);
  EXPECT_TRUE(rhs);
  EXPECT_EQ(lhs.GetCount(), 1u);
  EXPECT_EQ(rhs.GetCount(), 1u);
  EXPECT_EQ(*lhs, "rhs");
  EXPECT_EQ(*rhs, "lhs");
}

FIXTURE(Less) {
  std::set<Visitor::TShared<int>> s;
  s.insert(MakeShared<int>(42));
  s.insert(MakeShared<int>(42));
  s.insert(MakeShared<int>(0));
  EXPECT_EQ(s.size(), 3u);
}

FIXTURE(Hash) {
  std::unordered_set<Visitor::TShared<int>> us;
  us.insert(MakeShared<int>(42));
  us.insert(MakeShared<int>(42));
  us.insert(MakeShared<int>(0));
  EXPECT_EQ(us.size(), 3u);
}

FIXTURE(EmptyWeak) {
  TWeak<int> weak;
  EXPECT_TRUE(weak.IsExpired());
  EXPECT_EQ(weak.GetCount(), 0u);
}

FIXTURE(Weak) {
  TWeak<int> weak;
  EXPECT_TRUE(weak.IsExpired());
  /* extra */ {
    auto x = MakeShared<int>(101);
    weak = x;
    EXPECT_FALSE(weak.IsExpired());
    EXPECT_EQ(x.GetCount(), 1u);
    EXPECT_EQ(weak.GetCount(), 1u);
    auto y = weak.Lock();
    EXPECT_EQ(x.GetCount(), 2u);
    EXPECT_EQ(y.GetCount(), 2u);
    EXPECT_EQ(weak.GetCount(), 2u);
  }
  EXPECT_TRUE(weak.IsExpired());
  EXPECT_EQ(weak.GetCount(), 0u);
}
