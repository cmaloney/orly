/* <stig/parse_compiled_import.test.broken.cc> 

   Implements <stig/parse_compiled_import.h>.

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

#include <stig/parse_compiled_import.h>

#include <fcntl.h>

#include <io/device.h>
#include <io/binary_input_only_stream.h>
#include <test/kit.h>

using namespace std;
using namespace Stig;

FIXTURE(Typical) {
  Io::TBinaryInputOnlyStream strm(make_shared<Io::TDevice>(open("/home/jlucas/projects/src/stig/server/sample.import.arenas", O_RDONLY)));
  ParseCompiledImport(strm, [](size_t){ return true; }, [](const vector<std::pair<Base::TUuid, Var::TVar>> &){ return true; });
}

