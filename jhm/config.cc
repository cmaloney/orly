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

#include <fstream>
#include <iostream>

#include <base/split.h>

using namespace Base;
using namespace Jhm;
using namespace std;

TJson Jhm::ReadConfig(const string &filename) {
  ifstream in(filename);
  if(!in.is_open()) {
    THROW_ERROR(runtime_error) << "Unable to open config file " << quoted(filename);
  }
  TJson ret;
  ret.Read(in);
  return ret;
}

TConfig::TConfig(TJson base_config) {
  Push(move(base_config));
}

#include <base/not_implemented.h>

// Returns true if it added at least one item to entry.
// Searches down the config, finding entries that begin with the chunk key ('+'/'-'/'=' prefix just means deltas for the key)
// entry must be either Null, in which case we can promote it arbitrarily or an object (which means we can add).
// NOTE: Returns true iff the object couldn't be added to by another delta (We crossed a force-assign key or set it to a hard value).
//TODO: implement delta-based config sections (Add/remove/etc. items from a section explicitly with '+'/'-'/'=' prefix or postfix)
bool AddDeltas(const TJson &config, const vector<string> &chunks, TJson &entry) {
  assert(entry.GetKind() == TJson::Null || entry.GetKind() == TJson::Object);

  // Get to the bit we want;
  const Base::TJson *cur_elem = &config;
  for(const auto &chunk : chunks) {
    if (!cur_elem->Contains(chunk)) {
      return false; // Not in the given config
    }
    cur_elem = &((*cur_elem)[chunk]);
  }

  if (entry.GetKind() == TJson::Object) {
    if (cur_elem->GetKind() != TJson::Object) {
      // Found the end (Config overridden with different type).
      // TODO: Should this be an error?
      return true;
    }

    cur_elem->ForEachElem([&entry] (const std::string &key, const TJson &val) -> bool {
      // Insert if not exists.
      if (!entry.Contains(key)) {
        entry[key] = val;
      }
      return true;
    });
  } else if (entry.GetKind() == TJson::Array) {
    NOT_IMPLEMENTED_S("Merge multiple arrays at different configuration levels");
  } else {
    entry = *cur_elem;
  }

  return true;
}

TJson TConfig::GetEntry(const string &name) const {
  TJson ret;
  if (!TryGetEntry(name, ret)) {
    THROW_ERROR(runtime_error) << "Didn't find config entry for " << quoted(name);
  }
  return ret;
}


bool TConfig::TryGetEntry(const string &name, TJson &out) const {
  vector<string> chunks;
  Base::Split(".", name, chunks);

  bool found_something = false;
  TJson entry;

  // For each config, starting at the top of the stack. If the config contains the entry, use it.
  // If the config contains a section named the entry, use it as a basis.
  // If any entries are deltas, apply them
  // Continue going until we get to a hard assign for the entry or any level above it
  for(auto &config: ConfigStack) {
    if(found_something |= AddDeltas(config, chunks, entry)) {
      break;
    }
  }
  if (found_something) {
    out = entry;
  }
  return found_something;
}

void TConfig::Push(Base::TJson &&config) {
  assert(config.GetKind() == TJson::Object);
  ConfigStack.emplace_front(move(config));
}

void TConfig::PushBack(Base::TJson &&config) {
  assert(config.GetKind() == TJson::Object);
  ConfigStack.emplace_back(move(config));
}