/* <jhm/naming.cc>

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

#include <jhm/naming.h>

#include <cassert>
#include <cstdlib>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

#include <base/not_implemented.h>
#include <base/split.h>
#include <base/syntax_error.h>

using namespace std;
using namespace Base;
using namespace Jhm;

TStr GetCwd() {
  unique_ptr<char> cwd(getcwd(0, 0));
  return TStr(cwd.get());
}

TName::TName(const TStr &str) {
  assert(&str);

  //str must be of the format base[.ext]*
  if(str.find('/') != string::npos) {
    throw TSyntaxError(HERE, "JHM filenames cannot contain the character '/'.");
  }

  auto pos = str.find('.');
  if(pos == string::npos) {
    Base = str;
  } else {
    Base = str.substr(0, pos);
    if(str.size() == pos+1) {
      Extensions = {""}; //If we're a., we have one extension which is empty, not no extensions.
    } else {
      Split(".", str.substr(pos+1, string::npos), Extensions);
    }
  }
  Verify();
}

TName::TName(const TStr &base, const TStrList &extensions) : Base(base), Extensions(extensions) { Verify(); }
TName::TName(TStr &&base, TStrList &&extensions) : Base(move(base)), Extensions(move(extensions)) { Verify(); }

bool TName::operator==(const TName &that) const {
  assert(this);
  assert(&that);

  return Base == that.Base && Extensions == that.Extensions;
}
bool TName::operator!=(const TName &that) const {
  assert(this);
  assert(&that);

  return Base != that.Base || Extensions != that.Extensions;
}

const TStr &TName::GetBase() const {
  assert(this);
  return Base;
}

const TStrList &TName::GetExtensions() const {
  assert(this);
  return Extensions;
}

TName TName::SwapLastExtension(const string &new_ext) const {
  assert(this);
  TStrList ext_list(Extensions);
  ext_list.back() = new_ext;

  return TName(Base, move(ext_list));
}
TName TName::SwapLastExtension(const TStrList &new_ext) const {
  assert(this);
  TStrList ext_list(Extensions);
  ext_list.pop_back();
  ext_list.insert(ext_list.end(), new_ext.begin(), new_ext.end());
  return TName(Base, move(ext_list));
}

void TName::Write(ostream &strm) const {
  assert(this);
  assert(&strm);

  strm << Base;
  if(Extensions.size() > 0) {
    strm << '.';
    Join('.', Extensions, strm);
  }
}


void TName::Verify() const {
  assert(Base.find('.') == string::npos);
  assert(Base.find('/') == string::npos);
  for(const auto &it: Extensions) {
    assert(it.find('.') == string::npos);
    assert(it.find('/') == string::npos);
  }
}

//TODO: Should really move all these functions to operate on owned & interned pieces which share storage.
TNamespace::TNamespace(const TPiece<const char> &namespaces)
    : TNamespace(string(namespaces.GetStart(), namespaces.GetLimit())) {}

TNamespace::TNamespace(const TStr &namespaces) { //namespaces must be of format [a][/b]*
  assert(&namespaces);

  if(namespaces.size() > 0 && namespaces[0] == '/') {
    throw TSyntaxError(HERE, "A JHM namespace/branch may not begin with '/'");
  }
  if(namespaces.size() > 0 && namespaces[namespaces.size()-1] == '/') {
    throw TSyntaxError(HERE, "A JHM namespace/branch may not end with '/'");
  }

  Split("/", namespaces, Namespaces);

  for(const auto &it: Namespaces) {
    if(it.size() == 0) {
      throw TSyntaxError(HERE, "A JHM namespace may not be empty");
    }
    if(it == ".") {
      throw TSyntaxError(HERE, "A JHM namespace/branch may not contain '.'");
    }
    if(it == "..") {
      throw TSyntaxError(HERE, "A JHM namespace/branch may not contain '..'");
    }
  }

  Verify();
}
TNamespace::TNamespace(const TStrList &namespaces) : Namespaces(namespaces) { Verify(); }
TNamespace::TNamespace(TStrList &&namespaces) : Namespaces(move(namespaces)) { Verify(); }

bool TNamespace::operator==(const TNamespace &that) const {
  assert(this);
  assert(&that);

  return Namespaces == that.Namespaces;
}

bool TNamespace::operator!=(const TNamespace &that) const {
  assert(this);
  assert(&that);

  return Namespaces != that.Namespaces;
}

TNamespace TNamespace::AddNamespace(const TStr &new_ns) const {
  assert(this);
  assert(&new_ns);

  TStrList ns_new(Namespaces);
  ns_new.push_back(new_ns);

  return TNamespace(move(ns_new));
}

string TNamespace::AsStr() const {
  assert(this);
  ostringstream oss;
  Write(oss);
  return oss.str();
}

const TStrList &TNamespace::Get() const {
  assert(this);
  return Namespaces;
}

size_t TNamespace::GetHash() const {
  assert(this);

  //TODO: Better hash.
  size_t h = 0;
  for (auto &it: Namespaces) {
    h ^= hash<string>()(it);
  }
  return h;
}


void TNamespace::Write(ostream &strm) const {
  assert(this);
  assert(&strm);

  Join('/', Namespaces, strm);
}

TRelPath TNamespace::ToRelPath(const TStrList &extensions) const {
  assert(this);

  auto ns = Namespaces;
  ns.pop_back();
  return TRelPath(TNamespace(move(ns)), TName(Namespaces.back(), extensions));
}

void TNamespace::Verify() const {
  assert(this);
  for(const auto &it: Namespaces) {
    assert(it.find('/') == string::npos);
    assert(it.size() > 0);
    assert(it != "..");
    assert(it != ".");
  }
}

//This initializer line is FUGLY. But we don't want to default construct name or namespace, so it is necessary...
TRelPath::TRelPath(const TStr &str)
    : Namespaces(VerifyStrLastNotSlash(str) && str.rfind('/') != string::npos ? str.substr(0, str.rfind('/')) : ""),
      Name(str.rfind('/') != string::npos ? str.substr(str.rfind('/') + 1) : str) {}

TRelPath::TRelPath(const TNamespace &namespaces, const TName &name) : Namespaces(namespaces), Name(name) {}
TRelPath::TRelPath(TNamespace &&namespaces, TName &&name) : Namespaces(move(namespaces)), Name(move(name)) {}

bool TRelPath::operator==(const TRelPath &that) const {
  assert(this);
  assert(&that);

  return Name == that.Name && Namespaces == that.Namespaces;
}

bool TRelPath::operator!=(const TRelPath &that) const {
  assert(this);
  assert(&that);

  return Name != that.Name || Namespaces != that.Namespaces;
}

string TRelPath::AsStr() const {
  assert(this);
  ostringstream oss;
  Write(oss);
  return oss.str();
}

const TNamespace &TRelPath::GetNamespace() const {
  assert(this);
  return Namespaces;
}

const TName &TRelPath::GetName() const {
  assert(this);
  return Name;
}

TRelPath TRelPath::SwapLastExtension(const string &new_ext) const {
  assert(this);

  return TRelPath(Namespaces, Name.SwapLastExtension(new_ext));
}
TRelPath TRelPath::SwapLastExtension(const TStrList &new_ext) const {
  assert(this);

  return TRelPath(Namespaces, Name.SwapLastExtension(new_ext));
}

TNamespace TRelPath::ToNamespaceIncludingName() const {
  assert(this);

  return Namespaces.AddNamespace(Name.GetBase());
}


void TRelPath::Write(ostream &out) const {
  assert(this);
  assert(&out);

  if(Namespaces.Get().size() > 0) {
    out << Namespaces << "/";
  }
  out << Name;
}

bool TRelPath::VerifyStrLastNotSlash(const string &str) {
  assert(&str);
  if(str.size() > 0 && str[str.size()-1] == '/') {
    throw TSyntaxError(HERE, "A JHM RelPath cannot end in '/'");
  }
  return true;
}

TAbsBase TAbsBase::Find(const TStr &marker, bool &found_marker) {
  TStr working = GetCwd();

  if (working.back() == '/') {
    //NOTE: The call returns without a trailing slash, so we shouldn't have one...
    NOT_IMPLEMENTED();
  }

  struct stat st;

  do {
    string test_dir = working + '/' + marker;
    if(stat(test_dir.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
      found_marker = true;
      return TAbsBase(working);
    }
    working = working.substr(0, working.rfind('/'));
  } while (working.size());
  return TAbsBase("");
}

TAbsBase::TAbsBase(const TStr &str) : AbsBase(str) {
  Construct();
}
TAbsBase::TAbsBase(TStr &&str) : AbsBase(move(str)) {
  Construct();
}

bool TAbsBase::operator==(const TAbsBase &that) const {
  assert(this);
  assert(&that);

  return AbsBase == that.AbsBase;
}
bool TAbsBase::operator!=(const TAbsBase &that) const {
  assert(this);
  assert(&that);

  return AbsBase != that.AbsBase;
}

const TStr &TAbsBase::Get() const {
  assert(this);

  return AbsBase;
}

/* Take the given path in the string and, using the */
const TAbsPath TAbsBase::GetAbsPath(const TStr &orig_path) const {
  TStr path = orig_path;
  if (!path.size()) {
    return TAbsPath(*this, TRelPath(path));
  }
  if (path.front() == '/') {
  } else {
    //Pefix path with the current directory
    //NOTE: The string addition here is ugly...
    path = GetCwd() + '/' + path;
  }
  //See if we have a prefix match. If not, throw.
  assert(path.front() == '/');
  if(AbsBase.size() > path.size() || path.compare(1, AbsBase.size(), AbsBase) != 0) {
    auto slash_pos = orig_path.rfind('/');
    return TAbsPath(orig_path.substr(0, slash_pos), TRelPath(path.substr(slash_pos+1)));
  }
  return TAbsPath(*this, TRelPath(path.substr(AbsBase.size()+2)));
}

