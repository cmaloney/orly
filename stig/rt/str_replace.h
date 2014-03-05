/* <stig/rt/str_replace.h>

   Replace a regex in a string with something else.

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

#pragma once

#include <cassert>
#include <utility>
#include <string>

#include <base/no_copy_semantics.h>
#include <utf8/piece.h>
#include <utf8/regex.h>

namespace Stig {

  namespace Rt {

    /* Split a text into pieces based on regex delimiters.. */
    class TStrReplace final {
      NO_COPY_SEMANTICS(TStrReplace);
      public:

      using TPiece = Utf8::TPiece;
      using TRegex = Utf8::TRegex;

      static std::string Replace(std::string oldstr, const std::string &regex, const std::string &newstr) {
	TRegex Regex(regex.c_str());
	TPiece match_delim;  // going to use this to figure out where to call string::replace
	const char *curr_start = oldstr.c_str();
	const char *old_string = oldstr.c_str();
	while (Regex.TryGetMatch(curr_start, match_delim, 0)) {
	  oldstr.replace(match_delim.GetStart() - old_string, match_delim.GetLimit() - match_delim.GetStart(), newstr);
	  curr_start = match_delim.GetLimit();
	}
	return oldstr;
      }

    };  // TStrReplace

  }  // Rt

}  // Stig
