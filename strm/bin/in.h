/* <strm/bin/in.h>

   A consumer of in-flowing data in binary format.

   There are operator>> overloads for:
      * bool, char, float, double;
      * singed and unsigned ints of 8- to 64-bits;
      * std string;
      * std durations and time points;
      * enumerated types;
      * pairs and tuples; and
      * static arrays and std containers of any of these.

   All operations on integers, including the counts stored with containers,
   use var-int formatting.  If you want to decode an integer in fixed-width
   network byte order format, use the NBO() wrapper, like this:

       int x = 101;
       strm >> Nbo(&x);

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

#include <c14/utility.h>
#include <io/endian.h>
#include <strm/in.h>
#include <strm/bin/zig_zag.h>

namespace Strm {

  namespace Bin {

    /* An out-flowing stream of data written in binary format. */
    class TIn final
        : public In::TCons {
      public:

      /* Pass-thru to our base's constructor.
         NOTE: This should really be: using In::TCons::TCons; */
      TIn(In::TProd *prod)
          : TCons(prod) {}

      /* Read a bool as 'T' or 'F'. */
      TIn &operator>>(bool &that);

      /* Read a char as itself, a single byte. */
      TIn &operator>>(char &that) {
        assert(this);
        assert(&that);
        that = Pop();
        return *this;
      }

      /* Read a 32-bit float ISO 754 format; that is, as a native c-float. */
      TIn &operator>>(float &that) {
        assert(this);
        ReadShallow(that);
        return *this;
      }

      /* Read a 64-bit float ISO 754 format; that is, as a native c-double. */
      TIn &operator>>(double &that) {
        assert(this);
        ReadShallow(that);
        return *this;
      }

      /* Read an 8-bit signed integer as-is. */
      TIn &operator>>(int8_t &that) {
        assert(this);
        assert(&that);
        that = Pop();
        return *this;
      }

      /* Read a 16-bit signed integer as a zig-zag var-int. */
      TIn &operator>>(int16_t &that) {
        assert(this);
        assert(&that);
        that = FromZigZag<uint16_t>(ReadVarInt());
        return *this;
      }

      /* Read a 32-bit signed integer as a zig-zag var-int. */
      TIn &operator>>(int32_t &that) {
        assert(this);
        assert(&that);
        that = FromZigZag<uint32_t>(ReadVarInt());
        return *this;
      }

      /* Read a 64-bit signed integer as a zig-zag var-int. */
      TIn &operator>>(int64_t &that) {
        assert(this);
        assert(&that);
        that = FromZigZag<uint64_t>(ReadVarInt());
        return *this;
      }

      /* Read an 8-bit unsigned integer as-is. */
      TIn &operator>>(uint8_t &that) {
        assert(this);
        assert(&that);
        that = Pop();
        return *this;
      }

      /* Read a 16-bit unsigned integer as a var-int. */
      TIn &operator>>(uint16_t &that) {
        assert(this);
        assert(&that);
        that = ReadVarInt();
        return *this;
      }

      /* Read a 32-bit unsigned integer as a var-int. */
      TIn &operator>>(uint32_t &that) {
        assert(this);
        assert(&that);
        that = ReadVarInt();
        return *this;
      }

      /* Read a 64-bit unsigned integer as a var-int. */
      TIn &operator>>(uint64_t &that) {
        assert(this);
        assert(&that);
        that = ReadVarInt();
        return *this;
      }

      /* Read a std string as a length followed by bytes. */
      TIn &operator>>(std::string &that);

      /* Read a std duration as its tick count, extracted as is appropriate for
         its underlying representation type. */
      template <typename TRep, typename TPeriod>
      TIn &operator>>(std::chrono::duration<TRep, TPeriod> &that) {
        assert(this);
        assert(&that);
        TRep temp;
        *this >> temp;
        that = std::chrono::duration<TRep, TPeriod>(temp);
        return *this;
      }

      /* Read a std time point as its time-since-epoch, extracted as is
         appropriate for its underlying duration type. */
      template <typename TClock, typename TDuration>
      TIn &operator>>(
          std::chrono::time_point<TClock, TDuration> &that) {
        assert(this);
        assert(&that);
        TDuration temp;
        *this >> temp;
        that = std::chrono::time_point<TClock, TDuration>(temp);
        return *this;
      }

      /* Read an enumerated value as its underlying type; that is,
         as a char or int. */
      template <
          typename TSomeEnum,
          typename = typename std::enable_if<
              std::is_enum<TSomeEnum>::value>::type>
      TIn &operator>>(TSomeEnum &that) {
        assert(this);
        assert(&that);
        using under_t = typename std::underlying_type<TSomeEnum>::type;
        return *this >> reinterpret_cast<under_t &>(that);
      }

      /* Read a std pair as the two elements themselves, in order, each
         extracted as per the appropriate overload of operator>>.  We do not
         write a count, as it is implicitly exactly 2. */
      template <typename TFirst, typename TSecond>
      TIn &operator>>(std::pair<TFirst, TSecond> &that) {
        assert(this);
        assert(&that);
        return *this >> that.first >> that.second;
      }

      /* Read a std n-tuple as the n elements themselves, in order, each
         extracted as per the appropriate overload of operator>>.  We do not
         read a count, as it is implied by the type of the tuple.  Note
         that extracting the empty tuple reads nothing at all. */
      template <typename... TElems>
      TIn &operator>>(std::tuple<TElems...> &that) {
        assert(this);
        ReadTuple(that, c14::make_index_sequence<sizeof...(TElems)>());
        return *this;
      }

      /* Read an array of elements as just the elements themselves, in order,
         each extracted as per the appropriate overload of operator>>.  We do
         not read a count, as it is implied by the type of the static array.
         Note that extracting an array of size zero reads nothing at all. */
      template <typename TElem, size_t Size>
      TIn &operator>>(TElem (&elems)[Size]) {
        assert(this);
        ReadArrayWithoutCount(elems, elems + Size);
        return *this;
      }

      /* Read a std list of elements as a count followed by the elements
         themselves, in order, each extracted as per the appropriate overload
         of operator>>.  See ReadArrayWithCount() for more information. */
      template <typename TElem, typename TAlloc>
      TIn &operator>>(std::list<TElem, TAlloc> &that) {
        assert(this);
        ReadSeq(that);
        return *this;
      }

      /* Read a std map of elements as a count followed by the elements
         themselves (which are key-value pairs), in order, each extracted as per
         the appropriate overload of operator>>.  See ReadArrayWithCount() for
         more information. */
      template <
          typename TKey, typename TVal, typename TCompare, typename TAlloc>
      TIn &operator>>(std::map<TKey, TVal, TCompare, TAlloc> &that) {
        assert(this);
        ReadMap(that);
        return *this;
      }

      /* Read a std multimap of elements as a count followed by the elements
         themselves (which are key-value pairs), in order, each extracted as per
         the appropriate overload of operator>>.  See ReadArrayWithCount() for
         more information. */
      template <
          typename TKey, typename TVal, typename TCompare, typename TAlloc>
      TIn &operator>>(std::multimap<TKey, TVal, TCompare, TAlloc> &that) {
        assert(this);
        ReadMap(that);
        return *this;
      }

      /* Read a std set of elements as a count followed by the elements
         themselves, in order, each extracted as per the appropriate overload
         of operator>>.  See ReadArrayWithCount() for more information. */
      template <typename TElem, typename TCompare, typename TAlloc>
      TIn &operator>>(std::set<TElem, TCompare, TAlloc> &that) {
        assert(this);
        ReadSet(that);
        return *this;
      }

      /* Read a std multiset of elements as a count followed by the elements
         themselves, in order, each extracted as per the appropriate overload
         of operator>>.  See ReadArrayWithCount() for more information. */
      template <typename TElem, typename TCompare, typename TAlloc>
      TIn &operator>>(std::multiset<TElem, TCompare, TAlloc> &that) {
        assert(this);
        ReadSet(that);
        return *this;
      }

      /* Read a std unordered map of elements as a count followed by the
         elements themselves (which are key-value pairs), in no particular
         order, each extracted as per the appropriate overload of operator>>.
         See ReadArrayWithCount() for more information. */
      template <
          typename TKey, typename TVal,
          typename THash, typename TEq, typename TAlloc>
      TIn &operator>>(
          std::unordered_map<TKey, TVal, THash, TEq, TAlloc> &that) {
        assert(this);
        ReadMap(that);
        return *this;
      }

      /* Read a std unordered multimap of elements as a count followed by the
         elements themselves (which are key-value pairs), in no particular
         order, each extracted as per the appropriate overload of operator>>.
         See ReadArrayWithCount() for more information. */
      template <
          typename TKey, typename TVal,
          typename THash, typename TEq, typename TAlloc>
      TIn &operator>>(
          std::unordered_multimap<TKey, TVal, THash, TEq, TAlloc> &that) {
        assert(this);
        ReadMap(that);
        return *this;
      }

      /* Read a std unordered set of elements as a count followed by the
         elements themselves, in no particular order, each extracted as per the
         appropriate overload of operator>>.  See ReadArrayWithCount() for
         more information. */
      template <typename TElem, typename THash, typename TEq, typename TAlloc>
      TIn &operator>>(std::unordered_set<TElem, THash, TEq, TAlloc> &that) {
        assert(this);
        ReadSet(that);
        return *this;
      }

      /* Read a std unordered multiset of elements as a count followed by the
         elements themselves, in no particular order, each extracted as per the
         appropriate overload of operator>>.  See ReadArrayWithCount() for
         more information. */
      template <typename TElem, typename THash, typename TEq, typename TAlloc>
      TIn &operator>>(
          std::unordered_multiset<TElem, THash, TEq, TAlloc> &that) {
        assert(this);
        ReadSet(that);
        return *this;
      }

      /* Read a std vector of elements as a count followed by the elements
         themselves, in order, each extracted as per the appropriate overload
         of operator>>.  See ReadArrayWithCount() for more information. */
      template <typename TElem, typename TAlloc>
      TIn &operator>>(std::vector<TElem, TAlloc> &that) {
        assert(this);
        ReadSeq(that);
        return *this;
      }

      /* Read an array of elements as just the elements themselves, with no
         count.  Each element is extracted as per the appropriate overload of
         operator>>. */
      template <typename TIter>
      void ReadArrayWithoutCount(TIter begin, TIter end) {
        assert(this);
        for (; begin != end; ++begin) {
          *this >> *begin;
        }
      }

      /* Read the given structure verbatim, treating it as shallow data. */
      template <typename TThat>
      void ReadShallow(TThat &that) {
        assert(this);
        Read(&that, sizeof(that));
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
        TNbo(TVal &val)
            : Val(val) {}
        TVal &Val;
      };  // TNbo<TVal>

      /* Read an integer in NBO format. */
      template <
          typename TVal,
          typename = typename std::enable_if<
              std::is_integral<TVal>::value
          >::type
      >
      TIn &operator>>(TNbo<TVal> &&that) {
        assert(this);
        assert(&that);
        TVal temp;
        ReadShallow(temp);
        that.Val = Io::SwapEnds(temp);
        return *this;
      }

      using TCons::Read;
      using TCons::Peek;

      private:

      /* A lookup template for reserving space in containers into which we
         are streaming.  Most containers don't have a reserve() function, so
         the default template does nothing.  Explicit specializations
         follow. */
      template <typename TCont>
      struct For final {
        static void Reserve(TCont &/*that*/, size_t /*count*/) {}
      };  // For<TCont>

      /* Used by the operator>> overloads for all varieties of std maps. */
      template <typename TCont>
      void ReadMap(TCont &that) {
        assert(this);
        assert(&that);
        size_t count;
        *this >> count;
        TCont temp;
        std::pair<typename TCont::key_type, typename TCont::mapped_type> elem;
        for (; count; --count) {
          *this >> elem;
          temp.insert(std::move(elem));
        }
        that = std::move(temp);
      }

      /* Used by the operator>> overloads for std lists and std vectors. */
      template <typename TCont>
      void ReadSeq(TCont &that) {
        assert(this);
        assert(&that);
        size_t count;
        *this >> count;
        TCont temp;
        For<TCont>::Reserve(temp, count);
        typename TCont::value_type elem;
        for (; count; --count) {
          *this >> elem;
          temp.push_back(std::move(elem));
        }
        that = std::move(temp);
      }

      /* Used by the operator>> overloads for all varieties of std sets. */
      template <typename TCont>
      void ReadSet(TCont &that) {
        assert(this);
        assert(&that);
        size_t count;
        *this >> count;
        TCont temp;
        typename TCont::value_type elem;
        for (; count; --count) {
          *this >> elem;
          temp.insert(std::move(elem));
        }
        that = std::move(temp);
      }

      /* Used by the operator>> overload for tuples. */
      template <typename TSomeTuple, size_t... Idx>
      void ReadTuple(TSomeTuple &that, c14::index_sequence<Idx...>) {
        int x[] { (*this >> std::get<Idx>(that), 0)... };
        (void)x;
      }

      /* Used by the operator>> overloads for multi-byte integers. */
      uint64_t ReadVarInt();

    };  // TIn

    /* Explicit specialization for reserving space in std vectors. */
    template <typename TElem, typename TAlloc>
    struct TIn::For<std::vector<TElem, TAlloc>> final {
      static void Reserve(std::vector<TElem, TAlloc> &that, size_t count) {
        that.reserve(count);
      }
    };  // For<std::vector<TElem, TAlloc>>

    /* A helper for extracting out of a temporary stream. */
    template <typename TThat>
    inline TIn &&operator>>(TIn &&strm, TThat &that) {
      return std::move(strm >> that);
    }

    /* A helper for constructing NBO wrappers. */
    template <
        typename TVal,
        typename = typename std::enable_if<
            std::is_integral<TVal>::value
        >::type
    >
    TIn::TNbo<TVal> Nbo(TVal *val) {
      return TIn::TNbo<TVal>(*val);
    }

  }  // Bin

}  // Strm
