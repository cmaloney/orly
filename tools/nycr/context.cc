/* <tools/nycr/context.cc>

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

#include <tools/nycr/context.h>

#include <cassert>

using namespace std;
using namespace Tools::Nycr;

void TContext::ClearErrors() {
  assert(this);
  Errors.clear();
}

bool TContext::HasErrors() const {
  assert(this);
  return !Errors.empty();
}

void TContext::AddError(TPosRange pos, std::string &&msg) {
  assert(this);
  Errors.emplace_back(pos, std::move(msg));
}

void TContext::SortErrors() {
  assert(this);
  std::sort(Errors.begin(), Errors.end(), [](const TError &first, const TError &second) {
    return std::get<0>(first) < std::get<0>(second);
  });
}

bool TContext::ForEachError(const std::function<bool (const TPosRange &pos, const string &msg)> &cb) const {
  assert(this);
  for(const auto &error: Errors) {
    if (!cb(std::get<0>(error), std::get<1>(error))) {
      return false;
    }
  }
  return true;
}

void TContext::PrintErrors(std::ostream &out) const {
  assert(this);
  for(const auto &err: Errors) {
    out << std::get<0>(err) << ' ' << std::get<1>(err) << '\n';
  }
}
