/* <tools/nycr/symbol/named_member.cc>

   Implements <tools/nycr/symbol/named_member.h>.

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

#include <tools/nycr/symbol/named_member.h>

#include <tools/nycr/symbol/write_xml.h>

using namespace std;
using namespace Tools::Nycr::Symbol;

TNamedMember::~TNamedMember() {
  assert(this);
  SetCompound(0);
}

const TName &TNamedMember::GetName() const {
  assert(this);
  return Name;
}

void TNamedMember::WriteRhs(ostream &strm) const {
  assert(this);
  assert(&strm);
  strm << TLower(Name) << ':' << TLower(GetKind()->GetName());
}

void TNamedMember::WriteXml(ostream &strm) const {
  assert(this);
  assert(&strm);
  strm << TXmlTag(TXmlTag::NamedMember, Open)
       << TXml(GetName())
       << TXmlTag(TXmlTag::Kind, Open)
       << TXml(GetKind()->GetName())
       << TXmlTag(TXmlTag::Kind, Close)
       << TXmlTag(TXmlTag::NamedMember, Close);
}