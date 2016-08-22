#include <bit/jobs/dep_clang.h>

#include <iostream>

#include <clang-c/Index.h>

#include <base/split.h>
#include <bit/jobs/compile_c_family.h>
#include <bit/jobs/dep.h>

using namespace Base;
using namespace Bit;
using namespace Bit::Jobs;
using namespace std;

struct TDepStorage {
  unordered_set<string> Deps;
};

enum CXChildVisitResult onVisitCursor(CXCursor cursor, CXCursor, CXClientData client_data) {
  if (clang_getCursorKind(cursor) == CXCursor_InclusionDirective) {
    TDepStorage *store = reinterpret_cast<TDepStorage *>(client_data);
    CXFile file = clang_getIncludedFile(cursor);
    CXString filename = clang_getFileName(file);
    const char *fn = clang_getCString(filename);
    if (!fn) {
      return CXChildVisit_Continue;
    }
    store->Deps.insert(string(fn));
    clang_disposeString(filename);
  }
  return CXChildVisit_Continue;
}

CXIdxClientFile onPpIncludedFile(CXClientData client_data, const CXIdxIncludedFileInfo *file_info) {
  TDepStorage *store = reinterpret_cast<TDepStorage *>(client_data);
  CXString filename = clang_getFileName(file_info->file);
  const char *filename_cstr = clang_getCString(filename);
  if (filename_cstr) {
    store->Deps.insert(filename_cstr);
  }
  clang_disposeString(filename);
  return nullptr;
}

template <typename TVal>
vector<TVal> &append(vector<TVal> &lhs, const vector<TVal> &rhs) {
  lhs.insert(lhs.end(), rhs.begin(), rhs.end());
  return lhs;
}

unordered_set<string> Bit::Jobs::GetDepsUsingClang(TDep *job, TJob::TOutput &output) {
  // TODO(cmaloney): de-dup this between here and dep_wrap_compiler.cc
  // If the source is a C or C++ file, give extra arguments as the extra arguments we'd pass to the
  // compiler
  const TRelPath &input_path = job->GetInput()->RelPath;

  // Only C, C++ are supported currently.
  assert(input_path.EndsWith(".cc") || input_path.EndsWith(".c"));

  vector<string> cmd = input_path.EndsWith(".cc") ? TCompileCFamily::GetBaseCppArgs()
                                                  : TCompileCFamily::GetBaseCArgs();

  // TODO(cmaloney): Ask clang for it's paths that need to be added rather than
  // just assuming the include search path and the like.
  // clang++ -x c++ -std=c++11 -v -E /dev/null
  append(cmd, {"-internal-isystem", "/usr/include/c++/6.1.1"});
  append(cmd, {"-internal-isystem", "/usr/include/c++/6.1.1/x86_64-pc-linux-gnu"});
  append(cmd, {"-internal-isystem", "/usr/include/c++/6.1.1/backward"});
  append(cmd, {"-internal-isystem", "/usr/lib/clang/3.8.1/include"});

  // Since we skip function bodies everything which is a parameter is unused. Avoid that making errors.
  append(cmd, {"-Wno-unused-parameter"});

  // DEBUG: cout << AsStr("[", Join(cmd, ", "), "]\n");
  // TODO(cmaloney): de-dup this between here and Base::Subprocess::Exec
  // NOTE: This is definitely less efficient than strictly necessary, but we
  // don't actually copy the strings, just pointers, so not too bad.
  unique_ptr<const char *[]> argv(new const char *[cmd.size() + 1]);
  argv[cmd.size()] = nullptr;
  for (uint64_t i = 0; i < cmd.size(); ++i) {
    argv[i] = cmd[i].c_str();
  }

  // TODO(cmaloney): see if we can re-use some context to save time.
  // TODO(cmaloney): Instead of print diagnostics, need to stream them into output.
  CXIndex index = clang_createIndex(0, 1);
  CXIndexAction index_action = clang_IndexAction_create(index);

  IndexerCallbacks callbacks{};
  callbacks.ppIncludedFile = onPpIncludedFile;

  TDepStorage store;
  int error = clang_indexSourceFile(
      index_action,
      &store,
      &callbacks,
      sizeof(callbacks),
      CXIndexOpt_SuppressRedundantRefs | CXIndexOpt_SuppressWarnings,
      job->GetInput()->CmdPath.c_str(),
      const_cast<char **>(argv.get()),
      int(cmd.size()),
      nullptr,
      0,
      nullptr,
      CXTranslationUnit_Incomplete | CXTranslationUnit_SkipFunctionBodies);

  if (error != 0) {
     cout << AsStr(error, "\n");
  }

  clang_IndexAction_dispose(index_action);
  clang_disposeIndex(index);

  output.Result = TJob::TOutput::CompleteIfNeeds;
  return store.Deps;
}
