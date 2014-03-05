/* <stig/var/bool.h>

   A Stig bool, which is a bool.

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

#include <iostream>

#include <stig/rt/runtime_error.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TBool
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TBool &Add(const TVar &);

      /* TODO */
      virtual TBool &And(const TVar &);

      /* TODO */
      virtual TBool &Div(const TVar &);

      /* TODO */
      virtual TBool &Exp(const TVar &);

      /* TODO */
      virtual TBool &Intersection(const TVar &);

      /* TODO */
      virtual TBool &Mod(const TVar &);

      /* TODO */
      virtual TBool &Mult(const TVar &);

      /* TODO */
      virtual TBool &Or(const TVar &);

      /* TODO */
      virtual TBool &Sub(const TVar &);

      /* TODO */
      virtual TBool &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TBool &Union(const TVar &);

      /* TODO */
      virtual TBool &Xor(const TVar &);

      /* TODO */
      bool GetVal() const {
        assert(this);
        return Val;
      }

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      virtual void Touch();

      /* TODO */
      static TVar New(bool that);

      /* TODO */
      virtual void Write(std::ostream &stream) const;

      private:

      /* TODO */
      TBool(bool that);

      /* TODO */
      virtual ~TBool();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      bool Val;

    };  // TBool

    /* TODO */
    template <>
    struct TVar::TDt<bool> {

      /* TODO */
      bool static As(const TVar &that) {
        TBool *ptr = dynamic_cast<TBool *>(that.Impl.get());
        if (ptr) {
          return ptr->GetVal();
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to bool. Var is not a bool.");
      }

    };  // TDt<bool>

  }  // Var

}  // Stig
