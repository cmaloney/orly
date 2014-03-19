/* <stig/var/obj.h>

   A Stig object, which is a dictionary of vars.

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
#include <string>
#include <unordered_map>

#include <base/no_construction.h>
#include <base/no_copy_semantics.h>
#include <stig/rt/obj.h>
#include <stig/rt/runtime_error.h>
#include <stig/type/impl.h>
#include <stig/type/obj.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Type {

    /* TODO */
    template <typename TCompound>
    template <typename TVal>
    void TObj::Meta<TCompound>::TField<TVal>::GetVal(const TCompound &that, Var::TVar &out) const {
      assert(this);
      assert(&that);
      assert(&out);
      out = Var::TVar(that.*Member);
    }

  }

  namespace Var {

    class TObj
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TObj &Add(const TVar &);

      /* TODO */
      virtual TObj &And(const TVar &);

      /* TODO */
      virtual TObj &Div(const TVar &);

      /* TODO */
      virtual TObj &Exp(const TVar &);

      /* TODO */
      virtual TObj &Intersection(const TVar &);

      /* TODO */
      virtual TObj &Mod(const TVar &);

      /* TODO */
      virtual TObj &Mult(const TVar &);

      /* TODO */
      virtual TObj &Or(const TVar &);

      /* TODO */
      virtual TObj &Sub(const TVar &);

      /* TODO */
      virtual TObj &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TObj &Union(const TVar &);

      /* TODO */
      virtual TObj &Xor(const TVar &);

      /* TODO */
      template <typename TCompound, typename TVal>
      static void GetValFromField(const TCompound &compound, const typename Type::TObj::Meta<TCompound>::template TField<TVal> &field, TVar &out) {
        assert(&compound);
        assert(&field);
        assert(&out);
        out = TVar(compound.*(field.Member));
      }

      /* TODO */
      typedef std::unordered_map<std::string, TVar> TFieldsByName;
      typedef TFieldsByName TElems;

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      virtual void Touch();

      /* TODO */
      virtual void Write(std::ostream &) const;

      /* TODO */
      const TFieldsByName &GetVal() const {
        assert(this);
        return FieldsByName;
      }

      private:

      /* NOTE: This is left here in case someone wants to use the runtime object inspection.

      template <typename TCompound>
      TObj(const TCompound &that) {
        typedef typename TCompound::TDynamicMembers TFoobar;
        // If this assertion fails, it means you're trying to construct a TVar from a type for which there is no metadata.
        if (!Type::TObj::Meta<TCompound>::TClass::TryGetClass()) {
          throw Base::TNotImplementedError(HERE);
        }
        assert(Type::TObj::Meta<TCompound>::TClass::TryGetClass());
        for (auto field = Type::TObj::Meta<TCompound>::TAnyField::GetFirstField(); field; field = field->GetNextField()) {
          auto result = FieldsByName.insert(std::make_pair(field->GetName(), DefaultVar));
          // If this assertion fails, it means the metadata describing the fields of TCompound contains at least two fields with the same name.
          assert(result.second);
          field->GetVal(that, result.first->second);
          TypeMap[field->GetName()] = field->GetType();
        }
        SetHash();
      }
      */

      /* TODO */
      TObj(const TFieldsByName &that);

      /* TODO */
      virtual ~TObj();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      void SetHash();

      /* TODO */
      TFieldsByName FieldsByName;

      /* TODO */
      size_t Hash;

      /* TODO */
      std::map<std::string, Type::TType> TypeMap;

      /* TODO */
      static const TVar DefaultVar;

      /* TODO */
      friend class TVar;

    };  // TObj

    /* See <stig/var.h>. */
    template <typename TCompound>
    TVar::TVar(const TCompound &that) {
      *this = that.AsVar();
    }

    /* TODO */
    template <typename TVal>
    TVal Var::TVar::TDt<TVal>::As(const TVar &that) {
      TObj *ptr = dynamic_cast<TObj *>(that.Impl.get());
      if (ptr) {
        return TVal(ptr->GetVal());
      }
      throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to wrong type");
    }

  }  // Var

}  // Stig
