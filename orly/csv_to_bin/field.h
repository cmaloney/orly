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

#include <base/chrono.h>
#include <base/class_traits.h>
#include <base/demangle.h>
#include <base/json.h>
#include <base/opt.h>
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

    /* Forward-declaration for the benefit of TObj::GetFields(). */
    class TAnyFields;

    /* The base for objects which you want to translate from JSON. */
    class TObj {
      public:

      /* Do-little. */
      virtual ~TObj() {}

      /* Override to return the metadata describing the fields of this
         object. */
      virtual const TAnyFields &GetFields() const = 0;

      protected:

      /* Do-little. */
      TObj() {}

    };  // TObj

    /* The base for TFields<TSomeObj>, below. */
    class TAnyFields {
      NO_COPY(TAnyFields);
      public:

      /* Do-little. */
      virtual ~TAnyFields() {}

      /* Overridden to translate objects. */
      virtual void TranslateJson(TObj *that, const TJson &json) const = 0;

      protected:

      /* Do-little. */
      TAnyFields() {}

    };  // TAnyFields

    /* Some JSON types pass thru to native types directly. */
    template <typename TVal>
    inline void JsonAs(TVal &val, const TJson &json) {
      assert(&val);
      assert(&json);
      if (!json.TryAs(val)) {
        THROW_ERROR(TJsonMismatch)
            << "cannot translate from JSON " << json.GetKind()
            << " to " << Base::Demangle(typeid(TVal));
      }
    }

    /* Translate what we can as simple pass-throughs. */
    inline void TranslateJson(bool        &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(int8_t      &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(int16_t     &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(int32_t     &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(int64_t     &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(uint8_t     &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(uint16_t    &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(uint32_t    &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(uint64_t    &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(float       &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(double      &val, const TJson &json) { JsonAs(val, json); }
    inline void TranslateJson(std::string &val, const TJson &json) { JsonAs(val, json); }

    /* Translate TTimePnts from JSON strings. */
    inline void TranslateJson(
        Base::Chrono::TTimePnt &val, const TJson &json) {
      assert(&val);
      assert(&json);
      std::string temp;
      JsonAs(temp, json);
      // TODO: translate temp -> val
    }

    /* Translate UUIDs from JSON strings. */
    inline void TranslateJson(Base::TUuid &val, const TJson &json) {
      assert(&val);
      assert(&json);
      std::string temp;
      JsonAs(temp, json);
      val = Base::TUuid(temp.c_str());
    }

    /* Translate objects as JSON objects, re-entering the translation layer
       to handle the fields. */
    inline void TranslateJson(TObj &val, const TJson &json) {
      assert(&val);
      assert(&json);
      val.GetFields().TranslateJson(&val, json);
    }

    /* Translate optionals by accepting the JSON type 'null' or the
       type inside the TOpt. */
    template <typename TVal>
    inline void TranslateJson(Base::TOpt<TVal> &val, const TJson &json) {
      assert(&val);
      assert(&json);
      if (json.IsNull()) {
        val.Reset();
      } else {
        TVal temp;
        TranslateJson(temp, json);
        val = std::move(temp);
      }
    }

    /* Translate vectors by accepting a JSON vector and translating
       the elements one at at time. */
    template <typename TVal, typename TAlloc>
    inline void TranslateJson(
        std::vector<TVal, TAlloc> &val, const TJson &json) {
      assert(&val);
      assert(&json);
      if (json.GetKind() != TJson::Array) {
        THROW_ERROR(TJsonMismatch)
            << "cannot translate from JSON " << json.GetKind()
            << " to std::vector<" << Base::Demangle(typeid(TVal)) << '>';
      }
      const auto &json_array = json.GetArray();
      val.clear();
      val.reserve(json_array.size());
      for (const auto &sub_json: json_array) {
        TVal sub_val;
        TranslateJson(sub_val, sub_json);
        val.push_back(std::move(sub_val));
      }
    }

    /* The base for any field of TSomeObj. */
    template <typename TSomeObj>
    class TAnyField {
      NO_COPY(TAnyField);
      public:

      /* Do-little. */
      virtual ~TAnyField() {}

      /* The name of the field, which must be unique within TSomeObj. */
      const std::string &GetName() const noexcept {
        assert(this);
        return Name;
      }

      /* Overridden by TField<TSomeObj, TVal> to set the value of a field in
         the given instance of TSomeObj.  The value is taken from the given
         JSON object. */
      virtual void SetVal(TSomeObj *that, const TJson &json) const = 0;

      protected:

      /* Cache the name. */
      TAnyField(std::string &&name)
          : Name(std::move(name)) {}

      private:

      /* See accessor. */
      std::string Name;

    };  // TAnyField<TSomeObj>

    /* A field in TSomeObj of a particular type. */
    template <typename TSomeObj, typename TVal>
    class TField final
        : public TAnyField<TSomeObj> {
      public:

      /* The type of the member of TSomeObj to which we'll refer. */
      using TMember = TVal (TSomeObj::*);

      /* Cache the name and the member pointer. */
      TField(std::string &&name, TMember member)
          : TAnyField<TSomeObj>(std::move(name)), Member(member) {}

      /* Set the value of a field in the given instance of TSomeObj.  If the
         JSON object's type isn't compatible with the field, throw. */
      virtual void SetVal(TSomeObj *that, const TJson &json) const override {
        assert(this);
        assert(that);
        assert(&json);
        TranslateJson(that->*Member, json);
      }

      private:

      /* The member of TSomeObj to which we'll refer. */
      TMember Member;

    };  // TField<TSomeObj, TVal>

    /* A helper for constructing TField<TSomeObj, TVal> instances using type
       deduction. */
    template <typename TSomeObj, typename TVal>
    std::unique_ptr<TField<TSomeObj, TVal>> NewField(
        std::string &&name, TVal (TSomeObj::*member)) {
      return std::make_unique<TField<TSomeObj, TVal>>(
          std::move(name), member);
    }

    /* A collection of fields for a particular type of object. */
    template <typename TSomeObj>
    class TFields final
        : public TAnyFields {
      NO_COPY(TFields);
      public:

      /* Construct from a variadic list of unique pointers to
         TField<TSomeObj, TVal> instances. */
      template <typename... TVals>
      explicit TFields(
          std::unique_ptr<TField<TSomeObj, TVals>> &&... field_ptrs) {
        Helper<0, TVals...>::Init(this, std::move(field_ptrs)...);
      }

      /* Add a field to the collection. */
      void AddField(std::unique_ptr<TAnyField<TSomeObj>> &&field_ptr) {
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

      /* Set the values of all the fields in a particular TSomeObj instances,
         taking the values from the named elements on a JSON object.  The
         JSON object must have at least the elements we require.  Any
         additional elements we ignore. */
      virtual void TranslateJson(
          TObj *that, const TJson &json) const override {
        assert(this);
        assert(that);
        assert(&json);
        auto *obj = dynamic_cast<TSomeObj *>(that);
        if (!obj) {
          THROW_ERROR(TJsonMismatch)
              << "object is not of type \""
              << Base::Demangle(typeid(TSomeObj)) << '"';
        }
        if (json.GetKind() != Base::TJson::Object) {
          THROW_ERROR(TJsonMismatch)
              << "cannot translate from JSON " << json.GetKind()
              << " to " << Base::Demangle(typeid(TSomeObj));
        }
        for (const auto &field_ptr: FieldPtrs) {
          const auto &name = field_ptr->GetName();
          const TJson *sub_json = json.TryFind(name);
          if (!sub_json) {
            THROW_ERROR(TJsonMismatch)
                << "JSON object has no field named \""
                << name << '"';
          }
          field_ptr->SetVal(obj, *sub_json);
        }
      }

      private:

      /* This template helps during construction to pick apart the variadic
         argument list. */
      template <int Dummy, typename... TVals>
      struct Helper;

      /* Our fields in the order given to the constructor. */
      std::vector<std::unique_ptr<TAnyField<TSomeObj>>> FieldPtrs;

    };  // TFields<TSomeObj>

    /* Base case.  We do nothing. */
    template <typename TSomeObj>
    template <int Dummy>
    struct TFields<TSomeObj>::Helper<Dummy> {
      static void Init(TFields<TSomeObj> *) {}
    };  // TFields<TSomeObj>::THelper<Dummy>

    /* Recursive case.  We append the next argument to the collection and
       recurse. */
    template <typename TSomeObj>
    template <int Dummy, typename TVal, typename... TMoreVals>
    struct TFields<TSomeObj>::Helper<Dummy, TVal, TMoreVals...> {
      static void Init(
          TFields<TSomeObj> *that,
          std::unique_ptr<TField<TSomeObj, TVal>> &&field_ptr,
          std::unique_ptr<TField<TSomeObj, TMoreVals>> &&... more_field_ptrs) {
        that->AddField(std::move(field_ptr));
        Helper<Dummy, TMoreVals...>::Init(
            that, std::move(more_field_ptrs)...);
      }
    };  // TFields<TSomeObj>::THelper<Dummy, TVal, TMoreVals...>

  }  // Csv2Bin

}  // Orly
