/* <tools/nycr/symbol/for_each_final.cc>

   Implements <tools/nycr/symbol/for_each_final.h>.

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

#include <tools/nycr/symbol/for_each_final.h>

#include <cassert>
#include <queue>
#include <unordered_set>

#include <tools/nycr/symbol/base.h>
#include <tools/nycr/symbol/keyword.h>
#include <tools/nycr/symbol/language.h>
#include <tools/nycr/symbol/operator.h>
#include <tools/nycr/symbol/rule.h>

using namespace std;
using namespace Tools::Nycr::Symbol;

void Tools::Nycr::Symbol::ForEachFinal(
    const TBase *base, const function<void (const TFinal *)> &cb) {
  assert(base);
  assert(&cb);
  const TBase::TSubKinds &sub_kinds = base->GetSubKinds();
  for (auto iter = sub_kinds.begin(); iter != sub_kinds.end(); ++iter) {
    class TVisitor : public TKind::TVisitor {
      public:
      TVisitor(const function<void (const TFinal *)> &cb) : Cb(cb) {}
      virtual void operator()(const TBase *that) const { ForEachFinal(that, Cb); }
      virtual void operator()(const TLanguage *that) const { Cb(that); }
      virtual void operator()(const TOperator *that) const { Cb(that); }
      virtual void operator()(const TKeyword *that) const { Cb(that); }
      virtual void operator()(const TRule *that) const { Cb(that); }
      private:
      const function<void (const TFinal *)> &Cb;
    };
    (*iter)->Accept(TVisitor(cb));
  }
}
