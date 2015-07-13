#include <bit/naming.h>

#include <cassert>
#include <iomanip>

#include <base/thrower.h>
#include <util/path.h>

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

  // TODO(cmaloney): Normalize the path. Remove '/../', '/./', '//'

  // Ensure path always ends with '/'.
  if (path.back() != '/') {
    path.push_back('/');
  }

  return path;
}

TTree::TTree(string path) : Path(Clean(move(path))) {}

TTree TTree::Find(string start_dir, const std::string &marker) {
  // TODO(cmaloney): Normalize the path into a valid tree before starting. Saves
  // computation and makes it so that paths with relative paths in them work right.
  do {
    string test_dir = start_dir + '/' + marker;
    if(ExistsPath(test_dir.c_str())) {
      return TTree(start_dir);
    }

    start_dir.resize(start_dir.rfind('/'));
  } while(start_dir.size());

  THROWER(runtime_error) << "Unable to find " << quoted(marker) << " indicating root of tree";
}

TAbsPath::TAbsPath(const TTree &tree, const std::string &rel_path) : Path(tree.Path + rel_path) {
  assert(!rel_path.empty());
  assert(rel_path.front() != '/');
}
