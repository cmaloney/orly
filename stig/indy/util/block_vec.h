/* <stig/indy/util/block_vec.h>

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
#include <iterator>
#include <functional>
#include <map>
#include <sstream>

#include <base/likely.h>

namespace Stig {

  namespace Indy {

    namespace Util {

      /* TODO */
      class TBlockVec {
        public:

        /* TODO */
        typedef std::map<size_t, std::pair<size_t, size_t>> TBlockMap;

        /* TODO */
        class TRandomIterator
              : public std::iterator<std::random_access_iterator_tag, size_t> {
          protected:

          /* TODO */
          const TBlockMap *BlockMap;

          /* TODO */
          TBlockMap::const_iterator Iterator;

          /* TODO */
          size_t ElementIndex;

          public:

          /* TODO */
          typedef std::random_access_iterator_tag iterator_category;
          typedef typename std::iterator<std::random_access_iterator_tag, size_t>::value_type value_type;
          typedef typename std::iterator<std::random_access_iterator_tag, size_t>::difference_type difference_type;
          typedef typename std::iterator<std::random_access_iterator_tag, size_t>::reference reference;
          typedef typename std::iterator<std::random_access_iterator_tag, size_t>::pointer pointer;

          /* TODO */
          TRandomIterator() : BlockMap(nullptr), ElementIndex(0UL) {}

          /* TODO */
          inline TRandomIterator(const TRandomIterator &r)
              : BlockMap(r.BlockMap), Iterator(r.Iterator), ElementIndex(r.ElementIndex) {}

          /* TODO */
          inline TRandomIterator(const TBlockMap *block_map, const TBlockMap::const_iterator &it, size_t elem_idx)
              : BlockMap(block_map), Iterator(it), ElementIndex(elem_idx) {
            if (Iterator != BlockMap->end()) {
              Refresh();
            } else if (Iterator != BlockMap->begin()) {
              --Iterator;
            } /* otherwise it's an empty block vec */
          }

          /* TODO */
          inline TRandomIterator &operator=(const TRandomIterator &r) {
            BlockMap = r.BlockMap;
            Iterator = r.Iterator;
            ElementIndex = r.ElementIndex;
            Refresh();
            return *this;
          }

          /* TODO */
          inline TRandomIterator& operator++() {
            ++ElementIndex;
            Refresh();
            return *this;
          }

          /* TODO */
          inline TRandomIterator& operator--() {
            --ElementIndex;
            Refresh();
            return *this;
          }

          /* TODO */
          inline TRandomIterator operator++(int) {
            return TRandomIterator(BlockMap, Iterator, ElementIndex++);
          }

          /* TODO */
          inline TRandomIterator operator--(int) {
            return TRandomIterator(BlockMap, Iterator, ElementIndex--);
          }

          /* TODO */
          inline TRandomIterator operator+(const difference_type& n) const {
            return TRandomIterator(BlockMap, Iterator, ElementIndex + n);
          }

          /* TODO */
          inline TRandomIterator& operator+=(const difference_type& n) {
            ElementIndex += n;
            Refresh();
            return *this;
          }

          /* TODO */
          inline TRandomIterator operator-(const difference_type& n) const {
            return TRandomIterator(BlockMap, Iterator, ElementIndex - n);
          }

          /* TODO */
          inline TRandomIterator& operator-=(const difference_type& n) {
            ElementIndex -= n;
            Refresh();
            return *this;
          }

          /* TODO */
          inline size_t operator*() const {
            assert(BlockMap);
            assert(Iterator != BlockMap->end());
            const size_t start_idx = Iterator->first;
            const int64_t pos_from_start_idx = static_cast<int64_t>(ElementIndex) - start_idx;
            #ifndef NDEBUG
            const size_t end_idx = start_idx + Iterator->second.second;
            assert(pos_from_start_idx < static_cast<int64_t>(end_idx) && pos_from_start_idx >= 0L);
            #endif
            return Iterator->second.first + pos_from_start_idx;
          }

          /* TODO */
          inline size_t operator->() const {
            return **this;
          }

          /* TODO */
          friend inline bool operator==(const TRandomIterator &r1, const TRandomIterator &r2);

          /* TODO */
          friend inline bool operator!=(const TRandomIterator &r1, const TRandomIterator &r2);

          /* TODO */
          friend inline bool operator<(const TRandomIterator &r1, const TRandomIterator &r2);

          /* TODO */
          friend inline bool operator>(const TRandomIterator &r1, const TRandomIterator &r2);

          /* TODO */
          friend inline bool operator<=(const TRandomIterator &r1, const TRandomIterator &r2);

          /* TODO */
          friend inline bool operator>=(const TRandomIterator &r1, const TRandomIterator &r2);

          /* TODO */
          friend typename TRandomIterator::difference_type operator+(const TRandomIterator &r1, const TRandomIterator &r2);

          /* TODO */
          friend typename TRandomIterator::difference_type operator-(const TRandomIterator &r1, const TRandomIterator &r2);

          private:

          /* TODO */
          inline void Refresh() {
            assert(BlockMap);
            assert(Iterator != BlockMap->end());
            Start:
            const size_t start_idx = Iterator->first;
            const size_t end_idx = start_idx + Iterator->second.second;
            if (ElementIndex >= end_idx && Iterator != BlockMap->end()) {
              /* we need to move forward */
              ++Iterator;
              goto Start;
            } else if (ElementIndex < start_idx && Iterator != BlockMap->begin()) {
              /* we need to move backward */
              --Iterator;
              goto Start;
            }
            assert(Iterator == BlockMap->end() ||
                   (ElementIndex >= Iterator->first && ElementIndex < (Iterator->first + Iterator->second.second)));
          }

        };  // TRandomIterator

        /* TODO */
        TBlockVec() : VecSize(0UL) {}

        /* TODO */
        ~TBlockVec() {}

        /* TODO */
        inline size_t operator[](size_t pos) const {
          assert(this);
          assert(pos < VecSize);
          auto ub = BlockMap.upper_bound(pos);
          assert(ub != BlockMap.begin());
          --ub;
          assert(ub != BlockMap.end());
          const size_t expected_block_offset = pos - ub->first;
          assert(ub->second.second > expected_block_offset);
          return ub->second.first + expected_block_offset;
        }

        /* TODO */
        inline size_t Front() const {
          assert(this);
          assert(VecSize > 0UL);
          return BlockMap.begin()->second.first;
        }

        /* TODO */
        inline size_t Size() const {
          assert(this);
          return VecSize;
        }

        /* TODO */
        TRandomIterator begin() const {
          assert(this);
          return TRandomIterator(&BlockMap, BlockMap.begin(), 0UL);
        }

        /* TODO */
        TRandomIterator end() const {
          assert(this);
          return TRandomIterator(&BlockMap, BlockMap.end(), VecSize);
        }

        /* Push back a single block id */
        inline void PushBack(size_t block_id) {
          assert(this);
          if (unlikely(VecSize == 0)) {
            BlockMap.emplace(0, std::make_pair(block_id, 1UL));
          } else {
            assert(BlockMap.size() > 0UL);
            auto back = BlockMap.rbegin();
            /* back is the last vector index added. If the last block in the sequential allocation is 1 before what we're adding now, we can just add
               1 to the count. Otherwise we need to make a new entry. */
            if (back->second.first + back->second.second == block_id) {
              ++back->second.second;
            } else {
              BlockMap.emplace(VecSize, std::make_pair(block_id, 1UL));
            }
          }
          ++VecSize;
        }

        /* Push back a sequential range */
        inline void PushBack(const std::pair<size_t, size_t> &block_range) {
          assert(this);
          if (unlikely(VecSize == 0)) {
            BlockMap.emplace(0, std::make_pair(block_range.first, block_range.second));
          } else {
            assert(BlockMap.size() > 0UL);
            auto back = BlockMap.rbegin();
            /* back is the last vector index added. If the last block in the sequential allocation is 1 before what we're adding now, we can just add
               n to the count. Otherwise we need to make a new entry. */
            if (back->second.first + back->second.second == block_range.first) {
              back->second.second += block_range.second;
            } else {
              BlockMap.emplace(VecSize, std::make_pair(block_range.first, block_range.second));
            }
          }
          VecSize += block_range.second;
        }

        /* remove the last n elements from the vector */
        void Trim(size_t n) {
          assert(this);
          assert(n <= VecSize);
          const size_t pos = VecSize - n;
          auto pub = BlockMap.upper_bound(pos);
          auto ub = pub;
          --ub;
          assert(ub != BlockMap.end());
          const size_t idx_of_map_entry = ub->first;
          if (idx_of_map_entry == pos) {
            BlockMap.erase(ub, BlockMap.end());
          } else {
            assert(pos > idx_of_map_entry);
            ub->second.second = pos - idx_of_map_entry;
            BlockMap.erase(pub, BlockMap.end());
          }
          VecSize -= n;
        }

        /* TODO */
        const TBlockMap &GetSeqBlockMap() const {
          assert(this);
          return BlockMap;
        }

        /* Call back for each sequential range (block, num seq block) where the index is >= start_index and < end_index */
        void ForEachSeqRangeInRange(const std::function<bool (size_t, size_t)> &cb, size_t start_index, size_t end_index) const {
          assert(this);
          assert(start_index <= end_index);
          if (start_index < end_index) {
            auto ub = BlockMap.upper_bound(start_index);
            assert(ub != BlockMap.begin());
            --ub;
            assert(ub != BlockMap.end());
            const size_t from_block_offset = start_index - ub->first;
            assert(ub->second.second > from_block_offset);
            size_t left = end_index - start_index;
            size_t amt = std::min(left, ub->second.second - from_block_offset);
            bool cont = cb(ub->second.first + from_block_offset, amt);
            left -= amt;
            ++ub;
            for (; cont && left > 0; ) {
              assert(ub != BlockMap.end());
              amt = std::min(left, ub->second.second);
              cont = cb(ub->second.first, amt);
              left -= amt;
              ++ub;
            }
          }
        }

        /* Debug Print facility */
        void Print(std::stringstream &ss) const {
          assert(this);
          for (const auto &it : BlockMap) {
            ss << "[" << it.first << "] -> (" << it.second.first << " -> " << it.second.second << ")" << std::endl;
          }
        }

        private:

        /* A mapping from the starting index in the vector (of a sequential range) to the pair of (block_id -> num sequential blocks)*/
        TBlockMap BlockMap;

        /* TODO */
        size_t VecSize;

      };  // TBlockVec

      /* TODO */
      inline bool operator==(const TBlockVec::TRandomIterator &r1, const TBlockVec::TRandomIterator &r2) {
        return r1.ElementIndex == r2.ElementIndex;
      }

      /* TODO */
      inline bool operator!=(const TBlockVec::TRandomIterator &r1, const TBlockVec::TRandomIterator &r2) {
        return r1.ElementIndex != r2.ElementIndex;
      }

      /* TODO */
      inline bool operator<(const TBlockVec::TRandomIterator &r1, const TBlockVec::TRandomIterator &r2) {
        return r1.ElementIndex < r2.ElementIndex;
      }

      /* TODO */
      inline bool operator>(const TBlockVec::TRandomIterator &r1, const TBlockVec::TRandomIterator &r2) {
        return r1.ElementIndex > r2.ElementIndex;
      }

      /* TODO */
      inline bool operator<=(const TBlockVec::TRandomIterator &r1, const TBlockVec::TRandomIterator &r2) {
        return r1.ElementIndex <= r2.ElementIndex;
      }

      /* TODO */
      inline bool operator>=(const TBlockVec::TRandomIterator &r1, const TBlockVec::TRandomIterator &r2) {
        return r1.ElementIndex >= r2.ElementIndex;
      }

    }  // Util

  }  // Indy

}  // Stig