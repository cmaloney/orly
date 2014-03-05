/* <tools/nycr/escape.cc>

   Implements <tools/nycr/escape.h>.

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

#include <tools/nycr/escape.h>

#include <base/no_default_case.h>

using namespace std;
using namespace Tools::Nycr;

void TEscape::Write(ostream &strm) const {
  assert(this);
  assert(&strm);
  switch (EscapeStyle) {
    case TEscape::CStyle: {
      strm << '"';
      for (auto iter = Text.begin(); iter != Text.end(); ++iter) {
        char c = *iter;
        switch (c) {
          case '\\': {
            strm << "\\\\";
            break;
          }
          case '"': {
            strm << "\\\"";
            break;
          }
          case '\n': {
            strm << "\\n";
            break;
          }
          case  '\r': {
            strm << "\\r";
            break;
          }
          default: {
            strm << c;
          }
        }
      }
      strm << '"';
      break;
    }
    case TEscape::XmlStyle: {
      for (auto iter = Text.begin(); iter != Text.end(); ++iter) {
        char c = *iter;
        switch (c) {
          case '\\': {
          	break;
          }
          case '&': {
          	strm << "&amp;";
          	break;
          }
          case '<': {
          	strm << "&lt;";
          	break;
          }
          case '>': {
          	strm << "&gt;";
          	break;
          }
          case '\"': {
          	strm << "&quot;";
          	break;
          }
          case '\'': {
          	strm << "&#39;";
          	break;
          }
          default: {
          	strm << c;
          }
        }
      }
      break;
    }
    NO_DEFAULT_CASE;
  }
}
