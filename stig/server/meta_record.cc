/* <stig/server/meta_record.cc>

   Implements <stig/server/meta_record.h>.

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

#include <stig/server/meta_record.h>

using namespace std;
using namespace Base;
using namespace Stig::Server;

/* Metadata for TMetaRecord::TEntry. */
RECORD_ELEM(TMetaRecord::TEntry, TUuid, SessionId);
RECORD_ELEM(TMetaRecord::TEntry, TOpt<TUuid>, UserId);
RECORD_ELEM(TMetaRecord::TEntry, TMetaRecord::TEntry::TPackageFqName, PackageFqName);
RECORD_ELEM(TMetaRecord::TEntry, string, MethodName);
RECORD_ELEM(TMetaRecord::TEntry, TMetaRecord::TEntry::TArgByName, ArgByName);
RECORD_ELEM(TMetaRecord::TEntry, TMetaRecord::TEntry::TExpectedPredicateResults, ExpectedPredicateResults);
RECORD_ELEM(TMetaRecord::TEntry, Base::Chrono::TTimePnt, RunTimestamp);
RECORD_ELEM(TMetaRecord::TEntry, uint32_t, RandomSeed);

/* Metadata for TMetaRecord. */
RECORD_ELEM(TMetaRecord, TMetaRecord::TEntryByUpdateId, EntryByUpdateId);

const TMetaRecord::TEntry &TMetaRecord::GetEntry(const TUuid &id) const {
  assert(this);
  auto iter = EntryByUpdateId.find(id);
  assert(iter != EntryByUpdateId.end());
  return iter->second;
}
