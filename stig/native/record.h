/* <stig/native/record.h>

   Sabot type and state for native record types.

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

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <mutex>

#include <base/no_construction.h>
#include <base/unique_token.h>
#include <stig/native/state.h>
#include <stig/native/type.h>
#include <stig/sabot/to_native.h>

#include <server/daemonize.h>

/* Declare a record element. */
#define RECORD_ELEM(record_t, val_t, name)  \
  RECORD_ELEM_PASTER(record_t, val_t, name, UNIQUE_TOKEN(StigNativeRecordElem));

/* Declare a record element with a different field name than object type name. */
#define RECORD_ELEM_WITH_FIELD_NAME(record_t, val_t, field_name, name)  \
  RECORD_ELEM_PASTER_WITH_FIELD_NAME(record_t, val_t, field_name, name, UNIQUE_TOKEN(StigNativeRecordElem));

/* Pastes a unique name into the record element declaration.  Don't use this directly. */
#define RECORD_ELEM_PASTER(record_t, val_t, name, unique_name)  \
  static constexpr char unique_name[] = #name;  \
  template class ::Stig::Native::Record<record_t>::TElem<val_t, unique_name, &record_t::name>;

  /* Pastes a unique name into the record element declaration.  Don't use this directly. */
#define RECORD_ELEM_PASTER_WITH_FIELD_NAME(record_t, val_t, field_name, name, unique_name)  \
  static constexpr char unique_name[] = #name;  \
  template class ::Stig::Native::Record<record_t>::TElem<val_t, unique_name, &record_t::field_name>;

namespace Stig {

  namespace Native {

    /* Using a class as a finite, generic namespace with private members. */
    template <typename TRec>
    class Record {
      NO_CONSTRUCTION(Record);
      public:

      /* Convenience. */
      using TAnyState = Sabot::State::TAny;
      using TAnyType = Sabot::Type::TAny;
      using TSabotElem = std::pair<const char *, const Sabot::Type::TAny *>;

      /* Provides access to an element. */
      static Type::TAny *NewElem(size_t elem_idx, std::string &field_name, void *type_alloc) {
        if (elem_idx >= ElemCount) {
          throw Sabot::TIdxTooBig();
        }
        TAnyElem::CollectElems();
        field_name = std::string(Elems[elem_idx]->GetName());
        return Elems[elem_idx]->ConsType(type_alloc);
      }

      /* Provides access to an element. */
      static Type::TAny *NewElem(size_t elem_idx, void *&out_field_name_state, void *field_name_state_alloc, void *type_alloc) {
        if (elem_idx >= ElemCount) {
          throw Sabot::TIdxTooBig();
        }
        TAnyElem::CollectElems();
        out_field_name_state = Native::State::New(Elems[elem_idx]->GetName(), field_name_state_alloc);
        return Elems[elem_idx]->ConsType(type_alloc);
      }

      /* Provides access to an element. */
      static Type::TAny *NewElem(size_t elem_idx, void *type_alloc) {
        if (elem_idx >= ElemCount) {
          throw Sabot::TIdxTooBig();
        }
        TAnyElem::CollectElems();
        return Elems[elem_idx]->ConsType(type_alloc);
      }

      /* The number of elements in the record. */
      static size_t GetElemCount() {
        return ElemCount;
      }

      /* Construct a new state sabot around the given record. */
      static TAnyState *New(const TRec &rec, void *state_alloc) {
        return new (state_alloc) TState(rec);
      }

      /* The base class for all elements of the record. */
      class TAnyElem {
        public:

        /* Do-little. */
        virtual ~TAnyElem() {}

        /* Override to get the name of the element. */
        virtual const char *GetName() const = 0;

        /* TODO */
        virtual Type::TAny *ConsType(void *type_alloc) const = 0;

        /* Override to construct a new state sabot for this emement in the given record. */
        virtual TAnyState *NewStateSabot(const TRec &rec, void *state_alloc) const = 0;

        /* TODO */
        virtual void SetVal(TRec &rec, const TAnyState &state) const = 0;

