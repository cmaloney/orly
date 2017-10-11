#pragma once

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
  // First prime > 10k. Ideally ramp quite a bit more up. Not much memory and is an absolute
  // cap on number of files. Uses std::string instead of TRelPath as key since TRelPath doesn't
  // have a default constructor.
  Base::TThreadSafeInterner<10007, std::string, TFileInfo> Files;
};
}
