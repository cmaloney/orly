/* <stig/code_gen/split.cc>

   Implements <stig/code_gen/split.h>

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

#include <stig/code_gen/split.h>

#include <stig/type/seq.h>
#include <stig/type/unwrap.h>

using namespace std;
using namespace Stig::CodeGen;
using namespace Stig::Type;

TSplit::TPtr TSplit::New(
    const L0::TPackage *package,
    const TInline::TPtr &split_text,
    const TInline::TPtr &regex) {
  return TPtr(new TSplit(package, split_text, regex));
}

void TSplit::WriteExpr(TCppPrinter &out) const {
  assert(this);
  assert(&out);
  out
    << "TSplitGenerator::New("
    << SplitThisText << ", " << Regex << ')';
}

TSplit::TSplit(const L0::TPackage *package,
               const TInline::TPtr &split_text,
               const TInline::TPtr &regex)
  : TInline(package, TSplit::GetReturnType()),
    SplitThisText(split_text),
    Regex(regex) {}

/* for convenience/readability */
TType TSplit::GetReturnType() {
  return TSeq::Get(TAddr::Get(vector<pair<TAddrDir, TType>>{{TAddrDir::Asc, TStr::Get()}, {TAddrDir::Asc, TOpt::Get(TStr::Get())}}));
}