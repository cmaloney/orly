/* <stig/durable/test_manager.h>

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

#include <stig/durable/kit.h>

namespace Stig {

  namespace Durable {

    /* TODO */
    class TTestManager final
        : public TManager {
      public:

      /* TODO */
      TTestManager(size_t max_cache_size)
          : TManager(max_cache_size) {}

      private:

      /* TODO */
      virtual bool CanLoad(const TId &id) override {
        assert(this);
        return BlobById.find(id) != BlobById.end();
      }

      /* TODO */
      virtual void RunLayerCleaner() override {}

      /* TODO */
      virtual void CleanDisk(const TDeadline &now, TSem *sem) override {
        assert(this);
        assert(&now);
        assert(sem);
        std::unordered_map<TId, std::pair<TDeadline, std::string>> temp;
        for (const auto &item: BlobById) {
          if (item.second.first > now) {
            temp.insert(item);
          }
        }
        swap(BlobById, temp);
        sem->Push();
      }

      /* TODO */
      virtual void Delete(const TId &id, TSem *sem) override {
        assert(this);
        assert(sem);
        auto erased_count = BlobById.erase(id);
        assert(erased_count == 1);
        sem->Push();
      }

      /* TODO */
      virtual void Save(const TId &id, const TDeadline &deadline, const TTtl &/*ttl*/, const std::string &blob, TSem *sem) override {
        assert(this);
        assert(sem);
        BlobById[id] = std::make_pair(deadline, blob);
        sem->Push();
      }

      /* TODO */
      virtual bool TryLoad(const TId &id, std::string &blob) override {
        assert(this);
        assert(&blob);
        auto iter = BlobById.find(id);
        bool success = (iter != BlobById.end());
        if (success) {
          blob = iter->second.second;
        }
        return success;
      }

      private:

      /* TODO */
      std::unordered_map<TId, std::pair<TDeadline, std::string>> BlobById;

    };  //TTestManager

  }  // Durable

}  // Stig