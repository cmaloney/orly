/* <stig/type/sabot_to_type.h>

   Given a sabot, construct a Type.

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
#include <stdexcept>

#include <stig/sabot/all.h>
#include <stig/sabot/type.h>
#include <stig/var.h>

namespace Stig {

  namespace Type {

    /* Thrown by ToType() when the state has no translation to a stig type. */
    class TSabotToTypeTranslationError
        : public std::logic_error {
      public:

      /* Do-little. */
      TSabotToTypeTranslationError();

    };  // TSabotToTypeTranslationError

    /* TODO */
    class TToTypeVisitor final
        : public Sabot::TTypeVisitor {
      public:

      /* TODO */
      TToTypeVisitor(Type::TType &type)
          : Type(type) {
        assert(&type);
      }

      /* Overrides. */
      virtual void operator()(const Sabot::Type::TInt8 &type) const override;
      virtual void operator()(const Sabot::Type::TInt16 &type) const override;
      virtual void operator()(const Sabot::Type::TInt32 &type) const override;
      virtual void operator()(const Sabot::Type::TInt64 &type) const override;
      virtual void operator()(const Sabot::Type::TUInt8 &type) const override;
      virtual void operator()(const Sabot::Type::TUInt16 &type) const override;
      virtual void operator()(const Sabot::Type::TUInt32 &type) const override;
      virtual void operator()(const Sabot::Type::TUInt64 &type) const override;
      virtual void operator()(const Sabot::Type::TBool &type) const override;
      virtual void operator()(const Sabot::Type::TChar &type) const override;
      virtual void operator()(const Sabot::Type::TFloat &type) const override;
      virtual void operator()(const Sabot::Type::TDouble &type) const override;
      virtual void operator()(const Sabot::Type::TDuration &type) const override;
      virtual void operator()(const Sabot::Type::TTimePoint &type) const override;
      virtual void operator()(const Sabot::Type::TUuid &type) const override;
      virtual void operator()(const Sabot::Type::TBlob &type) const override;
      virtual void operator()(const Sabot::Type::TStr &type) const override;
      virtual void operator()(const Sabot::Type::TTombstone &type) const override;
      virtual void operator()(const Sabot::Type::TVoid &type) const override;
      virtual void operator()(const Sabot::Type::TDesc &type) const override;
      virtual void operator()(const Sabot::Type::TFree &type) const override;
      virtual void operator()(const Sabot::Type::TOpt &type) const override;
      virtual void operator()(const Sabot::Type::TSet &type) const override;
      virtual void operator()(const Sabot::Type::TVector &type) const override;
      virtual void operator()(const Sabot::Type::TMap &type) const override;
      virtual void operator()(const Sabot::Type::TRecord &type) const override;
      virtual void operator()(const Sabot::Type::TTuple &type) const override;

      private:

      /* TODO */
      void OnUnary(const Sabot::Type::TUnary &type) const;

      /* TODO */
      Type::TType &Type;

    };  // TToTypeVisitor

    /* TODO */
    inline Type::TType ToType(const Sabot::Type::TAny &state) {
      Type::TType type;
      state.Accept(TToTypeVisitor(type));
      return type;
    }

  }  // Type

}  // Stig