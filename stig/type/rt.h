/* <stig/type/rt.h>

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

#include <string>

#include <base/chrono.h>
#include <stig/rt/containers.h>
#include <stig/rt/generator.h>
#include <stig/rt/opt.h>
#include <stig/rt/unknown.h>
#include <stig/type.h>
#include <stig/uuid.h>

namespace Stig {

  namespace Type {

    /* TODO */
    template <>
    struct TDt<bool> {

      /* TODO */
      TType static GetType() {
        return TBool::Get();
      }

    };

    /* TODO */
    template <typename TKey, typename TVal>
    struct TDt<Rt::TDict<TKey, TVal>> {

      /* TODO */
      TType static GetType() {
        return TDict::Get(TDt<TKey>::GetType(), TDt<TVal>::GetType());
      }

    };

    /* TODO */
    template <>
    struct TDt<int64_t> {

      /* TODO */
      TType static GetType() {
        return TInt::Get();
      }

    };

    /* TODO */
    template <typename TVal>
    struct TDt<std::vector<TVal>> {

      /* TODO */
      TType static GetType() {
        return TList::Get(TDt<TVal>::GetType());
      }

    };

    /* TODO */
    template <typename TAddr, typename TVal>
    struct TDt<Rt::TMutable<TAddr, TVal>> {

      /* TODO */
      TType static GetType() {
        return TMutable::Get(TDt<TAddr>::GetType(), TDt<TVal>::GetType());
      }

    };

    /* TODO */
    template <typename TVal>
    struct TDt<Rt::TOpt<TVal>> {

      /* TODO */
      TType static GetType() {
        return TOpt::Get(TDt<TVal>::GetType());
      }

    };

    /* NOTE: This was the old way of building up the types of objects. We now just gen a specialization of TDt for each
             object.
    template <typename TCompound>
    struct TDt {
      typedef typename TCompound::TDynamicMembers TMagic;
      static TType GetType() {
        TObjElems elem_map;
        assert(Type::TObj::Meta<TCompound>::TClass::TryGetClass());
        for (auto field = Type::TObj::Meta<TCompound>::TAnyField::GetFirstField(); field; field = field->GetNextField()) {
          auto result = elem_map.insert(std::make_pair(field->GetName(), TType()));
          // If this assertion fails, it means the metadata describing the fields of TCompound contains at least two fields with the same name.
          assert(result.second);
          result.first->second = field->GetType();
        }
        return TObj::Get(elem_map);
      }
    }; // TDt<TCompound>
    */

    /* TODO */
    template <>
    struct TDt<double> {

      /* TODO */
      TType static GetType() {
        return TReal::Get();
      }

    };

    template <typename TVal>
    struct TDt<Rt::TGenerator<TVal>> {

      TType static GetType() {
        return TSeq::Get(TDt<TVal>::GetType());
      }

    };

    /* TODO */
    template <typename TVal>
    struct TDt<Rt::TSet<TVal>> {

      TType static GetType() {
        return TSet::Get(TDt<TVal>::GetType());
      }

    };

    /* TODO */
    template <>
    struct TDt<std::string> {

      /* TODO */
      TType static GetType() {
        return TStr::Get();
      }

    };

    /* TODO */
    template <>
    struct TDt<Base::Chrono::TTimeDiff> {

      /* TODO */
      TType static GetType() {
        return TTimeDiff::Get();
      }

    };

    /* TODO */
    template <>
    struct TDt<Base::Chrono::TTimePnt> {

      /* TODO */
      TType static GetType() {
        return TTimePnt::Get();
      }

    };

    /* TODO */
    template <>
    struct TDt<Stig::Rt::TUnknown> {

      /* TODO */
      TType static GetType() {
        return TUnknown::Get();
      }

    };

    /* TODO */
    template <>
    struct TDt<TUUID> {

      /* TODO */
      TType static GetType() {
        return TId::Get();
      }

    };

  } // Type

} // Stig