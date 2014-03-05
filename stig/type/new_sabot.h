/* <stig/type/new_sabot.h>

   Given a var type, get a type sabot.  The sabot is interned, so you don't have to delete it.

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

#include <stdexcept>

#include <stig/native/state.h>
#include <stig/sabot/type.h>
#include <stig/type.h>

namespace Stig {

  namespace Type {

    /* Thrown by GetSabot() when the type has no translation in the sabot system. */
    class TTypeTranslationError
        : public std::logic_error {
      public:

      /* Do-little. */
      TTypeTranslationError();

    };  // TTypeTranslationError

    /* Given a var, get a type sabot.  The sabot is interned, so you don't have to delete it.
       If the type has no translation into the sabot system, this function throws. */
    Sabot::Type::TAny *NewSabot(void *buf, const TType &type);

    /* Given a var, get a type sabot.  The sabot is interned, so you don't have to delete it.
       If the type has no translation into the sabot system, this function returns null. */
    Sabot::Type::TAny *TryNewSabot(void *buf, const TType &type);

    namespace ST {

      /* The template for sabots of unary types.
         TBase must refer to a vistitable descendant of Sabot::Type::TUnary. */
      template <typename TBase>
      class TUnary final
          : public TBase {
        public:

        /* Our version of the pin required by Sabot::Type::TUnary. */
        class TPin final
            : public TBase::TPin {
          public:

          /* Cache the element type. */
          TPin(const TType &elem)
              : Elem(elem) {}

          /* See Sabot::Type::TUnary. */
          virtual Sabot::Type::TAny *NewElem(void *buf) const override {
            assert(this);
            return NewSabot(buf, Elem);
          }

          private:

          /* Our element type. */
          TType Elem;

        };  // TUnary<TBase>::TPin

        /* TODO */
        TUnary(const TType &elem)
            : Elem(elem) {}

        /* Pin the array into memory. */
        virtual typename TBase::TPin *Pin(void *buf) const override {
          return new (buf) TPin(Elem);
        }

        private:

        /* Our element type. */
        TType Elem;

      };  // TUnary<TBase>

      /* Definitions of our unary leaves. */
      using TVector = TUnary<Sabot::Type::TVector>;
      using TOpt = TUnary<Sabot::Type::TOpt>;
      using TSet = TUnary<Sabot::Type::TSet>;
      using TFree = TUnary<Sabot::Type::TFree>;
      using TDesc = TUnary<Sabot::Type::TDesc>;

      /* The template for sabots of binary types.
         TBase must refer to a vistitable descendant of Sabot::Type::TBinary. */
      template <typename TBase>
      class TBinary final
          : public TBase {
        public:

        /* Our version of the pin required by Sabot::Type::TBinary. */
        class TPin final
            : public TBase::TPin {
          public:

          /* Cache the element type. */
          TPin(const TType &lhs, const TType &rhs)
              : Lhs(lhs), Rhs(rhs) {}

          /* See Sabot::Type::TBinary. */
          virtual Sabot::Type::TAny *NewLhs(void *buf) const override {
            assert(this);
            return NewSabot(buf, Lhs);
          }

          /* See Sabot::Type::TBinary. */
          virtual Sabot::Type::TAny *NewRhs(void *buf) const override {
            assert(this);
            return NewSabot(buf, Rhs);
          }

          private:

          /* Our element types. */
          TType Lhs, Rhs;

        };  // TBinary<TBase>::TPin

        /* TODO */
        TBinary(const TType &lhs, const TType &rhs)
            : Lhs(lhs), Rhs(rhs) {}

        /* Pin the array into memory. */
        virtual typename TBase::TPin *Pin(void *buf) const override {
          return new (buf) TPin(Lhs, Rhs);
        }

        private:

        /* Our element types. */
        TType Lhs, Rhs;

      };  // TBinary<TBase>

      /* Definitions of our binary leaves. */
      using TMap = TBinary<Sabot::Type::TMap>;

      /* Sabot for tuple types. */
      class TTuple final
          : public Sabot::Type::TTuple {
        public:

        /* Our version of the pin required by Sabot::Type::TTuple. */
        class TPin final
            : public Sabot::Type::TTuple::TPin {
          public:

          /* Cache the element type. */
          TPin(const TTuple *tuple)
              : Sabot::Type::TTuple::TPin(tuple), Tuple(tuple) {
            assert(tuple);
          }

          /* See Sabot::Type::TTuple. */
          virtual Sabot::Type::TAny *NewElem(size_t elem_idx, void *buf) const override {
            assert(this);
            const auto &field = Tuple->Addr->GetElems().at(elem_idx);
            const auto &elem = field.second;
            return (field.first != TAddrDir::Desc) ? NewSabot(buf, elem) : new (buf) TDesc(elem);
          }

          private:

          /* The tuple sabot we pin. */
          const TTuple *Tuple;

        };  // TTuple::TPin

        /* Cache the pointer to the address type we represent.
           This is safe because the type objects are interned. */
        TTuple(const TAddr *addr)
            : Addr(addr) {
          assert(addr);
        }

        /* See Sabot::Type::TNAry. */
        virtual size_t GetElemCount() const override {
          assert(this);
          return Addr->GetElems().size();
        }

        /* See Sabot::Type::TTuple. */
        virtual Sabot::Type::TTuple::TPin *Pin(void *buf) const override {
          return new (buf) TPin(this);
        }

        private:

        /* Points to the address type we represent. */
        const TAddr *Addr;

      };  // TTuple

      /* Sabot for record types. */
      class TRecord final
          : public Sabot::Type::TRecord {
        public:

        /* Our version of the pin required by Sabot::Type::TRecord. */
        class TPin final
            : public Sabot::Type::TRecord::TPin {
          public:

          /* Cache the element type. */
          TPin(const TRecord *record)
              : Sabot::Type::TRecord::TPin(record), Record(record) {
            assert(record);
          }

          /* See Sabot::Type::TRecord. */
          virtual Sabot::Type::TAny *NewElem(size_t elem_idx, std::string &name, void *buf) const override {
            assert(this);
            assert(&name);
            const auto &item = Record->Elems[elem_idx];
            name = item.first;
            return NewSabot(buf, item.second);
          }

          /* See Sabot::Type::TRecord. */
          virtual Sabot::Type::TAny *NewElem(size_t elem_idx, void *&out_field_name_state, void *field_name_state_alloc, void *buf) const override {
            assert(this);
            const auto &item = Record->Elems[elem_idx];
            out_field_name_state = Native::State::New(item.first, field_name_state_alloc);
            return NewSabot(buf, item.second);
          }

          /* See Sabot::Type::TRecord. */
          virtual Sabot::Type::TAny *NewElem(size_t elem_idx, void *buf) const override {
            assert(this);
            const auto &item = Record->Elems[elem_idx];
            return NewSabot(buf, item.second);
          }

          private:

          /* The record sabot we pin. */
          const TRecord *Record;

        };  // TRecord::TPin

        /* Cache the pointer to the object type we represent.
           This is safe because the type objects are interned. */
        TRecord(const TObj *addr) {
          assert(addr);
          for (const auto &item: addr->GetElems()) {
            Elems.push_back(item);
          }
          std::sort(Elems.begin(), Elems.end());
        }

        /* See Sabot::Type::TNAry. */
        virtual size_t GetElemCount() const override {
          assert(this);
          return Elems.size();
        }

        /* See Sabot::Type::TRecord. */
        virtual Sabot::Type::TRecord::TPin *Pin(void *buf) const override {
          return new (buf) TPin(this);
        }

        private:

        /* Points to the object type we represent. */
        std::vector<std::pair<std::string, Type::TType>> Elems;

      };  // TRecord

    }  // ST

  }  // Type

}  // Stig
