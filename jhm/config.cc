/* <jhm/config.cc>

   Implements <jhm/config.h>

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

#include <jhm/config.h>

#include <base/not_implemented.h>
#include <base/split.h>

using namespace Base;
using namespace Jhm;
using namespace std;

TConfig::TConfig(TJson base_config) {
  AddConfig(move(base_config));
}

TJson TConfig::GetEntry(const string &name) const {
  TJson ret;
  if (!TryGetEntry(name, ret)) {
    THROW_ERROR(runtime_error) << "Didn't find config entry for " << quoted(name);
  }
  return ret;
}

/* Resolves a scope / sequence of names to a json blob. */
static const TJson *ResolveName(const TJson *start, const vector<string> &chunks) {
  // Walk through
  for(const auto &chunk : chunks) {
    if (!start->Contains(chunk)) {
      return nullptr;
    }
    start = &((*start)[chunk]);
  }
  return start;
}

//TODO: Switch to vector<string> for name always, rather than splitting apart?
bool TConfig::TryGetEntry(const string &name, TJson &out) const {
  vector<string> chunks;
  Base::Split(".", name, chunks);

  // For each config, starting at the top of the stack. If the config contains the entry, use it.
  // TODO: Add delta support
  for(auto &config: Config) {
    const TJson *val = ResolveName(&config, chunks);
    if (val) {
      // NOTE: We have no choice but to copy
      // We don't return a const pointer to the object because we're going to merge deltas shortly, which means building
      // a new temporary.
      out = *val;
      return true;
    }
  }
  return false;
}

void TConfig::AddBase(TJson &&config, bool top) {
  assert(!ComputedStart);
  assert(!ComputedLocked);

  AddConfig(move(config), top);
}

void TConfig::AddComputed(TJson &&config) {
  assert(!ComputedLocked);

  if(!ComputedStart) {
    ComputedStart = Config.size();
  }

  AddConfig(move(config));
}

bool TConfig::ForEachComputed(const function<bool (const TJson &conf)> &cb) const {
  auto last_it = Config.end() - *ComputedStart;
  for(auto it = Config.begin(); it != last_it; ++it) {
    if (!cb(*it)) {
      return false;
    }
  }
  return true;
}

void TConfig::WriteComputed(ostream &out) const {
  ComputedLocked = true;
  // Write all but the non-computed config
  //NOTE: We hand-roll the outer json array, because that's considerably cheaper than building a TJson and having that
  // pretty-print for us.
  //NOTE: WE hand do the join, because Join doesn't know iterators / ranges.
  out << '[';
  bool first = true;
  ForEachComputed([&first,&out] (const TJson &config) {
    if (first) {
      first = false;
    } else {
      out << ',';
    }
    config.Write(out);
    return true;
  });
  out << ']';
}

void TConfig::LoadComputed(const string &filename) {
  assert(!ComputedLocked);
  Base::TJson computed = TJson::Read(filename.c_str());
  assert(computed.GetKind() == TJson::Array);

  // Read backwards building up stack
  uint32_t computed_size = computed.GetSize();
  for(uint32_t i=0; i < computed_size; ++i) {
    AddComputed(move(computed[computed_size - (i+1)]));
  }
  ComputedLocked = true;
}
const std::vector<Base::TJson> TConfig::GetComputed() const {
  vector<Base::TJson> ret;
  assert(ComputedStart);
  ret.reserve(Config.size()-ComputedStart);
  ForEachComputed([&ret](const TJson &json) {
    ret.emplace_back(json);
    return true;
  });
  return ret;
}
void TConfig::SetComputed(std::vector<Base::TJson> &&conf_stack) {
  assert(!ComputedLocked);
  for(auto &config: conf_stack) {
    Config.emplace_back(move(config));
  }
  ComputedLocked = true;
}

void TConfig::AddConfig(TJson &&config, bool top) {
  assert(config.GetKind() == TJson::Object);

  if (top) {
    Config.emplace_front(move(config));
  } else {
    assert(!ComputedStart);
    assert(!ComputedLocked);
    Config.emplace_back(move(config));
  }
}
