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
#include <bit/config.h>
#include <bit/file_type.h>
#include <bit/naming.h>
#include <bit/options.h>
#include <cmd/args.h>
#include <cmd/main.h>
#include <cmd/parse.h>
#include <util/path.h>
#include <util/stl.h>

using namespace Bit;
using namespace std;
using namespace Base;
using namespace Base::Subprocess;

// TODO(cmaloney): Allow users to provide via a 'configure' type mechanism.
static const char *SystemBitDir = "/usr/lib/bit";

// TODO(cmaloney): This should be a library function.
TTree GetHomeDirectory() {
  char *home_dir = getenv("HOME");
  if (home_dir) {
    return TTree(home_dir);
  } else {
    // TODO(cmaloney): Implement a sane fallback if HOME isn't set.
    NOT_IMPLEMENTED()
  }
}

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

// NOTE: If you have an optional dependency use TBuildTask<std::optional<TResult>>

// C++ Strongly typed ways of building. For generics, don't use TResult bits.
// TBuildTask is Awaitable (can use co_await on it).
template <typename TResult> 
struct TBuildTask{
  TBuildTask() = default;

  struct promise_type {
    using value_type = std::remove_reference<TResult>;
    using reference_type = std::conditional_t<std::is_reference_v<TResult>, TResult, TResult&>;
    using pointer_type = value_type*;

    promise_type() noexcept {}
    ~promise_type() noexcept {}

    void unhandled_exception() {
      Exception = std::current_exception(); 
    }
    
    TBuildTask<TResult> get_return_object() noexcept {
        return TBuildTask<TResult>{};
    }

    constexpr std::experimental::suspend_always initial_suspend() const { return {}; }
    constexpr std::experimental::suspend_always final_suspend() const { return {}; }

    // TODO(cmaloney): the TResult should be const.
    void return_value(TResult &value) noexcept {
      Value = std::addressof(value);
    }

    // TODO(cmaloney): noexcept?
    void return_value(TResult &&value) {
      ::new (static_cast<void*>(std::addressof(Value))) TResult(forward<TResult>(value));
    }

    private:
    // TODO(cmaloney): empty vs. value vs. exception flag?
    union {
      std::exception_ptr Exception;
      TResult Value; 
    };
  };

  bool await_ready() noexcept { return true; }
  void await_suspend(std::experimental::coroutine_handle<>) noexcept {}
  void await_resume() const noexcept {}
  // TODO(cmaloney): await_transform: That means add awaitable as a dependency for caching.
};

// Like a build task but runs optomistically / added to queue immediately. Use
// when a job needs to fan out into many jobs, then co_away the set of async build
// tasks.
template <typename TResult>
struct TBuildTaskAsync {
  bool IsDone() const;

  bool await_ready() noexcept { return true; }
  void await_suspend(std::experimental::coroutine_handle<>) noexcept {}
  void await_resume() const noexcept {}
  // TODO(cmaloney): await_transform: That means add awaitable as a dependency for caching.

  //Throws if IsDone returns false.
  TResult &Result();
};

// TODO(cmaloney): Combine: https://github.com/preshing/junction
// and: https://github.com/skarupke/flat_hash_map/blob/master/flat_hash_map.hpp
// into a fast, concurrenty hash map to use for file_environment.
// so that this is really, really fast as it is very high contention.
// Provides thread-safe access to FileInfo objects.
struct TFileEnvironment {
  TFileEnvironment(const TFileEnvironment&) = delete;
  TFileEnvironment(TFileEnvironment &&) = delete;

  TFileEnvironment(TTree src, TTree out) : Src(src), Out(out) {}

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
  // TODO(cmaloney): Not sure this should be the build task type, but keeping for now.
  // TODO: actually a build task of void, but meh
  TBuildTask<bool> EnsureExist(unordered_set<TRelPath>) {
    // TODO(cmaloney):
    // Load the file info, see if it has specifics on how to build
    // the file. If not, check all jobs which can output the given
    // extension, and see which of those are viable. 
    // If the above finds multiple ways to produce the file
    // error (user needs to disambiguate in config).
    // If there is only one, then co_await the specific job to
    // create the file.
  }

  const TTree Src, Out;

  private:
  // First prime > 10k. Ideally ramp quite a bit more up. Not much memory and is an absolute
  // cap on number of files. Uses std::string instead of TRelPath as key since TRelPath doesn't
  // have a default constructor.
  Base::TThreadSafeInterner<10007, std::string, TFileInfo> Files;
};


/* Attempts to find the tree for the given file and return the relative path.
   If the path doesn't begin with `/` it is assumed to be a relative path.
   If the path doesn't belong to any tree, the full path is given as the
   relative path. */
std::optional<TRelPath> TFileEnvironment::TryGetRelPath(std::string_view path) {
  assert(&path);

  // 0 length paths are illegal.
  assert(path.size() > 0);


  const auto make_rel_remove_prefix = [&path](const TTree &tree) {
    return TRelPath(path.substr(tree.Path.size()));
  };

  // If it's already a relative path, just return it.
  if (path[0] != '/') {
    return TRelPath(path);
  }

  // Search for the tree which contains the path.
  // Out might be a subdirectory of src, so check it first.
  if (path.compare(0, Out.Path.length(), Out.Path) == 0) {
    return make_rel_remove_prefix(Out);
  } else if (path.compare(0, Src.Path.length(), Src.Path) == 0) {
    return make_rel_remove_prefix(Src);
  }

  // Tree not known. Can't make a relative path.
  return std::optional<TRelPath>();
}

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

