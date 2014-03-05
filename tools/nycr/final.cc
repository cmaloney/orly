/* <tools/nycr/final.cc>

   Implements <tools/nycr/final.h>.

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

#include <tools/nycr/final.h>

#include <tools/nycr/all_decls.h>

using namespace Tools::Nycr;

TFinal::TFinal(const Syntax::TName *name, const Syntax::TOptSuper *opt_super)
    : TKind(name, opt_super) {}

const char *TDecl::TInfo<TFinal>::Name = "final";
