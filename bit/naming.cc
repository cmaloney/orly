#include <bit/naming.h>

#include <cassert>
#include <cstring>
#include <iomanip>

#include <base/thrower.h>
#include <util/path.h>

using namespace Base;
using namespace Bit;
using namespace std;
using namespace Util;

// Done as a helper function to satisfy constructor initializer list requirements.
static string Clean(string &&path) {
  // Make sure the path is more than just '/'
  if (path.size() < 1) {
    THROWER(TInvalidPath) << "Invalid tree \"" << quoted(path) << "\". Too short.";
  }

  // Must start with '/'.
  if (path.front() != '/') {
    THROWER(TInvalidPath) << "Invalid tree \"" << quoted(path) << "\". Must start with '/'";
  }

  path = Normalize(path);

  // Ensure path always ends with '/'.
  if (path.back() != '/') {
    path.push_back('/');
  }

  return path;
}

TTree::TTree(string path) : Path(Clean(move(path))) {}

TTree TTree::Find(string start_dir, const std::string &marker) {
  start_dir = Normalize(start_dir);
  if (start_dir.back() == '/') {
    start_dir.resize(start_dir.size() - 1);
  }

  do {
    string test_dir = start_dir + '/' + marker;
    if (ExistsPath(test_dir.c_str())) {
      return TTree(start_dir);
    }

    start_dir.resize(start_dir.rfind('/'));
  } while (start_dir.size());

  THROWER(runtime_error) << "Unable to find " << quoted(marker) << " indicating root of tree";
}

TAbsPath::TAbsPath(const TTree &tree, const TRelPath &rel_path)
    : TAbsPath(tree.Path + rel_path.Path) {}

TAbsPath TAbsPath::AddExtension(const char *extension) const {
  assert(extension[0] == '.');
  return TAbsPath(Path + extension);
}

TAbsPath::TAbsPath(std::string &&path) : Path(path) {
  assert(!path.empty());
  assert(path.front() == '/');
  assert(path.back() != '/');
}

TRelPath::TRelPath(const std::string &path) : Path(path) {
  assert(!path.empty());
  assert(path.front() != '/');
  assert(path.back() != '/');
}

TRelPath TRelPath::AddExtension(const char *extension) const {
  assert(extension[0] == '.');
  return TRelPath(Path + extension);
}

TOpt<TRelPath> TRelPath::TryRemoveExtension(const char *extension) const {
  if (EndsWith(extension)) {
    auto extesion_length = strlen(extension);
    return TRelPath(Path.substr(0, Path.length() - extesion_length));
  }
  return TOpt<TRelPath>();
}
bool TRelPath::EndsWith(const char *extension) const {
  auto extesion_length = strlen(extension);
  if (Path.length() < extesion_length) {
    return false;
  }
  return Path.compare(Path.length() - extesion_length, extesion_length, extension) == 0;
}

//TRelPath TRelPath::SwapExtension(const char *new_extension) const {
  // Find the last '.'. If there is no '.' before the first '/' then there are
  // no extensions and an exception is thrown.
//}

bool TRelPath::operator==(const TRelPath &that) const { return Path == that.Path; }

size_t std::hash<TRelPath>::operator()(const TRelPath &rel_path) const {
  return std::hash<std::string>()(rel_path.Path);
}
