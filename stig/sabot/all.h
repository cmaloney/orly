/* <stig/sabot/all.h>

   The header to include for native, core, var type and state sabots.

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

#include <stig/atom/kit2.h>
#include <stig/closure.h>
#include <stig/native/state.h>
#include <stig/sabot/defs.h>
#include <stig/sabot/get_tuple_size.h>
#include <stig/sabot/match_prefix_state.h>
#include <stig/sabot/order_states.h>
#include <stig/sabot/state_dumper.h>
#include <stig/sabot/to_native.h>
#include <stig/sabot/type_dumper.h>
#include <stig/type/new_sabot.h>
#include <stig/var/new_sabot.h>
#include <stig/var/sabot_to_var.h>
#include <stig/client/program/translate_expr.h>

namespace Stig {

  namespace Sabot {

    /* TODO */
    template <size_t... Sizes>
    class Maxer;

    /* TODO */
    template <size_t Size>
    class Maxer<Size> {
      NO_CONSTRUCTION(Maxer);
      public:

      /* TODO */
      static constexpr size_t Max = Size;

    };  // Maxer<Size>

    /* TODO */
    template <size_t Size>
    constexpr size_t Maxer<Size>::Max;

    /* TODO */
    template <size_t Size, size_t... MoreSizes>
    class Maxer<Size, MoreSizes...> {
      NO_CONSTRUCTION(Maxer);
      public:

      /* TODO */
      static constexpr size_t Max = (Size >= Maxer<MoreSizes...>::Max) ? Size : Maxer<MoreSizes...>::Max;

    };  // Maxer<Size, MoreSizes...>

    /* TODO */
    template <size_t Size, size_t... MoreSizes>
    constexpr size_t Maxer<Size, MoreSizes...>::Max;

    /* TODO */
    class TSizeChecker {
      NO_CONSTRUCTION(TSizeChecker);
      public:

      /* TODO */
      static constexpr size_t GetMaxTypePinSize() {
        return Maxer<
          sizeof(Native::Type::TDesc<int8_t>::TPin),
          sizeof(Native::Type::TFree<int8_t>::TPin),
          sizeof(Native::Type::TOpt<int8_t>::TPin),
          sizeof(Native::Type::TSet<int8_t>::TPin),
          sizeof(Native::Type::TVector<int8_t>::TPin),
          sizeof(Native::Type::TMap<int8_t, int8_t>::TPin),
          sizeof(Native::Type::TRecord<int8_t>::TPin),
          sizeof(Native::Type::TTuple<int8_t>::TPin),

          sizeof(Atom::TCore::ST::TFree::TPin),
          sizeof(Atom::TCore::ST::TDesc::TPin),
          sizeof(Atom::TCore::ST::TOpt::TPin),
          sizeof(Atom::TCore::ST::TSet::TPin),
          sizeof(Atom::TCore::ST::TVector::TPin),
          sizeof(Atom::TCore::ST::TMap::TPin),
          sizeof(Atom::TCore::ST::TRecord::TPin),
          sizeof(Atom::TCore::ST::TTuple::TPin),

          sizeof(Stig::Type::ST::TFree::TPin),
          sizeof(Stig::Type::ST::TOpt::TPin),
          sizeof(Stig::Type::ST::TSet::TPin),
          sizeof(Stig::Type::ST::TVector::TPin),
          sizeof(Stig::Type::ST::TMap::TPin),
          sizeof(Stig::Type::ST::TRecord::TPin),
          sizeof(Stig::Type::ST::TTuple::TPin),

          sizeof(Stig::TClosure::TType::TPin),

          sizeof(Client::Program::Type::TUnaryType<Sabot::Type::TOpt>::TPin),
          sizeof(Client::Program::Type::TUnaryExpr<Sabot::Type::TOpt>::TPin),
          sizeof(Client::Program::Type::TBinaryType<Sabot::Type::TMap>::TPin),
          sizeof(Client::Program::Type::TBinaryExpr<Sabot::Type::TMap>::TPin),
          sizeof(Client::Program::Type::TRecordType::TPin),
          sizeof(Client::Program::Type::TRecordExpr::TPin),
          sizeof(Client::Program::Type::TTupleType::TPin),
          sizeof(Client::Program::Type::TTupleExpr::TPin)

        >::Max;
      }

      /* TODO */
      static constexpr size_t GetMaxStatePinSize() {
        return Maxer<
          sizeof(Native::State::TArrayOfScalars<Sabot::State::TBlob>::TPin),
          sizeof(Native::State::TArrayOfScalars<Sabot::State::TStr>::TPin),
          sizeof(Native::State::TArrayOfSingleStates<Sabot::State::TDesc>::TPin),
          sizeof(Native::State::TArrayOfSingleStates<Sabot::State::TOpt>::TPin),
          sizeof(Native::State::TArrayOfSingleStates<Sabot::State::TSet>::TPin),
          sizeof(Native::State::TArrayOfSingleStates<Sabot::State::TVector>::TPin),
          sizeof(Native::State::TArrayOfPairsOfStates<Sabot::State::TMap>::TPin),
          sizeof(Native::State::TStaticArrayOfSingleStates<Sabot::State::TRecord>::TPin),
          sizeof(Native::State::TStaticArrayOfSingleStates<Sabot::State::TTuple>::TPin),

          sizeof(Atom::TCore::SS::TBlob::TPin),
          sizeof(Atom::TCore::SS::TStr::TPin),
          sizeof(Atom::TCore::SS::TArrayOfStates<Sabot::State::TDesc>::TPin),
          sizeof(Atom::TCore::SS::TArrayOfStates<Sabot::State::TOpt>::TPin),
          sizeof(Atom::TCore::SS::TArrayOfStates<Sabot::State::TSet>::TPin),
          sizeof(Atom::TCore::SS::TArrayOfStates<Sabot::State::TVector>::TPin),
          sizeof(Atom::TCore::SS::TArrayOfPairsOfStates<Sabot::State::TMap>::TPin),
          sizeof(Atom::TCore::SS::TRecord::TPin),
          sizeof(Atom::TCore::SS::TTuple::TPin),

          sizeof(Var::SS::TStr ::TPin),
          sizeof(Var::SS::TList::TPin),
          sizeof(Var::SS::TSet ::TPin),
          sizeof(Var::SS::TOpt ::TPin),
          sizeof(Var::SS::TDict::TPin),
          sizeof(Var::SS::TAddr::TPin),
          sizeof(Var::SS::TObj ::TPin),

          sizeof(Stig::TClosure::TState::TPin),

          sizeof(Client::Program::State::TStr::TPin),
          sizeof(Client::Program::State::TOpt::TPin),
          sizeof(Client::Program::State::TSet::TPin),
          sizeof(Client::Program::State::TList::TPin),
          sizeof(Client::Program::State::TDict::TPin),
          sizeof(Client::Program::State::TObj::TPin),
          sizeof(Client::Program::State::TAddr::TPin)

        >::Max;
      }

      static constexpr size_t GetMaxTypeSize() {
        return Maxer<
          sizeof(Native::Type::TFree<void>),
          sizeof(Sabot::Type::TTombstone),
          sizeof(Sabot::Type::TVoid),
          sizeof(Sabot::Type::TInt8),
          sizeof(Sabot::Type::TInt16),
          sizeof(Sabot::Type::TInt32),
          sizeof(Sabot::Type::TInt64),
          sizeof(Sabot::Type::TUInt8),
          sizeof(Sabot::Type::TUInt16),
          sizeof(Sabot::Type::TUInt32),
          sizeof(Sabot::Type::TUInt64),
          sizeof(Sabot::Type::TBool),
          sizeof(Sabot::Type::TChar),
          sizeof(Sabot::Type::TFloat),
          sizeof(Sabot::Type::TDouble),
          sizeof(Sabot::Type::TDuration),
          sizeof(Sabot::Type::TTimePoint),
          sizeof(Sabot::Type::TUuid),
          sizeof(Sabot::Type::TBlob),
          sizeof(Sabot::Type::TStr),
          sizeof(Native::Type::TDesc<int8_t>),
          sizeof(Native::Type::TOpt<int8_t>),
          sizeof(Native::Type::TSet<int8_t>),
          sizeof(Native::Type::TVector<int8_t>),
          sizeof(Native::Type::TMap<int8_t, int8_t>),
          sizeof(Native::Record<Native::State::TInt8>::TState),
          sizeof(Native::State::TTuple<int8_t>),

          sizeof(Atom::TCore::ST::TFree),
          sizeof(Atom::TCore::ST::TOpt),
          sizeof(Atom::TCore::ST::TDesc),
          sizeof(Atom::TCore::ST::TSet),
          sizeof(Atom::TCore::ST::TVector),
          sizeof(Atom::TCore::ST::TMap),
          sizeof(Atom::TCore::ST::TRecord),
          sizeof(Atom::TCore::ST::TTuple),

          sizeof(Stig::Type::ST::TFree),
          sizeof(Stig::Type::ST::TOpt),
          sizeof(Stig::Type::ST::TSet),
          sizeof(Stig::Type::ST::TVector),
          sizeof(Stig::Type::ST::TMap),
          sizeof(Stig::Type::ST::TRecord),
          sizeof(Stig::Type::ST::TTuple),

          sizeof(Stig::TClosure::TType),

          sizeof(Client::Program::Type::TUnaryType<Sabot::Type::TOpt>),
          sizeof(Client::Program::Type::TUnaryExpr<Sabot::Type::TOpt>),
          sizeof(Client::Program::Type::TBinaryType<Sabot::Type::TMap>),
          sizeof(Client::Program::Type::TBinaryExpr<Sabot::Type::TMap>),
          sizeof(Client::Program::Type::TRecordType),
          sizeof(Client::Program::Type::TRecordExpr),
          sizeof(Client::Program::Type::TTupleType),
          sizeof(Client::Program::Type::TTupleExpr)

        >::Max;
      }

      static constexpr size_t GetMaxStateSize() {
        return Maxer<
          sizeof(Native::State::TFree<void>),
          sizeof(Native::State::TTombstone),
          sizeof(Native::State::TVoid),
          sizeof(Native::State::TInt8),
          sizeof(Native::State::TInt16),
          sizeof(Native::State::TInt32),
          sizeof(Native::State::TInt64),
          sizeof(Native::State::TUInt8),
          sizeof(Native::State::TUInt16),
          sizeof(Native::State::TUInt32),
          sizeof(Native::State::TUInt64),
          sizeof(Native::State::TBool),
          sizeof(Native::State::TChar),
          sizeof(Native::State::TFloat),
          sizeof(Native::State::TDouble),
          sizeof(Native::State::TDuration),
          sizeof(Native::State::TTimePoint),
          sizeof(Native::State::TUuid),
          sizeof(Native::State::TBlob),
          sizeof(Native::State::TStr),
          sizeof(Native::State::TDesc<int8_t>),
          sizeof(Native::State::TOpt<int8_t>),
          sizeof(Native::State::TSet<int8_t>),
          sizeof(Native::State::TVector<int8_t>),
          sizeof(Native::State::TMap<int8_t, int8_t>),
          sizeof(Native::Record<Native::State::TInt8>::TState),
          sizeof(Native::State::TTuple<int8_t>),

          sizeof(Atom::TCore::SS::TFree),
          sizeof(Atom::TCore::SS::TTombstone),
          sizeof(Atom::TCore::SS::TVoid),
          sizeof(Atom::TCore::SS::TInt8),
          sizeof(Atom::TCore::SS::TInt16),
          sizeof(Atom::TCore::SS::TInt32),
          sizeof(Atom::TCore::SS::TInt64),
          sizeof(Atom::TCore::SS::TUInt8),
          sizeof(Atom::TCore::SS::TUInt16),
          sizeof(Atom::TCore::SS::TUInt32),
          sizeof(Atom::TCore::SS::TUInt64),
          sizeof(Atom::TCore::SS::TBool),
          sizeof(Atom::TCore::SS::TChar),
          sizeof(Atom::TCore::SS::TFloat),
          sizeof(Atom::TCore::SS::TDouble),
          sizeof(Atom::TCore::SS::TDuration),
          sizeof(Atom::TCore::SS::TTimePoint),
          sizeof(Atom::TCore::SS::TUuid),
          sizeof(Atom::TCore::SS::TBlob),
          sizeof(Atom::TCore::SS::TStr),
          sizeof(Atom::TCore::SS::TOpt),
          sizeof(Atom::TCore::SS::TDesc),
          sizeof(Atom::TCore::SS::TSet),
          sizeof(Atom::TCore::SS::TVector),
          sizeof(Atom::TCore::SS::TMap),
          sizeof(Atom::TCore::SS::TRecord),
          sizeof(Atom::TCore::SS::TTuple),

          sizeof(Var::SS::TBool    ),
          sizeof(Var::SS::TId      ),
          sizeof(Var::SS::TInt     ),
          sizeof(Var::SS::TReal    ),
          sizeof(Var::SS::TTimeDiff),
          sizeof(Var::SS::TTimePnt ),
          sizeof(Var::SS::TStr     ),
          sizeof(Var::SS::TList    ),
          sizeof(Var::SS::TSet     ),
          sizeof(Var::SS::TFree    ),
          sizeof(Var::SS::TOpt     ),
          sizeof(Var::SS::TDict    ),
          sizeof(Var::SS::TAddr    ),
          sizeof(Var::SS::TObj     ),

          sizeof(Stig::TClosure::TState),

          sizeof(Client::Program::State::TBool),
          sizeof(Client::Program::State::TInt),
          sizeof(Client::Program::State::TReal),
          sizeof(Client::Program::State::TTimePnt),
          sizeof(Client::Program::State::TTimeDiff),
          sizeof(Client::Program::State::TStr),
          sizeof(Client::Program::State::TOpt),
          sizeof(Client::Program::State::TSet),
          sizeof(Client::Program::State::TList),
          sizeof(Client::Program::State::TDict),
          sizeof(Client::Program::State::TObj),
          sizeof(Client::Program::State::TAddr)

        >::Max;
      }

    };  // TSizeChecker

    static_assert(Sabot::Type::GetMaxTypePinSize() == TSizeChecker::GetMaxTypePinSize(), "The maximum size of a type sabot pin is different from Sabot::Type::GetMaxTypePinSize().");
    static_assert(Sabot::State::GetMaxStatePinSize() == TSizeChecker::GetMaxStatePinSize(), "The maximum size of a state sabot pin is different from Sabot::State::GetMaxStatePinSize().");
    static_assert(Sabot::Type::GetMaxTypeSize() == TSizeChecker::GetMaxTypeSize(), "The maximum size of a type sabot is different from Sabot::Type::GetMaxTypeSize().");
    static_assert(Sabot::State::GetMaxStateSize() == TSizeChecker::GetMaxStateSize(), "The maximum size of a state sabot is different from Sabot::State::GetMaxStateSize().");

  }  // Sabot

}  // Stig
