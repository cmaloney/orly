/* <base/tuple_utils.test.cc>

   Unit test for <base/tuple_utils.h>

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

#include <base/tuple_utils.h>

#include <string>
#include <sstream>

#include <test/kit.h>

using namespace Base;

template <typename TElem>
struct TPrint {
  bool operator()(TElem &elem, size_t index) const {
    if (index) {
      std::cout << ' ';
    }
    std::cout << elem;
    return true;
  }
};  // TPrint

FIXTURE(WithoutCapture) {
  auto tup = std::make_tuple(4, 2, 5);
  ForEachElem<TPrint>(tup);
  std::cout << std::endl;
}

template <typename TElem>
struct TPrintToStrm {
  bool operator()(TElem &elem, size_t index, std::ostringstream &strm) const {
    if (index) {
      strm << ", ";
    }
    strm << elem;
    return true;
  }
}; // TPrintToStrm

FIXTURE(WithCapture) {
  auto tup = std::make_tuple(1, 2, 3);
  std::ostringstream strm;
  strm << '(';
  ForEachElem<TPrintToStrm>(tup, strm);
  strm << ')';
  EXPECT_EQ(strm.str(), "(1, 2, 3)");
}

template <typename TElem>
struct TAddOne {
  bool operator()(TElem &elem, size_t) const {
    elem += 1;
    return true;
  }
};  // TAddOne

template <>
struct TAddOne<std::string> {
  bool operator()(std::string &str, size_t) const {
    size_t elem;
    std::stringstream strm;
    strm << str;
    strm >> elem;
    elem += 1;
    strm.clear();
    strm << elem;
    strm >> str;
    return true;
  }
};  // TAddOne<std::string>

FIXTURE(Modify) {
  auto tup = std::make_tuple(1, 2.0, std::string("1"), 3.5);
  ForEachElem<TAddOne>(tup);
  EXPECT_TRUE(tup == std::make_tuple(2, 3.0, std::string("2"), 4.5));
}

template <typename TElem>
struct TAddOneUntilFive {
  bool operator()(TElem &elem, size_t) const {
    if (elem == 5) {
      return false;
    }
    elem += 1;
    return true;
  }
};  // TAddOneUntilFive

FIXTURE(AddOneUptoFive) {
  auto tup = std::make_tuple(3, 4, 5, 4);
  ForEachElem<TAddOneUntilFive>(tup);
  EXPECT_TRUE(tup == std::make_tuple(4, 5, 5, 4));
}

FIXTURE(GetHead) {
  auto tup = std::make_tuple(true, 101, 98.6);
  EXPECT_EQ(GetHead(tup), true);
  EXPECT_EQ(GetHead(GetTail(tup)), 101);
  EXPECT_EQ(GetHead(GetTail(GetTail(tup))), 98.6);
}
