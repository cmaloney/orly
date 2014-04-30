/* <orly/indy/disk/util/device_util.cc>

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

#include <orly/indy/disk/util/device_util.h>

#include <fstream>
#include <istream>
#include <stdexcept>

#include <base/thrower.h>

using namespace Orly::Indy::Disk::Util;
using namespace std;

bool TDeviceUtil::ForEachDevice(const function<bool(const char *)> &cb) {
  ifstream partitions("/proc/partitions");
  if(!partitions.is_open()) {
    THROW << "Unable to open /proc/partitions";
  }

  std::string line;

  //Make sure the formatting is what we expect (first line)
  getline(partitions, line);
  if (line != "major minor  #blocks  name") {
    THROW << "Unkwnown format of /proc/partitions";
  }

  // Skip empty line
  getline(partitions, line);

  //Read each entry, extract the last field 'name', and send it to the callback
  while(getline(partitions, line)) {
    if(!cb(line.substr(line.rfind(' ') + 1).c_str())) {
      return false;
    }
  }

  return true;
}
