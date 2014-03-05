/* <tools/nycr/symbol/name.cc>

   Implements <tools/nycr/symbol/name.h>.

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

#include <tools/nycr/symbol/name.h>

#include <algorithm>
#include <functional>
#include <unordered_set>

#include <base/thrower.h>
#include <tools/nycr/symbol/write_xml.h>

using namespace std;
using namespace Tools::Nycr;
using namespace Tools::Nycr::Symbol;

bool TName::operator<(const TName &that) const {
  assert(this);
  return lexicographical_compare(
      Parts.begin(), Parts.end(),
      that.Parts.begin(), that.Parts.end());
}

void TName::WriteLower(ostream &strm) const {
  assert(this);
  assert(&strm);
  for (auto iter = Parts.begin(); iter != Parts.end(); ++iter) {
    if (iter != Parts.begin()) {
      strm << '_';
    }
    strm << *iter;
  }
}

void TName::WriteUpper(ostream &strm) const {
  assert(this);
  assert(&strm);
  for (auto iter = Parts.begin(); iter != Parts.end(); ++iter) {
    const string &part = *iter;
    strm << static_cast<char>(toupper(part[0])) << part.substr(1);
  }
}

void TName::WriteXml(ostream &strm) const {
  assert(this);
  assert(&strm);
  strm << TXmlTag(TXmlTag::Name, Open, Indent, NoEndl);
  for (auto iter = Parts.begin(); iter != Parts.end(); ++iter) {
    strm << TXmlTag(TXmlTag::Part, Open, NoIndent, NoEndl)
	 << *iter
	 << TXmlTag(TXmlTag::Part, Close, NoIndent, NoEndl);
  }
  strm << TXmlTag(TXmlTag::Name, Close, NoIndent, Endl);
}

void TName::Init(const char *start, const char *limit) {
  assert(start <= limit);
  const char *anchor = start;
  for (const char *csr = start; csr <= limit; ++csr) {
    if (csr == limit || *csr == '_') {
      if (anchor == csr) {
        THROW << "invalid name \"" << string(start, limit) << '"';
      }
      Parts.push_back(string(anchor, csr));
      anchor = csr + 1;
    }
  }
}