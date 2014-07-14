/* <orly/csv_to_bin/field.h>

   Metadata to define fields in structures which can then be imported into
   from a compatible JSON object.

   Copyright 2010-2014 OrlyAtomics, Inc.

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
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <base/class_traits.h>
#include <base/demangle.h>
#include <base/json.h>
#include <base/thrower.h>
#include <base/uuid.h>

/* Used to call the helper NewField() on a member of a class.  It gives the
   field the same name as the member's identifier. */
#define NEW_FIELD(cls, member)  \
    ::Orly::CsvToBin::NewField(#member, &cls::member)

namespace Orly {

  namespace CsvToBin {

    /* Borrowing this definition from Base. */
    using TJson = Base::TJson;

    /* Thrown when a field in a JSON object is missing or of an unexpected
       and incompatible type. */
    DEFINE_ERROR(
        TJsonMismatch, std::runtime_error, "JSON object schema mismatch");

    /* TODO */
    template <typename TVal>
    struct TranslateJson {
      static bool TryAs(const TJson &that, TVal &out) {
        return that.TryAs(out);
      }
    };

    /* TODO */
    template <>
    struct TranslateJson<Base::TUuid> {
      static bool TryAs(const TJson &that, Base::TUuid &out) {
        std::string temp;
        bool success = that.TryAs(temp);
        if (success) {
          out = Base::TUuid(temp.c_str());
        }
        return success;
      }
    };

    /* The base for any field of TObj. */
    template <typename TObj>
    class TAnyField {
      NO_COPY(TAnyField);
      public:

      /* Do-little. */
      virtual ~TAnyField() {}

      /* The name of the field, which must be unique within TObj. */
      const std::string &GetName() const noexcept {
        assert(this);
        return Name;
      }

      /* Overridden by TField<TObj, TVal> to set the value of a field in
         the given instance of TObj.  The value is taken from the given JSON
         object. */
      virtual void SetVal(TObj *that, const TJson &json) const = 0;

      protected:

      /* Cache the name. */
      TAnyField(std::string &&name)
          : Name(std::move(name)) {}

      private:

      /* See accessor. */
      std::string Name;

    };  // TAnyField<TObj>

    /* A field in TObj of a particular type. */
    template <typename TObj, typename TVal>
    class TField final
        : public TAnyField<TObj> {
      public:

      /* The type of the member of TObj to which we'll refer. */
      using TMember = TVal (TObj::*);

      /* Cache the name and the member pointer. */
      TField(std::string &&name, TMember member)
          : TAnyField<TObj>(std::move(name)), Member(member) {}

      /* Set the value of a field in the given instance of TObj.  If the
         JSON object's type isn't compatible with the field, throw. */
      virtual void SetVal(TObj *that, const TJson &json) const override {
        assert(this);
        assert(that);
        assert(&json);
        if (!TranslateJson<TVal>::TryAs(json, that->*Member)) {
          THROW_ERROR(TJsonMismatch)
              << "field \"" << this->GetName() << "\" not compatible with "
              << Base::Demangle(typeid(TVal));
        }
      }

      private:

      /* The member of TObj to which we'll refer. */
      TMember Member;

    };  // TField<TObj, TVal>

    /* A helper for constructing TField<TObj, TVal> instances using type
       deduction. */
    template <typename TObj, typename TVal>
    std::unique_ptr<TField<TObj, TVal>> NewField(
        std::string &&name, TVal (TObj::*member)) {
      return std::make_unique<TField<TObj, TVal>>(std::move(name), member);
    }

    /* A collection of fields for a particular type of object. */
    template <typename TObj>
    class TFields final {
      NO_COPY(TFields);
      public:

      /* Construct from a variadic list of unique pointers to
         TField<TObj, TVal> instances. */
      template <typename... TVals>
      explicit TFields(
          std::unique_ptr<TField<TObj, TVals>> &&... field_ptrs) {
        Helper<0, TVals...>::Init(this, std::move(field_ptrs)...);
      }

      /* Add a field to the collection. */
      void AddField(std::unique_ptr<TAnyField<TObj>> &&field_ptr) {
        assert(this);
        assert(&field_ptr);
        assert(field_ptr);
        FieldPtrs.push_back(std::move(field_ptr));
      }

      /* The number of fields in this collection.  This can be zero. */
      size_t GetSize() const noexcept {
        assert(this);
        return FieldPtrs.size();
      }

      /* Set the values of all the fields in a particular TObj instances,
         taking the values from the named elements on a JSON object.  The
         JSON object must have at least the elements we require.  Any
         additional elements we ignore. */
      void SetVals(TObj &that, const TJson &json) const {
        assert(this);
        assert(&that);
        assert(&json);
        for (const auto &field_ptr: FieldPtrs) {
          const std::string &name = field_ptr->GetName();
          const TJson *sub_json = json.TryFind(name);
          if (!sub_json) {
            THROW_ERROR(TJsonMismatch)
                << "field \"" << name << "\" is missing";
          }
          field_ptr->SetVal(&that, *sub_json);
        }
      }

      private:

      /* This template helps during construction to pick apart the variadic
         argument list. */
      template <int Dummy, typename... TVals>
      struct Helper;

      /* Our fields in the order given to the constructor. */
      std::vector<std::unique_ptr<TAnyField<TObj>>> FieldPtrs;

    };  // TFields<TObj>

    /* Base case.  We do nothing. */
    template <typename TObj>
    template <int Dummy>
    struct TFields<TObj>::Helper<Dummy> {
      static void Init(TFields<TObj> *) {}
    };  // TFields<TObj>::THelper<Dummy>

    /* Recursive case.  We append the next argument to the collection and
       recurse. */
    template <typename TObj>
    template <int Dummy, typename TVal, typename... TMoreVals>
    struct TFields<TObj>::Helper<Dummy, TVal, TMoreVals...> {
      static void Init(
          TFields<TObj> *that,
          std::unique_ptr<TField<TObj, TVal>> &&field_ptr,
          std::unique_ptr<TField<TObj, TMoreVals>> &&... more_field_ptrs) {
        that->AddField(std::move(field_ptr));
        Helper<Dummy, TMoreVals...>::Init(
            that, std::move(more_field_ptrs)...);
      }
    };  // TFields<TObj>::THelper<Dummy, TVal, TMoreVals...>

  }  // Csv2Bin

}  // Orly
