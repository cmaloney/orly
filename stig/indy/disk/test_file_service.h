/* <stig/indy/disk/test_file_service.h>

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

#include <stig/indy/disk/file_service_base.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      /* TODO */
      class TTestFileService
          : public TFileServiceBase {
        NO_COPY_SEMANTICS(TTestFileService);
        public:

        /* TODO */
        virtual bool FindFile(const Base::TUuid &file_uid,
                              size_t gen_id,
                              size_t &out_block_id,
                              size_t &out_block_offset,
                              size_t &out_file_size,
                              size_t &out_num_keys) const override;

        /* TODO */
        virtual void InsertFile(const Base::TUuid &file_uid,
                                TFileObj::TKind file_kind,
                                size_t gen_id,
                                size_t starting_block_id,
                                size_t starting_block_offset,
                                size_t file_size,
                                size_t num_keys,
                                TSequenceNumber lowest_seq,
                                TSequenceNumber highest_seq,
                                TCompletionTrigger &completion_trigger) override;

        /* TODO */
        virtual void RemoveFile(const Base::TUuid &file_uid,
                                size_t gen_id,
                                TCompletionTrigger &completion_trigger) override;

        /* TODO */
        virtual void AppendFileGenSet(const Base::TUuid &file_uid,
                                      std::vector<TFileObj> &out_vec) override;

        /* TODO */
        virtual bool ForEachFile(const std::function<bool (const Base::TUuid &file_uid, const TFileObj &)> &cb) override;

        /* TODO */
        TTestFileService() {}

        /* TODO */
        virtual ~TTestFileService() {}

        private:

        /* TODO */
        typedef std::unordered_map<Base::TUuid, std::unordered_map<size_t, TFileObj>> TFileMap;

        /* TODO */
        static void AddToMap(TFileMap &file_map,
                             const Base::TUuid &file_uid,
                             TFileObj::TKind file_kind,
                             size_t file_gen,
                             size_t starting_block_id,
                             size_t starting_block_offset,
                             size_t file_size,
                             size_t num_keys,
                             TSequenceNumber lowest_seq,
                             TSequenceNumber highest_seq);

        /* TODO */
        static void RemoveFromMap(TFileMap &file_map,
                                  const Base::TUuid &file_uid,
                                  size_t file_gen);

        /* TODO */
        mutable std::mutex Mutex;

        /* TODO */
        TFileMap FileMap;

      };  // TFileServiceBase

    }  // Disk

  }  // Indy

}  // Stig