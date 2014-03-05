/* <stig/type/obj.h>

   TODO

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

#include <map>

#include <base/no_construction.h>
#include <stig/type/managed_type.h>
#include <stig/type/has_optional.h>

namespace Stig {

  namespace Var {

    /* TODO */
    class TVar;

  }  // Var

  namespace Type {

    /* NOTE: This is ordered because it makes comparing elem maps easiser, and it makes it so we can rely on having
             elements come out in a stable, asciibetical order. */
    typedef std::map<std::string, TType> TObjElems;

    /* TODO */
    class TObj : public TInternedType<TObj, TObjElems> {
      NO_COPY_SEMANTICS(TObj);
      public:

      typedef TObjElems TElems;

      /* TODO */
      template <typename TCompound_>
      class Meta {
        NO_CONSTRUCTION(Meta);
        public:

        typedef TCompound_ TCompound;

        /* TODO */
        class TAnyField {
          NO_COPY_SEMANTICS(TAnyField);
          public:

          /* TODO */
          virtual ~TAnyField() {}

          /* TODO */
          const std::string &GetName() const {
            assert(this);
            return Name;
          }

          /* TODO */
          const TAnyField *GetNextField() const {
            assert(this);
            return NextField;
          }

          /* TODO */
          virtual Type::TType GetType() const = 0;

          /* TODO */
          virtual void GetVal(const TCompound &that, Var::TVar &out) const = 0;

          /* TODO */
          static const TAnyField *GetFirstField() {
            return FirstField;
          }

          protected:

          /* TODO */
          TAnyField(const char *name)
              : Name(name), NextField(0) {
            assert(name);
            (LastField ? LastField->NextField : FirstField) = this;
            LastField = this;
          }

          private:

          /* TODO */
          std::string Name;

          /* TODO */
          TAnyField *NextField;

          /* TODO */
          static TAnyField *FirstField, *LastField;

        };  // TAnyField

        /* TODO */
        template <typename TVal>
        class TField
            : public TAnyField {
          NO_COPY_SEMANTICS(TField);
          public:

          /* TODO */
          typedef TVal (TCompound::*TMember);

          /* TODO */
          TField(const char *name, TMember member)
              : TAnyField(name), Member(member) {}

          /* TODO */
          virtual Type::TType GetType() const {
            assert(this);
            return Type::TDt<TVal>::GetType();
          }

          /* TODO */
          virtual void GetVal(const TCompound &that, Var::TVar &out) const;

          private:

          /* TODO */
          TMember Member;

        };  // TField<TVal>

        /* TODO */
        class TClass {
          NO_COPY_SEMANTICS(TClass);
          public:

          /* TODO */
          TClass(const char *name)
              : Name(name) {
            assert(!Class);
            Class = this;
          }

          /* TODO */
          ~TClass() {
            assert(this);
            assert(Class == this);
            Class = 0;
          }

          /* TODO */
          const std::string &GetName() const {
            assert(this);
            return Name;
          }

          /* TODO */
          static const TClass *TryGetClass() {
            return Class;
          }

          private:

          /* TODO */
          std::string Name;

          /* TODO */
          static TClass *Class;

        };  // TClass

      };  // Meta<TCompound>


      /* NOTE. We can make this more efficient but this is clearer as to what we're doing */
      static bool AreComparable(const TObj *lhs, const TObj *rhs, bool &is_optional) {
        assert(lhs);
        assert(rhs);
        assert(&is_optional);
        return lhs->IsSubsetOf(rhs, is_optional) || rhs->IsSubsetOf(lhs, is_optional);
      }

      /* TODO */
      const TObjElems &GetElems() const {
        assert(this);
        return std::get<0>(GetKey());
      }

      /* TODO */
      static TType Get(const TObjElems &elems) {
        return TInternedType::Get(elems);
      }

      /* TODO */
      bool IsSubsetOf(const TObj *that, bool &is_optional) const {
        assert(this);
        assert(that);
        assert(&is_optional);
        for (auto iter : GetElems()) {
          auto pos = that->GetElems().find(iter.first);
          if (pos == that->GetElems().end() || iter.second != pos->second) {
            return false;
          }

          //TODO: Rename HasOptional to ContainsOptional
          if (iter.second.Is<TOpt>() || Type::HasOptional(iter.second)) {
            is_optional = true;
          }
        }
        return true;
      }

      private:
      TObj(const TObjElems &elems) : TInternedType(elems) {}
      virtual ~TObj();

      virtual void Write(std::ostream &) const;

      friend class TInternedType;
    };  // TObj

    /* TODO */
    template <typename TCompound>
    typename TObj::Meta<TCompound>::TAnyField *TObj::Meta<TCompound>::TAnyField::FirstField = 0;

    /* TODO */
    template <typename TCompound>
    typename TObj::Meta<TCompound>::TAnyField *TObj::Meta<TCompound>::TAnyField::LastField = 0;

    /* TODO */
    template <typename TCompound>
    typename TObj::Meta<TCompound>::TClass *TObj::Meta<TCompound>::TClass::Class = 0;

  }  // Type

}  // Stig