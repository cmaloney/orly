#include <bit/jobs/dep_clang.h>

#include <clang-c/Index.h>

#include <bit/jobs/compile_c_family.h>
#include <bit/jobs/dep.h>

using namespace Bit;
using namespace Bit::Jobs;
using namespace std;

// Adapted from a gist by github.com/codebrainz
// https://gist.github.com/codebrainz/faabd73930c5346f7b50
enum CXChildVisitResult onVisitCursor(CXCursor cursor, CXCursor, CXClientData) {
  if (clang_getCursorKind(cursor) == CXCursor_InclusionDirective) {
    CXFile file = clang_getIncludedFile(cursor);
    CXString filename = clang_getFileName(file);
    const char *fn = clang_getCString(filename);
    fprintf(stdout, "%s\n", fn);
    clang_disposeString(filename);
  }
  return CXChildVisit_Continue;
}

unordered_set<string> GetDepsUsingClang(TDep *job, TJob::TOutput &output) {
  // TODO(cmaloney): de-dup this between here and dep_wrap_compiler.cc
  // If the source is a C or C++ file, give extra arguments as the extra arguments we'd pass to the
  // compiler
  const TRelPath &input_path = job->GetInput()->RelPath;

  // Only C, C++ are supported currently.
  assert(input_path.EndsWith(".cc") || input_path.EndsWith(".c"));

  vector<string> cmd = input_path.EndsWith(".cc") ? TCompileCFamily::GetBaseCppArgs()
                                                  : TCompileCFamily::GetBaseCArgs();

  cmd.push_back("-M");
  cmd.push_back("-MG");

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
  CXTranslationUnit tu =
      clang_createTranslationUnitFromSourceFile(index, job->GetInput()->CmdPath.c_str(), cmd.size(),
                                                const_cast<char **>(argv.get()), 0, nullptr);

  if (!tu) {
    output.Result = TJob::TOutput::Error;
    return {};
  }

  clang_visitChildren(clang_getTranslationUnitCursor(tu), onVisitCursor, NULL);
  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);
}
