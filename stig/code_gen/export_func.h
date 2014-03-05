/* <stig/code_gen/export_func.h>

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

#include <stig/code_gen/symbol_func.h>
#include <stig/code_gen/top_func.h>

namespace Stig {

  namespace CodeGen {

    class TExportFunc : public TTopFunc, public TSymbolFunc {
      public:
      static TPtr New(const L0::TPackage *package, const Jhm::TNamespace &ns, const Symbol::TFunction::TPtr &symbol);

      virtual void WriteCcName(TCppPrinter &out) const;
      void WriteName(TCppPrinter &out) const;

      private:
      TExportFunc(const L0::TPackage *package, const Jhm::TNamespace &ns, const Symbol::TFunction::TPtr &symbol);

      Jhm::TNamespace Namespace;
    }; // TExportFunc


  } // CodeGen

} // Stig