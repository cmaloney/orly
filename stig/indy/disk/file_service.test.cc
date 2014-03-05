/* <stig/indy/disk/file_service.test.cc>

   Unit test for <stig/indy/disk/file_service.h>.

   Copyright 2010-2014 Tagged

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include <stig/indy/disk/file_service.h>

#include <base/scheduler.h>
#include <stig/indy/fiber/fiber_test_runner.h>

#include <test/kit.h>

using namespace std;
using namespace chrono;
using namespace Base;
using namespace Stig::Indy::Disk;

const bool NoRealTimeMode = true;
const bool AbortOnError = false;

TBufBlock::TPool TBufBlock::Pool(Util::PhysicalBlockSize, 100);

void IntroduceRandomBitError(size_t *buf, size_t data_size, std::mt19937_64 &engine) {
  size_t bit = engine() % (data_size * 8);
  size_t mask = 1 << bit % (sizeof(size_t) * 8);
  buf[bit / (sizeof(size_t) * 8)] ^= mask;
}

void TestFileServiceRestart(size_t num_append_log_blocks,
                            const std::function<void (TFileService &)> &clean_image_cb,
                            const std::function<void (Util::TVolumeManager *vol_man, size_t image_1_block, size_t image_2_block, const std::vector<size_t> &append_log_block_vec)> &downtime_cb,
                            const std::function<void (TFileService &)> &cold_image_cb,
                            const std::function<void (const std::exception &ex, size_t phase)> &err_cb) {
  Stig::Indy::Fiber::TFiberTestRunner runner([&](std::mutex &mut, std::condition_variable &cond, bool &fin, Stig::Indy::Fiber::TRunner::TRunnerCons &runner_cons) {
    TScheduler scheduler(TScheduler::TPolicy(4, 4, milliseconds(10)));
    Base::TThreadLocalPoolManager<Stig::Indy::Fiber::TFrame, size_t, Stig::Indy::Fiber::TRunner *> *frame_pool_manager = Stig::Indy::Fiber::TFrame::LocalFramePool->GetPoolManager();

    Util::TCacheCb cache_cb = [](Util::TCacheInstr, const Util::TOffset , void *, size_t ) {};
    std::unique_ptr<Util::TMemoryDevice> mem_device(new Util::TMemoryDevice(512, 512, 262144, true /* fsync */, true));
    std::unique_ptr<Util::TVolume> volume(new Util::TVolume(Util::TVolume::TDesc{Util::TVolume::TDesc::Striped, mem_device->GetDesc(), Util::TVolume::TDesc::Fast, 1UL, 1UL, 1024UL, 8UL, 0.85}, cache_cb, &scheduler));
    volume->AddDevice(mem_device.get(), 0UL);
    std::unique_ptr<Util::TVolumeManager> vol_man(new Util::TVolumeManager(&scheduler));
    vol_man->AddNewVolume(volume.get());

    Util::TBlockRange tmp_range;
    vol_man->TryAllocateSequentialBlocks(Util::TVolume::TDesc::TStorageSpeed::Fast, 1UL, [&](const Util::TBlockRange &block_range) {
      tmp_range = block_range;
    });
    assert(tmp_range.second == 1UL);
    size_t image_1_block_id = tmp_range.first;
    vol_man->TryAllocateSequentialBlocks(Util::TVolume::TDesc::TStorageSpeed::Fast, 1UL, [&](const Util::TBlockRange &block_range) {
      tmp_range = block_range;
    });
    assert(tmp_range.second == 1UL);
    size_t image_2_block_id = tmp_range.first;
    std::vector<size_t> append_log_block_vec;
    vol_man->TryAllocateSequentialBlocks(Util::TVolume::TDesc::TStorageSpeed::Fast, num_append_log_blocks, [&](const Util::TBlockRange &range) {
      tmp_range = range;
    });
    append_log_block_vec.reserve(tmp_range.second);
    for (size_t i = 0; i < tmp_range.second; ++i) {
      append_log_block_vec.push_back(tmp_range.first + i);
    }
    Base::TUuid file_uid(Base::TUuid::Twister);
    size_t phase = 0UL;
    TFileService::TFileInitCb file_init_cb = [](TFileObj::TKind /*file kind*/,
                                                const Base::TUuid &/*file_uid*/,
                                                size_t /*gen_id*/,
                                                size_t /*starting block id*/,
                                                size_t /*starting block offset*/,
                                                size_t /*file_length*/) -> bool {return true;};
    try {
      /* initial clean fs */ {
        TFileService fs(&scheduler, runner_cons, frame_pool_manager, vol_man.get(), image_1_block_id, image_2_block_id, append_log_block_vec, file_init_cb, true, false);
        phase = 10;
        clean_image_cb(fs);
      } /* close initial clean fs */
      phase = 20;
      downtime_cb(vol_man.get(), image_1_block_id, image_2_block_id, append_log_block_vec);
      phase = 30;
      /* re-load existing fs */ {
        TFileService fs(&scheduler, runner_cons, frame_pool_manager, vol_man.get(), image_1_block_id, image_2_block_id, append_log_block_vec, file_init_cb, false, false);
        phase = 40;
        cold_image_cb(fs);
      }
      phase = 50;
    } catch (const std::exception &ex) {
      err_cb(ex, phase);
      sleep(2); /* this is because the disk controller is not shutting down properly. */
    }
    std::lock_guard<std::mutex> lock(mut);
    fin = true;
    cond.notify_one();
  }, 2);
}

