#pragma once

#include <optional>
#include <unordered_map>

#include <base/class_traits.h>
#include <base/interner.h>
#include <bit/naming.h>
#include <bit/file_info.h>

namespace Bit {


class TFileEnvironment {
  public:
  NO_COPY(TFileEnvironment)

  TFileEnvironment(TTree src, TTree out);

  /* Get the FileInfo which contains compilation state information for a given
     relative path. Relative paths should first be resolved from by calling
     GetRelPath. Always returns a TFileInfo / never returns null. */
  TFileInfo *GetInfo(TRelPath name);

  /* Attempts to find the tree for the given file and return the relative path.
     If the path doesn't begin with `/` it is assumed to be a relative path.
     If the path doesn't belong to any known tree, None is reeturned. */
  std::optional<TRelPath> TryGetRelPath(const std::string &path);

  /* Attempts to find the tree for the given file and return the relative path.
     If the path doesn't begin with a `/` it is assumed to be a relative path.
     If the path begins with `//` it is relative to the root of the project.
     Otherwise the path begins with a single `/` and is assumed to be a regular
     absolute path. */
  std::optional<TRelPath> EnvironmentRootedPath(const std::string &path);

  const TTree Src, Out;

  private:
  // TODO(cmaloney):
  // Combine: https://github.com/preshing/junction
  // and: https://github.com/skarupke/flat_hash_map/blob/master/flat_hash_map.hpp
  // into a fast, concurrenty hash map to use for file_environme nt.
  // so that this is really, really fast as it is very high contention.
  // Provides thread-safe access to FileInfo objects.
  // First prime > 10k. Ideally ramp quite a bit more up. Not much memory and is an absolute
  // cap on number of files. Uses std::string instead of TRelPath as key since TRelPath doesn't
  // have a default constructor.
  Base::TThreadSafeInterner<10007, std::string, TFileInfo> Files;
};
}
