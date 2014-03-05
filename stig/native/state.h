/* <stig/native/state.h>

   Describes the state of a sabot.

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
#include <cstring>

#include <base/no_construction.h>
#include <stig/native/type.h>
#include <stig/rt/mutable.h>
#include <stig/sabot/state.h>
#include <stig/uuid.h>

namespace Stig {

  namespace Native {

    /* See <stig/native/record.h> for the definition of this template. */
    template <typename TRecord>
    class Record;

    /* Using a class as a finite namespace with private members. */
    class State {
      NO_CONSTRUCTION(State);
      public:

      /* Convenience. */
      using TAny = Sabot::State::TAny;

      /* Construct a new state sabot around the given value. */
      template <typename TVal>
      static TAny *New(const TVal &val, void *state_alloc) {
        return Factory<TVal>::New(val, state_alloc);
      }

      template<typename> friend class Stig::Native::Record;

      private:

      /* Construct a new state sabot around the given value.
         The generic version defined here assumes the value is a record type.
         See below for explicit specializations for non-record types. */
      template <typename TVal>
      class Factory final {
        NO_CONSTRUCTION(Factory);
        public:

        /* Construct a new state sabot around the value. */
        static TAny *New(const TVal &val, void *state_alloc) {
          return Record<TVal>::New(val, state_alloc);
        }

      };  // State::Factory<TVal>

      /* A helper for TTuple. */
      template <size_t Idx, typename... TElems>
      class ForTuple;

      /* A helper for TRTTuple. */
      template <size_t Idx, typename... TElems>
      class ForRTTuple;

      /*
       *    Base classes.
       */

      /* The base for all empty states. */
      template <typename TEmptySabotState>
      class TEmpty
          : public TEmptySabotState {
        protected:

        /* Do-little. */
        TEmpty() {}

      };  // State::TEmpty<TEmptySabotState>

      /* The base for all scalar states. */
      template <typename TScalarSabotState>
      class TScalar
          : public TScalarSabotState {
        public:

        /* See Sabot::State::TScalar. */
        virtual const typename TScalarSabotState::TVal &Get() const override final {
          assert(this);
          return Val;
        }

        protected:

        /* Cache the reference to the value. */
        TScalar(const typename TScalarSabotState::TVal &val)
            : Val(val) {
          assert(&val);
        }

        private:

        /* Our value. */
        const typename TScalarSabotState::TVal &Val;

      };  // State::TScalar<TScalarSabotState>

      /* The base for all array-of-scalars states. */
      template <typename TArrayOfScalarsSabotState>
      class TArrayOfScalars
          : public TArrayOfScalarsSabotState {
        public:

        /* See Sabot::Type::TArrayOfScalars. */
        virtual size_t GetSize() const override final {
          assert(this);
          return Limit - Start;
        }

        /* See Sabot::Type::TArrayOfScalars. */
        virtual typename TArrayOfScalarsSabotState::TPin *Pin(void *alloc) const override final {
          assert(this);
          return new (alloc) TPin(this);
        }

        protected:

        /* Caches the pointers. */
        TArrayOfScalars(const typename TArrayOfScalarsSabotState::TVal *data, size_t size)
            : Start(data), Limit(data + size) {
          assert(data || !size);
        }

        private:

        /* Keeps the array in memory. */
        class TPin final
            : public TArrayOfScalarsSabotState::TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* Do little. */
          TPin(const TArrayOfScalars *array_of_scalars)
              : TArrayOfScalarsSabotState::TPin(array_of_scalars->Start, array_of_scalars->Limit) {}

        };  // TPin

        /* Pointers to our array. */
        const typename TArrayOfScalarsSabotState::TVal *Start, *Limit;

        /* size of TPin */
        friend class Sabot::TSizeChecker;

      };  // TArrayOfScalars<TArrayOfScalarsSabotState>

      /* The base for all array-of-single-states states. */
      template <typename TArrayOfSingleStatesSabotState>
      class TArrayOfSingleStates
          : public TArrayOfSingleStatesSabotState {
        public:

        /* See Sabot::Type::TArrayOfSingleStates. */
        virtual size_t GetElemCount() const override final {
          assert(this);
          return ElemCount;
        }

        /* See Sabot::Type::TArrayOfSingleStates. */
        virtual typename TArrayOfSingleStatesSabotState::TPin *Pin(void *alloc) const override final {
          assert(this);
          return new (alloc) TPin(this);
        }

        protected:

        /* Caches the element count. */
        TArrayOfSingleStates(size_t elem_count)
            : ElemCount(elem_count) {
        }

        /* Override to construct a new state sabot for the given element. */
        virtual TAny *NewElem(size_t elem_idx, void *state_alloc) const = 0;

        private:

        /* Keeps the array in memory. */
        class TPin final
            : public TArrayOfSingleStatesSabotState::TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* Do little. */
          TPin(const TArrayOfSingleStates *array_of_single_states)
              : TArrayOfSingleStatesSabotState::TPin(array_of_single_states),
                ArrayOfSingleStates(array_of_single_states) {}

          private:

          /* See Sabot::Type::TArrayOfSingleStates::TPin. */
          virtual TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override {
            assert(this);
            return ArrayOfSingleStates->NewElem(elem_idx, state_alloc);
          }

          /* The array to which we're attached. */
          const TArrayOfSingleStates *ArrayOfSingleStates;

        };  // TPin

        /* See accessor. */
        size_t ElemCount;

        /* size of TPin */
        friend class Sabot::TSizeChecker;

      };  // TArrayOfSingleStates<TArrayOfSingleStatesSabotState>

      /* The base for all array-of-pairs-of-states states. */
      template <typename TArrayOfPairsOfStatesSabotState>
      class TArrayOfPairsOfStates
          : public TArrayOfPairsOfStatesSabotState {
        public:

        /* See Sabot::Type::TArrayOfPairsOfStates. */
        virtual size_t GetElemCount() const override final {
          assert(this);
          return ElemCount;
        }

        /* See Sabot::Type::TArrayOfPairsOfStates. */
        virtual typename TArrayOfPairsOfStatesSabotState::TPin *Pin(void *alloc) const override final {
          assert(this);
          return new (alloc) TPin(this);
        }

        protected:

        /* Caches the element count. */
        TArrayOfPairsOfStates(size_t elem_count)
            : ElemCount(elem_count) {
        }

        /* Override to construct a new state sabot for the given lhs element. */
        virtual TAny *NewLhs(size_t elem_idx, void *state_alloc) const = 0;

        /* Override to construct a new state sabot for the given rhs element. */
        virtual TAny *NewRhs(size_t elem_idx, void *state_alloc) const = 0;

        private:

        /* Keeps the array in memory. */
        class TPin final
            : public TArrayOfPairsOfStatesSabotState::TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* Do little. */
          TPin(const TArrayOfPairsOfStates *array_of_pairs_of_states)
              : TArrayOfPairsOfStatesSabotState::TPin(array_of_pairs_of_states),
                ArrayOfPairsOfStates(array_of_pairs_of_states) {}

          private:

          /* See Sabot::Type::TArrayOfPairsOfStates::TPin. */
          virtual TAny *NewLhsInRange(size_t elem_idx, void *state_alloc) const override {
            assert(this);
            return ArrayOfPairsOfStates->NewLhs(elem_idx, state_alloc);
          }

          /* See Sabot::Type::TArrayOfPairsOfStates::TPin. */
          virtual TAny *NewRhsInRange(size_t elem_idx, void *state_alloc) const override {
            assert(this);
            return ArrayOfPairsOfStates->NewRhs(elem_idx, state_alloc);
          }

          /* The array to which we're attached. */
          const TArrayOfPairsOfStates *ArrayOfPairsOfStates;

        };  // TPin

        /* See accessor. */
        size_t ElemCount;

        /* size of TPin */
        friend class Sabot::TSizeChecker;

      };  // TArrayOfPairsOfStates<TArrayOfPairsOfStatesSabotState>

      /* The base for all array-of-single-states states. */
      template <typename TStaticArrayOfSingleStatesSabotState>
      class TStaticArrayOfSingleStates
          : public TStaticArrayOfSingleStatesSabotState {
        public:

        /* See Sabot::Type::TStaticArrayOfSingleStates. */
        virtual typename TStaticArrayOfSingleStatesSabotState::TPin *Pin(void *alloc) const override final {
          assert(this);
          return new (alloc) TPin(this);
        }

        protected:

        /* Caches the element count. */
        TStaticArrayOfSingleStates() {}

        /* Override to construct a new state sabot for the given element. */
        virtual TAny *NewElem(size_t elem_idx, void *state_alloc) const = 0;

        private:

        /* Keeps the array in memory. */
        class TPin final
            : public TStaticArrayOfSingleStatesSabotState::TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* Do little. */
          TPin(const TStaticArrayOfSingleStates *array_of_single_states)
              : TStaticArrayOfSingleStatesSabotState::TPin(array_of_single_states),
                StaticArrayOfSingleStates(array_of_single_states) {}

          private:

          /* See Sabot::Type::TStaticArrayOfSingleStates::TPin. */
          virtual TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const override {
            assert(this);
            return StaticArrayOfSingleStates->NewElem(elem_idx, state_alloc);
          }

          /* The array to which we're attached. */
          const TStaticArrayOfSingleStates *StaticArrayOfSingleStates;

        };  // TPin

        /* size of TPin */
        friend class Sabot::TSizeChecker;

      };  // TStaticArrayOfSingleStates<TStaticArrayOfSingleStatesSabotState>

      /*
       *    State classes.
       */

      /* State used for Native::TFree<TVal>. */
      template <typename TVal>
      class TFree final
          : public TEmpty<Sabot::State::TFree> {
        public:

        /* Do-little. */
        TFree() {}

        /* See Sabot::State::TFree. */
        virtual Sabot::Type::TFree *GetFreeType(void *type_alloc) const override {
          return Type::For<Native::TFree<TVal>>::GetFreeType(type_alloc);
        }

      };  // TFree<TVal>

      /* State used for Native::TTombstone. */
      class TTombstone final
          : public TEmpty<Sabot::State::TTombstone> {
        public:

        /* Do-little. */
        TTombstone() {}

        /* See Sabot::State::TTombstone. */
        virtual Sabot::Type::TTombstone *GetTombstoneType(void *type_alloc) const override {
          return Type::For<Native::TTombstone>::GetType(type_alloc);
        }

      };  // TTombstone

      /* State used for void. */
      class TVoid final
          : public TEmpty<Sabot::State::TVoid> {
        public:

        /* Do-little. */
        TVoid() {}

        /* See Sabot::State::TVoid. */
        virtual Sabot::Type::TVoid *GetVoidType(void *type_alloc) const override {
          return Type::For<void>::GetType(type_alloc);
        }

      };  // TVoid

      /* States used for scalars. */
      #define SCALAR(name)  \
        class T##name final  \
            : public TScalar<Sabot::State::T##name> {  \
          public:  \
          T##name(const TVal &val)  \
              : TScalar<Sabot::State::T##name>(val) {}  \
          virtual Sabot::Type::T##name *Get##name##Type(void *type_alloc) const override {  \
            return Type::For<TVal>::GetType(type_alloc);  \
          }  \
        };
      SCALAR(Int8)
      SCALAR(Int16)
      SCALAR(Int32)
      SCALAR(Int64)
      SCALAR(UInt8)
      SCALAR(UInt16)
      SCALAR(UInt32)
      SCALAR(UInt64)
      SCALAR(Bool)
      SCALAR(Char)
      SCALAR(Float)
      SCALAR(Double)
      SCALAR(Duration)
      SCALAR(TimePoint)
      SCALAR(Uuid)
      #undef SCALAR

      class TBoolRef final
          : public TScalar<Sabot::State::TBool> {
        public:
        TBoolRef(const std::vector<bool>::const_reference &val)
            : TScalar<Sabot::State::TBool>(Val), Val(static_cast<bool>(val)) {}
        virtual Sabot::Type::TBool *GetBoolType(void *type_alloc) const override {
          return Type::For<bool>::GetType(type_alloc);
        }
        private:
        bool Val;
      };

      class TStigUUID final
          : public TScalar<Sabot::State::TUuid> {
        public:
        TStigUUID(const Stig::TUUID &val)
            : TScalar<Sabot::State::TUuid>(Val), Val(val.GetRaw()) {}
        virtual Sabot::Type::TUuid *GetUuidType(void *type_alloc) const override {
          return Type::For<Base::TUuid>::GetType(type_alloc);
        }
        private:
        Base::TUuid Val;
      };


      /* States used for blob and str. */
      #define ARRAY_OF_SCALARS(name)  \
        class T##name final  \
            : public TArrayOfScalars<Sabot::State::T##name> {  \
          public:  \
          T##name(const TVal *data, size_t size)  \
              : TArrayOfScalars<Sabot::State::T##name>(data, size) {}  \
          virtual Sabot::Type::T##name *Get##name##Type(void *type_alloc) const override {  \
            return new (type_alloc) Sabot::Type::T##name();  \
          }  \
        };
      ARRAY_OF_SCALARS(Blob)
      ARRAY_OF_SCALARS(Str)
      #undef ARRAY_OF_SCALARS

      /* State used for Stig::TDesc<TElem>. */
      template <typename TElem>
      class TDesc final
          : public TArrayOfSingleStates<Sabot::State::TDesc> {
        public:

        /* Do-little. */
        TDesc(const Stig::TDesc<TElem> &val)
            : TArrayOfSingleStates<Sabot::State::TDesc>(1), Val(val) {}

        /* See Sabot::State::TDesc. */
        virtual Sabot::Type::TDesc *GetDescType(void *type_alloc) const override {
          return Type::For<Stig::TDesc<TElem>>::GetDescType(type_alloc);
        }

        private:

        /* See TArrayOfSingleStates<TElem>. */
        virtual TAny *NewElem(size_t, void *state_alloc) const override {
          return Factory<TElem>::New(*Val, state_alloc);
        }

        /* Cached reference to the value we are sabot to. */
        const Stig::TDesc<TElem> &Val;

      };  // TDesc<TElem>

      /* State used for Base::TOpt<TElem>. */
      template <typename TElem>
      class TOpt final
          : public TArrayOfSingleStates<Sabot::State::TOpt> {
        public:

        /* Do-little. */
        TOpt(const Base::TOpt<TElem> &val)
            : TArrayOfSingleStates<Sabot::State::TOpt>(val ? 1 : 0), Val(val) {}

        /* See Sabot::State::TOpt. */
        virtual Sabot::Type::TOpt *GetOptType(void *type_alloc) const override {
          return Type::For<Base::TOpt<TElem>>::GetOptType(type_alloc);
        }

        private:

        /* See TArrayOfSingleStates<TElem>. */
        virtual TAny *NewElem(size_t, void *state_alloc) const override {
          return Factory<TElem>::New(*Val, state_alloc);
        }

        /* Cached reference to the value we are sabot to. */
        const Base::TOpt<TElem> &Val;

      };  // TOpt<TElem>

      /* State used for std::set<TElem>. */
      template <typename TElem, typename TCompare = std::less<TElem>>
      class TSet final
          : public TArrayOfSingleStates<Sabot::State::TSet> {
        public:

        /* Do-little. */
        TSet(const std::set<TElem, TCompare> &val)
            : TArrayOfSingleStates<Sabot::State::TSet>(val.size()), Val(val), CachedIter(Val.begin()), CachedIterIdx(0UL) {}

        /* See Sabot::State::TSet. */
        virtual Sabot::Type::TSet *GetSetType(void *type_alloc) const override {
          return Type::For<std::set<TElem>>::GetSetType(type_alloc);
        }

        private:

        /* See TArrayOfSingleStates<TElem>. */
        virtual TAny *NewElem(size_t elem_idx, void *state_alloc) const override {
          assert(this);
          const int64_t cursor_diff = elem_idx - CachedIterIdx;
          switch (cursor_diff) {
            case 0L: {
              break;
            }
            case 1L: {
              ++CachedIter;
              ++CachedIterIdx;
              break;
            }
            case -1L: {
              --CachedIter;
              --CachedIterIdx;
              break;
            }
            default: {
              CachedIter = Val.begin();
              for (size_t i = 0; i < elem_idx; ++i) {
                ++CachedIter;
              }
              CachedIterIdx = elem_idx;
              break;
            }
          }
          return Factory<TElem>::New(*CachedIter, state_alloc);
        }

        /* Cached reference to the value we are sabot to. */
        const std::set<TElem, TCompare> &Val;

        /* Cached iterator to the set */
        mutable typename std::set<TElem, TCompare>::const_iterator CachedIter;

        /* Element index our cached iterator is pointing to */
        mutable size_t CachedIterIdx;

      };  // TSet<TElem>

      /* State used for std::vector<TElem>. */
      template <typename TElem>
      class TVector final
          : public TArrayOfSingleStates<Sabot::State::TVector> {
        public:

        /* Do-little. */
        TVector(const std::vector<TElem> &val)
            : TArrayOfSingleStates<Sabot::State::TVector>(val.size()), Val(val) {}

        /* See Sabot::State::TVector. */
        virtual Sabot::Type::TVector *GetVectorType(void *type_alloc) const override {
          return Type::For<std::vector<TElem>>::GetVectorType(type_alloc);
        }

        private:

        /* See TArrayOfSingleStates<TElem>. */
        virtual TAny *NewElem(size_t elem_idx, void *state_alloc) const override {
          return Factory<TElem>::New(Val[elem_idx], state_alloc);
        }

        /* Cached reference to the value we are sabot to. */
        const std::vector<TElem> &Val;

      };  // TVector<TElem>

      /* State used for std::map<TLhs, TRhs>. */
      template <typename TLhs, typename TRhs, typename TCompare = std::less<TLhs>>
      class TMap final
          : public TArrayOfPairsOfStates<Sabot::State::TMap> {
        public:

        /* Do-little. */
        TMap(const std::map<TLhs, TRhs, TCompare> &val)
            : TArrayOfPairsOfStates<Sabot::State::TMap>(val.size()), Val(val), CachedIter(Val.begin()), CachedIterIdx(0UL) {}

        /* See Sabot::State::TMap. */
        virtual Sabot::Type::TMap *GetMapType(void *type_alloc) const override {
          return Type::For<std::map<TLhs, TRhs>>::GetMapType(type_alloc);
        }

        private:

        /* The map iterator corresponding to the given element index. */
        typename std::map<TLhs, TRhs, TCompare>::const_iterator GetIter(size_t elem_idx) const {
          assert(this);
          const int64_t cursor_diff = elem_idx - CachedIterIdx;
          switch (cursor_diff) {
            case 0L: {
              break;
            }
            case 1L: {
              ++CachedIter;
              ++CachedIterIdx;
              break;
            }
            case -1L: {
              --CachedIter;
              --CachedIterIdx;
              break;
            }
            default: {
              CachedIter = Val.begin();
              for (size_t i = 0; i < elem_idx; ++i) {
                ++CachedIter;
              }
              CachedIterIdx = elem_idx;
              break;
            }
          }
          return CachedIter;
        }

        /* See TArrayOfSingleStates<TLhs, TRhs>. */
        virtual TAny *NewLhs(size_t elem_idx, void *state_alloc) const override {
          return Factory<TLhs>::New(GetIter(elem_idx)->first, state_alloc);
        }

        /* See TArrayOfSingleStates<TLhs, TRhs>. */
        virtual TAny *NewRhs(size_t elem_idx, void *state_alloc) const override {
          return Factory<TRhs>::New(GetIter(elem_idx)->second, state_alloc);
        }

        /* Cached reference to the value we are sabot to. */
        const std::map<TLhs, TRhs, TCompare> &Val;

        /* Cached iterator to the map */
        mutable typename std::map<TLhs, TRhs, TCompare>::const_iterator CachedIter;

        /* Element index our cached iterator is pointing to */
        mutable size_t CachedIterIdx;

      };  // TMap<TLhs, TRhs, TCompare>

      /* State used for std::tuple<TElems...>. */
      template <typename... TElems>
      class TTuple final
          : public TStaticArrayOfSingleStates<Sabot::State::TTuple> {
        public:

        /* Do-little. */
        TTuple(const std::tuple<TElems...> &val)
            : Val(val) {}

        /* See Sabot::State::TTuple. */
        virtual Sabot::Type::TTuple *GetTupleType(void *type_alloc) const override {
          return Type::For<std::tuple<TElems...>>::GetType(type_alloc);
        }

        private:

        /* See TArrayOfSingleStates<TElems...>. */
        virtual TAny *NewElem(size_t elem_idx, void *state_alloc) const override {
          return ForTuple<0, TElems...>::NewElem(elem_idx, Val, state_alloc);
        }

        /* Cached reference to the value we are sabot to. */
        const std::tuple<TElems...> &Val;

      };  // TTuple<TElems...>

      /* size of our states */
      friend class Stig::Sabot::TSizeChecker;

    };  // State

    /*
     *    Specializations of State::Factory<>.
     */

    /* Explicit specialization for Native::TFree<TElem>. */
    template <typename TElem>
    class State::Factory<Native::TFree<TElem>> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const Native::TFree<TElem> &, void *state_alloc) {
        return new (state_alloc) TFree<TElem>;
      }

    };  // State::Factory<Native::TFree<TElem>>

    /* Explicit specialization for Native::TTombstone. */
    template <>
    class State::Factory<Native::TTombstone> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const Native::TTombstone &, void *state_alloc) {
        return new (state_alloc) TTombstone;
      }

    };  // State::Factory<Native::TTombstone>

    /* Explicit specializations for scalar states. */
    #define SCALAR_FACTORY(val_t, name)  \
      template <>  \
      class State::Factory<val_t> final {  \
        NO_CONSTRUCTION(Factory);  \
        public:  \
        static TAny *New(const val_t &val, void *state_alloc) {  \
          return new (state_alloc) T##name(val);  \
        }  \
      };
    SCALAR_FACTORY(int8_t, Int8)
    SCALAR_FACTORY(int16_t, Int16)
    SCALAR_FACTORY(int32_t, Int32)
    SCALAR_FACTORY(int64_t, Int64)
    SCALAR_FACTORY(uint8_t, UInt8)
    SCALAR_FACTORY(uint16_t, UInt16)
    SCALAR_FACTORY(uint32_t, UInt32)
    SCALAR_FACTORY(uint64_t, UInt64)
    SCALAR_FACTORY(bool, Bool)
    SCALAR_FACTORY(char, Char)
    SCALAR_FACTORY(float, Float)
    SCALAR_FACTORY(double, Double)
    SCALAR_FACTORY(Sabot::TStdDuration, Duration)
    SCALAR_FACTORY(Sabot::TStdTimePoint, TimePoint)
    SCALAR_FACTORY(Base::TUuid, Uuid)
    #undef SCALAR_FACTORY

    template <>
    class State::Factory<std::vector<bool>::reference> final {
      NO_CONSTRUCTION(Factory);
      public:
      static TAny *New(const std::vector<bool>::const_reference &val, void *state_alloc) {
        return new (state_alloc) TBoolRef(val);
      }
    };

    template <>
    class State::Factory<Stig::TUUID> final {
      NO_CONSTRUCTION(Factory);
      public:
      static TAny *New(const Stig::TUUID &val, void *state_alloc) {
        return new (state_alloc) TStigUUID(val);
      }
    };

    /* Explicit specialization for Native::TBlob. */
    template <>
    class State::Factory<Native::TBlob> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const Native::TBlob &val, void *state_alloc) {
        return new (state_alloc) TBlob(val.data(), val.size());
      }

    };  // State::Factory<Native::TBlob>

    /* Explicit specialization for std::string. */
    template <>
    class State::Factory<std::string> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const std::string &val, void *state_alloc) {
        return new (state_alloc) TStr(val.data(), val.size());
      }

    };  // State::Factory<std::string>

    /* Explicit specialization for const char *. */
    template <>
    class State::Factory<const char *> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const char *val, void *state_alloc) {
        assert(val);
        return new (state_alloc) TStr(val, strlen(val));
      }

    };  // State::Factory<const char *>

    /* Explicit specialization for char[Size]. */
    template <size_t Size>
    class State::Factory<char[Size]> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Are we an empty static array? */
      static_assert(Size, "cannot construct native static sabot for char[0]");

      /* Convenience. */
      using TVal = char[Size];

      /* Construct a new state sabot around the value. */
      static TAny *New(const TVal &val, void *state_alloc) {
        return new (state_alloc) TStr(val, Size - 1);
      }

    };  // State::Factory<char[Size]>

    /* Explicit specialization for Stig::TDesc<TElem>. */
    template <typename TElem>
    class State::Factory<Stig::TDesc<TElem>> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const Stig::TDesc<TElem> &val, void *state_alloc) {
        return new (state_alloc) TDesc<TElem>(val);
      }

    };  // State::Factory<Stig::TDesc<TElem>>

    /* Explicit specialization for Base::TOpt<TElem>. */
    template <typename TElem>
    class State::Factory<Base::TOpt<TElem>> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const Base::TOpt<TElem> &val, void *state_alloc) {
        return new (state_alloc) TOpt<TElem>(val);
      }

    };  // State::Factory<Base::TOpt<TElem>>

    /* Explicit specialization for std::set<TElem>. */
    template <typename TElem, typename TCompare>
    class State::Factory<std::set<TElem, TCompare>> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const std::set<TElem, TCompare> &val, void *state_alloc) {
        return new (state_alloc) TSet<TElem, TCompare>(val);
      }

    };  // State::Factory<std::set<TElem>>

    /* Explicit specialization for std::vector<TElem>. */
    template <typename TElem>
    class State::Factory<std::vector<TElem>> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const std::vector<TElem> &val, void *state_alloc) {
        return new (state_alloc) TVector<TElem>(val);
      }

    };  // State::Factory<std::vector<TElem>>

    /* Explicit specialization for std::map<TLhs, TRhs, TCompare>. */
    template <typename TLhs, typename TRhs, typename TCompare>
    class State::Factory<std::map<TLhs, TRhs, TCompare>> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const std::map<TLhs, TRhs, TCompare> &val, void *state_alloc) {
        return new (state_alloc) TMap<TLhs, TRhs, TCompare>(val);
      }

    };  // State::Factory<std::map<TLhs, TRhs, TCompare>>

    /* Explicit specialization for std::tuple<TElems...>. */
    template <typename... TElems>
    class State::Factory<std::tuple<TElems...>> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const std::tuple<TElems...> &val, void *state_alloc) {
        return new (state_alloc) TTuple<TElems...>(val);
      }

    };  // State::Factory<std::tuple<TElems...>>

    /*
     *    Specializations of State::ForTuple<> and special case for State::TTuple<>.
     */

    /* Specialization for a non-empty partial tuple. */
    template <size_t Idx, typename TElem, typename... TMoreElems>
    class State::ForTuple<Idx, TElem, TMoreElems...> {
      NO_CONSTRUCTION(ForTuple);
      public:

      /* Our non-empty partial tuple type. */
      using TPartialTuple = std::_Tuple_impl<Idx, TElem, TMoreElems...>;

      /* If we want the 0th element, return the head of this partial tuple; otherwise, decrement the requested index and recurse. */
      static TAny *NewElem(size_t elem_idx, const TPartialTuple &partial_tuple, void *state_alloc) {
        return elem_idx
            ? ForTuple<Idx + 1, TMoreElems...>::NewElem(elem_idx - 1, TPartialTuple::_M_tail(partial_tuple), state_alloc)
            : Factory<TElem>::New(TPartialTuple::_M_head(partial_tuple), state_alloc);
      }

    };  // State::ForTuple<Idx, TElem, TMoreElems...>

    /* Specialization for an empty partial tuple. */
    template <size_t Idx>
    class State::ForTuple<Idx> {
      NO_CONSTRUCTION(ForTuple);
      public:

      /* Our partial tuple has no data, so referring to any element is always out-of-bounds. */
      static TAny *NewElem(size_t, const std::_Tuple_impl<Idx> &, void */*state_alloc*/) {
        throw Sabot::TIdxTooBig();
      }

    };  // State::ForTuple<Idx>

    /* State used for std::tuple<>. */
    template <>
    class State::TTuple<> final
        : public TStaticArrayOfSingleStates<Sabot::State::TTuple> {
      public:

      /* Do-little. */
      TTuple(const std::tuple<> &) {}

      /* See Sabot::State::TTuple. */
      virtual Sabot::Type::TTuple *GetTupleType(void *type_alloc) const override {
        return Type::For<std::tuple<>>::GetType(type_alloc);
      }

      private:

      /* See TArrayOfSingleStates<>. */
      virtual TAny *NewElem(size_t, void */*state_alloc*/) const override {
        throw Sabot::TIdxTooBig();
      }

    };  // State::TTuple<>

    /* State used for std::vector<TElem>. */
    template <>
    class State::TVector<bool> final
        : public TArrayOfSingleStates<Sabot::State::TVector> {
      public:

      /* Do-little. */
      TVector(const std::vector<bool> &val)
          : TArrayOfSingleStates<Sabot::State::TVector>(val.size()), Val(val) {}

      /* See Sabot::State::TVector. */
      virtual Sabot::Type::TVector *GetVectorType(void *type_alloc) const override {
        return Type::For<std::vector<bool>>::GetVectorType(type_alloc);
      }

      private:

      /* See TArrayOfSingleStates<TElem>. */
      virtual TAny *NewElem(size_t elem_idx, void *state_alloc) const override {
        return Factory<std::vector<bool>::reference>::New(Val[elem_idx], state_alloc);
      }

      /* Cached reference to the value we are sabot to. */
      const std::vector<bool> &Val;

    };  // State::TVector<TElem>

    /* Explicit specialization for Rt::TMutable<TAddr, TResVal>. */
    template <typename TAddr, typename TVal>
    class State::Factory<Rt::TMutable<TAddr, TVal>> final {
      NO_CONSTRUCTION(Factory);
      public:

      /* Construct a new state sabot around the value. */
      static TAny *New(const Rt::TMutable<TAddr, TVal> &val, void *state_alloc) {
        return State::New<TVal>(val.GetVal(), state_alloc);
      }

    };  // State::Factory<Rt::TMutable<TAddr, TVal>>

  }  // Native

}  // Stig
