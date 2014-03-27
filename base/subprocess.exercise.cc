/* <base/subprocess.exercise.cc>

   Exercises <base/subprocess.h>.

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

#include <base/subprocess.h>

#include <iostream>

using namespace std;
using namespace Base;

int main(int, char *[]) {
  TPump pump;
  auto subprocess = TSubprocess::New(pump);
  if (!subprocess) {
    cout << "Hello, world!";
    exit(111);
  }
  char buf[1024];
  ssize_t size = read(subprocess->GetStdOutFromChild(), buf, sizeof(buf));
  if (size >= 0) {
    buf[size] = '\0';
    cout << "I got: " << buf << endl;
  } else {
    cout << "<error>" << endl;
  }
  cout << "exit: " << subprocess->Wait() << endl;
  return 0;
}
