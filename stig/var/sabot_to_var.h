/* <stig/var/sabot_to_var.h>

   Given a sabot, construct a Var.

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

#include <stig/sabot/state.h>
#include <stig/sabot/type.h>
#include <stig/var.h>

namespace Stig {

  namespace Var {

    /* Thrown by ToVar() when the state has no translation to a stig type. */
    class TVarTranslationError
        : public std::logic_error {
      public:

      /* Do-little. */
      TVarTranslationError();

    };  // TVarTranslationError

    /* TODO */
    class TToVarVisitor final
        : public Sabot::TStateVisitor {
      public:

      /* TODO */
      TToVarVisitor(Var::TVar &var)
          : Var(var) {
        assert(&var);
      }

      /* Overrides. */
      virtual void operator()(const Sabot::State::TFree &state) const override;
      virtual void operator()(const Sabot::State::TTombstone &state) const override;
      virtual void operator()(const Sabot::State::TVoid &state) const override;
      virtual void operator()(const Sabot::State::TInt8 &state) const override;
      virtual void operator()(const Sabot::State::TInt16 &state) const override;
      virtual void operator()(const Sabot::State::TInt32 &state) const override;
      virtual void operator()(const Sabot::State::TInt64 &state) const override;
      virtual void operator()(const Sabot::State::TUInt8 &state) const override;
      virtual void operator()(const Sabot::State::TUInt16 &state) const override;
      virtual void operator()(const Sabot::State::TUInt32 &state) const override;
      virtual void operator()(const Sabot::State::TUInt64 &state) const override;
      virtual void operator()(const Sabot::State::TBool &state) const override;
      virtual void operator()(const Sabot::State::TChar &state) const override;
      virtual void operator()(const Sabot::State::TFloat &state) const override;
      virtual void operator()(const Sabot::State::TDouble &state) const override;
      virtual void operator()(const Sabot::State::TDuration &state) const override;
      virtual void operator()(const Sabot::State::TTimePoint &state) const override;
      virtual void operator()(const Sabot::State::TUuid &state) const override;
      virtual void operator()(const Sabot::State::TBlob &state) const override;
      virtual void operator()(const Sabot::State::TStr &state) const override;
      virtual void operator()(const Sabot::State::TDesc &state) const override;
      virtual void operator()(const Sabot::State::TOpt &state) const override;
      virtual void operator()(const Sabot::State::TSet &state) const override;
      virtual void operator()(const Sabot::State::TVector &state) const override;
      virtual void operator()(const Sabot::State::TMap &state) const override;
      virtual void operator()(const Sabot::State::TRecord &state) const override;
      virtual void operator()(const Sabot::State::TTuple &state) const override;

      private:

      /* TODO */
      Var::TVar &Var;

    };  // TToVarVisitor

    /* TODO */
    inline Var::TVar ToVar(const Sabot::State::TAny &state) {
      Var::TVar var;
      state.Accept(TToVarVisitor(var));
      return var;
    }

  }  // Var

  namespace Sabot {

    /* TODO */
    inline void ToNative(const Sabot::State::TAny &state, Var::TVar &val) {
      val = Var::ToVar(state);
    }

  }  // Sabot

}  // Stig