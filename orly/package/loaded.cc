/* <orly/package/loaded.cc>

   Implementes <orly/package/loaded.h>

   Copyright 2010-2014 OrlyAtomics, Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <orly/package/loaded.h>

#include <dlfcn.h>

#include <base/as_str.h>
#include <base/thrower.h>
#include <orly/package/api.h>

using namespace Base;
using namespace Jhm;
using namespace std;
using namespace Orly::Package;

static void *IfNull(const Base::TCodeLocation &code_location, void *handle) {
  if(!handle) {
    throw TDlError(code_location, dlerror());
  }
  return handle;
}

TLoaded::TPtr TLoaded::Load(const Jhm::TTree &package_dir, const TVersionedName &name) {
  return TPtr(new TLoaded(package_dir, name));
}

TLoaded::~TLoaded() {
  assert(this);
  dlclose(Handle);
}

bool TLoaded:: ForEachTest(const function<bool (const TTest *)> &cb) const {
  assert(this);

  for(auto &it: LinkInfo->PrimaryInfo->Tests) {
    if(!cb(it)) {
      return false;
    }
  }

  return true;
}

TFuncHolder::TPtr TLoaded::GetFunctionInfo(const Base::TPiece<const char> &func) const {
  assert(this);
  assert(&func);
  string func_name(func.GetStart(), func.GetLimit());
  auto iter = LinkInfo->PrimaryInfo->Functions.find(func_name);
  if (iter == LinkInfo->PrimaryInfo->Functions.end()) {
    DEFINE_ERROR(error_t, runtime_error, "unknown function name");
    THROW_ERROR(error_t) << '"' << func_name << '"';
  }
  return TFuncHolder::TPtr(new TFuncHolder(shared_from_this(), iter->second));
}

bool TLoaded::ForEachFunction(
    const std::function<bool(const std::string &name, const std::shared_ptr<const TFuncHolder> &func)> &cb) const {
  for (const auto &func : LinkInfo->PrimaryInfo->Functions) {
    if (!cb(func.first, GetFunctionInfo(AsPiece(func.first)))) {
      return false;
    }
  }
  return true;
}

const TVersionedName &TLoaded::GetName() const {
  assert(this);
  return Name;
}

const TIndexByIndexId &TLoaded::GetIndexByIndexId() const {
  assert(this);
  return LinkInfo->IndexByIndexId;
}

const TIndexIdSet &TLoaded::GetIndexIdSet() const {
  assert(this);
  return LinkInfo->PrimaryInfo->IndexIdSet;
}

const std::string &TLoaded::GetIndexPrefix() const {
  assert(this);
  return LinkInfo->IndexName;
}

bool TLoaded::ForEachIndexId(const std::function <bool (Base::TUuid *)> &cb) const {
  assert(this);
  for (auto id : LinkInfo->PrimaryInfo->IndexIdSet) {
    if (!cb(id)) {
      return false;
    }
  }
  return true;
}

TLoaded::TLoaded(const Jhm::TTree &package_dir, const TVersionedName &name) : Name(name) {
  string filename = AsStr(package_dir.GetAbsPath(name.GetSoRelPath()));

  Handle = IfNull(HERE, dlopen(filename.c_str(), RTLD_NOW));

  try {

    //TODO: We can actually link in such a way that we can use a dynamic cast here.
    int32_t api_verno = reinterpret_cast<int32_t (*)()>(IfNull(HERE, dlsym(Handle, "GetApiVersion")))();
    if(ORLY_API_VERSION != api_verno) {
      throw TLoaderError(HERE, "Package API version doesn't match server API version");
    }

    LinkInfo = reinterpret_cast<TLinkInfo*>(reinterpret_cast<TLinkInfo *(*)()>(IfNull(HERE, dlsym(Handle, "GetLinkInfo")))());

    assert(LinkInfo);
    if(LinkInfo->PrimaryName != AsStr(name.Name)) {
      std::ostringstream oss;
      oss << "Package name inside the package doesn't match the filename. It is illegal to rename Orly package '.so' files. Named '" << LinkInfo->PrimaryName << "' in package, expected '" << AsStr(name.Name) << "'.";
      throw TLoaderError(HERE, oss.str().c_str());
    }

    if(LinkInfo->PrimaryVersion != name.Version) {
      throw TLoaderError(HERE, "Package version inside the package doesn't match the filename. It is illegal to rename Orly package '.so' files.");
    }

  } catch (...) {
    dlclose(Handle);
    throw;
  }
}

const TParamMap &TFuncHolder::GetParameters() const {
  assert(this);

  return Func->Parameters;
}

const Orly::Type::TType &TFuncHolder::GetReturnType() const {
  assert(this);

  return Func->ReturnType;
}

Orly::Atom::TCore TFuncHolder::Call(TContext &ctx, const TArgMap &args) const {
  assert(this);
  assert(&ctx);
  assert(&args);

  return (Func->Runner)(ctx, args);
}

TFuncHolder::TFuncHolder(const TLoaded::TPtr &package, const TFuncInfo *func) : Package(package), Func(func) {
  assert(&package);
  assert(package);
  assert(func);
}
