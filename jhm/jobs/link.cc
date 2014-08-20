/* <jhm/jobs/link.cc>

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

#include <jhm/jobs/link.h>

#include <queue>
#include <sstream>

#include <base/split.h>
#include <jhm/env.h>
#include <jhm/file.h>

using namespace Base;
using namespace Jhm;
using namespace Jhm::Job;
using namespace std;
using namespace Util;

static TRelPath GetOutputName(const TRelPath &input) {
  assert(input.Path.EndsWith({"o"}));
  return TRelPath(SwapExtension(TPath(input.Path), {""}));
}

static TOpt<TRelPath> GetInputName(const TRelPath &output) {
  //TODO: Allow non-trivial prefixes before the empty extension.
  const auto &ext = output.Path.Extension;
  if (ext.size() > 0 && ext.at(ext.size()-1) == "") {
    return TRelPath(SwapExtension(TPath(output.Path), {"o"}));
  }

  return TOpt<TRelPath>();
}

TJobProducer TLink::GetProducer() {
  return TJobProducer{
    "link",
    {{""}},
    GetInputName,
    //TODO: Should be able to eliminate the lambda wrapper here...
    [] (TEnv &env, TFile *in_file) -> unique_ptr<TJob> {
      return unique_ptr<TJob>(new TLink(env, in_file));
    }
  };
}

const char *TLink::GetName() {
  return "link";
}

const unordered_set<TFile*> TLink::GetNeeds() {
  assert(this);

  // Seed the link search with the core input file.
  if (ObjFiles.empty()) {
    TFile *input = GetInput();
    ObjToCheck.insert(input);
    ObjFiles.insert(input);
  }

  // We build a queue so we can fill ObjToCheck as we go, and add more to be processed while we iterate.
  std::queue<TFile *> to_check;
  for (TFile *obj: ObjToCheck) {
    to_check.push(obj);
  }
  ObjToCheck.clear();

  // Get all the link objects of every link object until we have a complete set of link objects.
  vector<string> filtered_includes; // Hoisted out of loop
  while (!to_check.empty()) {
    TFile *obj = Pop(to_check);
    if (!Env.IsDone(obj)) {
      ObjToCheck.insert(obj);
      continue;
    }

    // Read out the cached link args, add them to our link set.
    if (!obj->GetConfig().TryRead({"c++","filtered_includes"}, filtered_includes)) {
      continue;
    }

    for(const auto &include: filtered_includes) {
      TFile *include_file = Env.TryGetFileFromPath(include);
      assert(include_file);
      if (!include_file) {
        THROW_ERROR(std::logic_error)
            << "Internal Error; We didn't find the C++ source file which should be in the src tre...";
      }
      TFile *obj_file = Env.GetFile(TRelPath(SwapExtension(TPath(include_file->GetRelPath().Path), {"o"})));
      if (Env.IsBuildable(obj_file)) {
        // Add the link. If it's new, queue it to be checked for new links that we need
        if (ObjFiles.insert(obj_file).second) {
          to_check.push(obj_file);
        }
      } else {
        AntiNeeds.insert(obj_file);
      }
    }
  }

  return ObjFiles;
}

unordered_set<TFile*> TLink::GetAntiNeeds() {
  return AntiNeeds;
}

string TLink::GetCmd() {
  assert(this);
  ostringstream oss;

  //TODO: If there are no C++ files, use 'gcc' to link instead of g++
  oss
    << "g++ -o " << GetSoleOutput()->GetPath() << ' '
    << Join(Env.GetConfig().Read<vector<string>>({"cmd","ld","flags"}), ' ');

  oss << ' ';

  // Link against every needed object file
  oss << Join(ObjFiles,
              ' ',
              [] (ostream &out, TFile *f) {
                out << f->GetPath();
              });

  // TODO: Be more intelligent / selective about link flags
  oss << ' ' << Join(Env.GetConfig().Read<vector<string>>({"cmd","ld","libs"}), ' ');

  return oss.str();
}

TTimestamp TLink::GetCmdTimestamp() const {
  static TTimestamp timestamp = GetTimestampSearchingPath("g++");
  return timestamp;
}

bool TLink::IsComplete() {
  assert(this);

  return true;
}


TLink::TLink(TEnv &env, TFile *in_file)
    : TJob(in_file, {env.GetFile(GetOutputName(in_file->GetRelPath()))}), Env(env) {}
