#include <bit/naming.h>

#include <cassert>
#include <cstring>
#include <iomanip>
#include <stdexcept>

#include <base/thrower.h>
#include <util/path.h>

using namespace Base;
using namespace Bit;
using namespace std;
using namespace Util;

// Done as a helper function to satisfy constructor initializer list requirements.
static string Clean(const string &path) {
  // Make sure the path is more than just '/'
  if (path.size() < 1) {
    THROWER(TInvalidPath) << "Invalid tree \"" << quoted(path) << "\". Too short.";
  }

  // Must start with '/'.
  if (path.front() != '/') {
    THROWER(TInvalidPath) << "Invalid tree \"" << quoted(path) << "\". Must start with '/'";
  }

  string normalizedPath = Normalize(path);

  // Ensure path always ends with '/'.
  if (normalizedPath.back() != '/') {
    normalizedPath.push_back('/');
  }

  return normalizedPath;
}

TTree::TTree(const string &path) : Path(Clean(path)) {}

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

TRelPath::TRelPath(std::string &&path) : Path(path) {
  assert(!path.empty());
  assert(path.front() != '/');
  assert(path.back() != '/');
}

TRelPath::TRelPath(std::string_view path) : Path(path) {
  assert(!path.empty());
  assert(path.front() != '/');
  assert(path.back() != '/');
}

TRelPath TRelPath::AddExtension(const char *extension) const {
  assert(extension[0] == '.');
  return TRelPath(Path + extension);
}

std::optional<TRelPath> TRelPath::TryRemoveExtension(const char *extension) const {
  if (EndsWith(extension)) {
    auto extesion_length = strlen(extension);
    return TRelPath(Path.substr(0, Path.length() - extesion_length));
  }
  return std::optional<TRelPath>();
}
bool TRelPath::EndsWith(const char *extension) const {
  auto extesion_length = strlen(extension);
  if (Path.length() < extesion_length) {
    return false;
  }
  return Path.compare(Path.length() - extesion_length, extesion_length, extension) == 0;
}

TRelPath TRelPath::SwapExtension(const char *old_extension, const char *new_extension) const {
  auto old_path_length = strlen(old_extension);
  auto old_extension_start = Path.size() - old_path_length;
  if (Path.compare(old_extension_start, old_path_length, old_extension) != 0) {
    THROWER(runtime_error) << "Internal Error: Old path doesn't end with expected extension " << quoted(old_extension);
  }
  string new_path = Path;
  new_path.resize(old_extension_start);
  new_path.append(new_extension);
  return TRelPath(new_path);
}

bool TRelPath::operator==(const TRelPath &that) const { return Path == that.Path; }

size_t std::hash<TRelPath>::operator()(const TRelPath &rel_path) const {
  return std::hash<std::string>()(rel_path.Path);
}


size_t std::hash<TAbsPath>::operator()(const TAbsPath &path) const {
  return std::hash<std::string>()(path.Path);
}
