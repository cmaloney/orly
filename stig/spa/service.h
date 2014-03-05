/* <stig/spa/service.h>

   The service of spa.

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
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>

#include <base/no_copy_semantics.h>
#include <base/scheduler.h>
#include <stig/atom/suprena.h>
#include <stig/package/manager.h>
#include <stig/package/name.h>
#include <stig/spa/flux_capacitor/api.h>
#include <stig/spa/stig_args.h>

/* Forward declarations. */
class TCompiler;
class TSpa;

namespace Stig {

  namespace Package {

    class TFuncHolder;

  }  // Package

  namespace Spa {

    /* TODO */
    const Base::TUuid SpaIndexId("00B4119F-A512-4E2A-8580-996C8E55697E");

    /* TODO */
    class TService {
      NO_COPY_SEMANTICS(TService);
      public:

      typedef Base::TPiece<const char> TStrPiece;
      using TStigArg = TArgs::TStigArg;

      /* TODO */
      static const char *GetNotifierStateStr(FluxCapacitor::TNotifierState state);

      TService();
      ~TService();

      /* TODO */
      void CreateSession(const Base::TOpt<TUUID> &acct, int ttl, TUUID &out);

      /* TODO */
      void CreatePrivatePov(const TUUID &session, const Base::TOpt<TUUID> &parent, int ttl, bool paused, TUUID &out);

      /* TODO */
      void CreateSharedPov(const Base::TOpt<TUUID> &parent, int ttl, bool paused, TUUID &out);

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
          const TUUID &private_pov,
          TStigArg &stig_args,
          Var::TVar &var
        );

      /* TODO */
      void Poll(
          const TUUID &session,
          const std::unordered_set<TUUID> &notifiers,
          Base::TOpt<std::chrono::milliseconds> timeout,
          std::unordered_map<TUUID, FluxCapacitor::TNotifierState> &out
        );

      /* Calls the given function in the given package within the given private point of view with the given arguments.
         The caller may pass UUID's to be notified about when the update promotes to that point of view, or if it fails
         before it reaches that location. The function will return an empty notifiers array if notify_povs is non-empty
         in the case that the function doesn't actually cause an "update" to be generated. Callers will likely want to
         detect this case. */
      void Try(
          const Package::TFuncHolder::TPtr &func,
          const TUUID &private_pov,
          const std::unordered_set<TUUID> &notify_povs,
          const TStigArg &stig_args,
          Atom::TCore &result_core,
          Atom::TSuprena &result_arena,
          std::unordered_map<TUUID, TUUID> &notifiers
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
      bool RunTest(const TUUID &session, const TUUID &spov_outer, const Package::TTestCase &test, bool verbose);
      bool RunTestBlock(const TUUID &session, const TUUID &spov, const Package::TTestBlock &test_block, bool verbose);

      Package::TManager PackageManager;

      /* TODO */
      Stig::Spa::FluxCapacitor::TGlobalPov GlobalPov;

      /* TODO */
      Atom::TSuprena Arena;

      /* TODO */
      Base::TScheduler Scheduler;

    };  // TService

  }  // Spa

}  // Stig
