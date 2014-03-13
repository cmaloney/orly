/* <strm/bin/out.h>

   A producer of out-flowing data in binary format.

   There are operator<< overloads for:
      * bool, char, float, double;
      * singed and unsigned ints of 8- to 64-bits;
      * c-string, std string;
      * std durations and time points;
      * enumerated types;
      * pairs and tuples; and
      * static arrays and std containers of any of these.

   All operations on integers, including the counts stored with containers,
   use var-int formatting.  If you want to encode an integer in fixed-width
   network byte order format, use the NBO() wrapper, like this:

       int x = 101;
       strm << Nbo(x);

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
#include <chrono>
#include <cstdint>
#include <cstring>
#include <list>
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <io/endian.h>
#include <strm/idx_iter.h>
#include <strm/out.h>
#include <strm/bin/var_int.h>
#include <strm/bin/zig_zag.h>

namespace Strm {

  namespace Bin {

    /* An out-flowing stream of data written in binary format. */
    class TOut final
        : public Out::TProd {
      public:

      /* Pass-thru to our base's constructor.
         NOTE: This should really be: using Out::TProd::TProd; */
      TOut(Out::TCons *cons)
          : TProd(cons) {}

      /* Write a bool as 'T' or 'F'. */
      TOut &operator<<(bool that) {
        assert(this);
        WriteShallow(that ? 'T' : 'F');
        return *this;
      }

      /* Write a char as itself, a single byte. */
      TOut &operator<<(char that) {
        assert(this);
        WriteShallow(that);
        return *this;
      }

      /* Write a 32-bit float ISO 754 format; that is, as a native c-float. */
      TOut &operator<<(float that) {
        assert(this);
        WriteShallow(that);
        return *this;
      }

      /* Write a 64-bit float ISO 754 format; that is, as a native c-double. */
      TOut &operator<<(double that) {
        assert(this);
        WriteShallow(that);
        return *this;
      }

      /* Write an 8-bit signed integer as-is. */
      TOut &operator<<(int8_t that) {
        assert(this);
        WriteShallow(that);
        return *this;
      }

      /* Write a 16-bit signed integer as a zig-zag var-int. */
      TOut &operator<<(int16_t that) {
        assert(this);
        WriteVarInt(ToZigZag(that));
        return *this;
      }

      /* Write a 32-bit signed integer as a zig-zag var-int. */
      TOut &operator<<(int32_t that) {
        assert(this);
        WriteVarInt(ToZigZag(that));
        return *this;
      }

      /* Write a 64-bit signed integer as a zig-zag var-int. */
      TOut &operator<<(int64_t that) {
        assert(this);
        WriteVarInt(ToZigZag(that));
        return *this;
      }

      /* Write an 8-bit unsigned integer as-is. */
      TOut &operator<<(uint8_t that) {
        assert(this);
        WriteShallow(that);
        return *this;
      }

      /* Write a 16-bit unsigned integer as a var-int. */
      TOut &operator<<(uint16_t that) {
        assert(this);
        WriteVarInt(that);
        return *this;
      }

      /* Write a 32-bit unsigned integer as a var-int. */
      TOut &operator<<(uint32_t that) {
        assert(this);
        WriteVarInt(that);
        return *this;
      }

      /* Write a 64-bit unsigned integer as a var-int. */
      TOut &operator<<(uint64_t that) {
        assert(this);
        WriteVarInt(that);
        return *this;
      }

      /* Write the given c-string as a length followed by bytes.  The length
         will not include the null-terminator, nor will the terminator be
         written.  See WriteString() for more information. */
      TOut &operator<<(const char *that) {
        assert(this);
        WriteString(that, strlen(that));
        return *this;
      }

      /* Write a std string as a length followed by bytes.  See WriteString()
         for more information. */
      TOut &operator<<(const std::string &that) {
        assert(this);
        WriteString(that.data(), that.size());
        return *this;
      }

      /* Write a std duration as its tick count, inserted as is appropriate for
         its underlying representation type. */
      template <typename TRep, typename TPeriod>
      TOut &operator<<(const std::chrono::duration<TRep, TPeriod> &that) {
        assert(this);
        assert(&that);
        return *this << that.count();
      }

      /* Write a std time point as its time-since-epoch, inserted as is
         appropriate for its underlying duration type. */
      template <typename TClock, typename TDuration>
      TOut &operator<<(
          const std::chrono::time_point<TClock, TDuration> &that) {
        assert(this);
        assert(&that);
        return *this << that.time_since_epoch();
      }

      /* Write an enumerated value as its underlying type; that is,
         as a char or int. */
      template <
          typename TSomeEnum,
          typename = typename std::enable_if<
              std::is_enum<TSomeEnum>::value>::type>
      TOut &operator<<(TSomeEnum that) {
        assert(this);
        using under_t = typename std::underlying_type<TSomeEnum>::type;
        return *this << static_cast<under_t>(that);
      }

      /* Write a std pair as the two elements themselves, in order, each
         inserted as per the appropriate overload of operator<<.  We do not
         write a count, as it is implicitly exactly 2. */
      template <typename TFirst, typename TSecond>
      TOut &operator<<(const std::pair<TFirst, TSecond> &that) {
        assert(this);
        return *this << that.first << that.second;
      }

      /* Write a std n-tuple as the n elements themselves, in order, each
         inserted as per the appropriate overload of operator<<.  We do not
         write a count, as it is implied by the type of the tuple.  Note
         that inserting the empty tuple writes nothing at all. */
      template <typename... TElems>
      TOut &operator<<(const std::tuple<TElems...> &that) {
        WriteTuple(that, MakeIdxIter<sizeof...(TElems)>());
        return *this;
      }

      /* Write an array of elements as just the elements themselves, in order,
         each inserted as per the appropriate overload of operator<<.  We do
         not write a count, as it is implied by the type of the static array.
         Note that inserting an array of size zero writes nothing at all. */
      template <typename TElem, size_t Size>
      TOut &operator<<(const TElem (&elems)[Size]) {
        assert(this);
        WriteArrayWithoutCount(elems, elems + Size);
        return *this;
      }

      /* Write a std list of elements as a count followed by the elements
         themselves, in order, each inserted as per the appropriate overload
         of operator<<.  See WriteArrayWithCount() for more information. */
      template <typename TElem, typename TAlloc>
      TOut &operator<<(const std::list<TElem, TAlloc> &that) {
        assert(this);
        WriteArrayWithCount(that.begin(), that.size());
        return *this;
      }

      /* Write a std map of elements as a count followed by the elements
         themselves (which are key-value pairs), in order, each inserted as per
         the appropriate overload of operator<<.  See WriteArrayWithCount() for
         more information. */
      template <
          typename TKey, typename TVal, typename TCompare, typename TAlloc>
      TOut &operator<<(
          const std::map<TKey, TVal, TCompare, TAlloc> &that) {
        assert(this);
        WriteArrayWithCount(that.begin(), that.size());
        return *this;
      }

      /* Write a std multimap of elements as a count followed by the elements
         themselves (which are key-value pairs), in order, each inserted as per
         the appropriate overload of operator<<.  See WriteArrayWithCount() for
         more information. */
      template <
          typename TKey, typename TVal, typename TCompare, typename TAlloc>
      TOut &operator<<(
          const std::multimap<TKey, TVal, TCompare, TAlloc> &that) {
        assert(this);
        WriteArrayWithCount(that.begin(), that.size());
        return *this;
      }

      /* Write a std set of elements as a count followed by the elements
         themselves, in order, each inserted as per the appropriate overload
         of operator<<.  See WriteArrayWithCount() for more information. */
      template <typename TElem, typename TCompare, typename TAlloc>
      TOut &operator<<(const std::set<TElem, TCompare, TAlloc> &that) {
        assert(this);
        WriteArrayWithCount(that.begin(), that.size());
        return *this;
      }

      /* Write a std multiset of elements as a count followed by the elements
         themselves, in order, each inserted as per the appropriate overload
         of operator<<.  See WriteArrayWithCount() for more information. */
      template <typename TElem, typename TCompare, typename TAlloc>
      TOut &operator<<(
          const std::multiset<TElem, TCompare, TAlloc> &that) {
        assert(this);
        WriteArrayWithCount(that.begin(), that.size());
        return *this;
      }

      /* Write a std unordered map of elements as a count followed by the
         elements themselves (which are key-value pairs), in no particular
         order, each inserted as per the appropriate overload of operator<<.
         See WriteArrayWithCount() for more information. */
      template <
          typename TKey, typename TVal,
          typename THash, typename TEq, typename TAlloc>
      TOut &operator<<(
          const std::unordered_map<TKey, TVal, THash, TEq, TAlloc> &that) {
        assert(this);
        WriteArrayWithCount(that.begin(), that.size());
        return *this;
      }

      /* Write a std unordered multimap of elements as a count followed by the
         elements themselves (which are key-value pairs), in no particular
         order, each inserted as per the appropriate overload of operator<<.
         See WriteArrayWithCount() for more information. */
      template <
          typename TKey, typename TVal,
          typename THash, typename TEq, typename TAlloc>
      TOut &operator<<(
          const std::unordered_multimap<
              TKey, TVal, THash, TEq, TAlloc> &that) {
        assert(this);
        WriteArrayWithCount(that.begin(), that.size());
        return *this;
      }

      /* Write a std unordered set of elements as a count followed by the
         elements themselves, in no particular order, each inserted as per the
         appropriate overload of operator<<.  See WriteArrayWithCount() for
         more information. */
      template <typename TElem, typename THash, typename TEq, typename TAlloc>
      TOut &operator<<(
          const std::unordered_set<TElem, THash, TEq, TAlloc> &that) {
        assert(this);
        WriteArrayWithCount(that.begin(), that.size());
        return *this;
      }

      /* Write a std unordered multiset of elements as a count followed by the
         elements themselves, in no particular order, each inserted as per the
         appropriate overload of operator<<.  See WriteArrayWithCount() for
         more information. */
      template <typename TElem, typename THash, typename TEq, typename TAlloc>
      TOut &operator<<(
          const std::unordered_multiset<TElem, THash, TEq, TAlloc> &that) {
        assert(this);
        WriteArrayWithCount(that.begin(), that.size());
        return *this;
      }

      /* Write a std vector of elements as a count followed by the elements
         themselves, in order, each inserted as per the appropriate overload
         of operator<<.  See WriteArrayWithCount() for more information. */
      template <typename TElem, typename TAlloc>
      TOut &operator<<(const std::vector<TElem, TAlloc> &that) {
        assert(this);
        WriteArrayWithCount(that.begin(), that.size());
        return *this;
      }

      using TOut::TProd::Write;

      /* Write the number of elements in the array as an unsigned var-int,
         followed by the elements themselves, each inserted as per the
         appropriate overload of operator<<. */
      template <typename TIter>
      void WriteArrayWithCount(TIter iter, size_t count) {
        assert(this);
        *this << count;
        for (; count; --count, ++iter) {
          *this << *iter;
        }
      }

      /* Write an array of elements as just the elements themselves, with no
         count.  Each element is inserted as per the appropriate overload of
         operator<<. */
      template <typename TIter>
      void WriteArrayWithoutCount(TIter begin, TIter end) {
        assert(this);
        for (; begin != end; ++begin) {
          *this << *begin;
        }
      }

      /* Write the given structure verbatim, treating it as shallow data. */
      template <typename TThat>
      void WriteShallow(const TThat &that) {
        assert(this);
        Write(&that, sizeof(that));
      }

      /* Write the given string an unsigned var-int, followed by bytes. */
      template <size_t Count>
      void WriteString(const char (&that)[Count]) {
        assert(this);
        WriteString(that, Count);
      }

      /* Write the given string an unsigned var-int, followed by bytes. */
      void WriteString(const char *start, const char *limit) {
        assert(this);
        assert(start <= limit);
        WriteString(start, limit - start);
      }

      /* Write the given string an unsigned var-int, followed by bytes. */
      void WriteString(const char *data, size_t size) {
        assert(this);
        *this << size;
        Write(data, size);
      }

      /* A wrapper around an integer value to distingish NBO operations from
         var-int operations. */
      template <
          typename TVal,
          typename = typename std::enable_if<
              std::is_integral<TVal>::value
          >::type
      >
      struct TNbo final {
        TNbo(TVal val)
            : Val(val) {}
        TVal Val;
      };  // TNbo<TVal>

      /* Write an integer in NBO format. */
      template <
          typename TVal,
          typename = typename std::enable_if<
              std::is_integral<TVal>::value
          >::type
      >
      TOut &operator<<(const TNbo<TVal> &that) {
        assert(this);
        assert(&that);
        TVal temp = Io::SwapEnds(that.Val);
        WriteShallow(temp);
        return *this;
      }

      private:

      /* Used by the operator<< overload for tuples. */
      template <typename TSomeTuple, size_t... Idx>
      void WriteTuple(const TSomeTuple &that, IdxIter<Idx...>) {
        using ignored_t = int[];
        ignored_t { (*this << std::get<Idx>(that), 0)... };
      }

      /* Used by the operator<< overloads for multi-byte integers. */
      void WriteVarInt(uint64_t that) {
        assert(this);
        TVarIntEncoder encoder(that);
        Write(encoder.GetStart(), encoder.GetSize());
      }

    };  // TOut

    /* A helper for inserting into a temporary stream. */
    template <typename TThat>
    inline TOut &&operator<<(TOut &&strm, const TThat &that) {
      return std::move(strm << that);
    }

    /* A helper for constructing NBO wrappers. */
    template <
        typename TVal,
        typename = typename std::enable_if<
            std::is_integral<TVal>::value
        >::type
    >
    TOut::TNbo<TVal> Nbo(TVal val) {
      return TOut::TNbo<TVal>(val);
    }

  }  // Bin

}  // Strm
