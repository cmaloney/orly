/* <stig/synth/def_factory.h>

   TODO

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

#pragma once

#include <stig/stig.package.cst.h>
#include <stig/synth/func_def.h>
#include <stig/synth/scope_and_def.h>

namespace Stig {

  namespace Synth {

    /* Forward declaration. */
    class TExprFactory;

    /* TODO */
    class TDefFactory
        : public Package::Syntax::TDef::TVisitor {
      public:

      /* TODO */
      virtual ~TDefFactory();

      protected:

      /* TODO */
      TDefFactory(const TExprFactory *expr_factory);

      /* TODO */
      void NewDefs(const Package::Syntax::TOptDefSeq *opt_def_seq) const;

      virtual void operator()(const Package::Syntax::TInstallerDef *that) const = 0;
      virtual void operator()(const Package::Syntax::TUpgraderDef *that) const = 0;
      virtual void operator()(const Package::Syntax::TUninstallerDef *that) const = 0;

      private:

      virtual void operator()(const Package::Syntax::TBadDef *that) const;
      virtual void operator()(const Package::Syntax::TFuncDef *that) const;
      virtual void operator()(const Package::Syntax::TTypeDef *that) const;
      virtual void operator()(const Package::Syntax::TImportDef *that) const;
      virtual void operator()(const Package::Syntax::TGeneratorDef *that) const;
      virtual void operator()(const Package::Syntax::TPackageDef *that) const;
      virtual void operator()(const Package::Syntax::TTestDef *that) const;

      /* TODO */
      const TExprFactory *ExprFactory;

    };  // TDefFactory

  }  // Synth

}  // Stig
