/* <io/binary_output_stream.h>

   An output stream in binary format.

   Copyright 2010-2014 OrlyAtomics, Inc.

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
#include <chrono>
#include <cstdint>
#include <cstring>
#include <list>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <uuid/uuid.h>

#include <base/class_traits.h>
#include <base/split.h>
#include <io/binary_stream.h>
#include <io/output_producer.h>
#include <util/tuple.h>

namespace Io {

  /* An output stream in binary format. */
  class TBinaryOutputStream
      : public virtual TBinaryStream,
        public TOutputProducer {
    NO_COPY(TBinaryOutputStream); public:

    /* Flushing is publicly available. */
    using TOutputProducer::Flush;

    /* TODO */
    using TOutputProducer::WriteExactly;

    /* Write built-in types. */
    void Write(bool that    ) { WriteWithoutSwap(that); }
    void Write(char that    ) { WriteWithoutSwap(that); }
    void Write(float that   ) { WriteWithoutSwap(that); }
    void Write(double that  ) { WriteWithoutSwap(that); }
    void Write(int8_t that  ) { WriteWithoutSwap(that); }
    void Write(int16_t that ) { WriteWithSwap(that); }
    void Write(int32_t that ) { WriteWithSwap(that); }
    void Write(int64_t that ) { WriteWithSwap(that); }
    void Write(uint8_t that ) { WriteWithoutSwap(that); }
    void Write(uint16_t that) { WriteWithSwap(that); }
    void Write(uint32_t that) { WriteWithSwap(that); }
    void Write(uint64_t that) { WriteWithSwap(that); }

    /* Write built-in types by ref. */
    void Write(const uuid_t &that) { WriteExactly(&that, sizeof(uuid_t)); }

    /* Write strings. */
    void Write(const char *that)        { WriteString(that, strlen(that)); }
    void Write(const std::string &that) { WriteString(that.data(), that.size()); }

    /* Write STL containers. */
    template <typename TVal, typename TAlloc>
    void Write(const std::list<TVal, TAlloc> &that) { WriteContainer(that); }
    template <typename TKey, typename TVal, typename TCompare, typename TAlloc>
    void Write(const std::map<TKey, TVal, TCompare, TAlloc> &that) { WriteContainer(that); }
    template <typename TKey, typename TVal, typename TCompare, typename TAlloc>
    void Write(const std::multimap<TKey, TVal, TCompare, TAlloc> &that) { WriteContainer(that); }
    template <typename TVal, typename TCompare, typename TAlloc>
    void Write(const std::multiset<TVal, TCompare, TAlloc> &that) { WriteContainer(that); }
    template <typename TFirst, typename TSecond>
    void Write(const std::pair<TFirst, TSecond> &that) { Write(that.first); Write(that.second); }
    template <typename TVal, typename TCompare, typename TAlloc>
    void Write(const std::set<TVal, TCompare, TAlloc> &that) { WriteContainer(that); }
    template <typename TKey, typename TVal, typename THash, typename TEq, typename TAlloc>
    void Write(const std::unordered_map<TKey, TVal, THash, TEq, TAlloc> &that) { WriteContainer(that); }
    template <typename TKey, typename TVal, typename THash, typename TEq, typename TAlloc>
    void Write(const std::unordered_multimap<TKey, TVal, THash, TEq, TAlloc> &that) { WriteContainer(that); }
    template <typename TVal, typename THash, typename TEq, typename TAlloc>
    void Write(const std::unordered_set<TVal, THash, TEq, TAlloc> &that) { WriteContainer(that); }
    template <typename TVal, typename THash, typename TEq, typename TAlloc>
    void Write(const std::unordered_multiset<TVal, THash, TEq, TAlloc> &that) { WriteContainer(that); }
    template <typename TVal, typename TAlloc>
    void Write(const std::vector<TVal, TAlloc> &that) { WriteContainer(that); }

    template <typename... TArgs>
    void Write(const std::tuple<TArgs...> &that) {
      assert(this);
      assert(&that);
      *this << Base::Concat(that);
    }

    template <typename TRep, typename TPeriod>
    void Write(const std::chrono::duration<TRep, TPeriod> &that) {
      assert(this);
      assert(&that);
      Write(that.count());
    }

    template <typename TContainer, typename TDelimiter, typename TFormat>
    void Write(const Base::TJoin<TContainer, TDelimiter, TFormat> &that) {
      assert(this);
      assert(&that);
      Base::WriteJoin(*this, that);
    }

    protected:

    /* Attach to the given consumer, if any.
       Construct our own pool. */
    explicit TBinaryOutputStream(const std::shared_ptr<TOutputConsumer> &output_consumer, const TPool::TArgs &args = TPool::TArgs())
        : TOutputProducer(output_consumer, std::make_shared<TPool>(args)) {}

    /* Attach to the given consumer, if any.
       Use the given pool, which must not be null. */
    TBinaryOutputStream(const std::shared_ptr<TOutputConsumer> &output_consumer, const std::shared_ptr<TPool> &pool)
        : TOutputProducer(output_consumer, pool) {}

    private:

    /* Write an STL container. */
    template <typename TThat>
    void WriteContainer(const TThat &that) {
      assert(this);
      assert(&that);
      Write(that.size());
      for (const typename TThat::value_type &val: that) {
        Write(val);
      }
    }

    /* Write a string. */
    void WriteString(const char *start, size_t size) {
      assert(this);
      Write(size);
      WriteExactly(start, size);
    }

    /* Write a built-in, converting to NBO if necessary. */
    template <typename TThat>
    void WriteWithSwap(TThat that) {
      assert(this);
      GetFormat().ConvertInt(that);
      WriteExactly(&that, sizeof(that));
    }

    /* Write a built-in without converting to NBO. */
    template <typename TThat>
    void WriteWithoutSwap(TThat that) {
      assert(this);
      WriteExactly(&that, sizeof(that));
    }

  };  // TBinaryOutputStream

  /* Stream inserters for built-in types. */
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, bool that    ) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, char that    ) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, float that   ) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, double that  ) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, int8_t that  ) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, int16_t that ) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, int32_t that ) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, int64_t that ) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, uint8_t that ) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, uint16_t that) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, uint32_t that) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, uint64_t that) { strm.Write(that); return strm; }

  /* Stream inserters for built-in types by ref. */
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const uuid_t &that) { strm.Write(that); return strm; }

  /* Stream inserters for strings. */
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const char *that       ) { strm.Write(that); return strm; }
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::string &that) { strm.Write(that); return strm; }

  /* Stream inserters for STL containers. */
  template <typename TVal, typename TAlloc>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::list<TVal, TAlloc> &that) { strm.Write(that); return strm; }
  template <typename TKey, typename TVal, typename TCompare, typename TAlloc>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::map<TKey, TVal, TCompare, TAlloc> &that) { strm.Write(that); return strm; }
  template <typename TKey, typename TVal, typename TCompare, typename TAlloc>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::multimap<TKey, TVal, TCompare, TAlloc> &that) { strm.Write(that); return strm; }
  template <typename TVal, typename TCompare, typename TAlloc>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::multiset<TVal, TCompare, TAlloc> &that) { strm.Write(that); return strm; }
  template <typename TFirst, typename TSecond>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::pair<TFirst, TSecond> &that) { strm.Write(that); return strm; }
  template <typename TVal, typename TCompare, typename TAlloc>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::set<TVal, TCompare, TAlloc> &that) { strm.Write(that); return strm; }
  template <typename TKey, typename TVal, typename THash, typename TEq, typename TAlloc>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::unordered_map<TKey, TVal, THash, TEq, TAlloc> &that) { strm.Write(that); return strm; }
  template <typename TKey, typename TVal, typename THash, typename TEq, typename TAlloc>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::unordered_multimap<TKey, TVal, THash, TEq, TAlloc> &that) { strm.Write(that); return strm; }
  template <typename TVal, typename THash, typename TEq, typename TAlloc>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::unordered_set<TVal, THash, TEq, TAlloc> &that) { strm.Write(that); return strm; }
  template <typename TVal, typename THash, typename TEq, typename TAlloc>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::unordered_multiset<TVal, THash, TEq, TAlloc> &that) { strm.Write(that); return strm; }
  template <typename TVal, typename TAlloc>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::vector<TVal, TAlloc> &that) { strm.Write(that); return strm; }

  /* Stream inserter for STL tuple. */
  template <typename... TArgs>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::tuple<TArgs...> &that) { strm.Write(that); return strm; }

  /* Stream inserter for std::chrono. */
  template <typename TRep, typename TPeriod>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const std::chrono::duration<TRep, TPeriod> &that) { strm.Write(that); return strm; }

  template <typename TContainer, typename TDelimiter, typename TFormat>
  inline TBinaryOutputStream &operator<<(TBinaryOutputStream &strm, const Base::TJoin<TContainer, TDelimiter, TFormat> &that) { strm.Write(that); return strm; }

  /* And again, for r-value references... */

  /* Stream inserters for built-in types. */
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, bool that    ) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, char that    ) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, float that   ) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, double that  ) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, int8_t that  ) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, int16_t that ) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, int32_t that ) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, int64_t that ) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, uint8_t that ) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, uint16_t that) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, uint32_t that) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, uint64_t that) { strm.Write(that); return std::move(strm); }

  /* Stream inserters for built-in types by ref. */
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const uuid_t &that) { strm.Write(that); return std::move(strm); }

  /* Stream inserters for strings. */
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const char *that       ) { strm.Write(that); return std::move(strm); }
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::string &that) { strm.Write(that); return std::move(strm); }

  /* Stream inserters for STL containers. */
  template <typename TVal, typename TAlloc>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::list<TVal, TAlloc> &that) { strm.Write(that); return std::move(strm); }
  template <typename TKey, typename TVal, typename TCompare, typename TAlloc>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::map<TKey, TVal, TCompare, TAlloc> &that) { strm.Write(that); return std::move(strm); }
  template <typename TKey, typename TVal, typename TCompare, typename TAlloc>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::multimap<TKey, TVal, TCompare, TAlloc> &that) { strm.Write(that); return std::move(strm); }
  template <typename TVal, typename TCompare, typename TAlloc>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::multiset<TVal, TCompare, TAlloc> &that) { strm.Write(that); return std::move(strm); }
  template <typename TFirst, typename TSecond>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::pair<TFirst, TSecond> &that) { strm.Write(that); return std::move(strm); }
  template <typename TVal, typename TCompare, typename TAlloc>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::set<TVal, TCompare, TAlloc> &that) { strm.Write(that); return std::move(strm); }
  template <typename TKey, typename TVal, typename THash, typename TEq, typename TAlloc>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::unordered_map<TKey, TVal, THash, TEq, TAlloc> &that) { strm.Write(that); return std::move(strm); }
  template <typename TKey, typename TVal, typename THash, typename TEq, typename TAlloc>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::unordered_multimap<TKey, TVal, THash, TEq, TAlloc> &that) { strm.Write(that); return std::move(strm); }
  template <typename TVal, typename THash, typename TEq, typename TAlloc>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::unordered_set<TVal, THash, TEq, TAlloc> &that) { strm.Write(that); return std::move(strm); }
  template <typename TVal, typename THash, typename TEq, typename TAlloc>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::unordered_multiset<TVal, THash, TEq, TAlloc> &that) { strm.Write(that); return std::move(strm); }
  template <typename TVal, typename TAlloc>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::vector<TVal, TAlloc> &that) { strm.Write(that); return std::move(strm); }

  /* Stream inserter for STL tuple. */
  template <typename... TArgs>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::tuple<TArgs...> &that) { strm.Write(that); return std::move(strm); }

  /* Stream inserter for std::chrono. */
  template <typename TRep, typename TPeriod>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const std::chrono::duration<TRep, TPeriod> &that) { strm.Write(that); return std::move(strm); }

  template <typename TContainer, typename TDelimiter, typename TFormat>
  inline TBinaryOutputStream &&operator<<(TBinaryOutputStream &&strm, const Base::TJoin<TContainer, TDelimiter, TFormat> &that) { strm.Write(that); return std::move(strm); }

}  // Io
