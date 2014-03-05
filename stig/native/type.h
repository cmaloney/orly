/* <stig/native/type.h>

   Sabot types for native non-record types.

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
#include <functional>
#include <mutex>

#include <base/no_construction.h>
#include <stig/sabot/type.h>
#include <stig/native/defs.h>

namespace Stig {

  namespace Sabot {

    /* Forward Declarations. */
    class TSizeChecker;

  }  // Sabot

  namespace Native {

    /* See <stig/native/record.h> for the definition of this template. */
    template <typename TRecord>
    class Record;

    /* Using a class as a finite namespace with private members. */
    class Type {
      NO_CONSTRUCTION(Type);
      public:

      /* For a native type, you can look up its native sabot type object in this template,
         like this:

           Stig::Native::For<int>::Type.Accept(my_visitor);
           Stig::Native::For<std::vector<int>>::Type.Accept(my_visitor);
           Stig::Native::For<std::tuple<bool, int, double>>::Type.Accept(my_visitor);

         The definitions of the type object classes (except for the singletons taken from Sabot::Type)
         are declared in this scope, but are private.  You shouldn't try to construct them yourself. */
      template <typename TVal>
      class For;

      /* Convenience. */
      using TAny = Sabot::Type::TAny;

      private:

      /* Used by TTuple (below) to walk template elements.
         Explicit specializations exist for non-empty and empty tuples. */
      template <typename... TElems>
      class ForTuple;

      /* Callback used by ForTuple<TElems...>::ForEachElem(). */
      using TCb = std::function<bool (const TAny &elem)>;

      /* Used to define the type objects for unary types.  TBase must refer to the Sabot::Type class
         which we're extending and TElem must refer to the native element type we represent.  This
         class is a singleton. */
      template <typename TBase, typename TElem>
      class TUnary
          : public TBase {
        public:

        /* TODO */
        class TPin : public TBase::TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          TPin() {}

          /* Override to construct a new type sabot. */
          virtual TAny *NewElem(void *type_alloc) const override {
            return For<TElem>::GetType(type_alloc);
          }

        };  // TPin

        /* TODO */
        virtual typename TBase::TPin *Pin(void *pin_alloc) const {
          return new (pin_alloc) TPin();
        }

        /* TODO */
        virtual TAny *GetType(void *type_alloc) const = 0;

        protected:

        /* Do-little. */
        TUnary() {}

      };  // TUnary<TBase, TElem>

      #define FOR_UNARY(name)  \
        template <typename TVal>  \
        class T##name final : public TUnary<Sabot::Type::T##name, TVal> {  \
          public:  \
          Sabot::Type::TAny *GetType(void *type_alloc) const { \
            return new (type_alloc) T##name(); \
          } \
        };
        FOR_UNARY(Desc)
        FOR_UNARY(Free)
        FOR_UNARY(Opt)
        FOR_UNARY(Set)
        FOR_UNARY(Vector)
      #undef FOR_UNARY

      /* Used to define the type objects for binary types.  TBase must refer to the Sabot::Type class
         which we're extending and TLhs and TRhs must refer to the native element types we represent.
         This class is a singleton. */
      template <typename TBase, typename TLhs, typename TRhs>
      class TBinary
          : public TBase {
        public:

        /* TODO */
        class TPin : public TBase::TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          TPin() {}

          /* See Sabot::Type::TBinary. */
          virtual TAny *NewLhs(void *type_alloc) const override {
            return For<TLhs>::GetType(type_alloc);
          }

          /* See Sabot::Type::TBinary. */
          virtual TAny *NewRhs(void *type_alloc) const override {
            return For<TRhs>::GetType(type_alloc);
          }

        };  // TPin

        /* TODO */
        virtual typename TBase::TPin *Pin(void *pin_alloc) const {
          return new (pin_alloc) TPin();
        }

        /* TODO */
        virtual TAny *GetType(void *type_alloc) const = 0;

        protected:

        /* Do-little. */
        TBinary() {}

      };  // TBinary<TBase, TElem>

      #define FOR_BINARY(name)  \
        template <typename TLhs, typename TRhs>  \
        class T##name final : public TBinary<Sabot::Type::T##name, TLhs, TRhs> {  \
          public:  \
          Sabot::Type::TAny *GetType(void *type_alloc) const { \
            return new (type_alloc) T##name(); \
          } \
        };
        FOR_BINARY(Map)
      #undef FOR_BINARY

      /* Used to define the type objects for record types.  This is a singleton. */
      template <typename TVal>
      class TRecord final
          : public Sabot::Type::TRecord {
        public:

        /* TODO */
        class TPin : public Sabot::Type::TRecord::TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          TPin(const TRecord *record) : Sabot::Type::TRecord::TPin(record) {}

          /* See Sabot::Type::TRecord. */
          virtual TAny *NewElem(size_t elem_idx, std::string &field_name, void *type_alloc) const override {
            return Record<TVal>::NewElem(elem_idx, field_name, type_alloc);
          }

          /* See Sabot::Type::TRecord. */
          virtual TAny *NewElem(size_t elem_idx, void *&out_field_name_sabot_state, void *field_name_state_alloc, void *type_alloc) const override {
            return Record<TVal>::NewElem(elem_idx, out_field_name_sabot_state, field_name_state_alloc, type_alloc);
          }

          /* See Sabot::Type::TRecord. */
          virtual TAny *NewElem(size_t elem_idx, void *type_alloc) const override {
            return Record<TVal>::NewElem(elem_idx, type_alloc);
          }

        };  // TPin

        /* See Sabot::Type::TNAry. */
        virtual size_t GetElemCount() const override {
          return Record<TVal>::GetElemCount();
        }

        /* TODO */
        virtual Sabot::Type::TRecord::TPin *Pin(void *pin_alloc) const {
          return new (pin_alloc) TPin(this);
        }

        /* TODO */
        virtual TAny *GetType(void *type_alloc) const {
          return new (type_alloc) TRecord();
        }

        /* Do-little. */
        TRecord() {}

      };  // TRecord<TVal>

      /* TODO */
      template <typename... TElems>
      class TupleUnroller;

      /* Used to define the type objects for tuple types.  This is a singleton. */
      template <typename... TElems>
      class TTuple final
          : public Sabot::Type::TTuple {
        public:

        /* TODO */
        class TPin : public Sabot::Type::TTuple::TPin {
          NO_COPY_SEMANTICS(TPin);
          public:

          /* TODO */
          TPin(const TTuple *tuple) : Sabot::Type::TTuple::TPin(tuple) {}

          /* See Sabot::Type::TTuple. */
          virtual TAny *NewElem(size_t elem_idx, void *type_alloc) const override {
            assert(this);
            return TupleUnroller<TElems...>::GetElem(elem_idx, type_alloc);
          }

        };  // TPin

        /* See Sabot::Type::TNAry. */
        virtual size_t GetElemCount() const override {
          return std::tuple_size<std::tuple<TElems...>>::value;
        }

        /* TODO */
        virtual Sabot::Type::TTuple::TPin *Pin(void *pin_alloc) const {
          return new (pin_alloc) TPin(this);
        }

        /* TODO */
        virtual TAny *GetType(void *type_alloc) const {
          return new (type_alloc) TTuple();
        }

        /* Do-little. */
        TTuple() {}

      };  // TTuple<TElems...>

      /* size of our types */
      friend class Stig::Sabot::TSizeChecker;

    };  // Type

    /* TODO */
    template <typename TElem, typename... TElems>
    class Type::TupleUnroller<TElem, TElems...> {
      NO_CONSTRUCTION(TupleUnroller);
      public:

      /* TODO */
      static Type::TAny *GetElem(size_t elem_idx, void *type_alloc) {
        return elem_idx ? TupleUnroller<TElems...>::GetElem(elem_idx - 1, type_alloc) : For<TElem>::GetType(type_alloc);
      }

    };  // Type::TupleUnroller

    /* TODO */
    template <>
    class Type::TupleUnroller<> {
      NO_CONSTRUCTION(TupleUnroller);
      public:

      /* TODO */
      static Type::TAny *GetElem(size_t /*elem_idx*/, void */*type_alloc*/) {
        throw Sabot::TIdxTooBig();
      }

    };  // Type::TupleUnroller

    /* The generic version of this template punts the type defintion work to Record<TVal>.  There are
       explicit specializations for all the non-record types so the system assumes that any type it's
       never heard of before must be a record. */
    template <typename TVal>
    class Type::For final {
      NO_CONSTRUCTION(For);
      public:

      /* See definition, below. */
      static Type::TAny *GetType(void *type_alloc) {
        return new (type_alloc) TRecord<TVal>();
      }

      /* TODO */
      static Type::TRecord<TVal> *GetRecordType(void *type_alloc) {
        return new (type_alloc) TRecord<TVal>();
      }

    };  // For<TVal>

    /* Explicit specializations for nullary types. */
    /*
    static const TAny &Type;  \
    static const Sabot::Type::T##name &name##Type;  \
    */
    #define FOR_NULLARY(val_t, name)  \
      template <>  \
      class Type::For<val_t> final {  \
        NO_CONSTRUCTION(For);  \
        public:  \
        static Sabot::Type::T##name *GetType(void *type_alloc) { \
          return new (type_alloc) Sabot::Type::T##name(); \
        } \
      };
      FOR_NULLARY(int8_t, Int8)
      FOR_NULLARY(int16_t, Int16)
      FOR_NULLARY(int32_t, Int32)
      FOR_NULLARY(int64_t, Int64)
      FOR_NULLARY(uint8_t, UInt8)
      FOR_NULLARY(uint16_t, UInt16)
      FOR_NULLARY(uint32_t, UInt32)
      FOR_NULLARY(uint64_t, UInt64)
      FOR_NULLARY(bool, Bool)
      FOR_NULLARY(char, Char)
      FOR_NULLARY(float, Float)
      FOR_NULLARY(double, Double)
      FOR_NULLARY(Sabot::TStdDuration, Duration)
      FOR_NULLARY(Sabot::TStdTimePoint, TimePoint)
      FOR_NULLARY(Base::TUuid, Uuid)
      FOR_NULLARY(Native::TBlob, Blob)
      FOR_NULLARY(std::string, Str)
      FOR_NULLARY(const char *, Str)
      FOR_NULLARY(Native::TTombstone, Tombstone)
      FOR_NULLARY(void, Void)
    #undef FOR_NULLARY

    /* Explicit specialization for char[n].
       This is useful because string literals are of this type. */
    template <size_t Size>
    class Type::For<char[Size]> final {
      NO_CONSTRUCTION(For);
      public:

      /* See definition, below. */
      static Type::TAny *GetType(void *type_alloc) {
        return new (type_alloc) Sabot::Type::TStr();
      }

    };  // Type::For<char[Size]>

    /* Explicit specialization for TDesc<TElem>. */
    template <typename TElem>
    class Type::For<TDesc<TElem>> final {
      public:

      /* See definition, below. */
      static Type::TAny *GetType(void *type_alloc) {
        return new (type_alloc) TDesc<TElem>();
      }

      /* TODO */
      static Type::TDesc<TElem> *GetDescType(void *type_alloc) {
        return new (type_alloc) TDesc<TElem>();
      }

    };  // Type::For<TDesc<TElem>>

    /* Explicit specialization for TFree<TElem>. */
    template <typename TElem>
    class Type::For<TFree<TElem>> final {
      public:

      /* See definition, below. */
      static Type::TAny *GetType(void *type_alloc) {
        return new (type_alloc) TFree<TElem>();
      }

      /* TODO */
      static Type::TFree<TElem> *GetFreeType(void *type_alloc) {
        return new (type_alloc) TFree<TElem>();
      }

    };  // Type::For<TFree<TElem>>

    /* Explicit specialization for Base::TOpt<TElem>. */
    template <typename TElem>
    class Type::For<Base::TOpt<TElem>> final {
      public:

      /* See definition, below. */
      static Type::TAny *GetType(void *type_alloc) {
        return new (type_alloc) TOpt<TElem>();
      }

      /* TODO */
      static Type::TOpt<TElem> *GetOptType(void *type_alloc) {
        return new (type_alloc) TOpt<TElem>();
      }

    };  // Type::For<Base::TOpt<TElem>>

    /* Explicit specialization for std::set<TElem>. */
    template <typename TElem>
    class Type::For<std::set<TElem>> final {
      public:

      /* See definition, below. */
      static Type::TAny *GetType(void *type_alloc) {
        return new (type_alloc) TSet<TElem>();
      }

      /* TODO */
      static Type::TSet<TElem> *GetSetType(void *type_alloc) {
        return new (type_alloc) TSet<TElem>();
      }

    };  // Type::For<std::set<TElem>>

    /* Explicit specialization for std::set<TElem, TCompare>. */
    template <typename TElem, typename TCompare>
    class Type::For<std::set<TElem, TCompare>> final {
      public:

      /* See definition, below. */
      static Type::TAny *GetType(void *type_alloc) {
        return new (type_alloc) TSet<TElem>();
      }

      /* TODO */
      static Type::TSet<TElem> *GetSetType(void *type_alloc) {
        return new (type_alloc) TSet<TElem>();
      }

    };  // Type::For<std::set<TElem, TCompare>>

    /* Explicit specialization for std::vector<TElem>. */
    template <typename TElem>
    class Type::For<std::vector<TElem>> final {
      public:

      /* See definition, below. */
      static Type::TAny *GetType(void *type_alloc) {
        return new (type_alloc) TVector<TElem>();
      }

      /* TODO */
      static Type::TVector<TElem> *GetVectorType(void *type_alloc) {
        return new (type_alloc) TVector<TElem>();
      }

    };  // Type::For<std::vector<TElem>>

    /* Explicit specialization for std::map<TLhs, TRhs>. */
    template <typename TLhs, typename TRhs>
    class Type::For<std::map<TLhs, TRhs>> final {
      public:

      /* See definition, below. */
      static Type::TAny *GetType(void *type_alloc) {
        return new (type_alloc) TMap<TLhs, TRhs>();
      }

      /* TODO */
      static Type::TMap<TLhs, TRhs> *GetMapType(void *type_alloc) {
        return new (type_alloc) TMap<TLhs, TRhs>();
      }

    };  // Type::For<std::map<TLhs, TRhs>>

    /* Explicit specialization for std::map<TLhs, TRhs, TCompare>. */
    template <typename TLhs, typename TRhs, typename TCompare>
    class Type::For<std::map<TLhs, TRhs, TCompare>> final {
      public:

      /* See definition, below. */
      static Type::TAny *GetType(void *type_alloc) {
        return new (type_alloc) TMap<TLhs, TRhs>();
      }

      /* TODO */
      static Type::TMap<TLhs, TRhs> *GetMapType(void *type_alloc) {
        return new (type_alloc) TMap<TLhs, TRhs>();
      }

    };  // Type::For<std::map<TLhs, TRhs, TCompare>>

    /* Explicit specialization for std::tuple<TElems...>. */
    template <typename... TElems>
    class Type::For<std::tuple<TElems...>> final {
      public:

      /* TODO */
      static Sabot::Type::TTuple *GetType(void *type_alloc) {
        return new (type_alloc) TTuple<TElems...>();
      }

    };  // Type::For<std::tuple<TElems...>>

    /* Explicit specialization for walking a non-empty tuple. */
    template <typename TElem, typename... TMoreElems>
    class Type::ForTuple<TElem, TMoreElems...> final {
      NO_CONSTRUCTION(ForTuple);
      public:

      /* Call back for the TElem type object, and, if the callback returns true, recurse over TMoreElems. */
      static bool ForEachElem(const TCb &cb) {
        return cb(For<TElem>::Type) && ForTuple<TMoreElems...>::ForEachElem(cb);
      }

    };  // Type::ForTuple<TElem, TMoreElems...>

    /* Explicit specialization for walking an empty tuple. */
    template <>
    class Type::ForTuple<> final {
      NO_CONSTRUCTION(ForTuple);
      public:

      /* Terminates the recursion successfully. */
      static bool ForEachElem(const TCb &) {
        return true;
      }

    };  // Type::ForTuple<>

  }  // Native

}  // Stig
