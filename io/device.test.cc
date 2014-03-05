/* <io/device.test.cc>

   Unit test for <io/device.h>.

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

#include <io/device.h>

#include <base/tuple_utils.h>
#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <test/kit.h>

using namespace std;
using namespace Base;
using namespace Io;

template <size_t N, typename... TArgs>
class TTupleWriter;

template <size_t N>
class TTupleWriter<N> {
  public:
  static void WriteTuple(ostream &, const _Tuple_impl<N> &) {}
};

template <size_t N, typename THead, typename... TRest>
class TTupleWriter<N, THead, TRest...> {
  public:
  static void WriteTuple(ostream &strm, const _Tuple_impl<N, THead, TRest...> &that) {
    if (N) {
      strm << ", ";
    }
    strm << GetHead(that);
    TTupleWriter<N + 1, TRest...>::WriteTuple(strm, GetTail(that));
  }
};

namespace std {
  template <typename... TArgs>
  ostream &operator<<(ostream &strm, const tuple<TArgs...> &that) {
    strm << '(';
    TTupleWriter<0, TArgs...>::WriteTuple(strm, that);
    return strm << ')';
  }

  template <typename TFirst, typename TSecond>
  ostream &operator<<(ostream &strm, const pair<TFirst, TSecond> &that) {
    return strm << that.first << ": " << that.second;
  }
}

template <typename TThat>
static void WriteContainer(ostream &strm, const TThat &that) {
  strm << '{';
  bool sep = false;
  for (const typename TThat::value_type &val: that) {
    if (sep) {
      strm << ", ";
    } else {
      sep = true;
    }
    strm << val;
  }
  strm << '}';
}

namespace std {
  template <typename TVal>
  ostream &operator<<(ostream &strm, const vector<TVal> &that) {
    WriteContainer(strm, that);
    return strm;
  }

  template <typename TVal>
  ostream &operator<<(ostream &strm, const set<TVal> &that) {
    WriteContainer(strm, that);
    return strm;
  }
}

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

template <size_t N, typename... TArgs>
class TTupleTester;

template <size_t N>
class TTupleTester<N> {
  public:
  static void TestTuple(TBinaryOutputStream &, TBinaryInputStream &, const _Tuple_impl<N> &) {}
};

template <size_t N, typename THead, typename... TRest>
class TTupleTester<N, THead, TRest...> {
  public:

  static void TestTuple(TBinaryOutputStream &out_strm, TBinaryInputStream &in_strm, const _Tuple_impl<N, THead, TRest...> &that) {
    RoundTrip(out_strm, in_strm, GetHead(that));
    TTupleTester<N + 1, TRest...>::TestTuple(out_strm, in_strm, GetTail(that));
  }
};

template <typename... TArgs>
void TestTuple(TBinaryOutputStream &out_strm, TBinaryInputStream &in_strm, const tuple<TArgs...> &that) {
  TTupleTester<0, TArgs...>::TestTuple(out_strm, in_strm, that);
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
