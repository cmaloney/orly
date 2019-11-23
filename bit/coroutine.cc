
// Coroutine prototype
#include <any>
#include <cstdlib>
#include <exception>
#include <experimental/coroutine>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <typeinfo>
#include <typeindex>

// TODO(cmaloney): Switch to a flat map/set, probably a small size pre-allocated optimized flat map
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

#include <unordered_set>
#include <variant>
#include <vector>

// TODO(cmaloney): This is a _lot_ of suppressed bits...
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wshadow-field"
#include <concurrentqueue/blockingconcurrentqueue.h>
#pragma clang diagnostic pop


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
  string Name;
  TFileType InType;
  TFileType OutType;
  function<optional<TTypedFile>(TTypedFile)> GuessInputFromOutput;
  function<vector<TTypedFile>(TTypedFile)> GetOutputForInput;
};

// Points to a BuildTask. Does ano-op if the task was already completed, runs the coroutine of the
// particular build task if needed.
template <typename TResult>
struct [[nodiscard]] TTaskAlias {
  struct promise_type;
  using THandle = experimental::coroutine_handle<promise_type>;

  struct promise_type {
    auto get_return_object() { return TTaskAlias{*this}; }
    void return_void() {}
    experimental::suspend_always initial_suspend() { return {}; }
    void unhandled_exception() { Result.emplace(std::current_exception()); }
    auto final_suspend() {
      struct final_awaiter {
        bool await_ready() { return false; }
        void await_resume() {}
        auto await_suspend(THandle me) { return me.promise().Waiter; }
      };
      return final_awaiter{};
    }

    private:
    experimental::coroutine_handle<> Waiter;
    optional<exception_ptr> Result;

    template <typename TResult2>
    friend struct TTaskAlias;
  };

  TTaskAlias(TTaskAlias && rhs) : Handle(rhs.Handle) { rhs.Handle = nullptr; }
  explicit TTaskAlias(promise_type & p) : Handle(THandle::from_promise(p)) {}
  ~TTaskAlias() {
    if (Handle) {
      Handle.destroy();
    }
  }

  bool await_ready() { return false; }
  void await_resume() {
    auto &result = Handle.promise().Result;
    if (result) {
      rethrow_exception(*result);
    }
  }
  void await_suspend(experimental::coroutine_handle<> waiter) {
    Handle.promise().Waiter = waiter;
    Handle.resume();
  }

  private:
  experimental::coroutine_handle<promise_type> Handle;
};

// TODO(cmaloney): Move as much of TBuildTask as possible to here.
// The generic "underbelly" of a build task.
struct TBuildTaskUntyped {
  TBuildTaskUntyped() = default;
  virtual ~TBuildTaskUntyped() = default;
};

// A build task encapsulates that we can run some arbitrary code / coroutine to "get"
// the needed data / result (This could be external tools or just local code).
template <typename TResult>
struct [[nodiscard]] TBuildTask : public TBuildTaskUntyped {
  struct promise_type;
  using THandle = experimental::coroutine_handle<promise_type>;

  struct promise_type {
    auto get_return_object() {
      return TBuildTask{*this};
    }
    // TODO(cmaloney): Switch away from using indicies here....
    auto return_value(TResult value) {
      cout << "return_value!\n" << endl;
      Result.template emplace<1>(std::move(value));
    }
    void unhandled_exception() { 
      cout<<"unhandled_exception!\n"<<endl;
      Result.template emplace<2>(std::current_exception()); 
    }
    experimental::suspend_always initial_suspend() { 
      cout<<"initial_suspend\n"<<endl;
      return {}; 
    }
    auto final_suspend() {
      cout<<"final_suspend\n"<<endl;
      struct final_awaiter {
        bool await_ready() { return false; }
        void await_resume() {}
        auto await_suspend(THandle me) { return me.promise().Waiter; }
      };
      return final_awaiter{};
    }

    private:
    experimental::coroutine_handle<> Waiter;
    variant<monostate, TResult, exception_ptr> Result;

    template <typename TResult2>
    friend struct TBuildTask;
  };

