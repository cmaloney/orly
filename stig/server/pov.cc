/* <stig/server/pov.cc>

   Implements <stig/server/pov.h>.

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

#include <stig/server/pov.h>

using namespace std;
using namespace Base;
using namespace Stig;
using namespace Stig::Server;

const Indy::L0::TManager::TPtr<Indy::TRepo> &TPov::GetRepo(const TServer *server) const {
  assert(this);
  std::lock_guard<std::mutex> lock(RepoLock);
  if (!Repo) {
    assert(server);
    auto repo_manager = server->GetRepoManager();
    TOpt<Indy::L0::TManager::TPtr<Indy::L0::TManager::TRepo>> parent_repo;
    if (SharedParents.empty()) {
      parent_repo = server->GetGlobalRepo();
    } else {
      parent_repo = repo_manager->ForceGetRepo(SharedParents.back());
    }
    Repo = repo_manager->GetRepo(GetId(), GetTtl(), parent_repo, Policy == TPolicy::Safe, true);
  }
  return Repo;
}

TPov::TPov(Durable::TManager *manager,
           const Base::TUuid &id,
           const Durable::TTtl &ttl,
           const TUuid &session_id,
           TAudience audience,
           TPolicy policy,
           const TSharedParents &shared_parents)
    : TObj(manager, id, ttl), SessionId(session_id), Audience(audience), Policy(policy), SharedParents(shared_parents) {}

TPov::TPov(Durable::TManager *manager, const Base::TUuid &id, Io::TBinaryInputStream &strm)
    : TObj(manager, id, strm) {
  strm >> SessionId >> reinterpret_cast<char &>(Audience) >> reinterpret_cast<char &>(Policy) >> SharedParents;
}

TPov::~TPov() {}

void TPov::Write(Io::TBinaryOutputStream &strm) const {
  assert(this);
  assert(&strm);
  TObj::Write(strm);
  strm << SessionId << reinterpret_cast<const char &>(Audience) << reinterpret_cast<const char &>(Policy);
  /* TODO: Why doesn't this work?: strm << SharedParents; */
  strm << SharedParents.size();
  for (const auto &id: SharedParents) {
    strm << id;
  }
}
