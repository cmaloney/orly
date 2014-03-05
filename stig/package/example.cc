/* <stig/package/example.cc>

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
/* Link file. */

uint64_t GetApiVersion() {
  return 1;
}

static TLinkInfo LinkInfo {
  "baz", //Package name
  { //Included Package map
    {{"foo"}, Foo::GetPackageInfo},
    {{"bar", "baz"}, Bar::Baz::GetPackageInfo()}
  }
};

TLinkInfo *GetLinkInfo() {
  return &LinkInfo;
}

/* Package file */

//TODO
void WithBlock1(TContext &ctx);

static TTestCase TestCase1 {
  "name",
  TestFunc1,
  /* Child test cases*/ {
    &TTestCase2
  }
};

static TTest Test1 {
  WithBlock1,
  /* TTestCase */ {
    &TestCase1
  }
};

std::unordered_map<std::string, Type::TType> GetParams_foobar() {
  return {{"a", Type::TInt::Get()}}
}

Var::TVar Run_foobar(TContext &ctx, const std::unordered_map<std::string, Var::TVar> &args, TUpdate &*out);

static const TFuncInfo FuncInfo_foobar ;

static const TPackageInfo Foo::PackageInfo {
  "foobar",
  int64_t verno,
  /* exported functions */ {
    {"foobar", {
      true, //May have side efects (not a candidate for /do)
      GetParams_foobar,
      Run_foobar
    }
  }
    {"whizbang", &FuncInfo_whizbang }
  },
  /* tests */ {
    &Test1,
    &Test2
  }
};

const TPackageInfo *Foo::GetPackageInfo() {
  return &PackageInfo;
}