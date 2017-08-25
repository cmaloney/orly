#pragma once

#include <mutex>
#include <unordered_map>

#include <base/class_traits.h>
#include <base/interner.h>
#include <base/opt.h>
#include <bit/naming.h>
#include <bit/file_info.h>

namespace Bit {

// Provides thread-safe access to FileInfo objects.
// Owns a
class TFileEnvironment {
  public:
  NO_COPY(TFileEnvironment)

  TFileEnvironment(TTree src, TTree out);

  /* Get the FileInfo which contains compilation state information for a given
     relative path. Relative paths should first be resolved from by calling
     GetRelPath. Always returns a TFileInfo / never returns null. */
  TFileInfo *GetFileInfo(TRelPath name);

  /* Attempts to find the tree for the given file and return the relative path.
     If the path doesn't begin with `/` it is assumed to be a relative path.
     If the path doesn't belong to any known tree, None is reeturned. */
  Base::TOpt<TRelPath> TryGetRelPath(const std::string &path);

  const TTree Src, Out;

  private:
  // TODO(cmaloney): Switch to lock free datastructures fo both of these. They
  // both are perfectly fine having things constructed lots of times. The
  // TFileInfo one has a requirement that everyone agree on one copy, but that
  // should be very doable.
  std::mutex Mutex;
  Base::TInterner<TRelPath, TFileInfo> Files;
};
}
