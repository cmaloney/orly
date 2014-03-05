/* <stig/package/manager.cc>

   Implements <stig/package/manager.h>

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

//TODO: We assume dlclose never fails.
#include <stig/package/manager.h>

#include <cassert>
#include <cstring>
#include <memory>
#include <sstream>

#include <stig/spa/error.h>
#include <base/assert_true.h>
#include <base/os_error.h>
#include <base/thrower.h>

//Force anything that includes the package manager to link against the stig runtime
// Which forces everything in the stig runtime to be part of the linking environment.
#include <stig/rt.h>

using namespace Base;
using namespace std;
using namespace Stig::Package;

TManager::TManager(const Jhm::TAbsBase &package_dir) : PackageDir(package_dir),
    InstallLock(TPotato::New()) {}

//When the installed map destructs, the shared pointers will naturally go away, unloading the packages.
TManager::~TManager() {}

TLoaded::TPtr TManager::Get(const TName &package) const {
  assert(this);
  assert(&package);

  TPotato::TSharedLock lock(InstallLock);
  auto it = Installed.find(package);
  if(it == Installed.end()) {
    std::ostringstream oss;
    oss << " Cannot get non-installed package '" << package << "'";
    throw TManagerError(HERE, oss.str().c_str());
  }
  return it->second;
}

void TManager::Install(const TVersionedNames &packages, const std::function<void (TLoaded::TPtr)> &post_cb) {
  assert(this);
  Load(packages, post_cb);
}

void TManager::Load(const TVersionedNames &packages, const std::function<void (TLoaded::TPtr)> &post_cb) {
  assert(this);
  assert(&packages);

  TPotato::TExclusiveLock lock(InstallLock);

  TInstalled installed(Installed);

  /* TODO: collect up errors, rather than throw on first. */
  //Ensure all package upgrades are actually upgrades, all files exist, build up map to swap in.
  for(const TVersionedName &package: packages) {
    auto installed_it = installed.find(package.Name);
    if(installed_it != installed.end()) {
      if(installed_it->second->GetName().Version > package.Version) {
        std::ostringstream oss;
        oss << "Cannot downgrade already installed package '" << package <<'\'';
        throw TManagerError(HERE, oss.str().c_str());
      }
      installed_it->second = TLoaded::Load(PackageDir, package);
      //auto ret = installed.insert(make_pair(package.Name, TLoaded::Load(PackageDir, package)));
      post_cb(installed_it->second);
    } else {
      auto ret = installed.insert(make_pair(package.Name, TLoaded::Load(PackageDir, package)));
      post_cb(ret.first->second);
    }
  }

  std::swap(Installed, installed);
}

void TManager::SetPackageDir(const Jhm::TAbsBase &package_dir) {
  assert(this);
  PackageDir = package_dir;
}


void TManager::Uninstall(const TVersionedNames &packages) {
  assert(this);
  assert(&packages);

  TPotato::TExclusiveLock lock(InstallLock);
  TInstalled installed(Installed);

  for(const TVersionedName &package: packages) {
    auto installed_it = installed.find(package.Name);
    if(installed_it == installed.end()) {
      std::ostringstream oss;
      oss << "Cannot uninstall package '" << package << "' because it is not installed";
      throw TManagerError(HERE, oss.str().c_str());
    }
    installed.erase(installed_it);
  }

  std::swap(Installed, installed);
}

void TManager::YieldInstalled(std::function<bool (const TVersionedName &name)> cb) const {
  assert(this);
  TPotato::TSharedLock lock(InstallLock);

  for(const auto &it: Installed) {
    if(!cb(it.second->GetName())) {
      break;
    }
  }
}