/* <stig/var/mutable.h>

   A Stig mutable, which is a Rt::TMutable.

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

#include <stig/rt/mutable.h>
#include <stig/rt/runtime_error.h>
#include <stig/type/rt.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TMutable
        : public TVar::TImpl {
      public:

      /* TODO */
      virtual Var::TVar &Index(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &Add(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &And(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &Div(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &Exp(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &Intersection(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &Mod(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &Mult(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &Or(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &Sub(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &SymmetricDiff(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &Union(const TVar &) final;

      /* TODO */
      virtual TVar::TImpl &Xor(const TVar &) final;

      /* TODO */
      const Var::TVar &GetVal() const {
        assert(this);
        return Val;
      }

      const Var::TVar &GetAddr() const {
        assert(this);
        return Addr;
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
      template <typename TAddr, typename TVal>
      TMutable(const Rt::TMutable<TAddr, TVal> &that)
            : Addr(that.GetOptAddr()), Val(that.GetVal()), Type(Type::TDt<Rt::TMutable<TAddr, TVal>>::GetType()) {
        SetHash();
      }

      /* TODO */
      TMutable(const Var::TVar &addr, const Var::TVar &val);

      /* TODO */
      virtual ~TMutable();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      void SetHash();

      /* TODO */
      Var::TVar Addr, Val;

      /* TODO */
      Type::TType Type;

      /* TODO */
      size_t Hash;

      /* TODO */
      friend class TVar;

    };  // TAddr

    template <typename TAddr, typename TVal>
    TVar TVar::Mutable(const Rt::TMutable<TAddr, TVal> &that) {
      return (new TMutable(that))->AsVar();
    }

    /* TODO */
    template <typename TAddr, typename TVal>
    struct TVar::TDt<Rt::TMutable<TAddr, TVal>> {

      /* TODO */
      Rt::TMutable<TAddr, TVal> static As(const TVar &that) {
        TMutable *ptr = dynamic_cast<TMutable *>(that.Impl.get());
        if (ptr) {
          return Rt::TMutable<TAddr, TVal>(TVar::TDt<TAddr>::As(ptr->GetAddr()), TVar::TDt<TVal>::As(ptr->GetVal()));
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to opt. Var is not an opt.");
      }

    };  // TDt<Rt::TMutable<TVal>>

  }  // Var

}  // Stig

