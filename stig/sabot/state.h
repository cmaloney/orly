/* <stig/sabot/state.h>

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

#include <memory>
#include <utility>

#include <base/no_copy_semantics.h>
#include <base/no_construction.h>
#include <stig/sabot/type.h>

namespace Stig {

  namespace Sabot {

    class TStateVisitor;

    /* Using a class as a finite namespace. */
    class State {
      NO_CONSTRUCTION(State);
      public:

      /*
       *    Forward declarations.
       */

      /* Empty states. */
      class TFree;
      class TTombstone;
      class TVoid;

      /* Scalar states. */
      class TInt8;
      class TInt16;
      class TInt32;
      class TInt64;
      class TUInt8;
      class TUInt16;
      class TUInt32;
      class TUInt64;
      class TBool;
      class TChar;
      class TFloat;
      class TDouble;
      class TDuration;
      class TTimePoint;
      class TUuid;

      /* Arrays of scalars. */
      class TBlob;
      class TStr;

      /* Arrays of single states. */
      class TDesc;
      class TOpt;
      class TSet;
      class TVector;

      /* Arrays of pairs of states. */
      class TMap;

      /* Arrays of static size. */
      class TRecord;
      class TTuple;

      /* TODO */
      static constexpr size_t GetMaxStatePinSize() { return 88; }
      static constexpr size_t GetMaxStateSize() { return 48; }

      /* TODO */
      template <typename TVal>
      class TPinWrapper {
        NO_COPY_SEMANTICS(TPinWrapper);
        public:

        /* TODO */
        TPinWrapper(TVal *pin) : Pin(pin) {}

        /* TODO */
        ~TPinWrapper() {
          Pin->~TVal();
        }

        /* TODO */
        TVal *operator->() const {
          return Pin;
        }

        /* TODO */
        TVal *get() const {
          return Pin;
        }

        private:

        /* TODO */
        TVal *Pin;

      };  // TPinWrapper

      /*
       *    Base classes.
       */

      /* The base for all state objects. */
      class TAny {
        NO_COPY_SEMANTICS(TAny);
        public:

        /* TODO */
        class TWrapper {
          NO_COPY_SEMANTICS(TWrapper);
          public:

          /* TODO */
          TWrapper(TAny *any) : Any(any) {}

          /* TODO */
          ~TWrapper() {
            Any->~TAny();
          }

          /* TODO */
          TAny *operator->() const {
            return Any;
          }

          /* TODO */
          const TAny &operator*() const {
            return *Any;
          }

          /* TODO */
          operator TAny *() const {
            return Any;
          }

          /* TODO */
          TAny *get() const {
            return Any;
          }

          private:

          /* TODO */
          TAny *Any;

        };  // TWrapper

        /* Do-little. */
        virtual ~TAny() {}

        /* TODO */
        virtual void Accept(const TStateVisitor &visitor) const = 0;

        /* Override to return the type object associated with this state. */
        virtual Type::TAny *GetType(void *type_alloc) const = 0;

        protected:

        /* Do-little. */
        TAny() {}

      };  // TAny

      /* The base for all states which represent a single value which can be returned directly.
         It is expected that sabot implementers will be able to store any such state without resorting to indirection. */
      template <typename TVal_>
      class TScalar
          : public TAny {
        public:

        /* Pull our template parameter into scope. */
        using TVal = TVal_;

        /* Override to provide access to the state. */
        virtual const TVal &Get() const = 0;

        protected:

        /* Do-little. */
        TScalar() {}

      };  // TScalar<TVal>

      /* The base for all states which are represented as arrays of scalars.  The sabot implementer may choose to store the array itself indirecty,
         so access to it is provided through a pin object.  The array need remain fixed in memory only as long as there is a pin referring
         to it. */
      template <typename TVal_>
      class TArrayOfScalars
          : public TAny {
        public:

        /* Pull our template parameter into scope. */
        using TVal = TVal_;

        /* Keeps the array in memory. */
        class TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          using TWrapper = TPinWrapper<TPin>;

          /* Do-little. */
          virtual ~TPin() {}

          /* The limit of the array. */
          const TVal *GetLimit() const {
            assert(this);
            return Limit;
          }

          /* The number of scalar values in the array. */
          size_t GetSize() const {
            return Limit - Start;
          }

          /* The start of the array. */
          const TVal *GetStart() const {
            assert(this);
            return Start;
          }

          protected:

          /* Caches the pointers. */
          TPin(const TVal *start, const TVal *limit)
              : Start(start), Limit(limit) {
            assert(start <= limit);
          }

          private:

          /* See accessors. */
          const TVal *Start, *Limit;

        };  // TPin

        /* Override to return the number of scalar values in the array.
           This information should be accessible even if the array isn't pinned. */
        virtual size_t GetSize() const  = 0;

        /* Pin the array into memory. */
        virtual TPin *Pin(void *alloc) const = 0;

        protected:

        /* Do-little. */
        TArrayOfScalars() {}

      };  // TArrayOfScalars

      /* The base for all states which are represented as arrays of state sabots.
         The class is otherwise similar to TArrayOfScalars, q.v. */
      class TArrayOfSingleStates
          : public TAny {
        public:

        /* Keeps the array in memory. */
        class TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          using TWrapper = TPinWrapper<TPin>;

          /* Do-little. */
          virtual ~TPin() {}

          /* The number of elements in the array. */
          size_t GetElemCount() const {
            return ElemCount;
          }

          /* Constructs a new state sabot for the given element. */
          TAny *NewElem(size_t elem_idx, void *state_alloc) const {
            assert(this);
            if (elem_idx >= ElemCount) {
              throw TIdxTooBig();
            }
            return NewElemInRange(elem_idx, state_alloc);
          }

          protected:

          /* Caches the element count. */
          TPin(const TArrayOfSingleStates *array_of_single_states) {
            assert(array_of_single_states);
            ElemCount = array_of_single_states->GetElemCount();
          }

          /* Override to construct a new state sabot for the given element. */
          virtual TAny *NewElemInRange(size_t elem_idx, void *state_alloc) const = 0;

          private:

          /* See accessor. */
          size_t ElemCount;

        };  // TPin

        /* Override to return the number of elements in the array.
           This information should be accessible even if the array isn't pinned. */
        virtual size_t GetElemCount() const  = 0;

        /* Pin the array into memory. */
        virtual TPin *Pin(void *alloc) const = 0;

        protected:

        /* Do-little. */
        TArrayOfSingleStates() {}

      };  // TArrayOfSingleStates

      /* The base for all states which are represented as arrays of pairs of state sabots.
         The class is otherwise similar to TArrayOfScalars, q.v. */
      class TArrayOfPairsOfStates
          : public TAny {
        public:

        /* Keeps the array in memory. */
        class TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          using TWrapper = TPinWrapper<TPin>;

          /* Do-little. */
          virtual ~TPin() {}

          /* The number of elements in the array. */
          size_t GetElemCount() const {
            return ElemCount;
          }

          /* Constructs a new state sabot for the given lhs. */
          TAny *NewLhs(size_t elem_idx, void *state_alloc) const {
            assert(this);
            if (elem_idx >= ElemCount) {
              throw TIdxTooBig();
            }
            return NewLhsInRange(elem_idx, state_alloc);
          }

          /* Constructs a new state sabot for the given rhs. */
          TAny *NewRhs(size_t elem_idx, void *state_alloc) const {
            assert(this);
            if (elem_idx >= ElemCount) {
              throw TIdxTooBig();
            }
            return NewRhsInRange(elem_idx, state_alloc);
          }

          protected:

          /* Caches the element count. */
          TPin(const TArrayOfPairsOfStates *array_of_pairs_of_states) {
            assert(array_of_pairs_of_states);
            ElemCount = array_of_pairs_of_states->GetElemCount();
          }

          /* Override to construct a new state sabot for the given lhs element. */
          virtual TAny *NewLhsInRange(size_t elem_idx, void *state_alloc) const = 0;

          /* Override to construct a new state sabot for the given rhs element. */
          virtual TAny *NewRhsInRange(size_t elem_idx, void *state_alloc) const = 0;

          private:

          /* See accessor. */
          size_t ElemCount;

        };  // TPin

        /* Override to return the number of elements in the array.
           This information should be accessible even if the array isn't pinned. */
        virtual size_t GetElemCount() const  = 0;

        /* Pin the array into memory. */
        virtual TPin *Pin(void *alloc) const = 0;

        protected:

        /* Do-little. */
        TArrayOfPairsOfStates() {}

      };  // TArrayOfPairsOfStates

      /* The number of elements in a record or tuple type. */
      static size_t GetStaticElemCount(const Sabot::Type::TAny &type);

      /* The base for all states which are represented as arrays of state sabots and for which the number of elements in the array is static;
         i.e., records and tuples. */
      class TStaticArrayOfSingleStates
          : public TArrayOfSingleStates {
        public:

        /* See TArray<TElem>. */
        virtual size_t GetElemCount() const override {
          assert(this);
          void *type_alloc = alloca(Type::GetMaxTypeSize());
          return GetStaticElemCount(*Type::TAny::TWrapper(this->GetType(type_alloc)));
        }

      };  // TArrayOfStaticElemCount

      /*
       *    Type classes.
       */

      /* Stateless states. */
      #define EMPTY_STATE(name)  \
        class T##name \
            : public TAny {  \
          public:  \
          virtual void Accept(const TStateVisitor &visitor) const override final;  \
          virtual Type::TAny *GetType(void *type_alloc) const override final;  \
          virtual Type::T##name *Get##name##Type(void *type_alloc) const = 0;  \
          protected: \
          T##name() {}  \
        };
      EMPTY_STATE(Free)
      EMPTY_STATE(Tombstone)
      EMPTY_STATE(Void)
      #undef EMPTY_STATE

      /* Scalar states. */
      #define SCALAR_STATE(val_t, name)  \
        class T##name  \
            : public TScalar<val_t> {  \
          public:  \
          virtual void Accept(const TStateVisitor &visitor) const override final;  \
          virtual Type::TAny *GetType(void *type_alloc) const override final;  \
          virtual Type::T##name *Get##name##Type(void *type_alloc) const = 0;  \
          protected:  \
          T##name() {}  \
        };
      SCALAR_STATE(int8_t, Int8)
      SCALAR_STATE(int16_t, Int16)
      SCALAR_STATE(int32_t, Int32)
      SCALAR_STATE(int64_t, Int64)
      SCALAR_STATE(uint8_t, UInt8)
      SCALAR_STATE(uint16_t, UInt16)
      SCALAR_STATE(uint32_t, UInt32)
      SCALAR_STATE(uint64_t, UInt64)
      SCALAR_STATE(bool, Bool)
      SCALAR_STATE(char, Char)
      SCALAR_STATE(float, Float)
      SCALAR_STATE(double, Double)
      SCALAR_STATE(TStdDuration, Duration)
      SCALAR_STATE(TStdTimePoint, TimePoint)
      SCALAR_STATE(Base::TUuid, Uuid)
      #undef SCALAR_STATE

      /* Arrays of scalars.  Note that, in the case of Str, the limit of the array is guaranteed to be pointing
         to a null terminator; however, the terminator does not count toward the size of the array. */
      #define ARRAY_OF_SCALARS(val_t, name)  \
        class T##name  \
            : public TArrayOfScalars<val_t> {  \
          public:  \
          virtual void Accept(const TStateVisitor &visitor) const override final;  \
          virtual Type::TAny *GetType(void *type_alloc) const override final;  \
          virtual Type::T##name *Get##name##Type(void *type_alloc) const = 0;  \
          protected:  \
          T##name() {}  \
        };
      ARRAY_OF_SCALARS(uint8_t, Blob)
      ARRAY_OF_SCALARS(char, Str)
      #undef ARRAY_OF_SCALARS

      /* Arrays of single states. */
      #define ARRAY_OF_SINGLE_STATES(name)  \
        class T##name \
            : public TArrayOfSingleStates {  \
          public:  \
          virtual void Accept(const TStateVisitor &visitor) const override final;  \
          virtual Type::TAny *GetType(void *type_alloc) const override final;  \
          virtual Type::T##name *Get##name##Type(void *type_alloc) const = 0;  \
          protected: \
          T##name() {}  \
        };
      ARRAY_OF_SINGLE_STATES(Desc)
      ARRAY_OF_SINGLE_STATES(Opt)
      ARRAY_OF_SINGLE_STATES(Set)
      ARRAY_OF_SINGLE_STATES(Vector)
      #undef ARRAY_OF_SINGLE_STATES

      /* Arrays of pairs of states. */
      #define ARRAY_OF_PAIRS_OF_STATES(name)  \
        class T##name \
            : public TArrayOfPairsOfStates {  \
          public:  \
          virtual void Accept(const TStateVisitor &visitor) const override final;  \
          virtual Type::TAny *GetType(void *type_alloc) const override final;  \
          virtual Type::T##name *Get##name##Type(void *type_alloc) const = 0;  \
          protected: \
          T##name() {}  \
        };
      ARRAY_OF_PAIRS_OF_STATES(Map)
      #undef ARRAY_OF_PAIRS_OF_STATES

      /* Static arrays of single states. */
      #define STATIC_ARRAY_OF_SINGLE_STATES(name)  \
        class T##name \
            : public TStaticArrayOfSingleStates {  \
          public:  \
          virtual void Accept(const TStateVisitor &visitor) const override final;  \
          virtual Type::TAny *GetType(void *type_alloc) const override final;  \
          virtual Type::T##name *Get##name##Type(void *type_alloc) const = 0;  \
          protected: \
          T##name() {}  \
        };
      STATIC_ARRAY_OF_SINGLE_STATES(Record)
      STATIC_ARRAY_OF_SINGLE_STATES(Tuple)
      #undef STATIC_ARRAY_OF_SINGLE_STATES

    };  // State

    /* TODO */
    class TStateVisitor {
      NO_COPY_SEMANTICS(TStateVisitor);
      public:

      /* TODO */
      virtual ~TStateVisitor() {}

      virtual void operator()(const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TTuple &rhs     ) const = 0;

      protected:

      /* TODO */
      TStateVisitor() {}

    };  // TStateVisitor

    /* TODO */
    class TStateDoubleVisitor {
      NO_COPY_SEMANTICS(TStateDoubleVisitor);
      public:

      /* TODO */
      virtual ~TStateDoubleVisitor() {}

      /* Overrides. */
      virtual void operator()(const State::TFree &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TFree &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TTombstone &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TTombstone &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TVoid &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TVoid &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TInt8 &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TInt8 &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TInt16 &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TInt16 &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TInt32 &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TInt32 &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TInt64 &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TInt64 &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TUInt8 &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TUInt8 &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TUInt16 &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TUInt16 &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TUInt32 &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TUInt32 &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TUInt64 &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TUInt64 &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TBool &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TBool &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TChar &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TChar &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TFloat &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TFloat &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TDouble &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TDouble &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TDuration &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TDuration &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TTimePoint &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TTimePoint &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TUuid &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TUuid &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TBlob &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TBlob &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TStr &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TStr &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TDesc &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TDesc &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TOpt &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TOpt &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TSet &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TSet &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TVector &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TVector &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TMap &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TMap &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TRecord &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TRecord &lhs, const State::TTuple &rhs     ) const = 0;

      virtual void operator()(const State::TTuple &lhs, const State::TFree &rhs      ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TTombstone &rhs ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TVoid &rhs      ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TInt8 &rhs      ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TInt16 &rhs     ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TInt32 &rhs     ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TInt64 &rhs     ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TUInt8 &rhs     ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TUInt16 &rhs    ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TUInt32 &rhs    ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TUInt64 &rhs    ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TBool &rhs      ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TChar &rhs      ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TFloat &rhs     ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TDouble &rhs    ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TDuration &rhs  ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TTimePoint &rhs ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TUuid &rhs      ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TBlob &rhs      ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TStr &rhs       ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TDesc &rhs      ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TOpt &rhs       ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TSet &rhs       ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TVector &rhs    ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TMap &rhs       ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TRecord &rhs    ) const = 0;
      virtual void operator()(const State::TTuple &lhs, const State::TTuple &rhs     ) const = 0;

      protected:

      /* TODO */
      TStateDoubleVisitor() {}

    };  // TStateDoubleVisitor

    void AcceptDouble(const State::TAny &lhs, const State::TAny &rhs, const TStateDoubleVisitor &double_visitor);

  }  // Sabot

}  // Stig
