/* <stig/native/all.h>

   The header to include for native type and state sabots.

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

#include <stig/native/record.h>

namespace Stig {

  namespace Native {

    /* Accept a visitor to a native type sabot. */
    template <typename TVal>
    void Accept(const Sabot::TTypeVisitor &visitor) {
      void *type_alloc = alloca(Sabot::Type::GetMaxTypeSize());
      Sabot::Type::TAny::TWrapper(Native::Type::For<TVal>::GetType(type_alloc))->Accept(visitor);
    }

    #if 0
    /* Accept a double visitor to a pair of native type sabots. */
    template <typename TLhs, typename TRhs>
    void AcceptDouble(const Sabot::Type::Sum::Double::TVisitor &visitor) {
      ::Accept(Type::For<TLhs>::Type, Type::For<TRhs>::Type, visitor);
    }

    /* Accept a visitor to a native state sabot. */
    template <typename TVal>
    void Accept(const TVal &val, const Sabot::State::TStateVisitor &visitor) {
      std::unique_ptr<Sabot::State::TAny>(State::New(val))->Accept(visitor);
    }

    /* Accept a double visitor to a pair of native state sabots. */
    template <typename TLhs, typename TRhs>
    void AcceptDouble(const TLhs &lhs, const TRhs &rhs, const Sabot::State::Sum::Double::TVisitor &visitor) {
      std::unique_ptr<Sabot::State::TAny>
          lhs_state(State::New(lhs)),
          rhs_state(State::New(rhs));
      ::AcceptDouble(*lhs_state, *rhs_state, visitor);
    }
    #endif

  }  // Native

}  // Stig
