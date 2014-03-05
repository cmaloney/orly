/* <stig/var/int.h>

   A Stig integer, which is 64 bits and signed.

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
#include <typeinfo>

#include <stig/rt/runtime_error.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TInt
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TInt &Add(const TVar &);

      /* TODO */
      virtual TInt &And(const TVar &);

      /* TODO */
      virtual TInt &Div(const TVar &);

      /* TODO */
      virtual TInt &Exp(const TVar &);

      /* TODO */
      virtual TInt &Intersection(const TVar &);

      /* TODO */
      virtual TInt &Mod(const TVar &);

      /* TODO */
      virtual TInt &Mult(const TVar &);

      /* TODO */
      virtual TInt &Or(const TVar &);

      /* TODO */
      virtual TInt &Sub(const TVar &);

      /* TODO */
      virtual TInt &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TInt &Union(const TVar &);

      /* TODO */
      virtual TInt &Xor(const TVar &);

      /* TODO */
      int64_t GetVal() const {
        assert(this);
        return Val;
      }

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      virtual void Touch();

      /* TODO */
      static TVar New(int64_t that);

      /* TODO */
      virtual void Write(std::ostream &stream) const;

      private:

      /* TODO */
      TInt(int64_t that);

      /* TODO */
      virtual ~TInt();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      int64_t Val;

    };  // TInt

    /* TODO */
    template <>
    struct TVar::TDt<int64_t> {

      /* TODO */
      int64_t static As(const TVar &that) {
        const Stig::Var::TInt *ptr = dynamic_cast<const TInt *>(that.Impl.get());
        if (ptr) {
          return ptr->GetVal();
        }
        std::cerr << "TYPE NAME: " << typeid(*that.Impl.get()).name() << std::endl;
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to int. Var is not an int.");
      }

    };  // TDt<int64_t>

  }  // Var

}  // Stig
