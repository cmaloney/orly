/* <stig/indy/disk_layer.h>

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

#include <base/no_copy_semantics.h>
#include <inv_con/ordered_list.h>
#include <stig/indy/manager_base.h>
#include <stig/indy/update.h>

namespace Stig {

  namespace Indy {

    /* TODO */
    class TDiskLayer
        : public L0::TManager::TRepo::TDataLayer {
      NO_COPY_SEMANTICS(TDiskLayer);
      public:

      /* TODO */
      TDiskLayer(L0::TManager *manager,
                 L0::TManager::TRepo *repo,
                 size_t gen_id,
                 size_t num_keys,
                 TSequenceNumber lowest_seq,
                 TSequenceNumber highest_seq);

      /* TODO */
      virtual ~TDiskLayer();

      /* TODO */
      virtual std::unique_ptr<Indy::TPresentWalker> NewPresentWalker(const TIndexKey &from,
                                                                     const TIndexKey &to) const override;

      /* TODO */
      virtual std::unique_ptr<Indy::TPresentWalker> NewPresentWalker(const TIndexKey &key) const override;

      /* TODO */
      virtual std::unique_ptr<Indy::TUpdateWalker> NewUpdateWalker(TSequenceNumber from) const override;

      /* TODO */
      inline virtual TKind GetKind() const override;

      /* TODO */
      inline size_t GetGenId() const;

      /* TODO */
      inline size_t GetNumKeys() const;

      /* TODO */
      inline virtual size_t GetSize() const override;

      /* TODO */
      inline virtual TSequenceNumber GetLowestSeq() const override;

      /* TODO */
      inline virtual TSequenceNumber GetHighestSeq() const override;

      private:

      /* TODO */
      L0::TManager::TRepo *Repo;

      /* TODO */
      size_t GenId;

      /* TODO */
      size_t NumKeys;

      /* TODO */
      TSequenceNumber LowestSeq, HighestSeq;

    };  // TDiskLayer

    /* TODO */
    inline L0::TManager::TRepo::TDataLayer::TKind TDiskLayer::GetKind() const {
      return L0::TManager::TRepo::TDataLayer::Disk;
    }

    /* TODO */
    inline size_t TDiskLayer::GetGenId() const {
      assert(this);
      return GenId;
    }

    /* TODO */
    inline size_t TDiskLayer::GetNumKeys() const {
      assert(this);
      return NumKeys;
    }

    /* TODO */
    inline size_t TDiskLayer::GetSize() const {
      assert(this);
      return NumKeys;
    }

    /* TODO */
    inline TSequenceNumber TDiskLayer::GetLowestSeq() const {
      assert(this);
      return LowestSeq;
    }

    /* TODO */
    inline TSequenceNumber TDiskLayer::GetHighestSeq() const {
      assert(this);
      return HighestSeq;
    }

  }  // Indy

}  // Stig

