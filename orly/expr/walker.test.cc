/* <orly/expr/walker.test.cc>

   This is intended to test the ForEachExpr to make sure it is walking the entire tree.
   Notably, does it collect all the objects that may need to be created?

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

#include <orly/orly.package.cst.h>

#include <orly/compiler.h>
#include <orly/expr/walker.h>
#include <orly/synth/package.h>
#include <orly/symbol/package.h>
#include <orly/symbol/scope.h>
#include <orly/type/object_collector.h>
#include <orly/type/type_czar.h>
#include <tools/nycr/error.h>

#include <test/kit.h>

using namespace Orly::Package::Syntax;
using namespace Orly;
using namespace std;

static Type::TTypeCzar type_czar;

/* Helper function turns a string into a TPackage (by reading in the concrete syntax tree)

NOTE: Copied in <orly/expr/addr_walker.test.cc> */
bool PrintCmds = false;

Symbol::TPackage::TPtr MakePackage(const string &orlyscript) {
  TPackage *cst = TPackage::ParseStr(orlyscript.c_str());
  if(Tools::Nycr::TError::GetFirstError()) {
    //Syntax error!
    Tools::Nycr::TError::PrintSortedErrors(std::cout);
    Tools::Nycr::TError::DeleteEach();
    throw std::invalid_argument("Orlyscript has syntax errors");
  }
  try {
    auto synth = new Orly::Synth::TPackage(Jhm::TNamespace(""), cst, false);

    return synth->GetSymbol();
  } catch (...) {
    delete cst;
    throw;
  }
}

/* Given a Orlyscript string, collects all of the objects used in the script */
void CollectObjects(const string &orlyscript, unordered_set<Orly::Type::TType> &objects) {
  Symbol::TPackage::TPtr my_package = MakePackage(orlyscript);
  assert(my_package);
  for (const auto &func: my_package->GetFunctions()) {
    Orly::Expr::TExpr::TPtr expr = func->GetExpr();
    Expr::ForEachExpr(expr, [&objects](const Expr::TExpr::TPtr &expr) {
      Orly::Type::CollectObjects(expr->GetType(), objects);
      return false;
      }, true);
  };
}

FIXTURE(Simple) {
  string simplescript =
    "write_obj = ((true) effecting {"
    "     new <[name]> <- <{ .foo:1 }>;"
    "   }) where { "
    "     name = given::(str);"
    "   };"
    "my_obj is <{ .foo: str }>;"
    "read_obj = (*<[name]>::(my_obj)) where {"
    "  name = given::(str);"
    "};";
  unordered_set<Orly::Type::TType> objects;
  CollectObjects(simplescript, objects);
  EXPECT_EQ(objects.size(), 2U);
}

FIXTURE(EmbeddedObjs) {
  string sscript =
    "George = {1943-02-25T0:0:0Z};"
    "George_as_obj = time_obj George;"
    "person_obj is <{ .name: str, .birthday: time_pnt }>;"
    "write_person = ((true) effecting {"
    "  new <[\"name\", person.name]> <- <{ .name:person.name, .birthday:person.birthday }>;"
    "  new <[\"birthmonth\", person.birthday]> <- <{ .name:person.name, .month:(time_obj person.birthday).month }>;"
    "}) where {"
    "   person = given::(person_obj);"
 "};";
  unordered_set<Orly::Type::TType> objects;
  CollectObjects(sscript, objects);
  EXPECT_EQ(objects.size(), 3U);
}

//TODO: Update the test case to be valid orlyscript.
#if 0
FIXTURE(PeopledirHistogram) {
  string histogram =
    "find_by_first_name = ( ( keys <[fname, free::(str)]>).1 as [str] ) where {"
    "  fname = given::(str);"
    "};"
    "histogram = (first_names_with_counts(.fname:first_names) as [namepair] sorted_by lhs.count > rhs.count) where {"
    "  namepair is <{.fname: str, .count: int}>;"
    "  first_names = **(keys <[free::(str), free::(str)]>.1 as {str});"
    "  first_names_with_counts = (<{.fname: fname, .count: length_of ( find_by_first_name (.fname: fname ))}>) where {"
    "      fname = given::(str);"
    "  };"
    "};";
  unordered_set<Orly::Type::TType> objects;
  CollectObjects(histogram, objects);
  EXPECT_EQ(objects.size(), 1U);
}
#endif