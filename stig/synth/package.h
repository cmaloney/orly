/* <stig/synth/package.h>

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

#include <cassert>

#include <base/assert_true.h>
#include <base/no_copy_semantics.h>
#include <jhm/naming.h>
#include <stig/symbol/package.h>
#include <stig/synth/def_factory.h>
#include <stig/synth/scope_and_def.h>

namespace Stig {

  namespace Synth {

    /* Synth TPackage class */
    class TPackage
        : public TScope {
      NO_COPY_SEMANTICS(TPackage);
      public:

      /* Constructor */
      TPackage(const Jhm::TNamespace &ns, const Package::Syntax::TPackage *root, bool report_version = false);

      /* This is the master driver for the build process.  Call this on the root scope and,
         by the time it returns, everything will have been bound and built. */
      void Build() const;

      /* Builds the symbol */
      void BuildSymbol();

      /* TODO */
      const Jhm::TNamespace &GetNamespace() const;

      /* Return the scope version of the package symbol */
      Symbol::TScope::TPtr GetScopeSymbol() const;

      /* Return the package symbol */
      Symbol::TPackage::TPtr GetSymbol() const;

      /* Return the package version number */
      unsigned int GetVersion() const;

      /* Check if Symbol is set */
      bool HasSymbol() const;

      private:

      /* Top level DefFactory */
      class TTopLevelDefFactory
          : public TDefFactory {
        NO_COPY_SEMANTICS(TTopLevelDefFactory);
        public:

        /* TODO */
        static void NewDefs(
            const TExprFactory *expr_factory,
            const Package::Syntax::TInstallerDef *&installer_def,
            const Package::Syntax::TOptDefSeq *opt_def_seq);

        private:

        /* TODO */
        TTopLevelDefFactory(const TExprFactory *expr_factory, const Package::Syntax::TInstallerDef *&installer_def);

        virtual void operator()(const Package::Syntax::TInstallerDef *that) const;
        virtual void operator()(const Package::Syntax::TUpgraderDef *that) const;
        virtual void operator()(const Package::Syntax::TUninstallerDef *that) const;

        /* TODO */
        const Package::Syntax::TInstallerDef *&InstallerDef;

      };  // TTopLevelDefFactory

      /* See accesor */
      Jhm::TNamespace Namespace;

      /* See accesor */
      Symbol::TPackage::TPtr Symbol;

      /* See accessor */
      unsigned int Version;

    };  // TPackage

  }  // Synth

}  // Stig
