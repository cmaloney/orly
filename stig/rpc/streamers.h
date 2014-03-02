/* <stig/rpc/streamers.h> 

   Utilities for streaming binary data of known type into and out of blobs.

   Here's an example of using the output streamers (called 'inserters') to build a blob:

      TBufferPool buffer_pool;
      int a = 101, b = 202;
      TBlob blob = (TBlob::TWriter(&buffer_pool) << a << " + " << b << " = " << (a + b)).DraftBlob();

   And here's an example of using the input streamers ('extractors') to read the data back:

      int a, b, sum;
      std::string str1, str2;
      TBlob::TReader(&blob) >> a >> str >> b >> str2 >> sum;

   There are streamers for all the built-in types (except wchar_t), Base::TUuid, c-string (inserter
   only), std::string, std::pair, std::tuple, std::chrono::duration, std::chrono::time point,
   std::list, std::vector, std::set, std::multiset, std::map, std::multimap, std::unordered_set,
   std::unordered_multiset, std::unordered_map, and std::unordered_multimap.

   The inserters for the multi-byte integer types produce blobs containing data in network byte order.
   The extracters reverse this process and produce their results in host byte order.

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

#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <list>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <base/no_construction.h>
#include <base/uuid.h>
#include <io/endian.h>
#include <stig/rpc/blob.h>

namespace Stig {

  namespace Rpc {

    /* Copy data from the blob verbatim. */
    template <typename TThat>
    inline TBlob::TReader &ReadAsIs(TBlob::TReader &reader, TThat &that) {
      assert(&reader);
      assert(&that);
      return reader.Read(&that, sizeof(that));
    }

    /* Copy data from the blob, but in network byte order. */
    template <typename TThat>
    inline TBlob::TReader &ReadNbo(TBlob::TReader &reader, TThat &that) {
      assert(&reader);
      reader.Read(&that, sizeof(that));
      that = Io::SwapEnds(that);
      return reader;
    }

    /* Copy a string from the blob.  The string's length comes first, in NBO, then the string itself. */
    inline TBlob::TReader &ReadStr(TBlob::TReader &reader, std::string &that) {
      assert(&reader);
      assert(&that);
      TBlob::TElemCount size;
      ReadNbo(reader, size);
      that.resize(size);
      return reader.Read(const_cast<char *>(that.data()), size);
    }

    /* Copy a container from the blob.  The container must support insert(). */
    template <typename TThat>
    inline TBlob::TReader &ReadContainerWithInsert(TBlob::TReader &reader, TThat &that) {
      assert(&reader);
      assert(&that);
      TBlob::TElemCount size;
      ReadNbo(reader, size);
      TThat temp;
      for (TBlob::TElemCount i = 0; i < size; ++i) {
        typename TThat::value_type elem;
        reader >> elem;
        temp.insert(elem);
      }
      std::swap(temp, that);
      return reader;
    }

    /* Copy a container from the blob.  The container must support push_back(). */
    template <typename TThat>
    inline TBlob::TReader &ReadContainerWithPushBack(TBlob::TReader &reader, TThat &that) {
      assert(&reader);
      assert(&that);
      TBlob::TElemCount size;
      ReadNbo(reader, size);
      TThat temp;
      for (TBlob::TElemCount i = 0; i < size; ++i) {
        typename TThat::value_type elem;
        reader >> elem;
        temp.push_back(elem);
      }
      std::swap(temp, that);
      return reader;
    }

    /* Copy the given object's bytes verbatim into the blob we're building. */
    template <typename TThat>
    inline TBlob::TWriter &WriteAsIs(TBlob::TWriter &writer, const TThat &that) {
      assert(&writer);
      assert(&that);
      return writer.Write(&that, sizeof(that));
    }

    /* Copy the given object's bytes into the blob we're building, but in network byte order. */
    template <typename TThat>
    inline TBlob::TWriter &WriteNbo(TBlob::TWriter &writer, const TThat &that) {
      assert(&writer);
      auto temp = Io::SwapEnds(that);
      return writer.Write(&temp, sizeof(temp));
    }

    /* Copy the given string into the blob we're building, prepending it with its size in NBO. */
    inline TBlob::TWriter &WriteStr(TBlob::TWriter &writer, const char *data, size_t size) {
      assert(&writer);
      assert(data || !size);
      WriteNbo<TBlob::TElemCount>(writer, size);
      return writer.Write(data, size);
    }

    /* Copy the contents of the given container to the end of the blob we're building.
       We start by writing the number of elements, then the elements themselves. */
    template <typename TThat>
    inline TBlob::TWriter &WriteContainer(TBlob::TWriter &writer, const TThat &that) {
      assert(&writer);
      assert(&that);
      WriteNbo<TBlob::TElemCount>(writer, that.size());
      for (auto iter = that.begin(); iter != that.end(); ++iter) {
        *&writer << *iter;
      }
      return *&writer;
    }

    /* A look-up template used when reading and writing tuples. */
    template <size_t Idx, typename... TArgs>
    class ForTuple;

    /* The base case of the tuple-handling look-up template. */
    template <size_t Idx>
    class ForTuple<Idx> final {
      NO_CONSTRUCTION(ForTuple);
      public:

      /* The type of partial tuple which we handle. */
      using TPart = std::_Tuple_impl<Idx>;

      /* Read the partial tuple.
         We're the base case, so we're a do-nothing. */
      static TBlob::TReader &ReadPart(TBlob::TReader &reader, const TPart &) {
        return reader;
      }

      /* Write the partial tuple.
         We're the base case, so we're a do-nothing. */
      static TBlob::TWriter &WritePart(TBlob::TWriter &writer, const TPart &) {
        return writer;
      }

    };  // TBlob::TWriter::ForTuple<Idx>

    /* The recurring case of the tuple-handling look-up template. */
    template <size_t Idx, typename TArg, typename... TMoreArgs>
    class ForTuple<Idx, TArg, TMoreArgs...> final {
      NO_CONSTRUCTION(ForTuple);
      public:

      /* The type of partial-tuple which we handle. */
      using TPart = std::_Tuple_impl<Idx, TArg, TMoreArgs...>;

      /* The type of the handler which occurs after us. */
      using ForMoreArgs = ForTuple<Idx + 1, TMoreArgs...>;

      /* Write a partial tuple.
         That means reading the head, then recurring to read the tail. */
      static TBlob::TReader &ReadPart(TBlob::TReader &reader, TPart &part) {
        assert(&reader);
        assert(&part);
        reader >> TPart::_M_head(part);
        return ForMoreArgs::ReadPart(reader, TPart::_M_tail(part));
      }

      /* Write the partial tuple.
         That means writing the head, then recurring to write the tail. */
      static TBlob::TWriter &WritePart(TBlob::TWriter &writer, const TPart &part) {
        assert(&writer);
        assert(&part);
        writer << TPart::_M_head(part);
        return ForMoreArgs::WritePart(writer, TPart::_M_tail(part));
      }

    };  // ForTuple<Idx, TArg, TMoreArgs...>

    /* Built-in inserters for blob writers. */
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, bool     that) { return WriteAsIs(writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, char     that) { return WriteAsIs(writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, float    that) { return WriteAsIs(writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, double   that) { return WriteAsIs(writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, int8_t   that) { return WriteAsIs(writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, int16_t  that) { return WriteNbo (writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, int32_t  that) { return WriteNbo (writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, int64_t  that) { return WriteNbo (writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, uint8_t  that) { return WriteAsIs(writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, uint16_t that) { return WriteNbo (writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, uint32_t that) { return WriteNbo (writer, that); }
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, uint64_t that) { return WriteNbo (writer, that); }

    /* C-string inserter for blob writers. */
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const char *that) {
      return WriteStr(writer, that, strlen(that));
    }

    /* Std string inserter for blob writers. */
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::string &that) {
      return WriteStr(writer, that.data(), that.size());
    }

    /* UUID inserter for blob writers. */
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const Base::TUuid &that) {
      const auto &raw = that.GetRaw();
      return WriteAsIs(writer, raw);
    }

    /* Std pair inserter for blob writers. */
    template <typename TFirst, typename TSecond>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::pair<TFirst, TSecond> &that) {
      return writer << that.first << that.second;
    }

    /* Std tuple inserter (for the empty tuple) for blob writers. */
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::tuple<> &) {
      return writer;
    }

    /* Std tuple inserter (for a non-empty tuple) for blob writers. */
    template <typename TArg, typename... TMoreArgs>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::tuple<TArg, TMoreArgs...> &that) {
      return ForTuple<0, TArg, TMoreArgs...>::WritePart(writer, that);
    }

    /* Std time point inserter for blob writers. */
    template <typename TClock, typename TDuration>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::chrono::time_point<TClock, TDuration> &that) {
      return writer << that.time_since_epoch();
    }

    /* Std duration inserter for blob writers. */
    template <typename TRep, typename TPeriod>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::chrono::duration<TRep, TPeriod> &that) {
      return writer << that.count();
    }

    /* Std list inserter for blob writers. */
    template <typename TElem, typename TAlloc>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::list<TElem, TAlloc> &that) {
      return WriteContainer(writer, that);
    }

    /* Std vector inserter for blob writers. */
    template <typename TElem, typename TAlloc>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::vector<TElem, TAlloc> &that) {
      return WriteContainer(writer, that);
    }

    /* Std set inserter for blob writers. */
    template <typename TElem, typename TCmp, typename TAlloc>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::set<TElem, TCmp, TAlloc> &that) {
      return WriteContainer(writer, that);
    }

    /* Std multiset inserter for blob writers. */
    template <typename TElem, typename TCmp, typename TAlloc>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::multiset<TElem, TCmp, TAlloc> &that) {
      return WriteContainer(writer, that);
    }

    /* Std map inserter for blob writers. */
    template <typename TKey, typename TVal, typename TCmp, typename TAlloc>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::map<TKey, TVal, TCmp, TAlloc> &that) {
      return WriteContainer(writer, that);
    }

    /* Std multimap inserter for blob writers. */
    template <typename TKey, typename TVal, typename TCmp, typename TAlloc>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::multimap<TKey, TVal, TCmp, TAlloc> &that) {
      return WriteContainer(writer, that);
    }

    /* Std unordered set inserter for blob writers. */
    template <typename TElem, typename THash, typename TEq, typename TAlloc>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::unordered_set<TElem, THash, TEq, TAlloc> &that) {
      return WriteContainer(writer, that);
    }

    /* Std unordered multiset inserter for blob writers. */
    template <typename TElem, typename THash, typename TEq, typename TAlloc>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::unordered_multiset<TElem, THash, TEq, TAlloc> &that) {
      return WriteContainer(writer, that);
    }

    /* Std unordered map inserter for blob writers. */
    template <typename TKey, typename TVal, typename THash, typename TEq, typename TAlloc>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::unordered_map<TKey, TVal, THash, TEq, TAlloc> &that) {
      return WriteContainer(writer, that);
    }

    /* Std unordered multimap inserter for blob writers. */
    template <typename TKey, typename TVal, typename THash, typename TEq, typename TAlloc>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &writer, const std::unordered_multimap<TKey, TVal, THash, TEq, TAlloc> &that) {
      return WriteContainer(writer, that);
    }

    /* This assists when you're using a temporary blob writer. */
    template <typename TThat>
    inline TBlob::TWriter &operator<<(TBlob::TWriter &&writer, const TThat &that) {
      return writer << that;
    }

    /* Built-in extractors for blob readers. */
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, bool     &that) { return ReadAsIs(reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, char     &that) { return ReadAsIs(reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, float    &that) { return ReadAsIs(reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, double   &that) { return ReadAsIs(reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, int8_t   &that) { return ReadAsIs(reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, int16_t  &that) { return ReadNbo (reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, int32_t  &that) { return ReadNbo (reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, int64_t  &that) { return ReadNbo (reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, uint8_t  &that) { return ReadAsIs(reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, uint16_t &that) { return ReadNbo (reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, uint32_t &that) { return ReadNbo (reader, that); }
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, uint64_t &that) { return ReadNbo (reader, that); }

    /* Std string extractor for blob readers. */
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::string &that) {
      return ReadStr(reader, that);
    }

    /* UUID inserter for blob readers. */
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, Base::TUuid &that) {
      uuid_t raw;
      ReadAsIs(reader, raw);
      that = raw;
      return reader;
    }

    /* Std pair extractor for blob readers. */
    template <typename TFirst, typename TSecond>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::pair<TFirst, TSecond> &that) {
      return reader >> that.first >> that.second;
    }

    /* Std tuple extractor (for the empty tuple) for blob readers. */
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::tuple<> &) {
      return reader;
    }

    /* Std tuple extractor (for a non-empty tuple) for blob readers. */
    template <typename TArg, typename... TMoreArgs>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::tuple<TArg, TMoreArgs...> &that) {
      return ForTuple<0, TArg, TMoreArgs...>::ReadPart(reader, that);
    }

    /* Std time point extractor for blob readers. */
    template <typename TClock, typename TDuration>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::chrono::time_point<TClock, TDuration> &that) {
      TDuration duration;
      reader >> duration;
      that = std::chrono::time_point<TClock, TDuration>(duration);
      return reader;
    }

    /* Std duration extractor for blob readers. */
    template <typename TRep, typename TPeriod>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::chrono::duration<TRep, TPeriod> &that) {
      TRep rep;
      reader >> rep;
      that = std::chrono::duration<TRep, TPeriod>(rep);
      return reader;
    }

    /* Std list extractor for blob readers. */
    template <typename TElem, typename TAlloc>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::list<TElem, TAlloc> &that) {
      return ReadContainerWithPushBack(reader, that);
    }

    /* Std vector extractor for blob readers. */
    template <typename TElem, typename TAlloc>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::vector<TElem, TAlloc> &that) {
      return ReadContainerWithPushBack(reader, that);
    }

    /* Std set extractor for blob readers. */
    template <typename TElem, typename TCmp, typename TAlloc>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::set<TElem, TCmp, TAlloc> &that) {
      return ReadContainerWithInsert(reader, that);
    }

    /* Std multiset extractor for blob readers. */
    template <typename TElem, typename TCmp, typename TAlloc>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::multiset<TElem, TCmp, TAlloc> &that) {
      return ReadContainerWithInsert(reader, that);
    }

    /* Std map extractor for blob readers. */
    template <typename TKey, typename TVal, typename TCmp, typename TAlloc>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::map<TKey, TVal, TCmp, TAlloc> &that) {
      return ReadContainerWithInsert(reader, that);
    }

    /* Std multimap extractor for blob readers. */
    template <typename TKey, typename TVal, typename TCmp, typename TAlloc>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::multimap<TKey, TVal, TCmp, TAlloc> &that) {
      return ReadContainerWithInsert(reader, that);
    }

    /* Std unordered set extractor for blob readers. */
    template <typename TElem, typename THash, typename TEq, typename TAlloc>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::unordered_set<TElem, THash, TEq, TAlloc> &that) {
      return ReadContainerWithInsert(reader, that);
    }

    /* Std unordered multiset extractor for blob readers. */
    template <typename TElem, typename THash, typename TEq, typename TAlloc>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::unordered_multiset<TElem, THash, TEq, TAlloc> &that) {
      return ReadContainerWithInsert(reader, that);
    }

    /* Std unordered map extractor for blob readers. */
    template <typename TKey, typename TVal, typename THash, typename TEq, typename TAlloc>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::unordered_map<TKey, TVal, THash, TEq, TAlloc> &that) {
      return ReadContainerWithInsert(reader, that);
    }

    /* Std unordered multimap extractor for blob readers. */
    template <typename TKey, typename TVal, typename THash, typename TEq, typename TAlloc>
    inline TBlob::TReader &operator>>(TBlob::TReader &reader, std::unordered_multimap<TKey, TVal, THash, TEq, TAlloc> &that) {
      return ReadContainerWithInsert(reader, that);
    }

    /* This assists when you're using a temporary blob reader. */
    template <typename TThat>
    inline TBlob::TReader &operator>>(TBlob::TReader &&reader, TThat &that) {
      return reader >> that;
    }

  }  // Rpc

}  // Stig
