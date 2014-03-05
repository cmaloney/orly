/* <starsha/dummy.cc>

   Used as the 'dummy' tool when testing Starsha.

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

#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;

int main(int, char *[]) {
  int result;
  for (;;) {
    string line;
    getline(cin, line);
    if (line == "end") {
      result = EXIT_SUCCESS;
      break;
    }
    if (line == "abend") {
      result = EXIT_FAILURE;
      break;
    }
    if (line == "nolf") {
      result = EXIT_SUCCESS;
      cout << "nolf";
      break;
    }
    if (line.compare(0, 4, "out:") == 0) {
      cout << line.substr(4) << endl;
      continue;
    }
    if (line.compare(0, 4, "err:") == 0) {
      cerr << line.substr(4) << endl;
      continue;
    }
    result = atoi(line.c_str());
    break;
  }
  return result;
}
