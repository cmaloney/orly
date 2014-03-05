/* <base/string_utils.cc>

   Implements <base/string_utils.h>.

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

#include <base/string_utils.h>

using namespace std;
using namespace Base;

const char *Base::CommaSep = ", ";

string Base::ConcatCStrArray(const char *c_strs[]) {
  assert(c_strs);
  ostringstream strm;
  for (const char *const *c_str = c_strs; *c_str; ++c_str) {
    strm << *c_str;
  }
  return strm.str();
}

string Base::ConcatCStrList(initializer_list<const char *> c_strs) {
  ostringstream strm;
  for (const char *c_str: c_strs) {
    strm << c_str;
  }
  return strm.str();
}

bool Base::GenCStrArray(const TCStrUser &c_str_user, const char *c_strs[]) {
  assert(c_str_user);
  assert(c_strs);
  for (const char *const *c_str = c_strs; *c_str; ++c_str) {
    if (!c_str_user(*c_str)) {
      return false;
    }
  }
  return true;
}

bool Base::GenCStrList(const TCStrUser &c_str_user, initializer_list<const char *> c_strs) {
  assert(c_str_user);
  for (const char *c_str: c_strs) {
    if (!c_str_user(c_str)) {
      return false;
    }
  }
  return true;
}

string Base::ToString(const TWriter &writer) {
  assert(writer);
  ostringstream strm;
  writer(strm);
  return strm.str();
}

void Base::WriteBracketedJoin(ostream &strm, const TCStrGen &c_str_gen, char open_c, char close_c, const char *sep) {
  assert(&strm);
  assert(c_str_gen);
  assert(sep);
  strm << open_c;
  bool sep_required = false;
  c_str_gen(
      [&strm, sep, &sep_required](const char *c_str) {
        if (c_str) {
          if (sep_required) {
            strm << sep;
          } else {
            strm << ' ';
            sep_required = true;
          }
          strm << c_str;
        }
        return true;
      }
  );
  if (sep_required) {
    strm << ' ';
  }
  strm << close_c;
}

void Base::WriteJoin(ostream &strm, const TCStrGen &c_str_gen, const char *sep) {
  assert(&strm);
  assert(c_str_gen);
  assert(sep);
  bool sep_required = false;
  c_str_gen(
      [&strm, sep, &sep_required](const char *c_str) {
        if (c_str) {
          if (sep_required) {
            strm << sep;
          } else {
            sep_required = true;
          }
          strm << c_str;
        }
        return true;
      }
  );
}