void TAbsBase::Write(ostream &strm) const {
  assert(this);
  strm << '/' << AbsBase;
}

void TAbsBase::StripSlashes() {
  //std::unique_ptr<char> real_path(realpath(AbsBase.c_str(), 0));
  //AbsBase = real_path.get();

  //TODO: We should normalize the filename here, but the realpath call mandates the file exists... Write our own realpath type thing if these assertions fail
  //Or cleanup your input string.

  assert(AbsBase.find("/../") == string::npos);
  assert(AbsBase.find("/./") == string::npos);

  for ( string::size_type pos = AbsBase.find( "//", 0 ); pos != string::npos; pos = AbsBase.find( "//", pos ) )
  {
    AbsBase.erase( pos, 1 );
  }
  assert(AbsBase.find("//") == string::npos);

  if(AbsBase.size() && AbsBase[0] == '/') {
    AbsBase.erase(0,1);
  }
  if(AbsBase.size() && AbsBase[AbsBase.size() - 1] == '/') {
    AbsBase.erase(AbsBase.size()-1);
  }
}

void TAbsBase::Construct() {
  if(!AbsBase.size() || AbsBase.front() != '/') {
    //If abs_base doesn't begin with a '/', it means base the absolute path on the current working directory.
    char* cwd_cstr(get_current_dir_name());
    try {
      //TODO: The string here isn't really necessary...
      string cwd(cwd_cstr + 1);
      if(cwd.size()) {
        assert(cwd.back() != '/');
      }
      AbsBase = cwd + '/' + AbsBase;
    } catch (...) {
      free(cwd_cstr);
      throw;
    }
    free(cwd_cstr);
  }
  StripSlashes();
  Verify();
}

