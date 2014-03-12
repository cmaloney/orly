/* <stig/sample.test.cc>

   Unit test for stig.

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

#include <fstream>
#include <iostream>
#include <string>

#include <base/make_dir.h>
#include <stig/compiler.h>
#include <stig/spa/honcho.h>
#include <stig/rt/runtime_error.h>
#include <stig/spa/service.h>
#include <stig/stig.package.cst.h>
#include <stig/var.h>

#include <test/kit.h>

using namespace std;
using namespace Stig;
using namespace Stig::Compiler;
using namespace Stig::Package;
using namespace Stig::Spa;

bool PrintCmds = false;  // Flag for Starsha's TRunner

TUUID POV_ID;

class TProgram {
  NO_COPY_SEMANTICS(TProgram);
  public:

  TProgram(const char *script) {
    ostringstream strm;
    strm << AbsBase << '/';
    TempFile = strm.str();
    Base::MakeDirs(TempFile.c_str());
    try {
      ofstream script_file(AbsPath.AsStr());
      script_file << script << endl;
      script_file.close();
    } catch (...) {
      throw;
    }
    try {
      Compile(AbsPath, AbsBase, false, true, false);
    } catch (const Compiler::TCompileFailure &ex) {
      cerr << "compile failure: " << ex.what() << endl;
      throw;
    }
    Service.GetPackageManager().Install({TVersionedName::Parse(Base::AsPiece(PackageName))});
  }

  ~TProgram() {
    Service.GetPackageManager().Uninstall({TVersionedName::Parse(Base::AsPiece(PackageName))});
    // unlink(TempFile.c_str());
  }

  void Try(const string &func, TService::TStigArg &args, const Var::TVar &expected) {
    try {
      Var::TVar var;
      //TODO: Needs notify povs. At least one.
      unordered_map<TUUID, TUUID> notifiers;
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Atom::TSuprena suprena;
      Atom::TCore result_core;
      Service.Try(Service.GetPackageManager().Get(Jhm::TNamespace("sample"))->GetFunctionInfo(Base::AsPiece(func)), POV_ID, unordered_set<TUUID>{}, args, result_core, suprena, notifiers);
      var = Var::ToVar(*Sabot::State::TAny::TWrapper(result_core.NewState(&suprena, state_alloc)));
      EXPECT_TRUE(var == expected);
    } catch (const std::exception &err) {
      cerr << err.what() << endl;
      throw;
    }
  }

  static TService Service;

  private:

  static THoncho Honcho;

  static const Jhm::TAbsBase AbsBase;

  static const Jhm::TAbsPath AbsPath;

  static const char *PackageName;

  std::string TempFile;

};  // TProgram

THoncho TProgram::Honcho;

TService TProgram::Service;

const Jhm::TAbsBase TProgram::AbsBase("/tmp/stig_sample/");

const Jhm::TAbsPath TProgram::AbsPath(AbsBase, Jhm::TRelPath("sample.stig"));

const char *TProgram::PackageName = "sample.0";

FIXTURE(Startup) {
  /* Parse command-line arguments */
  TProgram::Service.SetPackageDir("/tmp/stig_sample/");
  TUUID session;
  TProgram::Service.CreateSession(Base::TOpt<TUUID>(TUUID()), 1000, session);
  TProgram::Service.CreatePrivatePov(session, {}, 1000, false, POV_ID);
}

