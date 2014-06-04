/* <jhm/job.cc>

   A registry of job types.

   Used for type checking between different job kinds / figuring out if an input is valid for a given job kind.

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

#include <jhm/job.h>

#include <cassert>

#include <base/split.h>
#include <jhm/file.h>

using namespace Base;
using namespace Jhm;

TJob::TJob(TFile *input, TSet<TFile *> output) : Input(input), Output(move(output)) {
  assert(input);
  for (TFile *f : Output) {
    assert(f);
  }
}

std::ostream &Jhm::operator<<(std::ostream &out, TJob *job) {
  out << job->GetName() << '{' << job->GetInput() << " -> [";
  Join(", ", job->GetOutput(), out);
  out << "]}";

  return out;
}
