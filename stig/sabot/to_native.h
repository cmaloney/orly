/* <stig/sabot/to_native.h>

   Pull A native value back from a sabot

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
#include <ostream>

#include <stig/desc.h>
#include <stig/native/defs.h>
#include <stig/sabot/state.h>
#include <stig/sabot/state_dumper.h>
#include <stig/uuid.h>

namespace Stig {

  namespace Sabot {

    /* TODO */
    template <typename TVal>
    class TToNativeVisitor;

    /* TODO */
    template <typename TVal>
    void ToNative(const Sabot::State::TAny &state, TVal &val) {
      state.Accept(TToNativeVisitor<TVal>(val));
    }

    /* TODO */
    template <typename TVal>
    TVal AsNative(const Sabot::State::TAny &state) {
      TVal val;
      state.Accept(TToNativeVisitor<TVal>(val));
      return val;
    }

    template <>
    class TToNativeVisitor<int8_t> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(int8_t &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &state) const override           { Out = state.Get(); }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      int8_t &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<int16_t> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(int16_t &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &state) const override          { Out = state.Get(); }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      int16_t &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<int32_t> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(int32_t &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &state) const override          { Out = state.Get(); }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      int32_t &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<int64_t> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(int64_t &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &state) const override          { Out = state.Get(); }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      int64_t &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<uint8_t> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(uint8_t &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &state) const override          { Out = state.Get(); }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      uint8_t &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<uint16_t> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(uint16_t &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &state) const override         { Out = state.Get(); }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      uint16_t &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<uint32_t> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(uint32_t &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &state) const override         { Out = state.Get(); }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      uint32_t &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<uint64_t> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(uint64_t &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &state) const override         { Out = state.Get(); }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      uint64_t &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<bool> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(bool &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &state) const override           { Out = state.Get(); }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      bool &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<char> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(char &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &state) const override           { Out = state.Get(); }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      char &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<float> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(float &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &state) const override          { Out = state.Get(); }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      float &Out;
    };  // TToNativeVisitor


    template <>
    class TToNativeVisitor<double> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(double &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &state) const override         { Out = state.Get(); }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      double &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<Sabot::TStdDuration> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(Sabot::TStdDuration &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &state) const override       { Out = state.Get(); }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      Sabot::TStdDuration &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<Sabot::TStdTimePoint> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(Sabot::TStdTimePoint &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &state) const override      { Out = state.Get(); }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      Sabot::TStdTimePoint &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<Base::TUuid> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(Base::TUuid &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &state) const override           { Out = state.Get(); }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      Base::TUuid &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<Stig::TUUID> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(Stig::TUUID &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &state) const override           { Out = Stig::TUUID(state.Get().GetRaw()); }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      Stig::TUUID &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<std::string> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(std::string &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      virtual void operator()(const State::TStr &state) const override {
        void *pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TStr::TPin::TWrapper pin(state.Pin(pin_alloc));
        Out.assign(pin->GetStart(), pin->GetLimit());
      }
      private:
      std::string &Out;
    };  // TToNativeVisitor

    template <>
    class TToNativeVisitor<Native::TBlob> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(Native::TBlob &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &state) const override       {
        void *pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TBlob::TPin::TWrapper pin(state.Pin(pin_alloc));
        Out.assign(pin->GetStart(), pin->GetLimit());
      }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      private:
      Native::TBlob &Out;
    };  // TToNativeVisitor


    template <typename TVal>
    class TToNativeVisitor<Base::TOpt<TVal>> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(Base::TOpt<TVal> &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &state) const override            {
        void *pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TOpt::TPin::TWrapper pin(state.Pin(pin_alloc));
        if (pin->GetElemCount()) {
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          TVal val;
          ToNative(*Sabot::State::TAny::TWrapper(pin->NewElem(0, state_alloc)), val);
          Out = Base::TOpt<TVal>(val);
        } else {
          Out.Reset();
        }
      }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      Base::TOpt<TVal> &Out;
    };  // TToNativeVisitor

    template <typename TVal>
    class TToNativeVisitor<Stig::TDesc<TVal>> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(Stig::TDesc<TVal> &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &state) const override           {
        void *pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TDesc::TPin::TWrapper pin(state.Pin(pin_alloc));
        assert(pin->GetElemCount());
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        ToNative(*Sabot::State::TAny::TWrapper(pin->NewElem(0, state_alloc)), *Out);
      }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      Stig::TDesc<TVal> &Out;
    };  // TToNativeVisitor

    template <typename TVal>
    class TToNativeVisitor<std::vector<TVal>> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(std::vector<TVal> &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &state) const override         {
        void *pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TVector::TPin::TWrapper pin(state.Pin(pin_alloc));
        size_t elem_count = pin->GetElemCount();
        //TVal default_val;
        Out = std::vector<TVal>(elem_count/*, default_val*/);
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
          ToNative(*Sabot::State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc)), Out[elem_idx]);
        }
      }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      std::vector<TVal> &Out;
    };  // TToNativeVisitor

    /* specialization for vector<bool> because it's SPECIAL AND NEEDS ATTENTION *SIGH* */
    template <>
    class TToNativeVisitor<std::vector<bool>> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(std::vector<bool> &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &state) const override         {
        void *pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TVector::TPin::TWrapper pin(state.Pin(pin_alloc));
        size_t elem_count = pin->GetElemCount();
        bool dummy;
        Out = std::vector<bool>(elem_count, dummy);
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
          ToNative(*Sabot::State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc)), dummy);
          Out[elem_idx] = dummy;
        }
      }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      std::vector<bool> &Out;
    };  // TToNativeVisitor

    template <typename TVal, typename TCompare>
    class TToNativeVisitor<std::set<TVal, TCompare>> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(std::set<TVal, TCompare> &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &state) const override            {
        void *pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TVector::TPin::TWrapper pin(state.Pin(pin_alloc));
        size_t elem_count = pin->GetElemCount();
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
          TVal dummy;
          ToNative(*Sabot::State::TAny::TWrapper(pin->NewElem(elem_idx, state_alloc)), dummy);
          Out.insert(Out.end(), dummy);
        }
      }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      std::set<TVal, TCompare> &Out;
    };  // TToNativeVisitor

    template <typename TLhs, typename TRhs, typename TCompare>
    class TToNativeVisitor<std::map<TLhs, TRhs, TCompare>> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(std::map<TLhs, TRhs, TCompare> &out) : Out(out) {
        out.clear();
      }
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &state) const override {
        void *pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TMap::TPin::TWrapper pin(state.Pin(pin_alloc));
        size_t elem_count = pin->GetElemCount();
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        for (size_t elem_idx = 0; elem_idx < elem_count; ++elem_idx) {
          std::pair<TLhs, TRhs> item;
          ToNative(*Sabot::State::TAny::TWrapper(pin->NewLhs(elem_idx, state_alloc)), item.first);
          ToNative(*Sabot::State::TAny::TWrapper(pin->NewRhs(elem_idx, state_alloc)), item.second);
          Out.insert(Out.end(), item);
        }
      }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw; }
      private:
      std::map<TLhs, TRhs, TCompare> &Out;
    };  // TToNativeVisitor<std::map<TLhs, TRhs, TCompare>>

    /* TODO */
    template <typename TMyTuple, size_t pos, typename... TElems>
    class TTupleExtractor;

    /* TODO */
    template <typename TMyTuple, size_t pos, typename TElem, typename... TElems>
    class TTupleExtractor<TMyTuple, pos, TElem, TElems...> {
      NO_CONSTRUCTION(TTupleExtractor);
      public:
      /* TODO */
      void static Extract(const State::TTuple::TPin *pin, TMyTuple &out, void *state_alloc) {
        ToNative(*Sabot::State::TAny::TWrapper(pin->NewElem(pos, state_alloc)), std::get<pos>(out));
        TTupleExtractor<TMyTuple, pos + 1, TElems...>::Extract(pin, out, state_alloc);
      }
    };  // TTupleExtractor<TMyTuple, pos, TElem, TElems...>

    /* TODO */
    template <typename TMyTuple, size_t pos>
    class TTupleExtractor<TMyTuple, pos> {
      NO_CONSTRUCTION(TTupleExtractor);
      public:
      /* TODO */
      void static Extract(const State::TTuple::TPin */*pin*/, TMyTuple &/*out*/, void */*state_alloc*/) {}
    };  // TTupleExtractor<TMyTuple, pos>

    /* TODO */
    template <typename... TElems>
    class TToNativeVisitor<std::tuple<TElems...>> final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:
      /* TODO */
      TToNativeVisitor(std::tuple<TElems...> &out) : Out(out) {}
      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw; }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw; }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw; }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw; }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw; }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw; }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw; }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw; }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw; }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw; }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw; }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw; }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw; }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw; }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw; }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw; }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw; }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw; }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw; }
      virtual void operator()(const State::TRecord &/*state*/) const override     { throw; }
      virtual void operator()(const State::TTuple &state) const override          {
        void *pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TTuple::TPin::TWrapper pin(state.Pin(pin_alloc));
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        TTupleExtractor<std::tuple<TElems...>, 0, TElems...>::Extract(pin.get(), Out, state_alloc);
      }
      private:
      std::tuple<TElems...> &Out;
    };  // TToNativeVisitor

  }  // Sabot

}  // Stig