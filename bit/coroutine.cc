// Coroutine prototype
#include <optional>
#include <string>
#include <functional>
// TODO(cmaloney): Switch to a flat map/set, probably a small size pre-allocated optimized flat map
#include <unordered_set>
#include <vector>

#include <base/interner.h>
#include <base/not_implemented.h>
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


// C++ Strongly typed ways of building. For generics, don't use TResult bits.
// TBuildTask is Awaitable (can use co_await on it).
template <typename TResult> 
struct TBuildTask{
    
};

// TODO(cmaloney): Combine: https://github.com/preshing/junction
// and: https://github.com/skarupke/flat_hash_map/blob/master/flat_hash_map.hpp
// into a fast, concurrenty hash map to use for file_environment.
// so that this is really, really fast as it is very high contention.
// Provides thread-safe access to FileInfo objects.
struct TFileEnvironment {
  TFileEnvironment(const TFileEnvironment&) = delete;
  TFileEnvironment(TFileEnvironment &&) = delete;

  TFileEnvironment(TTree src, TTree out);

  /* Get the FileInfo which contains compilation state information for a given
     relative path. Relative paths should first be resolved from by calling
     GetRelPath. Always returns a TFileInfo / never returns null. */
  TFileInfo *GetInfo(TRelPath name);

  /* Attempts to find the tree for the given file and return the relative path.
     If the path doesn't begin with `/` it is assumed to be a relative path.
     If the path doesn't belong to any known tree, None is reeturned. */
  std::optional<TRelPath> TryGetRelPath(std::string_view path);

  /* Attempts to find the tree for the given file and return the relative path.
     If the path doesn't begin with a `/` it is assumed to be a relative path.
     If the path begins with `//` it is relative to the root of the project.
     Otherwise the path begins with a single `/` and is assumed to be a regular
     absolute path. */
  std::optional<TRelPath> EnvironmentRootedPath(std::string_view path);

  // Wait for a set of files 
  TBuildTask<bool> WaitForExist(unordered_set<TRelPath>);

  const TTree Src, Out;

  private:
  // First prime > 10k. Ideally ramp quite a bit more up. Not much memory and is an absolute
  // cap on number of files. Uses std::string instead of TRelPath as key since TRelPath doesn't
  // have a default constructor.
  Base::TThreadSafeInterner<10007, std::string, TFileInfo> Files;
};

struct TTaskMetadata {
  TFileEnvironment *Environment;
  const TProducer *Producer;
  const TFileInfo *Input;
  unordered_set<const TFileInfo *> Output;

  const TFileInfo* GetSoleOutput() const {
    assert(Output.size() == 1);
    return *Output.begin();
  }
};


template <typename TResult> 
TBuildTask<TResult> GetTaskFor(TRelPath path) {
    
}

template <typename TResult> 
TBuildTask<TResult> GetTaskFor(TTypedFile file) {
    
}

TFileInfo *GetFileInfo(string_view path);



// TOOD(cmaloney): Technically this should all be utf-8...
static inline bool local_isgraph(uint8_t character) { return character > 32 && character < 127; }

cppcoro::generator<string_view> parseIncludes(const TTaskMetadata &metadata) {
  vector<string> cmd{"clang++", "-std=c++2a", "-M", "-MG", metadata.Input->CmdPath};

  auto handle = Base::Subprocess::TProcessHandle(cmd);

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
  for (const auto &block: handle.Communicate()) {
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
                    co_yield string_view(buffer);
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

// One of potentially many specific overloads that make files introspectable.
struct TIncludeInfo {
  unordered_set<TRelPath> IncludedFiles;
};

// TODO(cmaloney): Move to a dedicated C++20 modules import scanner
// TODO(cmaloney): Move to a custom build task type.
// TODO(cmaloney): How do we build and cache the results of these?
TIncludeInfo ScanIncludes(TTaskMetadata metadata) {
  TIncludeInfo result;
  // TODO(cmaloney): Tweak what is the ideal number here
  result.IncludedFiles.reserve(1024);
  for (const auto &include_path : parseIncludes(metadata)) {
    // Filter out system inclues
    // TODO(cmaloney): Should ensure in the hash tree everything filtered out here
    // actually should be filtered out.
    auto rel_path = metadata.Environment->TryGetRelPath(include_path);
    if (rel_path) {
      result.IncludedFiles.emplace(move(*rel_path));
    }
  }
  return result;
}

struct TCompileInfo {
  unordered_set<TRelPath> LinkWants;
};

TBuildTask<TCompileInfo> Compile(TTaskMetadata metadata) {
  TCompileInfo result;
  auto include_info = ScanIncludes(metadata);

  // Ensure all included files have been built.
  co_await metadata.Environment->WaitForExist(include_info.IncludedFiles);

  // Actually perform the compilation
  vector<string> cmd{"clang++", "-std=c++2a", "-c", metadata.Input->CmdPath, "-o", metadata.GetSoleOutput()->CmdPath};

  result.LinkWants.reserve(include_info.IncludedFiles.size());
  for (const auto &include : include_info.IncludedFiles) {
    if (include.EndsWith(".h")) {
      NOT_IMPLEMENTED_S("C/C++ deps which don't end in .h")
    }
    // TODO(cmaloney): Rather than doing hand extension manipulation 
    // ideally ask environment for "link lib for this rel path".
    result.LinkWants.emplace(include.SwapExtension(".h", ".o"));
  }

  co_return result;
}

TBuildTask<TLinkInfo> Link(TTaskMetadata metadata) {
  // TODO(cmaloney): port job/link.cc
}
