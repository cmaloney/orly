/* <stig/var/real.h>

   A Stig real, which is a double.

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

    class TReal
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TReal &Add(const TVar &);

      /* TODO */
      virtual TReal &And(const TVar &);

      /* TODO */
      virtual TReal &Div(const TVar &);

      /* TODO */
      virtual TReal &Exp(const TVar &);

      /* TODO */
      virtual TReal &Intersection(const TVar &);

      /* TODO */
      virtual TReal &Mod(const TVar &);

      /* TODO */
      virtual TReal &Mult(const TVar &);

      /* TODO */
      virtual TReal &Or(const TVar &);

      /* TODO */
      virtual TReal &Sub(const TVar &);

      /* TODO */
      virtual TReal &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TReal &Union(const TVar &);

      /* TODO */
      virtual TReal &Xor(const TVar &);

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      double GetVal() const {
        assert(this);
        return Val;
      }

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      virtual void Touch();

      /* TODO */
      static TVar New(double that);

      /* TODO */
      virtual void Write(std::ostream &stream) const;

      private:

      /* TODO */
      TReal(double that);

      /* TODO */
      virtual ~TReal();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      double Val;

    };  // TReal

    /* TODO */
    template <>
    struct TVar::TDt<double> {

      /* TODO */
      double static As(const TVar &that) {
        TReal *ptr = dynamic_cast<TReal *>(that.Impl.get());
        if (ptr) {
          return ptr->GetVal();
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to real. Var is not a real.");
      }

    };  // TDt<double>

  }  // Var

}  // Stig