template <typename TResult>
TBuildTaskAsync<TResult> GetTaskForAsync(TRelPath path) {

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
  co_await metadata.Environment->EnsureExist(include_info.IncludedFiles);

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

struct TLinkInfo {
  std::unordered_set<TAbsPath> Libs;
};

using TLinkTask = TBuildTaskAsync<optional<TCompileInfo>>;

TBuildTask<TLinkInfo> Link(TTaskMetadata metadata) {
  TLinkInfo result;
  // Set of all files which have been checked
  unordered_set<TRelPath> checked;

  // TODO(cmaloney): This shouldn't be compile info rather "things you need to link against"
  // which is a more general things, TCompileInfo potentially providing it's interface, or a Compile
  // task co_returning multiple distinct types.
  // These are going to be used quite a bit as temporary storage, make inserting fast.
  // Keep same size since they swap each pass through the loop.
  vector<TLinkTask> needed;
  vector<TLinkTask> remaining_needs;
  needed.reserve(256);
  remaining_needs.reserve(256);

  // Each object file may have an arbitrarily set of object files it requries to be
  // linked against so work in "phases".
  //
  // Cycle through all the currently known link needs and mark them to be awaited to be built
  // if possible. If not possible the system should mark them as "if this becomes buildable
  // this build task should be invalidated."
  //
  // Once all currently known ones have been marked, await for any to be done. Once 1+ is done
  // then repeat to optimistically find as much available work as possible. Keep going until 
  // everything is known to be buildable or not, and we know the set of all things those things
  // need to be linked against.
  needed.push_back(GetTaskForAsync<optional<TCompileInfo>>(metadata.Input->Details.output_name));
  while (!needed.empty()) {
    co_await needed;

    // Filter out completed tasks, and find any new link wantsarising from the
    // new information we have.
    remaining_needs.resize(0);
    for (TBuildTaskAsync<optional<TCompileInfo>> need: needed) {
      if (!need.IsDone()) {
        remaining_needs.push_back(need);
        continue;
      }
      // If the target was not buildable, then skip it. It's something we'd like
      // to have linked against but there is no link lib needed for it.
      if (!need.Result()) {
        continue;
      }

      // Find any additional link wants the library has, and add them to the set of things we
      // need to find out about.
      for(const TRelPath &want: need.Result()->LinkWants) {
        // TODO(cmaloney): Easy helper for "insert if not found then do something additional"
        if (Util::Contains(checked, want)) {
          continue;
        }
        checked.insert(want);
        remaining_needs.push_back(GetTaskForAsync<optional<TCompileInfo>>(want));
      }
    }
    swap(needed, remaining_needs);
  }

  // TODO(cmaloney): Run the final link job.
  vector<string> cmd{"clang++", "-o", metadata.GetSoleOutput()->CmdPath};
  cmd.reserve(cmd.size() + result.Libs.size());
  // TODO(cmaloney): there is a C++ STL function I should use here...
  // TODO(cmaloney): Need the cmd paths.
  // TODO(cmaloney): Ordering of link libraries can matter. Cope with that.
  for (const auto &lib: result.Libs) {
    cmd.push_back(lib.Path);
  }
  co_return result;
}

int Main(int argc, char *argv[]) {
  Cmd::TArgs<TOptions> args{
      Cmd::Optional({"mixin", "m"}, &TOptions::Mixins,
                    "Configuration snippets to add on top of the base project configuration"),
      // TODO(cmaloney): Worker count is really coarse grained for what people want.
      // Ninja's pool options are more what is wanted / needed here.
      Cmd::Optional("max-parallel", &TOptions::WorkerCount,
                    "Max number of commands/jobs to run at once"),
      Cmd::Required(&TOptions::Targets, "targets", "List of files to try to produce"),
      // TODO(cmaloney): print-cmd is really handy but children can run arbitrary code...
      Cmd::Optional("print-cmd", &TOptions::PrintCmd, "Print the command line of all Exec calls")};
  TOptions options = Cmd::Parse(args, argc, argv);

  if (options.PrintCmd) {
    // TODO(cmaloney): Subprocess::SetEchoCommands(true);
  }


  // Stash the current directory since we change our working directory to always
  // be the root of the project, but we need to still resolve relative paths
  // from the invocation location.
  auto cwd = Util::GetCwd();
  TTree cwd_tree(cwd);

  // Find the root of the project from the current directory.
  // TODO(cmaloney): Add config like GIT_DIR, GIT_WORK_TREE so that bit doesn't
  // have to be run only from folders in the project.
  // TODO(cmaloney): CURPOS. src is definitely coming out wrong.
  // TODO(cmaloney): Allow subprojects to configure themselves pseudo-independently
  // of base projects to allow a llvm type project structure.
  TTree src = TTree::Find(cwd, "bit.json");

  TCoreDirs core_dirs{src.Path, GetHomeDirectory().Path, TTree(SystemBitDir).Path};

  // Load the config
  auto config = TConfig::Load(core_dirs);

  // Load user-given mixins. If none given, try loading `default` mixin.
  if (!options.Mixins.empty()) {
    for (const auto &mixin : options.Mixins) {
      config.AddMixin(mixin, core_dirs);
    }
  } else {
    config.AddMixin("default", core_dirs, true);
  }
  
  TFileEnvironment env(
    Bit::TTree("/home/firebird347/projectts/bit"),
    TTree("/home/firebird347/projects/.bit"));
  
  env.EnsureExist(unordered_set{TRelPath(string("bit/coroutine"))});
}
