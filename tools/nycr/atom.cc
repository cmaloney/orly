/* <tools/nycr/atom.cc>

   Implements <tools/nycr/atom.h>.

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

#include <tools/nycr/atom.h>

#include <cassert>

#include <tools/nycr/all_decls.h>
#include <tools/nycr/error.h>

using namespace std;
using namespace Tools::Nycr;

TAtom::TAtom(const Syntax::TName *name, const Syntax::TOptSuper *opt_super, const Syntax::TPattern *pattern)
    : TFinal(name, opt_super) {
  class TStrLiteralVisitor
      : public Syntax::TStrLiteral::TVisitor {
    public:
    TStrLiteralVisitor(const TPosRange *&pos_range, string &text)
        : PosRange(pos_range), Text(text) {}
    virtual void operator()(const Syntax::TDoubleQuotedRawStrLiteral *that) const {
      PosRange = &(that->GetLexeme().GetPosRange());
      Text = that->GetLexeme().AsDoubleQuotedRawString();
    }
    virtual void operator()(const Syntax::TDoubleQuotedStrLiteral *that) const {
      PosRange = &(that->GetLexeme().GetPosRange());
      Text = that->GetLexeme().AsDoubleQuotedString();
    }
    virtual void operator()(const Syntax::TSingleQuotedRawStrLiteral *that) const {
      PosRange = &(that->GetLexeme().GetPosRange());
      Text = that->GetLexeme().AsSingleQuotedRawString();
    }
    virtual void operator()(const Syntax::TSingleQuotedStrLiteral *that) const {
      PosRange = &(that->GetLexeme().GetPosRange());
      Text = that->GetLexeme().AsSingleQuotedString();
    }
    private:
    const TPosRange *&PosRange;
    string &Text;
  };  // TStrLiteralVisitor
  const TPosRange *pos_range;
  try {
    pattern->GetStrLiteral()->Accept(TStrLiteralVisitor(pos_range, PatternText));
  } catch (const exception &ex) {
    TError::TBuilder(*pos_range) << ex.what();
  }
  GetOptInt<Syntax::TPriLevel, Syntax::TNoPriLevel>(pattern->GetOptPriLevel(), Pri);
}

Symbol::TKind *TAtom::GetSymbolAsKind() const {
  assert(this);
  return GetSymbolAsAtom();
}

const char *TDecl::TInfo<TAtom>::Name = "atom";
