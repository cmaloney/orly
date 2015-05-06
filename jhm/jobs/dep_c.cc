#include <jhm/jobs/dep_c.h>

#include <base/as_str.h>
#include <base/split.h>
#include <base/subprocess.h>

using namespace Base;
using namespace Jhm;
using namespace Jhm::Job;
using namespace std;

vector<string> Jhm::Job::ParseDeps(const string &gcc_deps) {
  vector<string> deps;

  const char *tok_start = nullptr;
  bool eaten_start = false;
  bool is_first_item = true;

  // Convert it to a happy format
  // Remove leading .o:
  // Remove leading source file (Comes right after the ':')
  // Grab each token as a string.
  // If the string is '\' then it's a linebreak GCC added...
  for(uint32_t i = 0; i < gcc_deps.length(); ++i) {
    const char &c = gcc_deps[i];

    if(!eaten_start) {
      if(c == ':') {
        eaten_start = true;
      }
      continue;
    }

    if(tok_start) {
      // Still in token?
      if(isgraph(c)) {
        continue;
      }

      // Hit end of token. Grab token and submit.
      // NOTE: If token is '\', then discard (Indicates GCC's line continuation)
      string dep(tok_start, size_t(&c - tok_start));
      tok_start = nullptr;
      if(dep != "\\") {
        if(is_first_item) {
          is_first_item = false;
        } else {
          deps.emplace_back(move(dep));
        }
      }
    } else {
      // Hit start of token?
      if(isgraph(c)) {
        tok_start = &gcc_deps[i];
      }
    }
  }

  return deps;
}
