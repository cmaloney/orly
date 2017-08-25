#include <bit/file_environment.h>

#include <util/path.h>

#include <iostream>

using namespace Base;
using namespace Bit;
using namespace Util;
using namespace std;

TFileEnvironment::TFileEnvironment(TTree src, TTree out) : Src(src), Out(out) {}


TFileInfo *TFileEnvironment::GetFileInfo(TRelPath name) {
  // TODO(cmaloney): This locking scheme is going to be a major bottleneck...
  lock_guard<mutex> lock(Mutex);

  TFileInfo *result = Files.TryGet(name);
  if (result) {
    return result;
  }

  // If the file exists in src, make it there. Otherwise it is a generated / out
  // tree file.
  TAbsPath src_abs_path = TAbsPath(Src, name);

  auto add_file = [&](std::string cmd_path, bool is_src) {
    // NOTE: It is an intentional design decision that the file configuration must
    // always be in src / never be calculated. If it could have been calculated,
    // than that should be attached by whatever job produces the file, or by running
    // a job like the dependency finder which makes a side file of additional needed
    // information.

    // NOTE: This is explicitly separate from the make_unique becausewe need to
    // guarantee the TRelPath copying happens before

    auto file_info =
        make_unique<TFileInfo>(TRelPath(name), std::move(cmd_path),
                               TFileConfig(src_abs_path.AddExtension(".bitconfig.json")), is_src);
    return Files.Add(TRelPath(name), std::move(file_info));
  };

  if (ExistsPath(src_abs_path.Path.c_str())) {
    return add_file(name.Path, true);
  } else {
    return add_file(TAbsPath(Out, name).Path, false);
  }
}

/* Attempts to find the tree for the given file and return the relative path.
   If the path doesn't begin with `/` it is assumed to be a relative path.
   If the path doesn't belong to any tree, the full path is given as the
   relative path. */
TOpt<TRelPath> TFileEnvironment::TryGetRelPath(const std::string &path) {
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
  return TNone();
}
