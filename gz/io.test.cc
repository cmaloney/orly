/* <gz/io.test.cc>

   Unit test for <gz/input_producer.h> and <gz/output_consumer.h>.

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

#include <gz/input_producer.h>
#include <gz/output_consumer.h>

#include <string>

#include <io/binary_input_only_stream.h>
#include <io/binary_output_only_stream.h>
#include <test/kit.h>

using namespace std;
using namespace Gz;

FIXTURE(Typical) {
  static const char *path = "/tmp/gz.file.test.gz";
  static const string expected_msg = "Mofo the Psychic gorilla lives for compression.";
  /* Write a message to a temp file. */ {
    Io::TBinaryOutputOnlyStream strm(make_shared<TOutputConsumer>(path, "w"));
    strm << expected_msg;
  }
  string actual_msg;
  /* Read data back from the temp file. */ {
    Io::TBinaryInputOnlyStream strm(make_shared<TInputProducer>(path, "r"));
    strm >> actual_msg;
  }
  EXPECT_EQ(actual_msg, expected_msg);
}
