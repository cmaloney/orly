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

static TRelPath GetOutputName(const TRelPath &input) {
  assert(EndsWith(input.GetName().GetExtensions(), {"o"}));
  return input.SwapLastExtension("");
}

static TOpt<TRelPath> GetInputName(const TRelPath &output) {
  //TODO: Allow non-trivial prefixes before the empty extension.
  const auto &ext = output.GetName().GetExtensions();
  if (ext.size() > 0 && ext.at(ext.size()-1) == "") {
    return output.SwapLastExtension("o");
  }

  return *TOpt<TRelPath>::Unknown;
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
  if (!StartedNeeds) {
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
  vector<string> link_deps; // Hoisted out of loop
  while (!to_check.empty()) {
    TFile *obj = Pop(to_check);
    if (!Env.IsDone(obj)) {
      ObjToCheck.insert(obj);
      continue;
    }

    // Read out the cached link args, add them to our link set.
    if (!obj->GetConfig().TryRead("ld.link", link_deps)) {
      continue;
    }

    for(const auto &link_name: link_deps) {
      // Add the link. If it's new, queue it to be checked for new links that we need
      TFile *link = Env.TryGetFileFromPath(link_name);
      if (ObjFiles.insert(link).second) {
        to_check.push(link);
      }
    }
  }

  return ObjFiles;
}

string TLink::GetCmd() {
  assert(this);
  ostringstream oss;

  //TODO: If there are no C++ files, use 'gcc' to link instead of g++
  oss << "g++ -o " << GetSoleOutput()->GetPath() << ' ';

  Join(' ', Env.GetConfig().Read<vector<string>>("cmd.ld.flags"), oss);

  oss << ' ';

  // Link against every needed object file
  Join(' ', ObjFiles, [] (TFile *f, ostream &out) {
    out << f->GetPath();
  }, oss);

  // TODO: Be more intelligent / selective about link flags
  oss << ' ';
  Join(' ', Env.GetConfig().Read<vector<string>>("cmd.ld.libs"), oss);

  return oss.str();
}

bool TLink::IsComplete() {
  assert(this);

  return true;
}


TLink::TLink(TEnv &env, TFile *in_file)
    : TJob(in_file, {env.GetFile(GetOutputName(in_file->GetPath().GetRelPath()))}), Env(env) {}