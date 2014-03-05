/* <tools/nycr/symbol/kind.cc>

   Implements <tools/nycr/symbol/kind.h>.

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

#include <tools/nycr/symbol/kind.h>

#include <tools/nycr/symbol/base.h>
#include <tools/nycr/symbol/language.h>
#include <tools/nycr/symbol/operator.h>
#include <tools/nycr/symbol/keyword.h>
#include <tools/nycr/symbol/rule.h>

using namespace Tools::Nycr::Symbol;

TKind::TAnyBase::~TAnyBase() {}

TKind::TVisitor::~TVisitor() {}

TKind::TKind(const TName &name, TAnyBase *base)
    : Name(name), Base(base) {
  if (base) {
    base->SubKinds.insert(this);
  }
}

TKind::~TKind() {
  assert(this);
  if (Base) {
    Base->SubKinds.erase(this);
  }
}
