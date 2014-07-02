/* <orly/data/core_vector_generator_sample.cc>

   A sample file that shows how to use core_vector_generator.h
   It generates some data and then pushes it to the generator to
   gen a core-vector file

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

#include <base/chrono.h>

#include <orly/data/core_vector_generator.h>

using namespace std;
using namespace Base;
using namespace Orly::Data;

/* This is an example of a struct which can be used as a orly object. Please
   note the RECORD_ELEM lines below the struct definition. These are required
   meta-data for it to be a valid orly object*/
struct TDemoObj {
  TDemoObj() : val(0L), str("") {}
  TDemoObj(int64_t _val, const std::string &_str)
    : val(_val),
    str(_str) {}

  TDemoObj(const TDemoObj &that)
    : val(that.val),
      str(that.str) {}

  private:

  int64_t val;
  std::string str;

};
/* generate meta-data for the member variables of TDemoObj*/
RECORD_ELEM(TDemoObj, int64_t, val);
RECORD_ELEM(TDemoObj, std::string, str);

int main() {
  using tkey = std::tuple<int64_t, double, std::string, Base::Chrono::TTimePnt>;
  using tval = TDemoObj;
  TCoreVectorGenerator<tkey, tval> cvg("sample_cv_data", 50);
  for (int64_t i = 0; i < 151; ++i) {
    // push a tkey and a tval
    cvg.Push(tkey(i, i * 4L, "test", Base::Chrono::Now()), TDemoObj(i * 10L, "mstr"));
  }
  printf("Generated [%ld] files", cvg.Finalize());
  return EXIT_SUCCESS;
}
