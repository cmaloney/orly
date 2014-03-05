/* <stig/code_gen/top_func.h>

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

#include <stig/code_gen/function.h>

namespace Stig {

  namespace CodeGen {

    class TTopFunc : public virtual TFunction {
      public:

      //TODO: GetName, WriteName, and WriteCcName seem like they should be merged somehow...
      virtual void WriteCcName(TCppPrinter &out) const = 0;
      bool IsTopLevel() const;
      void WriteDecl(TCppPrinter &out) const;
      void WriteDef(TCppPrinter &out) const;

      protected:
      TTopFunc(const L0::TPackage *package);
    }; // TTopFunc


  } // CodeGen

} // Stig