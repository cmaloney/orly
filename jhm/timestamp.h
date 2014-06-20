/* <jhm/timestamp.h>

   Nanosecond precision timestamp

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

#pragma once

#include <sys/stat.h>
#include <time.h>

#include <ostream>
#include <string>

#include <base/opt.h>
#include <util/error.h>

namespace Jhm {

  /* Tries to get the timestamp for the given file. Returns unknown if the file doesn't exist / stat fails. */
  inline Base::TOpt<timespec> TryGetTimestamp(const std::string &name) {
    struct stat st;
    if (stat(name.c_str(), &st) != 0) {
      if (errno == ENOENT) {
        return *Base::TOpt<timespec>::Unknown;
      }
      Util::ThrowSystemError(errno);
    }
    return st.st_mtim;
  }
  inline timespec GetTimestamp(const std::string &name) {
    struct stat st;
    Util::IfLt0(stat(name.c_str(), &st));
    return st.st_mtim;
  }

  inline bool IsNewer(const timespec &lhs, const timespec &rhs) {
    return lhs.tv_sec > rhs.tv_sec || (lhs.tv_sec == rhs.tv_sec && lhs.tv_nsec > rhs.tv_nsec);
  }

  inline bool IsNewer(const timespec &lhs, const Base::TOpt<timespec> &rhs) {
    if (rhs) {
      return IsNewer(lhs, *rhs);
    }
    return true;
  }

  inline timespec Newer(const timespec &lhs, const timespec &rhs) {
    if (IsNewer(lhs, rhs)) {
      return lhs;
    }
    return rhs;
  }

  inline timespec Newer(const timespec &lhs, const Base::TOpt<timespec> &rhs) {
    if (rhs) {
      return Newer(lhs, *rhs);
    }
    return lhs;
  }

  inline Base::TOpt<timespec> Newer(const Base::TOpt<timespec> &lhs, const Base::TOpt<timespec> &rhs) {
    if (lhs) {
      return Newer(*lhs, rhs);
    }
    return rhs;
  }

  inline Base::TOpt<timespec> Older(const Base::TOpt<timespec> &lhs, const Base::TOpt<timespec> &rhs) {
    if (lhs) {
      if (IsNewer(*lhs, rhs)) {
        return rhs;
      } else {
        return lhs;
      }
    }
    return rhs;
  }

}

inline std::ostream &operator<<(std::ostream &out, const timespec &that) {
  return out << that.tv_sec << '.' << that.tv_nsec;
}

