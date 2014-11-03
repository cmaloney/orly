/* <jhm/jobs/nycr.cc>

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

#include <jhm/jobs/nycr.h>

#include <cassert>
#include <stdexcept>

#include <base/as_str.h>
#include <base/split.h>
#include <jhm/env.h>
#include <jhm/file.h>

using namespace Base;
using namespace Jhm;
using namespace Jhm::Job;
using namespace std;
using namespace Util;

static string GetNycrPath(TEnv &env) { return AsStr(*env.GetRoot()) + "/out/bootstrap/tools/nycr/nycr"; }

static TOpt<TRelPath> GetNycrLangInputName(const TRelPath &output) {
  if (output.Path.EndsWith({"nycr", "lang"})) {
    return TRelPath(DropExtension(TPath(output.Path), 1));
  }

  return TOpt<TRelPath>();
}

TJobProducer TNycrLang::GetProducer() {
  return TJobProducer{
    "nycr_lang",
    {{"nycr","lang"}},
    GetNycrLangInputName,
    [](TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
      return unique_ptr<TJob>(new TNycrLang(env, in_file));
    }
  };
}

const char *TNycrLang::GetName() {
  return "nycr_lang";
}

const unordered_set<TFile*> TNycrLang::GetNeeds() {
  return unordered_set<TFile*>();
}

vector<string> TNycrLang::GetCmd() {
  return vector<string>{GetNycrPath(Env), "-l", "--language-report-file",
                        GetSoleOutput()->GetPath(), GetInput()->GetPath()};
}

TTimestamp TNycrLang::GetCmdTimestamp() const {
  static TTimestamp timestamp = GetTimestamp(GetNycrPath(Env));
  return timestamp;
}

bool TNycrLang::IsComplete() {
  // Stash the languages in a machine-readable form.
  TJson languages = TJson::Read(AsStr(GetSoleOutput()->GetPath()).c_str());
  assert(languages.GetKind() == TJson::Array);
  GetSoleOutput()->PushComputedConfig(TJson::TObject{{"nycr", TJson::TObject{{"languages", move(languages)}}}});
  return true;
}

TNycrLang::TNycrLang(TEnv &env, TFile *input)
    : TJob(input, {env.GetFile(TRelPath(AddExtension(TPath(input->GetRelPath().Path), {"lang"})))}), Env(env) {}


/*

  foo.nycr ->
    for (lang: languages) {
      foo.lang.cst.h
      foo.lang.cst.cc
      foo.lang.dump.cc
      foo.lang.y
      foo.lang.bison.h
      foo.lang.l
      foo.lang.flex.h
      foo.lang.xml
      foo.lang.nycr
    }
*/

const static vector<vector<string>> LangSuffixes = {
  {"cst","h"},
  {"cst", "cc"},
  {"dump", "cc"},
  {"y"},
  {"bison", "h"},
  {"l"},
  {"flex", "h"},
  {"xml"},
  {"nycr"}
};

static TOpt<TRelPath> GetInputName(const TRelPath &output) {
  // If we end with a lang followed by a language suffix, get the nycr input name
  for (const auto &suffix: LangSuffixes) {
    if(output.Path.Extension.size() >= suffix.size() + 1 && output.Path.EndsWith(suffix)) {
      return TRelPath(AddExtension(DropExtension(TPath(output.Path), suffix.size() + 1), {"nycr"}));
    }
  }

  return TOpt<TRelPath>();
}

TJobProducer TNycr::GetProducer() {
  return TJobProducer{
    "nycr",
    LangSuffixes,
    GetInputName,
    //TODO: Should be able to eliminate the lambda wrapper here...
    [] (TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
      return unique_ptr<TJob>(new TNycr(env, in_file));
    }
  };
}

const char *TNycr::GetName() {
  return "nycr";
}

const unordered_set<TFile*> TNycr::GetNeeds() {
  assert(this);

  return {Need};
}

vector<string> TNycr::GetCmd() {
  assert(this);

  // Use the need to get the list of languages, finalize our output set.
  vector<string> languages = Need->GetConfig().Read<vector<string>>({"nycr","languages"});

  // Verify that everything in our stated output set is indeed produced by the file.
  // Also add all the files which aren't mentioned.
  unordered_set<TFile*> old_outputs = GetOutput();
  for(const string &lang: languages) {
    for (const auto &ext: LangSuffixes) {
      TFile *out = Env.GetFile(TRelPath(AddExtension(SwapExtension(TPath(GetInput()->GetRelPath().Path), {lang}), ext)));
      auto it = old_outputs.find(out);
      if (it == old_outputs.end()) {
        AddOutput(out);
      } else {
        old_outputs.erase(it);
      }
    }
  }
  if(old_outputs.size() > 0) {
    THROW_ERROR(logic_error) << " Nycr said to produce files (" << Join(old_outputs, ", ")
                             << "), but doesn't produce that language.";
  }
  MarkAllOutputsKnown();

  //TODO: use nycr in path?
  return vector<string> {
    GetNycrPath(Env),
    "-a",
    GetInput()->GetRelPath().Path.Name,
    "-p",
    AsStr(Join(GetInput()->GetRelPath().Path.Namespace, '/')),
    // Note: This finding of the output root is correct, but should really be a call to a helper function.
    "-r",
    AsStr(*Env.GetOut()) + AsStrFunc(WriteNamespace, GetInput()->GetRelPath().Path.Namespace, true),
    GetInput()->GetPath()
  };
}

TTimestamp TNycr::GetCmdTimestamp() const {
  return GetTimestamp(GetNycrPath(Env));
}

bool TNycr::IsComplete() {
  assert(this);

  return true;
}

TNycr::TNycr(TEnv &env, TFile *in_file)
    : TJob(in_file, TSet<TFile *>(), true),
      Env(env),
      Need(env.GetFile(TRelPath(AddExtension(TPath(in_file->GetRelPath().Path), {"lang"})))) {}
