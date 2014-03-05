/* <stig/indy/disk/in_file.test.cc>

   Unit test for <stig/indy/disk/in_file.h>.

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

#include <stig/indy/disk/in_file.h>

#include <stig/indy/disk/out_stream.h>
#include <stig/indy/disk/sim/mem_engine.h>
#include <stig/indy/fiber/fiber_test_runner.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig::Indy::Disk;
using namespace Stig::Indy::Disk::Util;

typedef TStream<LogicalPageSize, LogicalBlockSize, PhysicalBlockSize, CheckedPage, 0UL> TDataInStream;
typedef TOutStream<LogicalPageSize, LogicalBlockSize, PhysicalBlockSize, PageCheckedBlock> TDataOutStream;

TBufBlock::TPool TBufBlock::Pool(PhysicalBlockSize, 100UL);

class TMyInFile
    : public TInFile {
  NO_COPY_SEMANTICS(TMyInFile);
  public:

  TMyInFile(const Stig::Indy::Util::TBlockVec &block_vec) : BlockVec(block_vec) {}

  virtual size_t GetFileLength() const override {
    return BlockVec.Size() * LogicalBlockSize;
  }

  virtual size_t GetStartingBlock() const override {
    return BlockVec.Front();
  }

  virtual void ReadMeta(size_t /*offset*/, size_t &/*out*/) const override {
    throw;
  }

  virtual size_t FindPageIdOfByte(size_t offset) const override {
    return (BlockVec[offset / LogicalBlockSize] * PagesPerBlock) + ((offset % LogicalBlockSize) / LogicalPageSize);
  }

  private:

  const Stig::Indy::Util::TBlockVec &BlockVec;

};

FIXTURE(Stream) {
  Stig::Indy::Fiber::TFiberTestRunner runner([](std::mutex &mut, std::condition_variable &cond, bool &fin, Stig::Indy::Fiber::TRunner::TRunnerCons &) {
    const TScheduler::TPolicy scheduler_policy(4, 10, milliseconds(10));
    TScheduler scheduler;
    scheduler.SetPolicy(scheduler_policy);
    Sim::TMemEngine mem_engine(&scheduler,
                               64 /* disk space: 64 MB */,
                               16,
                               4096 /* page cache slots: 1GB */,
                               1 /* num page lru */,
                               16 /* block cache slots: 1GB */,
                               1 /* num block lru */);

    const size_t num_blocks_to_write = 40UL;

    Stig::Indy::Util::TBlockVec block_vec;
    mem_engine.GetEngine()->AppendReserveBlocks(TVolume::TDesc::Fast, num_blocks_to_write, block_vec);
    unordered_map<size_t, shared_ptr<const TBufBlock>> collision_map {};
    TCompletionTrigger trigger;

    const size_t num_to_write = (num_blocks_to_write * LogicalBlockSize) / sizeof(size_t);

    /* stream data out */ {
      std::unordered_set<size_t> written_block_set{};
      TDataOutStream out_stream(HERE, 0UL, mem_engine.GetVolMan(), 0UL, block_vec, collision_map,
                                trigger, RealTime, true,
                                #ifndef NDEBUG
                                written_block_set,
                                #endif
                                [/* causing ICE if left to default */](TVolumeManager *){throw; return 0U;});
      for (size_t i = 0; i < num_to_write; ++i) {
        out_stream << i;
      }
    }
    TMyInFile in_file(block_vec);
    TDataInStream in_stream(HERE, 0UL, RealTime, &in_file, mem_engine.GetPageCache(), 0UL);
    size_t in_val;
    for (size_t i = 0; i < num_to_write; ++i) {
      in_stream.Read(in_val);
      EXPECT_EQ(in_val, i);
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  });
}