void CleanImageCbInsertOnly(const Base::TUuid &file_uid, size_t num_files, TFileService &fs) {
  for (size_t i = 1; i < (num_files + 1); ++i) {  /* insert files */
    TCompletionTrigger trigger;
    fs.InsertFile(file_uid,
                  TFileObj::DataFile,
                  i,
                  i * 12UL,
                  i * 123UL,
                  i * 4096 * 16 * 32,
                  i * 2013UL,
                  i * 9UL,
                  i * 2022UL,
                  trigger);
    trigger.Wait();
  }
}

void ColdImageCbCheckInsertOnly(const Base::TUuid &file_uid, size_t num_files, TFileService &fs) {
  for (size_t i = 1; i < (num_files + 1); ++i) {  /* check files */
    size_t out_block_id, out_block_offset, out_file_size, out_num_keys;
    bool found = fs.FindFile(file_uid,
                             i,
                             out_block_id,
                             out_block_offset,
                             out_file_size,
                             out_num_keys);
    if (EXPECT_TRUE(found)) {
      EXPECT_EQ(out_block_id, i * 12UL);
      EXPECT_EQ(out_block_offset, i * 123UL);
      EXPECT_EQ(out_file_size, i * 4096 * 16 * 32);
      EXPECT_EQ(out_num_keys, i * 2013UL);
    }
  }
}

void CleanImageCbInsertRemoveOdd(const Base::TUuid &file_uid, size_t num_files, TFileService &fs) {
  for (size_t i = 1; i < (num_files + 1); ++i) {  /* insert files */
    TCompletionTrigger trigger;
    fs.InsertFile(file_uid,
                  TFileObj::DataFile,
                  i,
                  i * 12UL,
                  i * 123UL,
                  i * 4096 * 16 * 32,
                  i * 2013UL,
                  i * 9UL,
                  i * 2022UL,
                  trigger);
    trigger.Wait();
    if (i % 2 == 1) {
      fs.RemoveFile(file_uid,
                    i,
                    trigger);
      trigger.Wait();
    }
  }
}

void ColdImageCbCheckInsertRemoveOdd(const Base::TUuid &file_uid, size_t num_files, TFileService &fs) {
  for (size_t i = 1; i < (num_files + 1); ++i) {  /* check files */
    size_t out_block_id, out_block_offset, out_file_size, out_num_keys;
    bool found = fs.FindFile(file_uid,
                             i,
                             out_block_id,
                             out_block_offset,
                             out_file_size,
                             out_num_keys);
    if (i % 2 == 0) {
      if (EXPECT_TRUE(found)) {
        EXPECT_EQ(out_block_id, i * 12UL);
        EXPECT_EQ(out_block_offset, i * 123UL);
        EXPECT_EQ(out_file_size, i * 4096 * 16 * 32);
        EXPECT_EQ(out_num_keys, i * 2013UL);
      }
    } else {
      EXPECT_FALSE(found);
    }
  }
}

FIXTURE(NoBaseImageRestartInsertOnly) {
  const size_t num_files = 500UL;
  const size_t num_append_log_blocks = 4UL;
  Base::TUuid file_uid(Base::TUuid::Twister);
  auto downtime_cb = [](Util::TVolumeManager */*vol_man*/, size_t /*image_1_block*/, size_t /*image_2_block*/, const std::vector<size_t> &/*append_log_block_vec*/){};
  bool caught_err = false;
  auto err_cb = [&caught_err](const std::exception &/*ex*/, size_t /*phase*/) {
    caught_err = true;
    EXPECT_TRUE(false);
  };
  TestFileServiceRestart(num_append_log_blocks,
                         std::bind(CleanImageCbInsertOnly, file_uid, num_files, std::placeholders::_1),
                         downtime_cb,
                         std::bind(ColdImageCbCheckInsertOnly, file_uid, num_files, std::placeholders::_1),
                         err_cb);
  EXPECT_FALSE(caught_err);
}