void TAbsBase::Verify() const {
  assert(this);
  assert(AbsBase.size() == 0 || (AbsBase[0] != '/' && AbsBase[AbsBase.size()-1] != '/'));
}

TAbsPath::TAbsPath(const TAbsBase &tree, const TRelPath &rel_path) : Path(rel_path), Tree(tree) {}
TAbsPath::TAbsPath(TAbsBase &&tree, TRelPath &&rel_path) : Path(move(rel_path)), Tree(tree) {}

bool TAbsPath::operator==(const TAbsPath &that) const {
  assert(this);
  assert(&that);

  return Tree == that.Tree && Path == that.Path;
}
bool TAbsPath::operator!=(const TAbsPath &that) const {
  assert(this);
  assert(&that);

  return Tree != that.Tree || Path != that.Path;
}

string TAbsPath::AsStr() const {
  assert(this);
  ostringstream oss;
  Write(oss);
  return oss.str();
}

const TAbsBase &TAbsPath::GetAbsBase() const {
  assert(this);

  return Tree;
}

const TRelPath &TAbsPath::GetRelPath() const {
  assert(this);

  return Path;
}

void TAbsPath::Write(ostream &strm) const {
  assert(this);
  assert(&strm);

  if(Tree.Get().size() > 0) {
    strm << Tree << '/';
  }
  strm << Path;
}

ostream &Jhm::operator<<(ostream &strm, const TName &name) {
  assert(&strm);
  assert(&name);

  name.Write(strm);

  return strm;
}


ostream &Jhm::operator<<(ostream &strm, const TNamespace &ns) {
  assert(&strm);
  assert(&ns);

  ns.Write(strm);

  return strm;
}

ostream &Jhm::operator<<(ostream &strm, const TRelPath &rel_path) {
  assert(&strm);
  assert(&rel_path);

  rel_path.Write(strm);

  return strm;
}


ostream &Jhm::operator<<(ostream &strm, const TAbsBase &abs_base) {
  assert(&strm);
  assert(&abs_base);

  abs_base.Write(strm);

  return strm;
}

ostream &Jhm::operator<<(ostream &strm, const TAbsPath &abs_path) {
  assert(&strm);
  assert(&abs_path);

  abs_path.Write(strm);

  return strm;
}