  TBuildTask(TBuildTask && rhs) : Handle(rhs.Handle) { rhs.Handle = nullptr; }
  virtual ~TBuildTask() {
    if (Handle) {
      Handle.destroy();
    }
  }
  explicit TBuildTask(promise_type & p) : Handle(THandle::from_promise(p)) {}
  bool await_ready() { return false; }
  TResult await_resume() {
    auto &result = Handle.promise().Result;
    // TODO(cmaloney): Switch away from using indicies here....
    if (result.index() == 1) {
      return get<1>(result);
    }
    rethrow_exception(get<2>(result));
  }
  void await_suspend(experimental::coroutine_handle<> waiter) {
    Handle.promise().Waiter = waiter;
    Handle.resume();
  }

  private:
  experimental::coroutine_handle<promise_type> Handle;
};

// TODO(cmaloney): Move to a struct in C++20 and use designated initializers...
using TResultRecord = tuple<type_index, TTypedFile>;

// NOTE: Singleton for each given build task.
struct TBuildTaskRecord {
  // Allow referencing a build task by a specific output file it produces.
  // 
  // Used when it's important the file exists but doesn't matter exactly how
  // it is created.
  TBuildTaskRecord(TTypedFile output) : Reference(output) {}

  // Reference a build task by inquiring about a specific output created around the
  // particular file.
  //
  // Used for cases such as wanting the result of an analysis, such as
  // a dependency scanner.
  TBuildTaskRecord(TResultRecord output) : Reference(output) {}

  // Make this a value type. NOTE: Don't really quite want this... but easier to have
  // Ideally the Reference ust always be set....
  TBuildTaskRecord() = default;
  TBuildTaskRecord(const TBuildTaskRecord &) = default;
  TBuildTaskRecord(TBuildTaskRecord &&) = default;

  // TODO(cmaloney): Constructor to refer to a specific build task directly.

  bool IsDone() const {
    return Result; 
  }

  const any &GetResultGeneric() const {
    assert(Result);
    return *Result;
  }

  // This probably shouldn't exist on this class?
  // Should this actually be a WaitSingle ?
  // TODO(cmaloney): This should be operator co_await.

  template <typename TResult>
  const TResult &GetResult() {
    return any_cast<const TResult &>(GetResultGeneric());
  }

  private:
  // The typed file being looked for or a generic pointer to the result.
  std::variant<std::monostate, TTypedFile, TResultRecord> Reference;
  // TODO:
  // https://stlab.cc/libraries/concurrency/future/future/ may be what to use here...
  // This might need to be atomic. Currently assuming _only_ the main thread ever sets it, and
  // always does so with other synchronization / scheduling with the thread which called it.
  any* Result = nullptr;
};

// TODO(cmaloney): Combine: https://github.com/preshing/junction
// and: https://github.com/skarupke/flat_hash_map/blob/master/flat_hash_map.hpp
// into a fast, concurrenty hash map to use for file_environment.
// so that this is really, really fast as it is very high contention.
// Provides thread-safe access to FileInfo objects.
struct TFileEnvironment {
  TFileEnvironment(const TFileEnvironment &) = delete;
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
  TBuildTask<bool> FileExist(unordered_set<TRelPath>) {
    // TODO(cmaloney):
    // Load the file info, see if it has specifics on how to build
    // the file. If not, check all jobs which can output the given
    // extension, and see which of those are viable.
    // If the above finds multiple ways to produce the file
    // error (user needs to disambiguate in config).
    // If there is only one, then co_await the specific job to
    // create the file.
    cout << "Hello, World!\n" << endl;
    co_return true;
  }

  const TTree Src, Out;

  private:
  // First prime > 10k. Ideally ramp quite a bit more up. Not much memory and is an absolute
  // cap on number of files. Uses std::string instead of TRelPath as key since TRelPath doesn't
  // have a default constructor.
  Base::TThreadSafeInterner<10007, std::string, TFileInfo> Files;
};

// Forward declaration to cut off circular dependency. 
// TODO(cmaloney): Needs a better name
class TTaskRunner {
  public:
  
  TTaskRunner(uint64_t num_threads);
  ~TTaskRunner();

  // Wait for all the referenced tasks to complete before returning.
  TTaskAlias<void> WaitForAll(std::vector<TBuildTaskRecord> &tasks) {
    cout<<"ALL\n";
    co_return;
  }

  // Wait for 1 (or more) of the referenced tasks to complete before returning.
  // At the scheduler's discretion based on what makes the most sense / how busy
  // things are.
  // TODO(cmaloney): Should return set of things which completed directly so callers
  // don't have to try and figure it back out...
  TTaskAlias<void> WaitForAny(std::vector<TBuildTaskRecord> &tasks) {
    cout<<"ANY\n";
    co_return;
  }

private:
  void ProcessQueue();

