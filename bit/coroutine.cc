// Coroutine prototype
#include <optional>
#include <string>
// TODO(cmaloney): Switch to a flat map, probably a small size pre-allocated optimized flat map
#include <functional>
#include <unordered_set>
#include <vector>

#include <base/subprocess_coro.h>
#include <bit/file_type.h>
#include <bit/naming.h>

using namespace Bit;
using namespace std;
using namespace Base;
using namespace Base::Subprocess;

struct TTypedFile {
  TFileType type;
  TRelPath output_name;
};

// One per file ever. Use TTypedFile if to do operations without manipulation.
struct TFileInfo {
  // Non-copyable
  TFileInfo(const TFileInfo &) = delete;
  TFileInfo(const TFileInfo &&) = delete;
  const string CmdPath;
  const TTypedFile Details;
};

struct TProducer {
  // NOTE: TFileType should be string keyable, default to a string
  // which is a mime-type, convertable to a file type integer (used internally for searching)
  // The
  string Name;
  TFileType InType;
  TFileType OutType;
  function<optional<TTypedFile>(TTypedFile)> GuessInputFromOutput;
  function<vector<TTypedFile>(TTypedFile)> GetOutputForInput;
  // TODO: How to make build task return the TTypedFile of the ideal output?
  /// function<unique_ptr<TBuildTask<TOutType>>()> MakeBuildTask;
};

struct TTaskMetadata {
  const TProducer *Producer;
  TFileInfo *Input;
  unordered_set<TFileInfo *> Output;
};

// One of potentially many specific overloads that make files introspectable.
struct TIncludeInfo {
  unordered_set<TRelPath> IncludedFiles;
};


// C++ Strongly typed ways of building. For generics, don't use TResult bits.
// TBuildTask is Awaitable (can use co_await on it).
template <typename TResult> 
struct TBuildTask{
    
};

template <typename TResult> 
TBuildTask<TResult> GetTaskFor(TRelPath path) {
    
}

template <typename TResult> 
TBuildTask<TResult> GetTaskFor(TTypedFile file) {
    
}

TFileInfo *GetFileInfo(string path);

// TODO(cmaloney): Move to a dedicated import scanner
TBuildTask<TIncludeInfo> ScanIncludes(TTaskMetadata metadata) {
  vector<string> cmd{"clang++", "-std=c++2a", "-M", "-MG", metadata.Input.CmdPath};

  // TODO(cmaloney): Switch to run process in background / on background
  // thread+fork pool, while parsing the stdout/stderr using a generator which
  // continuously pipes the stdout/stderr into the parser
  auto handle = Base::Subprocess::TProcessHandle(cmd);
  vector<TFileInfo*> dependencies;
  // TODO(cmaloney): Tweak what is the ideal number here
  dependencies.reserve(128);
  for (const auto &include_path : GenerateIncludes(handle.Communicate())) {
    dependencies.push_back(GetFileInfo(include_path));
  }

  // TODO(cmaloney): Is this await needed?
  co_await dependencies;
}


// TOOD(cmaloney): Technically this should all be utf-8...
static inline bool local_isgraph(uint8_t character) { return character > 32 && character < 127; }

cppcoro::generator<string_view> GenerateIncludes(TTaggedBlockGenerator &output) {
  unordered_set<string> deps;

  bool in_tok = false;
  bool eaten_start = false;
  bool is_first_item = true;

  string buffer;
  // TODO(cmaloney): Measure and tune/tweak/adjust this as needed.
  buffer.reserve(1024);

  // Convert it to a happy format
  // Remove leading .o:
  // Remove leading source file (Comes right after the ':')
  // Grab each token as a string.
  // If the string is '\' then it's a linebreak GCC added...
  for (const auto &block: output) {
    // Ignore all error output
    if (block.Kind == TBlockKind::Error) {
      // TODO(cmaloney): Some good way of forwarding stderr in case of failures?
      continue;
    }

      // Parse the characters into an include
      for (auto c : block.Bytes) {
            if (!eaten_start) {
            if (c == ':') {
                eaten_start = true;
            }
            continue;
            }

            if (in_tok) {
            // Still in token?
            if (local_isgraph(c)) {
                buffer.push_back(char(c));
                continue;
            }

            // Hit end of token. Grab token and submit.
            // NOTE: If token is '\', then discard (Indicates GCC's line continuation)
            if (buffer != "\\") {
                if (is_first_item) {
                    is_first_item = false;
                } else {
                    co_yield string_view(deps);
                }
            }
            buffer.resize(0);
            in_tok = false;
            } else {
            // Hit start of token?
            if (local_isgraph(c)) {
                buffer.push_back(char(c));
                in_tok = true;
            }
            }
      }
  }
}