/* <stig/code_gen/package.h>

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

#include <unordered_map>

#include <base/uuid.h>
#include <stig/type/impl.h>
#include <stig/code_gen/export_func.h>
#include <stig/code_gen/id.h>
#include <stig/code_gen/test.h>
#include <stig/package/name.h>

namespace Stig {

  namespace CodeGen {

    class TPackage : public L0::TPackage {
      NO_COPY_SEMANTICS(TPackage);

      public:
      typedef std::unordered_set<Type::TType> TTypes;
      typedef std::unordered_set<TExportFunc::TPtr> TExports;

      //NOTE: We only make this a vector so that our output is more "stable". Also, hashing test containers would be a little annoying.
      typedef std::vector<TTest*> TTests;

      TPackage(const Symbol::TPackage::TPtr &package);
      virtual ~TPackage();

      void Emit(const Jhm::TAbsBase &out_dir) const;
      void EmitObjectHeaders(const Jhm::TAbsBase &out_dir) const;

      private:
      void WriteStartingComment(TCppPrinter &out, const Jhm::TRelPath &path) const;
      void WriteHeader(TCppPrinter &strm, const Jhm::TRelPath &path) const;
      void WriteCc(TCppPrinter &strm, const Jhm::TRelPath &path) const;
      void WriteLink(TCppPrinter &strm, const Jhm::TRelPath &path) const;
      //This one isn't really a CppPrinter...
      void WriteSignatures(TCppPrinter &strm, const Jhm::TRelPath &path) const;
      void WriteInclude(TCppPrinter &strm) const;
      void WriteImportIncludes(TCppPrinter &strm) const;

      //NOTE: When we are building out function definition trees, we make a new TId::TGen for each top level func. This
      //      same generator is used for every sub function. This makes the '&' lambda capture work.
      TTests Tests;
      TExports Exports;
      TTypes Objects;
      std::unordered_set<std::pair<Type::TType, Type::TType>> NeededObjectComparisons;
      std::unordered_set<Package::TName> NeededPackages;
    }; // TPackage


  } // CodeGen

} // Stig