/* <stig/package/api.h>

   API for use by Stig packages. The package loader will call specific functions in a package to get the information.

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

#include <base/chrono.h>
#include <base/no_copy_semantics.h>
#include <stig/indy/key.h>
#include <stig/type.h>
#include <stig/var.h>
#include <stig/uuid.h>
#include <stig/package/api_version.h>

namespace Stig {

  namespace Package {

    /* Linked Packages provide two functions

    1) int32_t GetApiVersion();

    2) const TLinkInfo *GetLinkInfo(); */

    //Note: Assertion Predicates, Effects get gathered up in an update in the context.
    class TContext;
    struct TInfo;
    struct TTestCase;

    typedef std::unordered_map<std::string, Type::TType> TParamMap;
    typedef std::unordered_map<std::string, Indy::TKey> TArgMap;
    typedef std::unordered_map<Base::TUuid, std::pair<Stig::Type::TType, Stig::Type::TType>> TTypeByIndexIdMap;
    typedef std::unordered_set<Base::TUuid *> TIndexIdSet;

    /* Note: We use a vector here rather than an unordered set because we want test execution output to be deterministic.
       Yes, technically we can run test in arbitrary ordering, and there is no ordering inside a test block, but
       practically speakign doing so just creates a lot of unnnecessary / erroneous changes in outpu when looking at a
       specific test failure. */
    typedef std::vector<const TTestCase*> TTestBlock;

    /* Information about what is the primary (explicitly installed) package, as well as all packages which were compiled
       together to make the entire shared object. */
    struct TLinkInfo {
      /* The name of the overall package which is represented. This includes the full namespace of the package. */
      std::string PrimaryName;

      /* The version of the overall package which is represented. */
      uint64_t PrimaryVersion;

      /* The TInfo structure for the package which this shared object represents. */
      const TInfo *PrimaryInfo;

      /* A dictionary of all the packages which were compiled into this single shared object. These are the packages
         that were needed because of imports. */
      std::unordered_map<std::vector<std::string>, const TInfo*> IncludedPackages;

      /* A dictionary of the index ids to the <key, val> types they map to. This is an intermediate solution, as in the future we would
         like to be able to assign a different index even if this type combination is the same. This requires a tighter bond between the
         compiler and the server. */
      TTypeByIndexIdMap TypeByIndexIdMap;
    }; // TLinkInfo

    /* Information about a packages top level function. */
    struct TFuncInfo {

      /* All the parameter names and types which are required to be passed to the Runner */
      TParamMap Parameters;

      /* The return type of the function */
      Type::TType ReturnType;

      /* A runner which takes __exactly__ the arguments needed (will assertion fail otherwise) and returns the result
         of the function. The function, when called, may throw. The exceptions should be handled gracefully by the
         caller, and may indicate things such as Stig program assertion failures, multiple mutations of the same part of
         a key, etc. */
      Atom::TCore (*Runner)(TContext &, const TArgMap &);
    }; // TFuncInfo

    //NOTE: There are commonalities betwen test case and test, but inheritance messes with just using an initializer list in code gen.
    /* An individaul test case inside a package. */
    struct TTestCase {

      /* The optional name of the test. If it is not specified, it will be empty. */
      std::string Name;

      /* The code location of the test case. This is used primarily when there is no name give to the test case. */
      TPosRange Loc;

      /* The test case function. */
      const TFuncInfo *Func;

      /* A list of test cases which should be run if this test case passes inside the shared point of view created by
         running this test case. */
      TTestBlock SubCases;
    }; // TTestCase

    /* A test section inside a Stig program. A test block contains both a with, and a list of tests to run. */
    struct TTest {
      /* A function which, when run, will create all the database effects specified by the with clause. */
      const TFuncInfo *WithBlock;

      /* A collection of the test cases. */
      TTestBlock SubCases;
    }; // TTest

    /* Information about a single Stig program inside of a package */
    struct TInfo {
      /* The name of the package */
      std::string Name;
      /* The version of the package */
      uint64_t Version;

      /* All the functions in the program, by name. */
      std::unordered_map<std::string, const TFuncInfo*> Functions;

      /* All the test sections in the progam. */
      std::vector<const TTest*> Tests;

      /* A set of pointers to our Index Ids. This will allow the server to change their values to the mapped ones used in the database. */
      TIndexIdSet IndexIdSet;

    }; // TInfo

  } // Package

} // Stig