/* <io/device.test.cc>

   Unit test for <io/device.h>.

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

#include <io/device.h>

#include <base/split.h>
#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <util/tuple.h>

#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Io;
using namespace Util;

namespace std {

  template <typename TLhs, typename TRhs>
  ostream &operator<<(ostream &strm, const pair<TLhs, TRhs> &that) {
    return strm << '(' << that.first << ", " << that.second << ')';
  }

  template <typename TElem>
  ostream &operator<<(ostream &strm, const set<TElem> &that) {
    return strm << '(' << Join(that, ", ") << ')';
  }

  template <typename... TElems>
  ostream &operator<<(ostream &strm, const tuple<TElems...> &that) {
    return strm << '(' << Join(that, ", ") << ')';
  }

  template <typename TElem>
  ostream &operator<<(ostream &strm, const vector<TElem> &that) {
    return strm << '(' << Join(that, ", ") << ')';
  }

}  // std

template <typename TExpected, typename TActual = TExpected>
static void RoundTrip(TBinaryOutputStream &out_strm, TBinaryInputStream &in_strm, const TExpected &expected) {
  assert(&out_strm);
  assert(&in_strm);
  assert(&expected);
  out_strm << expected;
  out_strm.Flush();
  TActual actual;
  in_strm >> actual;
  EXPECT_EQ(actual, expected);
}

template <typename... TArgs>
void TestTuple(TBinaryOutputStream &out_strm,
               TBinaryInputStream &in_strm,
               const tuple<TArgs...> &that) {
  Util::ForEach(that,
                [&](const auto &elem) { RoundTrip(out_strm, in_strm, elem); });
}

FIXTURE(Typical) {
  TFd readable_fd, writeable_fd;
  TFd::Pipe(readable_fd, writeable_fd);
  TBinaryOutputOnlyStream out_strm(make_shared<TDevice>(writeable_fd));
  TBinaryInputOnlyStream in_strm(make_shared<TDevice>(readable_fd));
  TestTuple(out_strm, in_strm, make_tuple(
      true, false,
      'x', '\0',
      static_cast<float>(98.6),
      static_cast<double>(3.1415927),
      static_cast<int8_t>(101),
      static_cast<int16_t>(101),
      static_cast<int32_t>(101),
      static_cast<int64_t>(101),
      static_cast<uint8_t>(101),
      static_cast<uint16_t>(101),
      static_cast<uint32_t>(101),
      static_cast<uint64_t>(101),
      string("mofo"),
      make_pair(101, 202),
      vector<int>({1, 2, 3}),
      set<int>({1, 2, 3}),
      make_tuple(1, 2, 3)));
  RoundTrip<const char *, string>(out_strm, in_strm, "mofo");
}
