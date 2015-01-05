/* <util/tuple.test.cc>

   Unit test for <util/tuple.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <util/tuple.h>

#include <string>
#include <sstream>

#include <base/as_str.h>

#include <test/kit.h>

using namespace Util;

FIXTURE(ForEach) {
  std::istringstream strm("0 0 0");
  auto x = std::make_tuple(4, 2, 5);
  ForEach(x, [&strm](auto &elem) { strm >> elem; });
  EXPECT_TRUE(x == std::make_tuple(0, 0, 0));
}

struct TPrint {
  explicit TPrint(std::ostream &strm) : Strm(strm) {}

  template <std::size_t Idx, typename TElem>
  void operator()(const TElem &elem) const {
    (Idx ? Strm << ' ' : Strm) << elem;
  }

  private:
  std::ostream &Strm;

};  // TPrint

FIXTURE(WithoutCapture) {
  auto tup = std::make_tuple(4, 2, 5);
  ForEach(tup, TPrint(std::cout));
  std::cout << std::endl;
}

FIXTURE(WithCapture) {
  auto tup = std::make_tuple(1, 2, 3);
  std::ostringstream strm;
  strm << '(';
  ForEach(tup, TPrint(strm));
  strm << ')';
  EXPECT_EQ(strm.str(), "(1 2 3)");
}

struct TAddOne {
  template <typename TElem>
  void operator()(TElem &elem) const {
    elem += 1;
  }
  void operator()(std::string &elem) const { elem = std::to_string(std::stoi(elem) + 1); }
};  // TAddOne

FIXTURE(Modify) {
  auto tup = std::make_tuple(1, 2.0, std::string("1"), 3.5);
  ForEach(tup, TAddOne());
  EXPECT_TRUE(tup == std::make_tuple(2, 3.0, std::string("2"), 4.5));
}