  moodycamel::BlockingConcurrentQueue<TBuildTaskUntyped*> ToRun;

  std::vector<std::unique_ptr<std::thread>> Runners;
};

TTaskRunner::TTaskRunner(uint64_t num_threads) {
  for (uint64_t i = 0; i < num_threads; ++i) {
    Runners.push_back(make_unique<thread>(bind(&TTaskRunner::ProcessQueue, this)));
  }
}

TTaskRunner::~TTaskRunner() {
  // TODO(cmaloney): Signal all workers it is time to exit.
  for (auto &thread_ptr: Runners) {
    thread_ptr->join();
    thread_ptr.reset();
  }
}

void TTaskRunner::ProcessQueue() {
  // TODO(cmaloney): Pop coroutines off todo queue, run them.
}

// Context used to get back to the job runner and general environment information.
struct TBitContext {
  // TODO(cmaloney): Once clang supports designated initializers, use that instead
  // of an explicit constructor.
  TBitContext() = default;
  TBitContext(const TBitContext &) = delete;
  TBitContext(TBitContext &&) = delete;

  TTaskRunner *Runner = nullptr;
  TFileEnvironment *Environment = nullptr;
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
  TBitContext *Context;
  const TProducer *Producer;
  const TFileInfo *Input;
  unordered_set<const TFileInfo *> Output;

  const TFileInfo *GetSoleOutput() const {
    assert(Output.size() == 1);
    return *Output.begin();
  }
};

TBuildTaskRecord GetTaskRecord(TRelPath path) {
  return TBuildTaskRecord{TTypedFile{TFileType::Unset, path}};
}

TBuildTaskRecord GetTaskRecord(TTypedFile file) {
  return TBuildTaskRecord{file};
}

template <typename TResult>
TBuildTaskRecord GetTaskRecordByOutput(TRelPath path) {
  return TBuildTaskRecord{TResultRecord{std::type_index(typeid(TResult)), TTypedFile{TFileType::Unset, path}}};
}

