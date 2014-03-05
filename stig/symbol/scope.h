/* <stig/symbol/scope.h>

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
#include <memory>
#include <unordered_set>
#include <vector>

#include <base/assert_true.h>
#include <base/no_copy_semantics.h>
#include <stig/symbol/root.h>

namespace Stig {

  namespace Symbol {

    /* Forward declarations */
    class TFunction;

    namespace Test {

      class TTest;

    }  // Test

    class TScope {
      NO_COPY_SEMANTICS(TScope);
      public:

      typedef std::shared_ptr<TScope> TPtr;

      typedef std::shared_ptr<TFunction> TFunctionPtr;

      typedef std::shared_ptr<Test::TTest> TTestPtr;

      typedef std::unordered_set<TFunctionPtr> TFunctionSet;

      typedef std::vector<TTestPtr> TTests;

      virtual ~TScope();

      void Add(const TFunctionPtr &function);

      void Add(const TTestPtr &test);

      const TFunctionSet &GetFunctions() const;

      const TTests &GetTests() const;

      void Remove(const TFunctionPtr &function);

      void Remove(const TTestPtr &test);

      void TypeCheck() const;

      protected:

      TScope();

      private:

      TFunctionSet Functions;

      TTests Tests;

    };  // TScope

  }  // Symbol

}  // Stig