FIXTURE(NoBaseImageRestartInsertRemove) {
  const size_t num_files = 300UL;
  const size_t num_append_log_blocks = 4UL;
  Base::TUuid file_uid(Base::TUuid::Twister);
  auto downtime_cb = [](Util::TVolumeManager */*vol_man*/, size_t /*image_1_block*/, size_t /*image_2_block*/, const std::vector<size_t> &/*append_log_block_vec*/){};
  bool caught_err = false;
  auto err_cb = [&caught_err](const std::exception &ex, size_t phase) {
    std::cout << ex.what() << std::endl;
    std::cout << phase << std::endl;
    caught_err = true;
    EXPECT_TRUE(false);
  };
  TestFileServiceRestart(num_append_log_blocks,
                         std::bind(CleanImageCbInsertRemoveOdd, file_uid, num_files, std::placeholders::_1),
                         downtime_cb,
                         std::bind(ColdImageCbCheckInsertRemoveOdd, file_uid, num_files, std::placeholders::_1),
                         err_cb);
  EXPECT_FALSE(caught_err);
}

FIXTURE(WithSingleBaseImageRestartInsertOnly) {
  const size_t num_files = 680UL;
  const size_t num_append_log_blocks = 4UL;
  Base::TUuid file_uid(Base::TUuid::Twister);
  auto downtime_cb = [](Util::TVolumeManager */*vol_man*/, size_t /*image_1_block*/, size_t /*image_2_block*/, const std::vector<size_t> &/*append_log_block_vec*/){};
  bool caught_err = false;
  auto err_cb = [&caught_err](const std::exception &/*ex*/, size_t /*phase*/) {
    caught_err = true;
    EXPECT_TRUE(false);
  };
  TestFileServiceRestart(num_append_log_blocks,
                         std::bind(CleanImageCbInsertOnly, file_uid, num_files, std::placeholders::_1),
                         downtime_cb,
                         std::bind(ColdImageCbCheckInsertOnly, file_uid, num_files, std::placeholders::_1),
                         err_cb);
  EXPECT_FALSE(caught_err);
}

FIXTURE(WithSingleBaseImageRestartInsertRemove) {
  const size_t num_files = 500UL;
  const size_t num_append_log_blocks = 4UL;
  Base::TUuid file_uid(Base::TUuid::Twister);
  auto downtime_cb = [](Util::TVolumeManager */*vol_man*/, size_t /*image_1_block*/, size_t /*image_2_block*/, const std::vector<size_t> &/*append_log_block_vec*/){};
  bool caught_err = false;
  auto err_cb = [&caught_err](const std::exception &/*ex*/, size_t /*phase*/) {
    caught_err = true;
    EXPECT_TRUE(false);
  };
  TestFileServiceRestart(num_append_log_blocks,
                         std::bind(CleanImageCbInsertRemoveOdd, file_uid, num_files, std::placeholders::_1),
                         downtime_cb,
                         std::bind(ColdImageCbCheckInsertRemoveOdd, file_uid, num_files, std::placeholders::_1),
                         err_cb);
  EXPECT_FALSE(caught_err);
}

FIXTURE(WithMultiBaseImageRestartInsertOnly) {
  const size_t num_files = 2048UL;
  const size_t num_append_log_blocks = 4UL;
  Base::TUuid file_uid(Base::TUuid::Twister);
  auto downtime_cb = [](Util::TVolumeManager */*vol_man*/, size_t /*image_1_block*/, size_t /*image_2_block*/, const std::vector<size_t> &/*append_log_block_vec*/){};
  bool caught_err = false;
  auto err_cb = [&caught_err](const std::exception &/*ex*/, size_t /*phase*/) {
    caught_err = true;
    EXPECT_TRUE(false);
  };
  TestFileServiceRestart(num_append_log_blocks,
                         std::bind(CleanImageCbInsertOnly, file_uid, num_files, std::placeholders::_1),
                         downtime_cb,
                         std::bind(ColdImageCbCheckInsertOnly, file_uid, num_files, std::placeholders::_1),
                         err_cb);
  EXPECT_FALSE(caught_err);
}

FIXTURE(WithMultiBaseImageRestartInsertRemove) {
  const size_t num_files = 2048UL;
  const size_t num_append_log_blocks = 4UL;
  Base::TUuid file_uid(Base::TUuid::Twister);
  auto downtime_cb = [](Util::TVolumeManager */*vol_man*/, size_t /*image_1_block*/, size_t /*image_2_block*/, const std::vector<size_t> &/*append_log_block_vec*/){};
  bool caught_err = false;
  auto err_cb = [&caught_err](const std::exception &/*ex*/, size_t /*phase*/) {
    caught_err = true;
    EXPECT_TRUE(false);
  };
  TestFileServiceRestart(num_append_log_blocks,
                         std::bind(CleanImageCbInsertRemoveOdd, file_uid, num_files, std::placeholders::_1),
                         downtime_cb,
                         std::bind(ColdImageCbCheckInsertRemoveOdd, file_uid, num_files, std::placeholders::_1),
                         err_cb);
  EXPECT_FALSE(caught_err);
}

