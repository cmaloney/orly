/* <gz/file.test.cc>

   Unit test for <gz/file.h>.

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

#include <gz/file.h>

#include <cstring>

#include <test/kit.h>

using namespace std;
using namespace Gz;

FIXTURE(Typical) {
  static const char
      *path = "/tmp/gz.file.test.gz",
      *msg  = "Mofo the Psychic gorilla lives for compression.";
  static size_t msg_size = strlen(msg);
  /* Write a message to a temp file. */ {
    TFile file(path, "w");
    const char *csr = msg;
    size_t size = msg_size;
    while (size) {
      size_t actl = file.WriteAtMost(csr, size);
      csr  += actl;
      size -= actl;
    }
  }
  /* Read data back from the temp file. */
  char buffer[msg_size];
  char *csr = buffer;
  size_t size = 0;
  TFile file(path, "r");
  for (;;) {
    size_t actl = file.ReadAtMost(csr, sizeof(buffer) - size);
    if (!actl) {
      break;
    }
    csr  += actl;
    size += actl;
    if (!EXPECT_LE(size, sizeof(buffer))) {
      return;
    }
  }
  /* Got the message? */
  if (EXPECT_EQ(size, msg_size)) {
    EXPECT_EQ(strncmp(buffer, msg, size), 0);
  }
}
