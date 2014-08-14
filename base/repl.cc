/* <base/repl.cc>

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

#include <base/repl.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <memory>

#include <third_party/linenoise/linenoise.h>

using namespace std;

// TODO: Eliminate the need for this by making the linenoise library reentrant / refactoring it to be good C++ (Or
// rewriting it altogether)
static const vector<Base::TInfo> *CompletionSingleton(bool set, const vector<Base::TInfo> *completions) {
  static const vector<Base::TInfo> *Completions = nullptr;
  if (set) {
    Completions = completions;
  }
  return Completions;
}

static void ReplCompletionCallback(const char *buf, linenoiseCompletions *lc) {
  // TODO: Use a more efficient data structure for prefix match
  auto buf_len = strlen(buf);
  for (const auto &completion : *CompletionSingleton(false, nullptr)) {
    if (completion.Name.compare(0, min(completion.Name.length(), buf_len), buf) == 0) {
      linenoiseAddCompletion(lc, completion.Name.c_str());
    }
  }
}

bool Base::Repl(const string &ps1,
                const vector<TInfo> &completions,
                const function<bool(const string &)> &process_cmd) {
  assert(&ps1);
  assert(&completions);
  assert(&process_cmd);

  static bool InUse(false);
  // NOTE: This isn't a perfect lock. But it's good enough to catch most accidental misuses of the library.
  if (InUse) {
    THROW_ERROR(TReplInUse);
  } else {
    InUse = true;
  }

  int max_completion_len = 0;
  for (const auto &info : completions) {
    max_completion_len = max(max_completion_len, int(info.Example.length()));
  }

  CompletionSingleton(true, &completions);
  linenoiseSetCompletionCallback(ReplCompletionCallback);

  typedef unique_ptr<char, void (*)(void *)> TPtr;
  TPtr line_holder(nullptr, free);
  while (line_holder = TPtr(linenoise(ps1.c_str()), free)) {
    // Yes, this is a needless copy.
    // No, it isn't perf critical (Mainly limited by actual user input or network IO)
    // Yes, it gives me easier string comparison, length, etc. Much easier coding == win.
    // This can be replaced with a string_view when that enters the standard.
    string line(line_holder.get());
    if (line == "help" || line == "?") {
      for (const auto &info : completions) {
        printf("%-*s    %s.\n", max_completion_len, info.Example.c_str(), info.Desc.c_str());
      }
    } else if (!line.empty()) {
      // A regular command
      if (!process_cmd(line)) {
        return false;
      }
    }

    // Add all non-empty lines to history
    if (!line.empty()) {
      linenoiseHistoryAdd(line.c_str());
    }
  }
  return true;
}