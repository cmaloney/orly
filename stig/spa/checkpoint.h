/* <stig/spa/checkpoint.h>

   A checkpoint in spa.

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

#include <stig/package/name.h>
#include <stig/spa/flux_capacitor/api.h>

namespace Stig {

  namespace Spa {

    typedef std::unordered_map<FluxCapacitor::TKV, Var::TPtr<Var::TChange>> TCheckpointStmts;
    typedef std::unordered_set<Package::TVersionedName> TCheckpointPackages;

    void ReadCheckpoint(const char *path, TCheckpointStmts &stmts, TCheckpointPackages &packages, Atom::TCore::TExtensibleArena *arena);

  }  // Spa

}  // Stig