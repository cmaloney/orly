/* <stig/rpc/args.h> 

   Utilities for readning and writing name-value argument lists.

   Use WriteArgs() to (yes, you guessed it) write a set of function arguments into a blob.
   For example, call to a function like:

       AddPoint(double x, double y, const std::string &label);

   Might be encoded like this:

       TBlob::TWriter writer(&buffer_pool);
       writer << "AddPoint";
       WriteArgs(writer, "x", 1.5, "y", 2.5, "label", "my point exactly");
       TBlob blob = writer.DraftBlob();

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
#include <cstdint>
#include <string>
#include <type_traits>

#include <base/no_construction.h>
#include <stig/rpc/blob.h>

namespace Stig {

  namespace Rpc {

    /* TODO */
    enum class TArgKind : uint8_t {
      Bool, Int, Real, Str
    };  // TArgKind

    /* TODO */
    template <typename TVal>
    class ForArg;

    /* Specialization for bool. */
    template <>
    class ForArg<bool> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Bool;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, bool that) {
        return writer << that;
      }

    };  // ForArg<bool>

    /* Specialization for char. */
    template <>
    class ForArg<char> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Str;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, char that) {
        char temp[2];
        temp[0] = that;
        temp[1] = '\0';
        return writer << temp;
      }

    };  // ForArg<char>

    /* Specialization for float. */
    template <>
    class ForArg<float> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Real;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, float that) {
        return writer << static_cast<double>(that);
      }

    };  // ForArg<float>

    /* Specialization for double. */
    template <>
    class ForArg<double> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Real;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, double that) {
        return writer << that;
      }

    };  // ForArg<double>

    /* Specialization for int8_t. */
    template <>
    class ForArg<int8_t> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Int;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, int8_t that) {
        return writer << static_cast<int64_t>(that);
      }

    };  // ForArg<int8_t>

    /* Specialization for int16_t. */
    template <>
    class ForArg<int16_t> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Int;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, int16_t that) {
        return writer << static_cast<int64_t>(that);
      }

    };  // ForArg<int16_t>

    /* Specialization for int32_t. */
    template <>
    class ForArg<int32_t> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Int;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, int32_t that) {
        return writer << static_cast<int64_t>(that);
      }

    };  // ForArg<int32_t>

    /* Specialization for int64_t. */
    template <>
    class ForArg<int64_t> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Int;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, int64_t that) {
        return writer << that;
      }

    };  // ForArg<int64_t>

    /* Specialization for uint8_t. */
    template <>
    class ForArg<uint8_t> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Int;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, uint8_t that) {
        return writer << static_cast<uint64_t>(that);
      }

    };  // ForArg<uint8_t>

    /* Specialization for uint16_t. */
    template <>
    class ForArg<uint16_t> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Int;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, uint16_t that) {
        return writer << static_cast<uint64_t>(that);
      }

    };  // ForArg<uint16_t>

    /* Specialization for uint32_t. */
    template <>
    class ForArg<uint32_t> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Int;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, uint32_t that) {
        return writer << static_cast<uint64_t>(that);
      }

    };  // ForArg<uint32_t>

    /* Specialization for uint64_t. */
    template <>
    class ForArg<uint64_t> final {
      NO_CONSTRUCTION(ForArg);
      public:

      /* The kind of var this is. */
      static constexpr TArgKind ArgKind = TArgKind::Int;

      /* Write the var. */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, uint64_t that) {
        return writer << that;
      }

    };  // ForArg<uint64_t>

    /* TODO */
    template <typename TVar>
    inline TBlob::TWriter &WriteArg(TBlob::TWriter &writer, const char *name, const TVar &val) {
      assert(&writer);
      assert(name);
      assert(&val);
      using for_arg = ForArg<typename std::decay<TVar>::type>;
      writer << name;
      TArgKind kind = for_arg::ArgKind;
      writer.WriteAsIs(kind);
      return for_arg::Write(writer, val);
    }

    /* TODO */
    template <typename... TPairs>
    class ArgsWriter;

    /* TODO */
    template <>
    class ArgsWriter<> final {
      NO_CONSTRUCTION(ArgsWriter);
      public:

      /* TODO */
      static constexpr TBlob::TElemCount Size = 0;

      /* TODO */
      static TBlob::TWriter &Write(TBlob::TWriter &writer) {
        return writer;
      }

    };  // ArgsWriter<>

    /* TODO */
    template <size_t N, typename TVal, typename... TMorePairs>
    class ArgsWriter<char[N], TVal, TMorePairs...> final {
      NO_CONSTRUCTION(ArgsWriter);
      public:

      /* TODO */
      using ForMoreArgs = ArgsWriter<TMorePairs...>;

      /* TODO */
      static constexpr TBlob::TElemCount Size = ForMoreArgs::Size + 1;

      /* TODO */
      static TBlob::TWriter &Write(TBlob::TWriter &writer, const char *name, const TVal &val, const TMorePairs &... more_pairs) {
        WriteArg(writer, name, val);
        return ForMoreArgs::Write(writer, more_pairs...);
      }

    };  // ArgsWriter<TPair, TMorePairs...>

    /* TODO */
    template <typename... TPairs>
    inline TBlob::TWriter &WriteArgs(TBlob::TWriter &writer, const TPairs &... pairs) {
      assert(&writer);
      using vars_writer = ArgsWriter<TPairs...>;
      TBlob::TElemCount size = vars_writer::Size;
      writer.WriteNbo(size);
      return vars_writer::Write(writer, pairs...);
    }

    /* TODO */
    class TArgsReader {
      public:

      /* TODO */
      virtual ~TArgsReader();

      /* TODO */
      bool ReadArgs(TBlob::TReader &reader) const;

      /* TODO */
      virtual bool operator()(std::string &name, bool val) const = 0;
      virtual bool operator()(std::string &name, int64_t val) const = 0;
      virtual bool operator()(std::string &name, double val) const = 0;
      virtual bool operator()(std::string &name, std::string &val) const = 0;

      protected:

      /* TODO */
      TArgsReader() {}

    };  // TArgsReader

  }  // Rpc

}  // Stig

