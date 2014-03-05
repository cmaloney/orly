/* <stig/synth/package.cc>

   Implements <stig/synth/package.h>

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

#include <stig/synth/package.h>

#include <iostream>

#include <stig/error.h>
#include <stig/synth/get_pos_range.h>
#include <stig/synth/new_expr.h>
#include <tools/nycr/error.h>

using namespace Stig;
using namespace Stig::Synth;

TPackage::TPackage(const Jhm::TNamespace &ns, const Package::Syntax::TPackage *root, bool report_version)
    : Namespace(ns), Symbol(nullptr) {
  assert(root);
  const Package::Syntax::TInstallerDef *installer_def = nullptr;
  TExprFactory expr_factory(this);
  TTopLevelDefFactory::NewDefs(&expr_factory, installer_def, root->GetOptDefSeq());
  Version = installer_def ? installer_def->GetPackageVersion()->GetIntLiteral()->GetLexeme().AsUInt32() : 0;
  if (report_version) {
    std::cout << Version << std::endl;
  } else {
    if (!Tools::Nycr::TError::GetFirstError()) {
      BuildSymbol();
      Build();
    }
  }
}

void TPackage::Build() const {
  assert(this);
  Bind();
  int pass = 0;
  while (!Tools::Nycr::TError::GetFirstError()) {
    ++pass;
    if (BuildEachDef(pass) != Continue) {
      break;
    }
  }
}

void TPackage::BuildSymbol() {
  assert(this);
  assert(!Symbol);
  Symbol = Symbol::TPackage::New(Namespace, Version);
}

const Jhm::TNamespace &TPackage::GetNamespace() const {
  assert(this);
  return Namespace;
}

Symbol::TScope::TPtr TPackage::GetScopeSymbol() const {
  assert(this);
  return Symbol;
}

Symbol::TPackage::TPtr TPackage::GetSymbol() const {
  assert(this);
  return Symbol;
}

unsigned int TPackage::GetVersion() const {
  assert(this);
  return Version;
}

bool TPackage::HasSymbol() const {
  assert(this);
  return Symbol.get();
}

void TPackage::TTopLevelDefFactory::NewDefs(
    const TExprFactory *expr_factory,
    const Package::Syntax::TInstallerDef *&installer_def,
    const Package::Syntax::TOptDefSeq *opt_def_seq) {
  TTopLevelDefFactory(expr_factory, installer_def).TDefFactory::NewDefs(opt_def_seq);
}

TPackage::TTopLevelDefFactory::TTopLevelDefFactory(const TExprFactory *expr_factory, const Package::Syntax::TInstallerDef *&installer_def)
    : TDefFactory(expr_factory),
      InstallerDef(installer_def) {}

void TPackage::TTopLevelDefFactory::operator()(const Package::Syntax::TInstallerDef *that) const {
  if (!InstallerDef) {
    InstallerDef = that;
  } else {
    Tools::Nycr::TError::TBuilder(GetPosRange(that))
        << " package installer already defined at " << GetPosRange(InstallerDef);
  }
}

void TPackage::TTopLevelDefFactory::operator()(const Package::Syntax::TUpgraderDef *that) const {
  throw TNotImplementedError(HERE, TPosRange(
      that->GetPackageKwd()->GetLexeme().GetPosRange(),
      that->GetSemi()->GetLexeme().GetPosRange()));
}

void TPackage::TTopLevelDefFactory::operator()(const Package::Syntax::TUninstallerDef *that) const {
  throw TNotImplementedError(HERE, TPosRange(
      that->GetNotKwd()->GetLexeme().GetPosRange(),
      that->GetSemi()->GetLexeme().GetPosRange()));
}
