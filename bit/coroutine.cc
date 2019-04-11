// Coroutine prototype
#include <optional>
#include <string>
// TODO(cmaloney): Switch to a flat map, probably a small size pre-allocated optimized flat map
#include <functional>
#include <unordered_set>
#include <vector>

#include <base/subprocess.h>
#include <bit/file_type.h>
#include <bit/naming.h>

using namespace Bit;
using namespace std;

struct TTypedFile {
    TFileType type;
    TRelPath output_name;
};

// One per file ever. Use TTypedFile if to do operations without manipulation.
struct TFileInfo {
  const std::string CmdPath;
  const TTypedFile Details;
};

struct TProducer {
    // NOTE: TFileType should be string keyable, default to a string
    // which is a mime-type, convertable to a file type integer (used internally for searching)
    // The
    string Name;
    TFileType InType;
    TFileType OutType;
    std::function<optional<TTypedFile>(TTypedFile)> GuessInputFromOutput;
    std::function<vector<TTypedFile>(TTypedFile)> GetOutputForInput;
    // TODO: How to make build task return the TTypedFile of the ideal output?
    /// std::function<std::unique_ptr<TBuildTask<TOutType>>()> MakeBuildTask;
};

struct TTaskMetadata {
    const TProducer *Producer;
    TFileInfo *Input;
    std::unordered_set<TFileInfo *> Output;
};

// One of potentially many specific overloads that make files introspectable.
struct TIncludeInfo {
    std::unordered_set<TRelPath> IncludedFiles;
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

vector<string> GetBaseArgs() {
    return {"clang++", "-std=c++2a"};
}

// TODO(cmaloney): Move to import scanning?
// auto deps = GetDepsUsingClang(this, output);
// auto deps = GetDepsWrappingCompiler(this, output);
TBuildTask<TIncludeInfo> ScanIncludes(TTaskMetadata metadata) {
  vector<string> cmd{"clang++", "-std=c++2a"}

  // TODO(cmaloney): Switch to run process in background / on background
  // thread+fork pool, while parsing the stdout/stderr using a generator which
  // continuously pipes the stdout/stderr into the parser
  Base::Subprocess::Run()
  for (const auto &dependency : GenerateIncludes(metadata))
  co_await subprocess.
  
  auto deps = GetDepsWrappingCompiler(this, output);



    // Run clang 
}