        /* Collect all the elements of the record type into a sorted array.
           If we've already done so for this record type, then this function does nothing. */
        static void CollectElems() {
          std::lock_guard<std::mutex> lock(Mutex);
          if (!Elems) {
            /* Copy the pointers to the elements from the linked list into an array. */
            Elems = new TAnyElem*[ElemCount];
            TAnyElem
                **csr   = Elems,
                **limit = Elems + ElemCount;
            for (auto elem = FirstElem; elem; elem = elem->NextElem) {
              assert(csr < limit);
              *csr++ = elem;
            }
            assert(csr == limit);
            /* Sort the array by element name. */
            std::sort(
                Elems, limit,
                [](const TAnyElem *lhs, const TAnyElem *rhs) {
                  assert(lhs);
                  assert(rhs);
                  return strcmp(lhs->GetName(), rhs->GetName()) < 0;
                }
            );
          }
        }

        protected:

        /* Link into the list of elements. */
        TAnyElem() {
          NextElem = FirstElem;
          FirstElem = this;
          ++ElemCount;
        }

        private:

        /* The next element in this record, if any.  See FirstElem, below. */
        TAnyElem *NextElem;

        /* The first element in this record, if any.
           This leads to a singly linked list of elements.  The list is in no particular order. */
        static TAnyElem *FirstElem;

      };  // Record<TRec>::TAnyElem

      /* TODO */
      static const TAnyElem *TryGetElem(const char *name) {
        TAnyElem::CollectElems();
        for (size_t elem_idx = 0; elem_idx < ElemCount; ++elem_idx) {
          const TAnyElem *elem = Elems[elem_idx];
          if (strcmp(elem->GetName(), name) == 0) {
            return elem;
          }
        }
        return nullptr;
      }

      /* A singleton representing a particular element of our record. */
      template <typename TVal, const char *Name, TVal (TRec::*Member)>
      class TElem final
          : public TAnyElem {
        public:

        /* See TAnyElem. */
        virtual const char *GetName() const override {
          return Name;
        }

        /* See TAnyElem. */
        virtual Type::TAny *ConsType(void *type_alloc) const override {
          return Type::For<TVal>::GetType(type_alloc);
        }

        /* See TAnyElem. */
        virtual TAnyState *NewStateSabot(const TRec &rec, void *state_alloc) const override {
          return State::Factory<TVal>::New(rec.*Member, state_alloc);
        }

        /* See TAnyElem. */
        virtual void SetVal(TRec &rec, const TAnyState &state) const override {
          Sabot::ToNative(state, rec.*Member);
        }

        /* Our single instance. */
        static const TElem Elem;

        private:

        /* Do-little. */
        TElem() {}

      };  // Record<TRec>::TElem

      private:

      /* State sabot. */
      class TState final
          : public State::TStaticArrayOfSingleStates<Sabot::State::TRecord> {
        public:

        /* Do-little. */
        TState(const TRec &rec)
            : Rec(rec) {}

        /* See Sabot::State::TRecord. */
        virtual Sabot::Type::TRecord *GetRecordType(void *type_alloc) const override {
          return Type::For<TRec>::GetRecordType(type_alloc);
        }

        private:

        /* See TArrayOfSingleStates<TElems...>. */
        virtual TAny *NewElem(size_t elem_idx, void *state_alloc) const override {
          TAnyElem::CollectElems();
          return Elems[elem_idx]->NewStateSabot(Rec, state_alloc);
        }

        private:

        /* Cached reference to the record we are sabot to. */
        const TRec &Rec;

      };  // Record<TRec>::TState

      /* The number of elements in TAnyElem's linked list. */
      static size_t ElemCount;

      /* Used by CollectElems() to covers the construction of Elems and SabotElems. */
      static std::mutex Mutex;

      /* A sorted array of all the elements in our record.  This is null until the first time TAnyElem::CollectElems() is called. */
      static TAnyElem **Elems;

      /* size of TState */
      friend class Stig::Sabot::TSizeChecker;

    };  // Record<TRec>

    /* See declaration. */
    template <typename TRec>
    typename Record<TRec>::TAnyElem *Record<TRec>::TAnyElem::FirstElem = nullptr;

    /* See declaration. */
    template <typename TRec>
    size_t Record<TRec>::ElemCount = 0;

    /* See declaration. */
    template <typename TRec>
    std::mutex Record<TRec>::Mutex;

    /* See declaration. */
    template <typename TRec>
    typename Record<TRec>::TAnyElem **Record<TRec>::Elems = nullptr;

    /* See declaration. */
    template <typename TRec>
    template <typename TVal, const char *Name, TVal (TRec::*Member)>
    const Record<TRec>::TElem<TVal, Name, Member> Record<TRec>::TElem<TVal, Name, Member>::Elem;

  }  // Native

  namespace Sabot {

