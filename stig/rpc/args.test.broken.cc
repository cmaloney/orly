/* <stig/rpc/args.test.broken.cc> 

   Unit test for <stig/rpc/args.h>.

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

#include <stig/rpc/args.h>

#include <sstream>

#include <test/kit.h>

using namespace std;
using namespace Stig::Rpc;

static string ToString(TBlob::TReader &&reader) {
  struct reader_t final : public TArgsReader {
    ostream &Strm;
    reader_t(ostream &strm) : Strm(strm) {}
    virtual bool operator()(string &name, bool     val) const override { Strm << '(' << name << ", " << val << ')'; return true; }
    virtual bool operator()(string &name, int64_t  val) const override { Strm << '(' << name << ", " << val << ')'; return true; }
    virtual bool operator()(string &name, double   val) const override { Strm << '(' << name << ", " << val << ')'; return true; }
    virtual bool operator()(string &name, string  &val) const override { Strm << '(' << name << ", " << val << ')'; return true; }
  };
  ostringstream strm;
  reader_t(strm).ReadArgs(reader);
  return strm.str();
}

FIXTURE(Typical) {
  TBufferPool buffer_pool;
  TBlob::TWriter writer(&buffer_pool);
  WriteArgs(writer, "flag", true, "short_str", 'a', "temp", 98.6, "num", 101);
  auto blob = writer.DraftBlob();
  EXPECT_EQ(ToString(TBlob::TReader(&blob)), "(flag, 1)(short_str, a)(temp, 98.6)(num, 101)");
}

