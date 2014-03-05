/* <stig/server/meta_record.h>

   TODO

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
#include <utility>

#include <stig/sabot/all.h>

namespace Stig {

  namespace Server {

    /* TODO */
    class TMetaRecord {
      public:

      /* TODO */
      class TEntry {
        public:

        /* TODO */
        using TPackageFqName = std::vector<std::string>;

        /* TODO */
        using TArgByName = std::map<std::string, Var::TVar>;

        /* Why not vector of bool?  Because up yours, STL explicit specialization with weird return types on operator[], that's why. */
        using TExpectedPredicateResults = std::vector<uint8_t>;

        /* TODO */
        TEntry() {}

        /* TODO */
        TEntry(
            const Base::TUuid &session_id, const Base::TOpt<Base::TUuid> &user_id, const TPackageFqName &package_fq_name, const std::string &method_name, TArgByName &&arg_by_name,
            TExpectedPredicateResults &&expected_predicate_results, Base::Chrono::TTimePnt now, uint32_t random_seed)
            : SessionId(session_id), UserId(user_id), PackageFqName(package_fq_name), MethodName(method_name), ArgByName(std::move(arg_by_name)),
              ExpectedPredicateResults(std::move(expected_predicate_results)), RunTimestamp(now), RandomSeed(random_seed) {}

        /* TODO */
        const TArgByName &GetArgByName() const {
          assert(this);
          return ArgByName;
        }

        const TExpectedPredicateResults &GetExpectedPredicateResults() const {
          assert(this);
          return ExpectedPredicateResults;
        }

        /* TODO */
        const std::string &GetMethodName() const {
          assert(this);
          return MethodName;
        }

        /* TODO */
        const TPackageFqName &GetPackageFqName() const {
          assert(this);
          return PackageFqName;
        }

        uint32_t GetRandomSeed() const {
          assert(this);
          return RandomSeed;
        }

        const Base::Chrono::TTimePnt &GetRunTimestamp() const {
          assert(this);
          return RunTimestamp;
        }

        /* TODO */
        const Base::TUuid &GetSessionId() const {
          assert(this);
          return SessionId;
        }

        /* TODO */
        const Base::TOpt<Base::TUuid> &GetUserId() const {
          assert(this);
          return UserId;
        }

        private:

        /* TODO */
        Base::TUuid SessionId;

        /* TODO */
        Base::TOpt<Base::TUuid> UserId;

        /* TODO */
        TPackageFqName PackageFqName;

        /* TODO */
        std::string MethodName;

        /* TODO */
        TArgByName ArgByName;

        /* TODO */
        TExpectedPredicateResults ExpectedPredicateResults;

        /* TODO */
        Base::Chrono::TTimePnt RunTimestamp;

        /* TODO */
        uint32_t RandomSeed;

      };  // TMetaRecord::TEntry

      /* TODO */
      using TEntryByUpdateId = std::map<Base::TUuid, TEntry>;

      /* TODO */
      TMetaRecord() {}

      /* TODO */
      TMetaRecord(const Base::TUuid &update_id, TEntry &&entry) {
        EntryByUpdateId.insert(std::make_pair(update_id, std::forward<TEntry>(entry)));
      }

      /* TODO */
      const TEntry &GetEntry(const Base::TUuid &id) const;

      /* TODO */
      const TEntryByUpdateId &GetEntryByUpdateId() const {
        assert(this);
        return EntryByUpdateId;
      }

      private:

      /* TODO */
      TEntryByUpdateId EntryByUpdateId;

    };  // TMetaRecord

  }  // Server

}  // Stig
