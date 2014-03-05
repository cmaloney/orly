/* <stig/indy/disk/test_file_service.cc>

   Implements <stig/indy/disk/test_file_service.h>.

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

#include <stig/indy/disk/test_file_service.h>

using namespace std;
using namespace Stig::Indy::Disk;

bool TTestFileService::FindFile(const Base::TUuid &file_uid,
                                size_t gen_id,
                                size_t &out_block_id,
                                size_t &out_block_offset,
                                size_t &out_file_size,
                                size_t &out_num_keys) const {
  assert(this);
  std::lock_guard<std::mutex> lock(Mutex);
  auto uid_ret = FileMap.find(file_uid);
  if (uid_ret != FileMap.end()) {
    const auto &gen_map = uid_ret->second;
    auto gen_ret = gen_map.find(gen_id);
    if (gen_ret != gen_map.end()) {
      const auto &file = gen_ret->second;
      out_block_id = file.StartingBlockId;
      out_block_offset = file.StartingBlockOffset;
      out_file_size = file.FileSize;
      out_num_keys = file.NumKeys;
      return true;
    }
    return false;
  }
  return false;
}

void TTestFileService::InsertFile(const Base::TUuid &file_uid,
                                  TFileObj::TKind file_kind,
                                  size_t gen_id,
                                  size_t starting_block_id,
                                  size_t starting_block_offset,
                                  size_t file_size,
                                  size_t num_keys,
                                  TSequenceNumber lowest_seq,
                                  TSequenceNumber highest_seq,
                                  TCompletionTrigger &completion_trigger) {
  assert(this);
  completion_trigger.WaitForOneMore();
  /* hold the mutex */ {
    std::lock_guard<std::mutex> lock(Mutex);
    AddToMap(FileMap, file_uid, file_kind, gen_id, starting_block_id, starting_block_offset, file_size, num_keys, lowest_seq, highest_seq);
  }  // release the mutex
  completion_trigger.Callback(Success, "");
}

void TTestFileService::RemoveFile(const Base::TUuid &file_uid,
                                  size_t gen_id,
                                  TCompletionTrigger &completion_trigger) {
  assert(this);
  completion_trigger.WaitForOneMore();
  /* acquire Mutex */ {
    std::lock_guard<std::mutex> lock(Mutex);
    RemoveFromMap(FileMap,
                  file_uid,
                  gen_id);
  }  // release Mutex
  completion_trigger.Callback(Success, "");
}

void TTestFileService::AppendFileGenSet(const Base::TUuid &file_uid,
                                        std::vector<TFileObj> &out_vec) {
  /* acquire Mutex */ {
    std::lock_guard<std::mutex> lock(Mutex);
    auto pos = FileMap.find(file_uid);
    if (pos != FileMap.end()) {
      for (const auto &iter : pos->second) {
        out_vec.emplace_back(iter.second);
      }
    }
  }  // release Mutex
}

bool TTestFileService::ForEachFile(const std::function<bool (const Base::TUuid &file_uid, const TFileObj &)> &cb) {
  std::lock_guard<std::mutex> lock(Mutex);
  for (auto uid_map : FileMap) {
    for (auto file : uid_map.second) {
      if (!cb(uid_map.first, file.second)) {
        return false;
      }
    }
  }
  return true;
}

void TTestFileService::AddToMap(TFileMap &file_map,
                                const Base::TUuid &file_uid,
                                TFileObj::TKind file_kind,
                                size_t file_gen,
                                size_t starting_block_id,
                                size_t starting_block_offset,
                                size_t file_size,
                                size_t num_keys,
                                TSequenceNumber lowest_seq,
                                TSequenceNumber highest_seq) {
  auto res = file_map.find(file_uid);
  if (res != file_map.end()) {
    auto r = res->second.emplace(file_gen, TFileObj(file_kind,
                                                    file_gen,
                                                    starting_block_id,
                                                    starting_block_offset,
                                                    file_size,
                                                    num_keys,
                                                    lowest_seq,
                                                    highest_seq));
    if (!r.second) {
      throw std::logic_error("Trying to insert file that already exists");
    }
  } else {
    file_map.emplace(file_uid, std::unordered_map<size_t, TFileObj>{{file_gen, TFileObj(file_kind,
                                                                                        file_gen,
                                                                                        starting_block_id,
                                                                                        starting_block_offset,
                                                                                        file_size,
                                                                                        num_keys,
                                                                                        lowest_seq,
                                                                                        highest_seq)}});
  }
}

void TTestFileService::RemoveFromMap(TFileMap &file_map,
                                     const Base::TUuid &file_uid,
                                     size_t file_gen) {
  auto res = file_map.find(file_uid);
  if (res != file_map.end()) {
    const size_t num_removed = res->second.erase(file_gen);
    if (num_removed != 1) {
      throw std::logic_error("Trying to remove file gen that does not exist");
    }
  } else {
    throw std::logic_error("Trying to remove file that does not exist");
  }
}