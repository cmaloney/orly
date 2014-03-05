/* <stig/var/err.h>

   A Stig error, which is a Stig::Rt::TError.

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

#include <stig/type/rt.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TErr
        : public TVar::TImpl {
      public:

      /* TODO */
      typedef Rt::TError<TVar> TErrType;

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TErr &Add(const TVar &);

      /* TODO */
      virtual TErr &And(const TVar &);

      /* TODO */
      virtual TErr &Div(const TVar &);

      /* TODO */
      virtual TErr &Exp(const TVar &);

      /* TODO */
      virtual TErr &Intersection(const TVar &);

      /* TODO */
      virtual TErr &Mod(const TVar &);

      /* TODO */
      virtual TErr &Mult(const TVar &);

      /* TODO */
      virtual TErr &Or(const TVar &);

      /* TODO */
      virtual TErr &Sub(const TVar &);

      /* TODO */
      virtual TErr &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TErr &Union(const TVar &);

      /* TODO */
      virtual TErr &Xor(const TVar &);

      /* TODO */
      const TErrType &GetVal() const {
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
      virtual void Write(std::ostream &) const;

      private:

      /* TODO */
      template <typename TVal>
      TErr(const Rt::TError<TVal> &that) : Val(Stig::Rt::TError<TVar>(that.GetErr())), Type(Type::TDt<TVal>::GetType()) {
        SetHash();
      }

      /* TODO */
      virtual ~TErr();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      void SetHash();

      /* TODO */
      TErrType Val;

      /* TODO */
      Type::TType Type;

      /* TODO */
      size_t Hash;

      /* TODO */
      friend class TVar;

    };  // TErr

    template <typename TVal>
    TVar::TVar(const Rt::TError<TVal> &that) {
      Impl = std::shared_ptr<TImpl>(new TErr(that), TImpl::Delete);
    }

  }  // Var

}  // Stig
