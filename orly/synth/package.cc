/* <orly/synth/package.cc>

   Implements <orly/synth/package.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/synth/package.h>

#include <iostream>

#include <base/as_str.h>
#include <orly/error.h>
#include <orly/synth/context.h>
#include <orly/synth/cst_utils.h>
#include <orly/synth/get_pos_range.h>
#include <orly/synth/new_expr.h>

using namespace Orly;
using namespace Orly::Synth;

TPackage::TPackage(const Package::TName &name, const Package::Syntax::TPackage *root, bool report_version)
    : Name(name), Symbol(nullptr) {
  assert(root);
  const Package::Syntax::TInstallerDef *installer_def = nullptr;
  TExprFactory expr_factory(this);
  TTopLevelDefFactory::NewDefs(&expr_factory, installer_def, root->GetOptDefSeq());

  if (installer_def) {
    Version = installer_def->GetPackageVersion()->GetIntLiteral()->GetLexeme().AsUInt32();
    auto using_stmt = TryGetNode<Package::Syntax::TUsingName, Package::Syntax::TNoUsing>(installer_def->GetOptUsing());
    if (using_stmt) {
      Package::TName index_name;

      Synth::ForEach<Package::Syntax::TName>(using_stmt->GetPackageNameMemberList(),
                                             [&index_name](const Package::Syntax::TName *name) {
        index_name.Name.push_back(name->GetLexeme().GetText());
        return true;
      });

      IndexName = Base::AsStr(index_name);
    }
  } else {
    Version = 0;
    IndexName = Base::AsStr(name);
  }

  if (report_version) {
    std::cout << Version << std::endl;
  } else {
    if (!GetContext().HasErrors()) {
      BuildSymbol();
      Build();
    }
  }
}

void TPackage::Build() const {
  assert(this);
  Bind();
  int pass = 0;
  while (!GetContext().HasErrors()) {
    ++pass;
    if (BuildEachDef(pass) != Continue) {
      break;
    }
  }
}

void TPackage::BuildSymbol() {
  assert(this);
  assert(!Symbol);
  Symbol = Symbol::TPackage::New(Name, IndexName, Version);
}

const Package::TName &TPackage::GetName() const {
  assert(this);
  return Name;
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
    GetContext().AddError(GetPosRange(that),
                          Base::AsStr("package installer already defined at ", GetPosRange(InstallerDef)));
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
