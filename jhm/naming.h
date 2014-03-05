/* <jhm/naming.h>

   Classes to represent and easily manipulate our C++ namespacing/filename conventions as mandated by JHM.

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#pragma once

#include <cassert>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include <base/piece.h>

/* TODO:
    Make these less copy happy, usually they all are derived from the same source text. This means a TTape style model.
    Name copy construction mutators (Replace extension, remove prefix, etc.). */

namespace Jhm {

  /* Useful universal type defs. */
  typedef std::string TStr;
  typedef std::vector<TStr> TStrList;

  /* Forward Declarations */
  class TAbsPath;
  class TRelPath;

  /* TODO */
  class TNamingError : public Base::TFinalError<TNamingError> {
    public:

    /* Constructor. */
    TNamingError(const Base::TCodeLocation &loc, const char *msg=0) {
      PostCtor(loc, msg);
    }
  };  // TNamingError

  class TName {
    public:

    TName(const TStr &str); //str must be of the format base[.ext]*
    TName(const TStr &base, const TStrList &extensions);
    TName(TStr &&base, TStrList &&extensions);

    bool operator==(const TName &that) const;
    bool operator!=(const TName &that) const;

    const TStr &GetBase() const;
    const TStrList &GetExtensions() const;

    TName SwapLastExtension(const TStr &new_ext) const;
    TName SwapLastExtension(const TStrList &new_ext) const;

    void Write(std::ostream &strm) const;

    private:
    void Verify() const;

    TStr Base;
    TStrList Extensions;
  };

  class TNamespace {
    public:
    TNamespace(const TStr &ns); //namespaces must be of format [a][/b]*
    TNamespace(const Base::TPiece<const char> &ns);
    TNamespace(const TStrList &ns);
    TNamespace(TStrList &&ns);

    bool operator==(const TNamespace &that) const;
    bool operator!=(const TNamespace &that) const;

    TNamespace AddNamespace(const TStr &new_ns) const;
    TStr AsStr() const;
    const TStrList &Get() const;
    size_t GetHash() const;
    void Write(std::ostream &strm) const;

    /* Takes the namespace, chops off the last part using that as the filename and attaching the given extension list. */
    TRelPath ToRelPath(const TStrList &extensions={}) const;

    private:
    void Verify() const;

    TStrList Namespaces;
  };

  class TRelPath {
    public:
    TRelPath(const TStr &str); //Format: [namespaces/]name
    TRelPath(const TNamespace &ns, const TName &name);
    TRelPath(TNamespace &&ns, TName &&name);
    //TODO: Constructor which can pass prefix/extension information to TName.

    bool operator==(const TRelPath &that) const;
    bool operator!=(const TRelPath &that) const;

    TStr AsStr() const;

    const TNamespace &GetNamespace() const;
    const TName &GetName() const;

    TRelPath SwapLastExtension(const TStr &new_ext) const;
    TRelPath SwapLastExtension(const TStrList &new_ext) const;
    TNamespace ToNamespaceIncludingName() const;

    void Write(std::ostream &strm) const;

    private:
    static bool VerifyStrLastNotSlash(const TStr &str);

    TNamespace Namespaces;
    TName Name;
  };

  class TAbsBase {
    public:

    /* Locate an absolute base by finding a designated marker name. If the marker isn't found in the first case we throw
       an exception. In the second case, we use the current working directory. */
    static TAbsBase Find(const TStr &marker);
    static TAbsBase Find(const TStr &marker, bool &found_marker);

    //NOTE: It may turn out to be more efficient to make these always shared pointers when putting them into TAbsPath.
    TAbsBase(const TStr &str);
    TAbsBase(TStr &&str);

    bool operator==(const TAbsBase &that) const;
    bool operator!=(const TAbsBase &that) const;

    const TStr &Get() const;

    /* Take the given path in the string and, using the */
    const TAbsPath GetAbsPath(const TStr &str) const;

    void Write(std::ostream &strm) const;

    private:
    /* Perform common construction steps */
    void Construct();

    void StripSlashes();
    void Verify() const;

    TStr AbsBase;
  };

  class TAbsPath {
    public:
    TAbsPath(const TAbsBase &tree, const TRelPath &rel_path);
    TAbsPath(TAbsBase &&tree, TRelPath &&rel_path);

    bool operator==(const TAbsPath &that) const;
    bool operator!=(const TAbsPath &that) const;

    TStr AsStr() const;

    const TAbsBase &GetAbsBase() const;
    const TRelPath &GetRelPath() const;

    void Write(std::ostream &strm) const;

    private:
    TRelPath Path;
    TAbsBase Tree;
  };

  std::ostream &operator<<(std::ostream &strm, const TName &name);
  std::ostream &operator<<(std::ostream &strm, const TNamespace &ns);
  std::ostream &operator<<(std::ostream &strm, const TRelPath &rel_path);
  std::ostream &operator<<(std::ostream &strm, const TAbsBase &abs_base);
  std::ostream &operator<<(std::ostream &strm, const TAbsPath &abs_path);
}


namespace std {
  template <>
  struct hash<Jhm::TRelPath> {

    size_t operator()(const Jhm::TRelPath &rel_path) const {
      assert(&rel_path);
      //TODO: Eliminate the cast to string. For now it is the shortest path to completion
      return hash<std::string>()(rel_path.AsStr());
    }

    typedef size_t result_type;
    typedef Jhm::TRelPath argument_type;
  };

  template <>
  struct hash<Jhm::TNamespace> {

    size_t operator()(const Jhm::TNamespace &ns) const {
      assert(&ns);
      return ns.GetHash();
    }

    typedef size_t result_type;
    typedef Jhm::TNamespace argument_type;
  };

}