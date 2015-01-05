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

#include <util/path.h>

using namespace Base;
using namespace Jhm;
using namespace std;
using namespace Util;

TConfig::TConfig() {}

TConfig::TConfig(const string &filename) { AddFile(filename); }

TConfig::TConfig(const vector<string> &files) {
  for(const string &filename : files) {
    AddFile(filename);
  }
}

TJson TConfig::GetEntry(const initializer_list<string> &name) const {
  TJson ret;
  if(!TryGetEntry(name, ret)) {
    THROW_ERROR(TNotFound) << "Didn't find config entry for \"" << Join(name, '.') << '\"';
  }
  return ret;
}

/* Resolves a scope / sequence of names to a json blob. */
static const TJson *ResolveName(const TJson *start,
                                const initializer_list<string> &name,
                                bool &is_delta) {
  // Walk through
  auto end = name.end();
  for(auto it = name.begin(); it != end; ++it) {
    if(!start->Contains(*it)) {
      // Check for deltas. NOTE: Deltas must be at the last layer of the config asked for only.
      // NOTE: Currently only adding deltas are supported.
      string delta_name = '+' + *it;
      if(start->Contains(delta_name)) {
        if(it != end - 1) {
          THROW_ERROR(TJson::TSyntaxError) << "Explicit delta configuration can only be specified "
                                              "at the last part of a config option";
        }
        is_delta = true;
        return &((*start)[delta_name]);
      } else {
        return nullptr;
      }
    }
    start = &((*start)[*it]);
  }
  return start;
}

// NOTE: The delta is also the return
TJson ApplyDelta(const TJson &base, TJson &&delta) {
  TJson ret(base);

  if(ret.GetKind() != delta.GetKind()) {
    THROW_ERROR(TJson::TSyntaxError)
        << "Delta configuration must always be between same type. base type: " << ret.GetKind()
        << " delta type: " << delta.GetKind();
  }

  if(ret.GetKind() == TJson::Object) {
    // Add / replace keys in base with the delta's values.
    for (auto &elem: delta.GetObject()) {
      ret[elem.first] = move(elem.second);
    }
  } else if(ret.GetKind() == TJson::Array) {
    // Append our array to their array
    TJson::TArray &that = ret.GetArray();
    for (TJson &elem: delta.GetArray()) {
      that.emplace_back(move(elem));
    }
  } else {
    THROW_ERROR(TJson::TSyntaxError)
        << "Delta configuration is only supported on JSON arrays and objects. Got a "
        << ret.GetKind();
  }

  return ret;
}

// TODO: Switch to vector<string> for name always, rather than splitting apart?
bool TConfig::TryGetEntry(const initializer_list<string> &name, TJson &out) const {
  // For each config, starting at the top of the stack. If the config contains the entry, use it.
  // TODO: Add delta support
  TJson ret;
  bool has_delta = false;
  for(auto &config : Config) {
    bool is_delta = false;
    const TJson *val = ResolveName(&config, name, is_delta);
    if(val) {
      if(has_delta || is_delta) {
        if(!has_delta) {
          ret = *val;
          has_delta = true;
          continue;
        }
        // Apply the delta to the given object
        ret = ApplyDelta(*val, move(ret));
        // TODO: Cope with the delta
      } else {
        // Raw assign. End of stack / return.
        out = *val;
        return true;
      }
    }
  }
  if(has_delta) {
    out = move(ret);
  }
  return has_delta;
}

void TConfig::AddComputed(TJson &&config) {
  assert(!ComputedLocked);

  if(!ComputedStart) {
    ComputedStart = Config.size();
  }

  AddConfig(move(config));
}

bool TConfig::ForEachComputed(const function<bool(const TJson &conf)> &cb) const {
  auto last_it = Config.end() - *ComputedStart;
  for(auto it = Config.begin(); it != last_it; ++it) {
    if(!cb(*it)) {
      return false;
    }
  }
  return true;
}

void TConfig::WriteComputed(ostream &out) const {
  ComputedLocked = true;
  // Write all but the non-computed config
  // NOTE: We hand-roll the outer json array, because that's considerably cheaper than building a
  // TJson and having that
  // pretty-print for us.
  // NOTE: WE hand do the join, because Join doesn't know iterators / ranges.
  out << '[';
  bool first = true;
  ForEachComputed([&first, &out](const TJson &config) {
    if(first) {
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
  if(computed.GetKind() != TJson::Array) {
    throw TInvalidConfig("Invalid config file. Top level isn't a JSON Array.");
  }

  // Read backwards building up stack
  uint64_t computed_size = computed.GetSize();
  for(uint64_t i = 0; i < computed_size; ++i) {
    AddComputed(move(computed[computed_size - (i + 1)]));
  }
  ComputedLocked = true;
}
const std::vector<Base::TJson> TConfig::GetComputed() const {
  vector<Base::TJson> ret;
  assert(ComputedStart);
  ret.reserve(Config.size() - ComputedStart);
  ForEachComputed([&ret](const TJson &json) {
    ret.emplace_back(json);
    return true;
  });
  return ret;
}
void TConfig::SetComputed(std::vector<Base::TJson> &&conf_stack) {
  assert(!ComputedLocked);
  for(auto &config : conf_stack) {
    Config.emplace_back(move(config));
  }
  ComputedLocked = true;
}

void TConfig::AddConfig(TJson &&config, bool top) {
  if(config.GetKind() != TJson::Object) {
    throw TInvalidConfig("Invalid config file. Top level isn't a JSON object.");
  }

  if(top) {
    Config.emplace_front(move(config));
  } else {
    assert(!ComputedStart);
    assert(!ComputedLocked);
    Config.emplace_back(move(config));
  }
}

void TConfig::AddFile(const string &filename) {
  auto timestamp = TryGetTimestamp(filename);
  if(timestamp) {
    Timestamp = Newest(Timestamp, timestamp);
    AddConfig(TJson::Read(filename.c_str()));
  }
}
