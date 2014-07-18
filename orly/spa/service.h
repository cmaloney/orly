/* <orly/spa/service.h>

   The service of spa.

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

#pragma once

#include <cassert>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>

#include <base/class_traits.h>
#include <base/scheduler.h>
#include <orly/atom/suprena.h>
#include <orly/package/manager.h>
#include <orly/package/name.h>
#include <orly/spa/flux_capacitor/api.h>
#include <orly/spa/orly_args.h>

/* Forward declarations. */
class TCompiler;
class TSpa;

namespace Orly {

  namespace Package {

    class TFuncHolder;

  }  // Package

  namespace Spa {

    /* TODO */
    const Base::TUuid SpaIndexId("00B4119F-A512-4E2A-8580-996C8E55697E");

    /* TODO */
    class TService {
      NO_COPY(TService);
      public:

      typedef Base::TPiece<const char> TStrPiece;
      using TOrlyArg = TArgs::TOrlyArg;

      /* TODO */
      static const char *GetNotifierStateStr(FluxCapacitor::TNotifierState state);

      TService();
      ~TService();

      /* TODO */
      void CreateSession(const Base::TOpt<Base::TUuid> &acct, int ttl, Base::TUuid &out);

      /* TODO */
      void CreatePrivatePov(const Base::TUuid &session, const Base::TOpt<Base::TUuid> &parent, int ttl, bool paused, Base::TUuid &out);

      /* TODO */
      void CreateSharedPov(const Base::TOpt<Base::TUuid> &parent, int ttl, bool paused, Base::TUuid &out);

      /* TODO */
      void Finalize();

      //Use for GetInstalled, InstallPackage, UninstallPackage
      Package::TManager &GetPackageManager();
      const Package::TManager &GetPackageManager() const;

      /* TODO */
      void LoadCheckpoint(const std::string &filename);

      /* TODO */
      bool RunTestSuite(const Package::TName &name, bool verbose);

      /* TODO */
      void SaveCheckpoint(const std::string &filename);

      /* TODO */
      void SetPackageDir(const std::string &dir);

      /* TODO */
      void Do(
          const std::shared_ptr<Package::TFuncHolder> &func,
          const Base::TUuid &private_pov,
          TOrlyArg &orly_args,
          Var::TVar &var
        );

      /* TODO */
      void Poll(
          const Base::TUuid &session,
          const std::unordered_set<Base::TUuid> &notifiers,
          Base::TOpt<std::chrono::milliseconds> timeout,
          std::unordered_map<Base::TUuid, FluxCapacitor::TNotifierState> &out
        );

      /* Calls the given function in the given package within the given private point of view with the given arguments.
         The caller may pass UUID's to be notified about when the update promotes to that point of view, or if it fails
         before it reaches that location. The function will return an empty notifiers array if notify_povs is non-empty
         in the case that the function doesn't actually cause an "update" to be generated. Callers will likely want to
         detect this case. */
      void Try(
          const Package::TFuncHolder::TPtr &func,
          const Base::TUuid &private_pov,
          const std::unordered_set<Base::TUuid> &notify_povs,
          const TOrlyArg &orly_args,
          Atom::TCore &result_core,
          Atom::TSuprena &result_arena,
          std::unordered_map<Base::TUuid, Base::TUuid> &notifiers
        );

      /* TODO */
      Atom::TCore::TExtensibleArena *GetArena() {
        return &Arena;
      }

      /* TODO */
      Base::TScheduler *GetScheduler() {
        return &Scheduler;
      }

      private:
      void OnPovFail(FluxCapacitor::TPov *pov);

      //TODO: Should really be const functions on the service...
      bool RunTest(const Base::TUuid &session, const Base::TUuid &spov_outer, const Package::TTestCase &test, bool verbose);
      bool RunTestBlock(const Base::TUuid &session, const Base::TUuid &spov, const Package::TTestBlock &test_block, bool verbose);

      Package::TManager PackageManager;

      /* TODO */
      Orly::Spa::FluxCapacitor::TGlobalPov GlobalPov;

      /* TODO */
      Atom::TSuprena Arena;

      /* TODO */
      Base::TScheduler Scheduler;

    };  // TService

  }  // Spa

}  // Orly