/*****************************************
************* ARITHMETIC *****************
*****************************************/
FIXTURE(Arithmetic) {
  TProgram program("x1 = 3 + 5;"
                   "x2 = 3 - 5;"
                   "x3 = 3 * 5;"
                   "x4 = 6 / 3;"
                   "x5 = 5 % 3;"
                   "x6 = 2 ** 5;");
  TService::TStigArg args;
  program.Try("x1", args, Var::TVar(8));
  program.Try("x2", args, Var::TVar(-2));
  program.Try("x3", args, Var::TVar(15));
  program.Try("x4", args, Var::TVar(2));
  program.Try("x5", args, Var::TVar(2));
  program.Try("x6", args, Var::TVar(32.0));
}
/*****************************************
************** EQUALITY ******************
*****************************************/
FIXTURE(Equality) {
  TProgram program("x01 = 3 == 5;"
                   "x02 = 5 == 5;"
                   "x03 = 5 == 3;"
                   "x04 = 3 > 5;"
                   "x05 = 5 > 5;"
                   "x06 = 5 > 3;"
                   "x07 = 3 >= 5;"
                   "x08 = 5 >= 5;"
                   "x09 = 5 >= 3;"
                   "x10 = 3 < 5;"
                   "x11 = 5 < 5;"
                   "x12 = 5 < 3;"
                   "x13 = 3 <= 5;"
                   "x14 = 5 <= 5;"
                   "x15 = 5 <= 3;"
                   "x16 = 3 != 5;"
                   "x17 = 5 != 5;"
                   "x18 = 5 != 3;");
  TService::TStigArg args;
  program.Try("x01", args, Var::TVar(false));
  program.Try("x02", args, Var::TVar(true));
  program.Try("x03", args, Var::TVar(false));
  program.Try("x04", args, Var::TVar(false));
  program.Try("x05", args, Var::TVar(false));
  program.Try("x06", args, Var::TVar(true));
  program.Try("x07", args, Var::TVar(false));
  program.Try("x08", args, Var::TVar(true));
  program.Try("x09", args, Var::TVar(true));
  program.Try("x10", args, Var::TVar(true));
  program.Try("x11", args, Var::TVar(false));
  program.Try("x12", args, Var::TVar(false));
  program.Try("x13", args, Var::TVar(true));
  program.Try("x14", args, Var::TVar(true));
  program.Try("x15", args, Var::TVar(false));
  program.Try("x16", args, Var::TVar(true));
  program.Try("x17", args, Var::TVar(false));
  program.Try("x18", args, Var::TVar(true));
}
/*****************************************
************** LOGICAL *******************
*****************************************/
FIXTURE(Logical) {
  TProgram program("x01 = true and true;"
                   "x02 = true and false;"
                   "x03 = false and true;"
                   "x04 = false and false;"
                   "x05 = true or true;"
                   "x06 = true or false;"
                   "x07 = false or true;"
                   "x08 = false or false;"
                   "x09 = true xor true;"
                   "x10 = true xor false;"
                   "x11 = false xor true;"
                   "x12 = false xor false;");
  TService::TStigArg args;
  program.Try("x01", args, Var::TVar(true));
  program.Try("x02", args, Var::TVar(false));
  program.Try("x03", args, Var::TVar(false));
  program.Try("x04", args, Var::TVar(false));
  program.Try("x05", args, Var::TVar(true));
  program.Try("x06", args, Var::TVar(true));
  program.Try("x07", args, Var::TVar(true));
  program.Try("x08", args, Var::TVar(false));
  program.Try("x09", args, Var::TVar(false));
  program.Try("x10", args, Var::TVar(true));
  program.Try("x11", args, Var::TVar(true));
  program.Try("x12", args, Var::TVar(false));
}
/*****************************************
**************** LIST ********************
*****************************************/
FIXTURE(List) {
  TProgram program("x01 = [1, 2, 3, 4];"
                   "x02 = [1, 2, 3, 4] + [5];");
  TService::TStigArg args;
  program.Try("x01", args, Var::TVar(vector<int64_t>({1,2,3,4})));
  program.Try("x02", args, Var::TVar(vector<int64_t>({1,2,3,4,5})));
}
/*****************************************
**************** SET *********************
*****************************************/
FIXTURE(Set) {
  TProgram program("x = {1, 2, 3, 4};");
  TService::TStigArg args;
  program.Try("x", args, Var::TVar(Rt::TSet<int64_t>({1,2,3,4})));
}
/*****************************************
**************** DICT ********************
*****************************************/
FIXTURE(Dict) {
  TProgram program("x = {1:1.1, 2:2.2, 3:3.3, 4:4.4};");
  TService::TStigArg args;
  program.Try("x", args, Var::TVar(Rt::TDict<int64_t, double>({make_pair(1, 1.1), make_pair(2, 2.2), make_pair(3, 3.3), make_pair(4, 4.4)})));
}
FIXTURE(DuplicateKeysRuntimeError) {
  TProgram program("key = 1;"
                   "x = {key:true, 1:false, 2:true};");
  TService::TStigArg args;
  auto func = [&]{ program.Try("x", args, Var::TVar(string("ExpectRuntimeError"))); };
  EXPECT_THROW_FUNC(Rt::TRuntimeError, func);
}

/*****************************************
**************** SEQ *********************
*****************************************/
FIXTURE(Seq) {
  TProgram program("x1 = [1..5] as [int];"
                   "x2 = [1..5) as [int];"
                   "x3 = [2, 4..6] as [int];"
                   "x4 = (y as [int]) where {"
                   "  y = [1..5];"
                   "};"
                   "x5 = ((y + 10) as [int]) where {"
                   "  y = [1..5];"
                   "};"
                   "x6 = ((y - 10) as [int]) where {"
                   "  y = [1..5];"
                   "};"
                   "x7 = ((y * 10) as [int]) where {"
                   "  y = [1..5];"
                   "};"
                   "x8 = ((y / 10) as [int]) where {"
                   "  y = [10, 20..50];"
                   "};"
                   "x9 = ((y ** 2) as [real]) where {"
                   "  y = [1..5];"
                   "};"
                   "x10 = ((((y + 5) -2) * 2) as [int]) where {"
                   "  y = [1..5];"
                   "};"
                   "x11 = ([y] as [[int]]) where {"
                   "  y = [1..5];"
                   "};"
                   "x12 = ([y] + [5] as [[int]]) where {"
                   "  y = [1..5];"
                   "};");
  TService::TStigArg args;
  program.Try("x1" , args, Var::TVar(vector<int64_t>({1,2,3,4,5})));
  program.Try("x2" , args, Var::TVar(vector<int64_t>({1,2,3,4})));
  program.Try("x3" , args, Var::TVar(vector<int64_t>({2,4,6})));
  program.Try("x4" , args, Var::TVar(vector<int64_t>({1,2,3,4,5})));
  program.Try("x5" , args, Var::TVar(vector<int64_t>({11,12,13,14,15})));
  program.Try("x6" , args, Var::TVar(vector<int64_t>({-9,-8,-7,-6,-5})));
  program.Try("x7" , args, Var::TVar(vector<int64_t>({10,20,30,40,50})));
  program.Try("x8" , args, Var::TVar(vector<int64_t>({1,2,3,4,5})));
  program.Try("x9" , args, Var::TVar(vector<double>({1.0,4.0,9.0,16.0,25.0})));
  program.Try("x10", args, Var::TVar(vector<int64_t>({8,10,12,14,16})));
  program.Try("x11", args, Var::TVar(vector<vector<int64_t>>({vector<int64_t>({1}),vector<int64_t>({2}),vector<int64_t>({3}),vector<int64_t>({4}),vector<int64_t>({5})})));
  program.Try("x12", args, Var::TVar(vector<vector<int64_t>>({vector<int64_t>({1,5}),vector<int64_t>({2,5}),vector<int64_t>({3,5}),vector<int64_t>({4,5}),vector<int64_t>({5,5})})));
}