    /* TODO */
    template <typename TVal>
    class TToNativeVisitor final
        : public TStateVisitor {
      NO_COPY_SEMANTICS(TToNativeVisitor);
      public:

      /* TODO */
      TToNativeVisitor(TVal &record)
          : Out(record) {}

      /* Overrides. */
      virtual void operator()(const State::TFree &/*state*/) const override       { throw TNotImplemented(); }
      virtual void operator()(const State::TTombstone &/*state*/) const override  { throw TNotImplemented(); }
      virtual void operator()(const State::TVoid &/*state*/) const override       { throw TNotImplemented(); }
      virtual void operator()(const State::TInt8 &/*state*/) const override       { throw TNotImplemented(); }
      virtual void operator()(const State::TInt16 &/*state*/) const override      { throw TNotImplemented(); }
      virtual void operator()(const State::TInt32 &/*state*/) const override      { throw TNotImplemented(); }
      virtual void operator()(const State::TInt64 &/*state*/) const override      { throw TNotImplemented(); }
      virtual void operator()(const State::TUInt8 &/*state*/) const override      { throw TNotImplemented(); }
      virtual void operator()(const State::TUInt16 &/*state*/) const override     { throw TNotImplemented(); }
      virtual void operator()(const State::TUInt32 &/*state*/) const override     { throw TNotImplemented(); }
      virtual void operator()(const State::TUInt64 &/*state*/) const override     { throw TNotImplemented(); }
      virtual void operator()(const State::TBool &/*state*/) const override       { throw TNotImplemented(); }
      virtual void operator()(const State::TChar &/*state*/) const override       { throw TNotImplemented(); }
      virtual void operator()(const State::TFloat &/*state*/) const override      { throw TNotImplemented(); }
      virtual void operator()(const State::TDouble &/*state*/) const override     { throw TNotImplemented(); }
      virtual void operator()(const State::TDuration &/*state*/) const override   { throw TNotImplemented(); }
      virtual void operator()(const State::TTimePoint &/*state*/) const override  { throw TNotImplemented(); }
      virtual void operator()(const State::TUuid &/*state*/) const override       { throw TNotImplemented(); }
      virtual void operator()(const State::TBlob &/*state*/) const override       { throw TNotImplemented(); }
      virtual void operator()(const State::TStr &/*state*/) const override        { throw TNotImplemented(); }
      virtual void operator()(const State::TDesc &/*state*/) const override       { throw TNotImplemented(); }
      virtual void operator()(const State::TOpt &/*state*/) const override        { throw TNotImplemented(); }
      virtual void operator()(const State::TSet &/*state*/) const override        { throw TNotImplemented(); }
      virtual void operator()(const State::TVector &/*state*/) const override     { throw TNotImplemented(); }
      virtual void operator()(const State::TMap &/*state*/) const override        { throw TNotImplemented(); }
      virtual void operator()(const State::TTuple &/*state*/) const override      { throw TNotImplemented(); }

      virtual void operator()(const State::TRecord &state) const override {
        // get the record's type
        void *record_type_alloc = alloca(Type::GetMaxTypeSize());
        Type::TRecord::TWrapper record_type(state.GetRecordType(record_type_alloc));
        // pin the record's type
        void *record_type_pin_alloc = alloca(Type::GetMaxTypePinSize());
        Type::TRecord::TPin::TWrapper record_type_pin(record_type->Pin(record_type_pin_alloc));
        // pin the record's state
        void *record_state_pin_alloc = alloca(State::GetMaxStatePinSize());
        State::TRecord::TPin::TWrapper record_state_pin(state.Pin(record_state_pin_alloc));
        // iterate over the record sabot (type and state, in parallel)
        std::string elem_name;
        void
            *elem_type_alloc  = alloca(Sabot::Type::GetMaxTypeSize()),
            *elem_state_alloc = alloca(Sabot::State::GetMaxStateSize());
        for (size_t elem_idx = 0; elem_idx < record_type_pin->GetElemCount(); ++elem_idx) {
          Type::TAny::TWrapper elem_type(record_type_pin->NewElem(elem_idx, elem_name, elem_type_alloc));
          const auto *native_elem = Native::Record<TVal>::TryGetElem(elem_name.c_str());
          if (native_elem) {
            State::TAny::TWrapper elem_state(record_state_pin->NewElem(elem_idx, elem_state_alloc));
            native_elem->SetVal(Out, *elem_state);
          }
        }
      }

      private:

      /* The record we're filling in. */
      TVal &Out;

    };  // TToNativeVisitor

  }  // Sabot

}  // Stig
