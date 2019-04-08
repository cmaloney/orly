// Coroutine prototype
#include <optional>
#include <string>
// TODO(cmaloney): Switch to a flat map, probably a small size pre-allocated optimized flat map
#include <unordered_set>
#include <vector>

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
  const TRelPath RelPath;
};

struct TProducer {
    // NOTE: TFileType should be string keyable, default to a string
    // which is a mime-type, convertable to a file type integer (used internally for searching)
    // The
    string Name;
    TFileType InType;
    TFileType OutType;
    std::function<optional<TTypedFile>(TRelPath)> GuessInputFromOutput;
    std::function<vector<TTypedFile>(TRelPath)> GetOutputForInput;
    // TODO: How to make build task return the TTypedFile of the ideal output?
    /// std::function<std::unique_ptr<TBuildTask<TOutType>>()> MakeBuildTask;
};

struct TTaskMetadata {
    const TProducer *Producer;
    TFileInfo *Input;
    std::unordered_set<TFileInfo *> Output;
};

// One of potentially many
struct TDepInfo {
    std::unordered_set<TRelPath> IncludedFiles;
};


// C++ Strongly typed ways of building. For generics, don't use TResult bits.
// TBuildTask is Awaitable (can use co_await on it).
template <typename TResult> 
struct TBuildTask{
    
};

struct TBuilder {
    
};

template <typename TResult> 
TBuildTask<TResult> GetTaskFor(TRelPath BaseFile) {
    
}

template <typename TResult> 
TBuildTask<TResult> GetTaskFor(TTypedFile BaseFile) {
    
}
