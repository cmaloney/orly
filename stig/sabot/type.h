/* <stig/sabot/type.h>

   Abstract (well, mostly abstract) objects describing the type of a sabot.

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

#include <utility>

#include <base/no_copy_semantics.h>
#include <base/no_construction.h>
#include <stig/sabot/defs.h>

namespace Stig {

  namespace Sabot {

    /* Forward Declarations. */
    class TTypeVisitor;

    /* Using a class as a finite namespace. */
    class Type {
      NO_CONSTRUCTION(Type);
      public:

      /*
       *    Forward declarations.
       */

      /* Nullary types.  These are final classes, not abstract bases.  In fact, they are singletons. */
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
      class TBlob;
      class TStr;
      class TTombstone;
      class TVoid;

      /* Unary types.  These are base classes which the sabot implementer must finalize. */
      class TDesc;
      class TFree;
      class TOpt;
      class TSet;
      class TVector;

      /* Binary types.  These are base classes which the sabot implementer must finalize. */
      class TMap;

      /* N-ary types.  These are base classes which the sabot implementer must finalize. */
      class TRecord;
      class TTuple;

      #if 0
      /* The sum of all the types in this scope. */
      using Sum = Visitor::Sum<
        TInt8, TInt16, TInt32, TInt64, TUInt8, TUInt16, TUInt32, TUInt64, TBool, TChar,
        TFloat, TDouble, TDuration, TTimePoint, TUuid, TBlob, TStr, TTombstone, TVoid,
        TDesc, TFree, TOpt, TSet, TVector, TMap, TRecord, TTuple
      >;  // Sum
      #endif

      /* TODO */
      static constexpr size_t GetMaxTypePinSize() { return 88; }
      static constexpr size_t GetMaxTypeSize() { return 40; }

      /* TODO */
      template <typename TVal>
      class TWrapperBase {
        NO_COPY_SEMANTICS(TWrapperBase);
        public:

        /* TODO */
        TWrapperBase(TVal *val) : Val(val) {}

        /* TODO */
        ~TWrapperBase() {
          Val->~TVal();
        }

        /* TODO */
        TVal *operator->() const {
          return Val;
        }

        /* TODO */
        const TVal &operator*() const {
          return *Val;
        }

        /* TODO */
        operator TVal *() const {
          return Val;
        }

        /* TODO */
        TVal *get() const {
          return Val;
        }

        private:

        /* TODO */
        TVal *Val;

      };  // TWrapperBase

      /*
       *    Base classes.
       */

      /* The base for all type objects. */
      class TAny {
        NO_COPY_SEMANTICS(TAny);
        public:

        /* TODO */
        using TWrapper = TWrapperBase<TAny>;

        /* Do-little. */
        virtual ~TAny() {}

        /* TODO */
        virtual void Accept(const TTypeVisitor &vistor) const = 0;

        protected:

        /* Do-little. */
        TAny() {}

      };  // TAny

      /* The base class for all unary types. */
      class TUnary
          : public TAny {
        public:

        /* TODO */
        using TWrapper = TWrapperBase<TUnary>;

        /* Keeps the array in memory. */
        class TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          using TWrapper = TWrapperBase<TPin>;

          /* Do-little. */
          virtual ~TPin() {}

          /* The number of elements in the array. */
          size_t GetElemCount() const {
            return 1UL;
          }

          /* Override to construct a new type sabot. */
          virtual TAny *NewElem(void *type_alloc) const = 0;

          protected:

          /* Do-little. */
          TPin() {}

        };  // TPin

        /* Pin the array into memory. */
        virtual TPin *Pin(void *alloc) const = 0;

        protected:

        /* Do-little */
        TUnary() {}

      };  // TUnary

      /* The base class for all binary types. */
      class TBinary
          : public TAny {
        public:

        /* TODO */
        using TWrapper = TWrapperBase<TBinary>;

        /* Keeps the array in memory. */
        class TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          using TWrapper = TWrapperBase<TPin>;

          /* Do-little. */
          virtual ~TPin() {}

          /* The number of elements in the array. */
          size_t GetElemCount() const {
            return 2UL;
          }

          /* Override to return our lhs type. */
          virtual TAny *NewLhs(void *type_alloc) const = 0;

          /* Override to return our rhs type. */
          virtual TAny *NewRhs(void *type_alloc) const = 0;

          protected:

          /* Do-little. */
          TPin() {}

        };  // TPin

        /* Pin the array into memory. */
        virtual TPin *Pin(void *alloc) const = 0;

        protected:

        /* Do-little */
        TBinary() {}

      };  // TBinary

      /* The base class for all n-ary types. */
      class TNAry
          : public TAny {
        public:

        /* TODO */
        using TWrapper = TWrapperBase<TNAry>;

        /* Override to return the number of elements in the type. */
        virtual size_t GetElemCount() const = 0;

        protected:

        /* Do-little */
        TNAry() {}

      };  // TNary

      /*
       *    Final classes.
       */

      /* Nullary types. */
      /*
      static const T##name name;  \
          private: \
      */
      #define NULLARY_TYPE(name)  \
        class T##name final  \
            : public TAny {  \
          public:  \
          virtual void Accept(const TTypeVisitor &vistor) const override;  \
          T##name() {}  \
        };
      NULLARY_TYPE(Int8)
      NULLARY_TYPE(Int16)
      NULLARY_TYPE(Int32)
      NULLARY_TYPE(Int64)
      NULLARY_TYPE(UInt8)
      NULLARY_TYPE(UInt16)
      NULLARY_TYPE(UInt32)
      NULLARY_TYPE(UInt64)
      NULLARY_TYPE(Bool)
      NULLARY_TYPE(Char)
      NULLARY_TYPE(Float)
      NULLARY_TYPE(Double)
      NULLARY_TYPE(Duration)
      NULLARY_TYPE(TimePoint)
      NULLARY_TYPE(Uuid)
      NULLARY_TYPE(Blob)
      NULLARY_TYPE(Str)
      NULLARY_TYPE(Tombstone)
      NULLARY_TYPE(Void)
      #undef NULLARY_TYPE

      /* Unary types. */
      #define UNARY_TYPE(name)  \
        class T##name \
            : public TUnary {  \
          public:  \
          virtual void Accept(const TTypeVisitor &vistor) const override final;  \
          protected:  \
          T##name() {}  \
        };
      UNARY_TYPE(Desc)
      UNARY_TYPE(Free)
      UNARY_TYPE(Opt)
      UNARY_TYPE(Set)
      UNARY_TYPE(Vector)
      #undef UNARY_TYPE

      /* Binary types. */
      #define BINARY_TYPE(name)  \
        class T##name \
            : public TBinary {  \
          public:  \
          virtual void Accept(const TTypeVisitor &vistor) const override final;  \
          protected:  \
          T##name() {}  \
        };
      BINARY_TYPE(Map)
      #undef BINARY_TYPE

      /* Record type. */
      class TRecord
          : public TNAry {
        public:

        /* TODO */
        using TWrapper = TWrapperBase<TRecord>;

        /* See TAny. */
        virtual void Accept(const TTypeVisitor &vistor) const override final;

        /* Keeps the array in memory. */
        class TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          using TWrapper = TWrapperBase<TPin>;

          /* Do-little. */
          virtual ~TPin() {}

          /* The number of types in the array. */
          size_t GetElemCount() const {
            return ElemCount;
          }

          /* Override to callback to provide access to an element with the name. */
          virtual TAny *NewElem(size_t elem_idx, std::string &name, void *type_alloc) const = 0;

          /* Override to callback to provide access to an element with the name. */
          virtual TAny *NewElem(size_t elem_idx, void *&out_field_name_sabot_state, void *field_name_state_alloc, void *type_alloc) const = 0;

          /* Override to callback to provide acces to an element without the name. */
          virtual TAny *NewElem(size_t elem_idx, void *type_alloc) const = 0;

          protected:

          /* Caches the element count. */
          TPin(const TRecord *record) {
            assert(record);
            ElemCount = record->GetElemCount();
          }

          private:

          /* See accessor. */
          size_t ElemCount;

        };  // TPin

        /* Pin the array into memory. */
        virtual TPin *Pin(void *alloc) const = 0;

        protected:

        /* Do-little. */
        TRecord() {}

      };  // TRecord

      /* Tuple type. */
      class TTuple
          : public TNAry {
        public:

        /* TODO */
        using TWrapper = TWrapperBase<TTuple>;

        /* The type of our callback. */
        using TCb = std::function<bool (const TAny &type)>;

        /* See TAny. */
        virtual void Accept(const TTypeVisitor &vistor) const override final;

        /* Keeps the array in memory. */
        class TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          using TWrapper = TWrapperBase<TPin>;

          /* Do-little. */
          virtual ~TPin() {}

          /* The number of types in the array. */
          size_t GetElemCount() const {
            return ElemCount;
          }

          /* Override to callback to provide access to an element. */
          virtual TAny *NewElem(size_t elem_idx, void *type_alloc) const = 0;

          protected:

          /* Caches the element count. */
          TPin(const TTuple *tuple) {
            assert(tuple);
            ElemCount = tuple->GetElemCount();
          }

          private:

          /* See accessor. */
          size_t ElemCount;

        };  // TPin

        /* Pin the array into memory. */
        virtual TPin *Pin(void *alloc) const = 0;

        protected:

        /* Do-little. */
        TTuple() {}

      };  // TTuple

    };  // Type

    /* TODO */
    class TTypeVisitor {
      NO_COPY_SEMANTICS(TTypeVisitor);
      public:

      /* TODO */
      virtual ~TTypeVisitor() {}

      virtual void operator()(const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TTuple &rhs    ) const = 0;

      protected:

      /* TODO */
      TTypeVisitor() {}

    };  // TTypeVisitor


    /* TODO */
    class TTypeDoubleVisitor {
      NO_COPY_SEMANTICS(TTypeDoubleVisitor);
      public:

      /* TODO */
      virtual ~TTypeDoubleVisitor() {}

      /* double visitor */
      virtual void operator()(const Type::TInt8 &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TInt8 &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TInt16 &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TInt16 &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TInt32 &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TInt32 &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TInt64 &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TInt64 &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TUInt8 &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt8 &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TUInt16 &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt16 &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TUInt32 &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt32 &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TUInt64 &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TUInt64 &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TBool &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TBool &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TChar &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TChar &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TFloat &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TFloat &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TDouble &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TDouble &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TDuration &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TDuration &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TTimePoint &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TTimePoint &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TUuid &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TUuid &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TBlob &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TBlob &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TStr &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TStr &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TTombstone &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TTombstone &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TVoid &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TVoid &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TDesc &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TDesc &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TFree &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TFree &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TOpt &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TOpt &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TSet &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TSet &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TVector &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TVector &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TMap &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TMap &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TRecord &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TRecord &lhs, const Type::TTuple &rhs    ) const = 0;

      virtual void operator()(const Type::TTuple &lhs, const Type::TInt8 &rhs     ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TInt16 &rhs    ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TInt32 &rhs    ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TInt64 &rhs    ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TUInt8 &rhs    ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TUInt16 &rhs   ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TUInt32 &rhs   ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TUInt64 &rhs   ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TBool &rhs     ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TChar &rhs     ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TFloat &rhs    ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TDouble &rhs   ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TDuration &rhs ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TTimePoint &rhs) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TUuid &rhs     ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TBlob &rhs     ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TStr &rhs      ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TTombstone &rhs) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TVoid &rhs     ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TDesc &rhs     ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TFree &rhs     ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TOpt &rhs      ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TSet &rhs      ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TVector &rhs   ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TMap &rhs      ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TRecord &rhs   ) const = 0;
      virtual void operator()(const Type::TTuple &lhs, const Type::TTuple &rhs    ) const = 0;

      protected:

      /* TODO */
      TTypeDoubleVisitor() {}

    };  // TTypeDoubleVisitor

    void AcceptDouble(const Type::TAny &lhs, const Type::TAny &rhs, const TTypeDoubleVisitor &double_visitor);

  }  // Sabot

}  // Stig
