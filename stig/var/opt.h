/* <stig/var/opt.h>

   A Stig opt, which is a Rt::TOpt.

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
#include <stig/type/rt.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TOpt
        : public TVar::TImpl {
      public:

      /* TODO */
      typedef Rt::TOpt<TVar> TOptType;

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TOpt &Add(const TVar &);

      /* TODO */
      virtual TOpt &And(const TVar &);

      /* TODO */
      virtual TOpt &Div(const TVar &);

      /* TODO */
      virtual TOpt &Exp(const TVar &);

      /* TODO */
      virtual TOpt &Intersection(const TVar &);

      /* TODO */
      virtual TOpt &Mod(const TVar &);

      /* TODO */
      virtual TOpt &Mult(const TVar &);

      /* TODO */
      virtual TOpt &Or(const TVar &);

      /* TODO */
      virtual TOpt &Sub(const TVar &);

      /* TODO */
      virtual TOpt &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TOpt &Union(const TVar &);

      /* TODO */
      virtual TOpt &Xor(const TVar &);

      /* TODO */
      const TOptType &GetVal() const {
        assert(this);
        return Val;
      }

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      Type::TType GetInnerType() const;


      /* TODO */
      virtual void Touch();

      /* TODO */
      virtual void Write(std::ostream &) const;

      private:

      /* TODO */
      #if defined(STIG_HOST)
      template <typename TVal>
      TOpt(const Rt::TOpt<TVal> &that) : Type(Type::TDt<TVal>::GetType()) {
        if (that) {
          Val = TVar(*that);
        }
        SetHash();
      }
      #endif

      /* TODO */
      TOpt(const Rt::TOpt<TVar> &that, const Type::TType &type);

      /* TODO */
      virtual ~TOpt();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      void SetHash();

      /* TODO */
      TOptType Val;

      /* TODO */
      Type::TType Type;

      /* TODO */
      size_t Hash;

      /* TODO */
      friend class TVar;

    };  // TOpt

    template <typename TVal>
    TVar TVar::Opt(const Rt::TOpt<TVal> &that) {
      TOpt::TOptType val;
      if (that.IsKnown()) {
        val = TVar(that.GetVal());
      }
      return (new TOpt(val, Type::TDt<TVal>::GetType()))->AsVar();
    }

    /* TODO */
    template <typename TVal>
    struct TVar::TDt<Rt::TOpt<TVal>> {

      /* TODO */
      Rt::TOpt<TVal> static As(const TVar &that) {
        TOpt *ptr = dynamic_cast<TOpt *>(that.Impl.get());
        if (ptr) {
          if (ptr->GetVal().IsKnown()) {
            return Rt::TOpt<TVal>(TVar::TDt<TVal>::As(ptr->GetVal().GetVal()));
          } else {
            return *Rt::TOpt<TVal>::Unknown;
          }
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to opt. Var is not an opt.");
      }

    };  // TDt<Rt::TOpt<TVal>>

  }  // Var

}  // Stig