template <typename TResult>
TBuildTaskRecord GetTaskRecordByOutput(TTypedFile file) {
  return TBuildTaskRecord{TResultRecord{std::type_index(typeid(TResult)), file}};
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
  for (const auto &block : handle.Communicate()) {
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

      if (!in_tok) {
        // Hit start of token?
        if (local_isgraph(c)) {
          buffer.push_back(char(c));
          in_tok = true;
        }
        continue;
      }

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
      continue;
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
// TOOD(cmaloney): Move to clang-scan-deps
TIncludeInfo ScanIncludes(TTaskMetadata metadata) {
  TIncludeInfo result;
  // TODO(cmaloney): Tweak what is the ideal number here
  result.IncludedFiles.reserve(1024);
  for (const auto &include_path : parseIncludes(metadata)) {
    // Filter out system inclues
    // TODO(cmaloney): Should ensure in the hash tree everything filtered out here
    // actually should be filtered out.
    auto rel_path = metadata.Context->Environment->TryGetRelPath(include_path);
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
  vector<TBuildTaskRecord> Records;
  // Records.reserve(include_info.IncludedFiles.size());
  for(const auto &include: include_info.IncludedFiles) {
    Records.emplace_back(GetTaskRecord(include));
  }

  co_await metadata.Context->Runner->WaitForAll(Records);

  // Actually perform the compilation
  vector<string> cmd{"clang++", "-std=c++2a",
                     "-c",      metadata.Input->CmdPath,
                     "-o",      metadata.GetSoleOutput()->CmdPath};

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

TBuildTask<TLinkInfo> Link(TTaskMetadata metadata) {
  TLinkInfo result;
  // Set of all files which have been checked
  unordered_set<TRelPath> checked;

  // TODO(cmaloney): This shouldn't be compile info rather "things you need to link against"
  // which is a more general things, TCompileInfo potentially providing it's interface, or a Compile
  // task co_returning multiple distinct types.
  // These are going to be used quite a bit as temporary storage, make inserting fast.
  // Keep same size since they swap each pass through the loop.
  // TODO(cmaloney): Possibly make a "type" continuous wrapper for this. Is more code though for now.
  vector<TBuildTaskRecord> needed;
  vector<TBuildTaskRecord> remaining_needs;
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
  needed.push_back(GetTaskRecordByOutput<optional<TCompileInfo>>(metadata.Input->Details.output_name));
  while (!needed.empty()) {
    co_await metadata.Context->Runner->WaitForAny(needed);

    // Filter out completed tasks, and find any new link wantsarising from the
    // new information we have.
    remaining_needs.resize(0);
    for (TBuildTaskRecord &need : needed) {
      if (!need.IsDone()) {
        remaining_needs.push_back(need);
        continue;
      }

      // TODO(cmaloney): Don't actually need to be non-typesafe here, but making it
      // typesafe makes more types that aren't critical path.
      // How needed is constructed it's guaranteed this is the "right" result type.
      const auto &details = need.GetResult<optional<TCompileInfo>>();

      // If the target was not buildable, then skip it. It's something we'd like
      // to have linked against but there is no link lib needed for it.
      if (!details) {
        continue;
      }

      // Find any additional link wants the library has, and add them to the set of things we
      // need to find out about.
      for (const TRelPath &want : details->LinkWants) {
        // TODO(cmaloney): Easy helper for "insert if not found then do something additional"
        if (Util::Contains(checked, want)) {
          continue;
        }
        checked.insert(want);
        remaining_needs.push_back(GetTaskRecordByOutput<optional<TCompileInfo>>(want));
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
  for (const auto &lib : result.Libs) {
    cmd.push_back(lib.Path);
  }
  co_return result;
}

// Use this as your TResult to look for a file / path to exist.
struct TFileExist {};

struct TSyncWaitTask final {
  struct promise_type {
    using THandle = std::experimental::coroutine_handle<promise_type>;

    promise_type() noexcept = default;
    void start() {
      THandle::from_promise(*this).resume();
      if (Exception) {
        std::rethrow_exception(Exception);
      }
    }
    auto get_return_object() noexcept {
      return THandle::from_promise(*this);
    }
    void return_void() {}
    std::experimental::suspend_always initial_suspend() noexcept{ return {}; }
    auto final_suspend() {
      struct notifier {
        bool await_ready() const noexcept { return false; }
        void await_suspend(THandle) const noexcept {}
        void await_resume() noexcept {}
      };
      return notifier{};
    }
    void unhandled_exception() {
      Exception = std::current_exception();
    }
    private:
    std::exception_ptr Exception;
  };

  using THandle = std::experimental::coroutine_handle<promise_type>;

  TSyncWaitTask(THandle handle) noexcept : Handle(handle) {}
  ~TSyncWaitTask() {
    if (Handle) {
      Handle.destroy();
    }
  }

  void start() noexcept {
    Handle.promise().start();
  }

  private:
  THandle Handle;
};

// Exists as a separate function so the caller can manually
// advance the known sync wait task awaitable, then it can do 
// the "standard" coroutine advancement
template <typename TAwaitable>
TSyncWaitTask MakeSyncWaitTask(TAwaitable&& awaitable) {
  co_await forward<TAwaitable>(awaitable);
}

// TODO(cmaloney): Probably just use sync_wait from cppcoro instead of this bastardized
// version of that.
template <typename TAwaitable>
void SyncWait(TAwaitable&& awaitable) {
  auto task = MakeSyncWaitTask(forward<TAwaitable>(awaitable));
  task.start();
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

  TFileEnvironment environment(Bit::TTree("/home/firebird347/projectts/bit"),
                       TTree("/home/firebird347/projects/.bit"));
  TTaskRunner runner(1);

  // TODO(cmaloney): Move to designated initializers and no pointers once C++20 happens.
  // and designated initializers are a thing...
  TBitContext Context;
  Context.Environment = &environment;
  Context.Runner = &runner;
  // NOTE: This should spin up the threads so they are waiting
  // TODO(cmaloney): options.WorkerCount instead of 1
  // TODO: Add the requested files to the set of things to be built
  vector<TBuildTaskRecord> tasks{GetTaskRecordByOutput<TFileExist>(TTypedFile{TFileType::Executable, TRelPath{string{"bit/coroutine"}}})};
  // TODO(cmaloney): Rather than just waiting for all here, wait for events which mean we need to change
  // the ux / output (finishing all is one of those events), such as individual file completion, job failure, etc.
  SyncWait(Context.Runner->WaitForAll(tasks));

  // TODO: Report results to user.
  return 0;
}
