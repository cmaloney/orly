/* <stig/code_gen/match.cc>

   Implements <stig/code_gen/match.h>

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

#include <stig/code_gen/match.h>

#include <stig/type/seq.h>
#include <stig/type/unwrap.h>

using namespace std;
using namespace Stig::CodeGen;
using namespace Stig::Type;

TMatch::TPtr TMatch::New(
    const L0::TPackage *package,
    const TInline::TPtr &match_text,
    const TInline::TPtr &regex) {
  return TPtr(new TMatch(package, match_text, regex));
}

void TMatch::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);
  out
    << "TMatchGenerator::New("
    << MatchFromText << ", " << Regex << ')';
}

TMatch::TMatch(const L0::TPackage *package, const TInline::TPtr &match_text, const TInline::TPtr &regex)
  : TInline(package, TMatch::GetReturnType()), MatchFromText(match_text), Regex(regex) {}

/* for convenience/readability */
TType TMatch::GetReturnType() {
  return TSeq::Get(TStr::Get());
}