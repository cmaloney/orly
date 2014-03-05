/* <stig/synth/addr_dir_visitor.h>

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
#include <stig/shared_enum.h>

namespace Stig {

  namespace Synth {

      class TAddrDirVisitor
          : public Package::Syntax::TOptOrdering::TVisitor {
        NO_COPY_SEMANTICS(TAddrDirVisitor);
        public:

        TAddrDirVisitor(TAddrDir &addr_dir)
            : AddrDir(addr_dir) {}

        const TAddrDir &GetAddrDir() const {
          assert(this);
          return AddrDir;
        }

        private:

        virtual void operator()(const Package::Syntax::TNoOrdering   *) const final { AddrDir = Asc; /* Default */ }
        virtual void operator()(const Package::Syntax::TDescOrdering *) const final { AddrDir = Desc; }
        virtual void operator()(const Package::Syntax::TAscOrdering  *) const final { AddrDir = Asc;  }

        TAddrDir &AddrDir;

      };  // TAddrDirVisitor

  }  // Synth

}  // Stig