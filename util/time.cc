/* <util/time.cc>

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

#include <util/time.h>

#include <sys/stat.h>

#include <base/as_str.h>
#include <base/split.h>
#include <util/error.h>

using namespace Base;
using namespace std;
using namespace std::chrono;
using namespace Util;

timespec Util::ToTimespec(nanoseconds ns) {
  auto s = duration_cast<seconds>(ns);
  ns -= s;
  return timespec{s.count(), ns.count()};
}

TTimestamp Util::ToTimestamp(timespec time) { return ToTimestampClock<system_clock>(time); }

static TTimestamp MtimeToTimestamp(struct stat st) {
#ifdef __APPLE__
  return ToTimestamp(st.st_mtimespec);
#else
  return ToTimestamp(st.st_mtim);
#endif
}

/* Tries to get the timestamp for the given file. Returns unknown if the file doesn't exist / stat
 * fails. */
TOptTimestamp Util::TryGetTimestamp(const std::string &name) {
  struct stat st;
  if(stat(name.c_str(), &st) != 0) {
    if(errno == ENOENT) {
      return TOptTimestamp();
    }
    ThrowSystemError(errno);
  }
  return MtimeToTimestamp(st);
}

TTimestamp Util::GetTimestamp(const std::string &name) {
  struct stat st;
  IfLt0(stat(name.c_str(), &st));
  return MtimeToTimestamp(st);
}

TTimestamp Util::GetTimestampSearchingPath(const string &name) {
  char *path = getenv("PATH");

  vector<string> potential;
  Split(":", path, potential);

  struct stat st;

  for(string &prefix : potential) {
    if(!prefix.empty() && prefix.back() != '/') {
      prefix += '/';
    }
    if(stat((prefix + name).c_str(), &st) != 0) {
      if(errno == ENOENT) {
        continue;
      } else {
        ThrowSystemError(errno);
      }
    } else {
      return MtimeToTimestamp(st);
    }
  }
  ThrowSystemError(ENOENT);
}

bool Util::IsNewer(TTimestamp lhs, TOptTimestamp rhs) {
  if(rhs) {
    return lhs > *rhs;
  }
  return true;
}

TTimestamp Util::Newest(TTimestamp lhs, TOptTimestamp rhs) {
  if(rhs) {
    return std::max(lhs, *rhs);
  }
  return lhs;
}

TOptTimestamp Util::Newest(TOptTimestamp lhs, TOptTimestamp rhs) {
  if(!lhs) {
    return rhs;
  }
  if(!rhs) {
    return lhs;
  }
  return std::max(*lhs, *rhs);
}

TOptTimestamp Util::Oldest(TOptTimestamp lhs, TOptTimestamp rhs) {
  if(!lhs) {
    return rhs;
  }
  if(!rhs) {
    return lhs;
  }
  return std::min(*lhs, *rhs);
}

std::string Util::ToStr(const TTimestamp &ts) { return AsStr(ts.time_since_epoch().count()); }