FIXTURE(WithCorruptBaseImageRestart) {
  const size_t num_files = 2048UL;
  const size_t num_append_log_blocks = 4UL;
  Base::TUuid file_uid(Base::TUuid::Twister);
  auto downtime_cb = [](Util::TVolumeManager *vol_man, size_t image_1_block, size_t image_2_block, const std::vector<size_t> &/*append_log_block_vec*/) {
    std::mt19937_64 engine;
    std::unique_ptr<TBufBlock> image_1_buf(new TBufBlock());
    std::unique_ptr<TBufBlock> image_2_buf(new TBufBlock());
    TCompletionTrigger trigger;
    vol_man->ReadBlock(HERE, Util::CheckedBlock, 0UL, image_1_buf->GetData(), image_1_block, RealTime, trigger);
    vol_man->ReadBlock(HERE, Util::CheckedBlock, 0UL, image_2_buf->GetData(), image_2_block, RealTime, trigger);
    trigger.Wait();
    size_t *buf_1 = reinterpret_cast<size_t *>(image_1_buf->GetData());
    size_t *buf_2 = reinterpret_cast<size_t *>(image_2_buf->GetData());
    const size_t version_1 = buf_1[0];
    const size_t version_2 = buf_2[0];
    if (EXPECT_NE(version_1, version_2)) {
      size_t *cur_buf = nullptr;
      TBufBlock *cur_buf_block = nullptr;
      size_t cur_block_id = -1;
      /* let's corrupt the image block that has the highest version number */
      if (version_1 > version_2) {
        cur_buf = buf_1;
        cur_buf_block = image_1_buf.get();
        cur_block_id = image_1_block;
      } else {
        cur_buf = buf_2;
        cur_buf_block = image_2_buf.get();
        cur_block_id = image_2_block;
      }
      IntroduceRandomBitError(cur_buf, Util::PhysicalBlockSize, engine);
      vol_man->WriteBlock(HERE, Util::FullBlock, 0UL, cur_buf_block->GetData(), cur_block_id, RealTime, Util::TCacheInstr::NoCache, trigger);
      trigger.Wait();
    }
  };
  auto cold_image_cb = [](TFileService &/*fs*/) {
    EXPECT_TRUE(false);
  };
  bool caught_err = false;
  auto err_cb = [&caught_err](const std::exception &/*ex*/, size_t phase) {
    caught_err = true;
    EXPECT_EQ(phase, 30UL);
  };
  TestFileServiceRestart(num_append_log_blocks,
                         std::bind(CleanImageCbInsertRemoveOdd, file_uid, num_files, std::placeholders::_1),
                         downtime_cb,
                         cold_image_cb,
                         err_cb);
  EXPECT_TRUE(caught_err);
}

FIXTURE(WithCorruptAppendLogRestart) {
  const size_t num_files = 2048UL;
  const size_t num_append_log_blocks = 4UL;
  Base::TUuid file_uid(Base::TUuid::Twister);
  auto downtime_cb = [](Util::TVolumeManager *vol_man, size_t /*image_1_block*/, size_t /*image_2_block*/, const std::vector<size_t> &append_log_block_vec) {
    std::mt19937_64 engine;
    std::unique_ptr<TBufBlock> append_buf(new TBufBlock());
    TCompletionTrigger trigger;
    vol_man->Read(HERE, Util::CheckedSector, 0UL, append_buf->GetData(), append_log_block_vec[0] * Util::PhysicalBlockSize, Util::PhysicalSectorSize, RealTime, trigger);
    trigger.Wait();
    size_t *buf = reinterpret_cast<size_t *>(append_buf->GetData());
    IntroduceRandomBitError(buf, Util::PhysicalSectorSize, engine);
    vol_man->Write(HERE, Util::FullSector, 0UL, append_buf->GetData(), append_log_block_vec[0] * Util::PhysicalBlockSize, Util::PhysicalSectorSize, RealTime, Util::TCacheInstr::NoCache, trigger);
    trigger.Wait();
  };
  auto cold_image_cb = [](TFileService &/*fs*/) {
    EXPECT_TRUE(false);
  };
  bool caught_err = false;
  auto err_cb = [&caught_err](const std::exception &/*ex*/, size_t phase) {
    caught_err = true;
    EXPECT_EQ(phase, 30UL);
  };
  TestFileServiceRestart(num_append_log_blocks,
                         std::bind(CleanImageCbInsertRemoveOdd, file_uid, num_files, std::placeholders::_1),
                         downtime_cb,
                         cold_image_cb,
                         err_cb);
  EXPECT_TRUE(caught_err);
}
