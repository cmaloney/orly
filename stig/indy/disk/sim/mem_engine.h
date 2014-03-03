/* <stig/indy/disk/sim/mem_engine.h> 

   TODO

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

#pragma once

#include <stig/indy/disk/util/engine.h>
#include <stig/indy/disk/test_file_service.h>

namespace Stig {

  namespace Indy {

    namespace Disk {

      namespace Sim {

        /* TODO */
        class TMemEngine {
          NO_COPY_SEMANTICS(TMemEngine);
          public:

          /* TODO */
          TMemEngine(Base::TScheduler *scheduler, size_t num_mb, size_t num_slow_mb, size_t page_cache_size, size_t num_page_lru, size_t block_cache_size, size_t num_block_lru) {
            const size_t logical_block_size = 512;
            const size_t physical_block_size = 512;
            const size_t num_logical_block_per_stripe = 1024UL;
            const size_t num_fast_logical_block = num_mb * ((1024 * 1024) / logical_block_size);
            const size_t num_slow_logical_block = num_slow_mb * ((1024 * 1024) / logical_block_size);
            const size_t min_discard_blocks = 8UL;
            const double high_utilization_threshold = 0.85;
            if (num_fast_logical_block % NumFastMemDevice != 0) {
              throw std::runtime_error("TMemEngine num_fast_logical_block must be a multiple of NumFastMemDevice");
            }
            if (num_slow_logical_block % NumSlowMemDevice != 0) {
              throw std::runtime_error("TMemEngine num_slow_logical_block must be a multiple of NumSlowMemDevice");
            }
            CacheCb = [this](Util::TCacheInstr cache_instr, const Util::TOffset logical_start_offset, void *buf, size_t count) {
              switch (cache_instr) {
                case Util::TCacheInstr::CacheAll: {
                  assert(PageCache.get());
                  assert(BlockCache.get());
                  assert(logical_start_offset % Util::TPageCache::DataSize == 0);
                  assert(count % Util::TPageCache::DataSize == 0);
                  assert(logical_start_offset % Util::TBlockCache::DataSize == 0);
                  assert(count % Util::TBlockCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TPageCache::DataSize); ++i) {
                    PageCache->Replace((logical_start_offset / Util::TPageCache::DataSize) + i, reinterpret_cast<uint8_t *>(buf) + (i * Util::TPageCache::DataSize));
                  }
                  for (size_t i = 0; i < (count / Util::TBlockCache::DataSize); ++i) {
                    BlockCache->Replace((logical_start_offset / Util::TBlockCache::DataSize) + i, reinterpret_cast<uint8_t *>(buf) + (i * Util::TBlockCache::DataSize));
                  }
                }
                case Util::TCacheInstr::CachePageOnly: {
                  assert(PageCache.get());
                  assert(logical_start_offset % Util::TPageCache::DataSize == 0);
                  assert(count % Util::TPageCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TPageCache::DataSize); ++i) {
                    PageCache->Replace((logical_start_offset / Util::TPageCache::DataSize) + i, reinterpret_cast<uint8_t *>(buf) + (i * Util::TPageCache::DataSize));
                  }
                  break;
                }
                case Util::TCacheInstr::CacheBlockOnly: {
                  assert(BlockCache.get());
                  assert(logical_start_offset % Util::TBlockCache::DataSize == 0);
                  assert(count % Util::TBlockCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TBlockCache::DataSize); ++i) {
                    BlockCache->Replace((logical_start_offset / Util::TBlockCache::DataSize) + i, reinterpret_cast<uint8_t *>(buf) + (i * Util::TBlockCache::DataSize));
                  }
                  break;
                }
                case Util::TCacheInstr::ClearAll: {
                  assert(PageCache.get());
                  assert(BlockCache.get());
                  assert(logical_start_offset % Util::TPageCache::DataSize == 0);
                  assert(count % Util::TPageCache::DataSize == 0);
                  assert(logical_start_offset %Util::TBlockCache::DataSize == 0);
                  assert(count % Util::TBlockCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TPageCache::DataSize); ++i) {
                    PageCache->Clear((logical_start_offset / Util::TPageCache::DataSize) + i);
                  }
                  for (size_t i = 0; i < (count / Util::TBlockCache::DataSize); ++i) {
                    BlockCache->Clear((logical_start_offset / Util::TBlockCache::DataSize) + i);
                  }
                }
                case Util::TCacheInstr::ClearPageOnly: {
                  assert(PageCache.get());
                  assert(logical_start_offset % Util::TPageCache::DataSize == 0);
                  assert(count % Util::TPageCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TPageCache::DataSize); ++i) {
                    PageCache->Clear((logical_start_offset / Util::TPageCache::DataSize) + i);
                  }
                  break;
                }
                case Util::TCacheInstr::ClearBlockOnly: {
                  assert(BlockCache.get());
                  assert(logical_start_offset %Util::TBlockCache::DataSize == 0);
                  assert(count % Util::TBlockCache::DataSize == 0);
                  for (size_t i = 0; i < (count / Util::TBlockCache::DataSize); ++i) {
                    BlockCache->Clear((logical_start_offset / Util::TBlockCache::DataSize) + i);
                  }
                  break;
                }
                case Util::TCacheInstr::NoCache: {
                  break;
                }
              }
            };
            for (size_t i = 0; i < NumFastMemDevice; ++i) {
              FastMemDeviceArray[i] = std::unique_ptr<Util::TMemoryDevice>(new Util::TMemoryDevice(logical_block_size, physical_block_size, num_fast_logical_block / NumFastMemDevice, true /* fsync */, true));
            }
            for (size_t i = 0; i < NumSlowMemDevice; ++i) {
              SlowMemDeviceArray[i] = std::unique_ptr<Util::TMemoryDevice>(new Util::TMemoryDevice(logical_block_size, physical_block_size, num_slow_logical_block / NumSlowMemDevice, true /* fsync */, true));
            }
            FastVolume = std::unique_ptr<Util::TVolume>(new Util::TVolume(Util::TVolume::TDesc{Util::TVolume::TDesc::Striped, FastMemDeviceArray[0]->GetDesc(), Util::TVolume::TDesc::Fast, 1UL, NumFastMemDevice, num_logical_block_per_stripe, min_discard_blocks, high_utilization_threshold}, CacheCb, scheduler));
            SlowVolume = std::unique_ptr<Util::TVolume>(new Util::TVolume(Util::TVolume::TDesc{Util::TVolume::TDesc::Striped, SlowMemDeviceArray[0]->GetDesc(), Util::TVolume::TDesc::Slow, 1UL, NumSlowMemDevice, num_logical_block_per_stripe, min_discard_blocks, high_utilization_threshold}, CacheCb, scheduler));
            for (size_t i = 0; i < NumFastMemDevice; ++i) {
              FastVolume->AddDevice(FastMemDeviceArray[i].get(), i);
            }
            for (size_t i = 0; i < NumSlowMemDevice; ++i) {
              SlowVolume->AddDevice(SlowMemDeviceArray[i].get(), i);
            }
            VolMan = std::unique_ptr<Util::TVolumeManager>(new Util::TVolumeManager(scheduler));
            VolMan->AddNewVolume(FastVolume.get());
            VolMan->AddNewVolume(SlowVolume.get());
            PageCache = std::unique_ptr<Util::TPageCache>(new Util::TPageCache(VolMan.get(), page_cache_size, num_page_lru));
            BlockCache = std::unique_ptr<Util::TBlockCache>(new Util::TBlockCache(VolMan.get(), block_cache_size, num_block_lru));
            Engine = std::unique_ptr<Util::TEngine>(new Util::TEngine(VolMan.get(), PageCache.get(), BlockCache.get(), &TestFileService, false));
          }

          /* TODO */
          Util::TEngine *GetEngine() const {
            return Engine.get();
          }

          /* TODO */
          Util::TPageCache *GetPageCache() const {
            return PageCache.get();
          }

          /* TODO */
          Util::TBlockCache *GetBlockCache() const {
            return BlockCache.get();
          }

          /* TODO */
          Util::TVolumeManager *GetVolMan() const {
            return VolMan.get();
          }

          private:

          static constexpr size_t NumFastMemDevice = 4UL;

          static constexpr size_t NumSlowMemDevice = 1UL;

          /* TODO */
          TTestFileService TestFileService;

          /* TODO */
          std::unique_ptr<Util::TMemoryDevice> FastMemDeviceArray[NumFastMemDevice];
          std::unique_ptr<Util::TMemoryDevice> SlowMemDeviceArray[NumSlowMemDevice];
          std::unique_ptr<Util::TVolume> FastVolume;
          std::unique_ptr<Util::TVolume> SlowVolume;
          std::unique_ptr<Util::TVolumeManager> VolMan;

          /* TODO */
          std::unique_ptr<Util::TPageCache> PageCache;
          std::unique_ptr<Util::TBlockCache> BlockCache;
          std::unique_ptr<Util::TEngine> Engine;

          /* TODO */
          Util::TCacheCb CacheCb;

        };

      }  // Sim

    }  // Disk

  }  // Indy

}  // Stig
