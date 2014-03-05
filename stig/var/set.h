/* <stig/var/set.h>

   A Stig set, which is a Rt::TSet.

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

#include <stig/rt/containers.h>
#include <stig/rt/runtime_error.h>
#include <stig/type/rt.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TSet
        : public TVar::TImpl {
      public:

      /* TODO */
      typedef Rt::TSet<TVar> TSetType;

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TSet &Add(const TVar &);

      /* TODO */
      virtual TSet &And(const TVar &);

      /* TODO */
      virtual TSet &Div(const TVar &);

      /* TODO */
      virtual TSet &Exp(const TVar &);

      /* TODO */
      virtual TSet &Intersection(const TVar &);

      /* TODO */
      virtual TSet &Mod(const TVar &);

      /* TODO */
      virtual TSet &Mult(const TVar &);

      /* TODO */
      virtual TSet &Or(const TVar &);

      /* TODO */
      virtual TSet &Sub(const TVar &);

      /* TODO */
      virtual TSet &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TSet &Union(const TVar &);

      /* TODO */
      virtual TSet &Xor(const TVar &);

      /* TODO */
      const Type::TType &GetElemType() const {
        assert(this);
        return Type;
      }

      /* TODO */
      const TSetType &GetVal() const {
        assert(this);
        return Val;
      }

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      virtual void Write(std::ostream &) const;

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      virtual void Touch();

      private:

      /* TODO */
      #if defined(STIG_HOST)
      template <typename TVal>
      TSet(const Rt::TSet<TVal> &that) : Type(Type::TDt<TVal>::GetType()) {
        for (auto iter = that.begin(); iter != that.end(); ++iter) {
          Val.insert(TVar(*iter));
        }
        SetHash();
      }
      #endif

      /* TODO */
      TSet(const Rt::TSet<TVar> &that, const Type::TType &type);

      /* TODO */
      virtual ~TSet();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      void SetHash();

      /* TODO */
      TSetType Val;

      /* TODO */
      Type::TType Type;

      /* TODO */
      size_t Hash;

      /* TODO */
      friend class TVar;

    };  // TSet

    /* TODO */
    template <typename TVal>
    TVar TVar::Set(const Rt::TSet<TVal> &that) {
      Rt::TSet<TVar> val;
      for (auto iter = that.begin(); iter != that.end(); ++iter) {
        val.insert(TVar(*iter));
      }
      return (new TSet(val, Type::TDt<TVal>::GetType()))->AsVar();
    }

    /* TODO */
    template<typename TVal>
    struct TVar::TDt<Rt::TSet<TVal>> {

      /* TODO */
      Rt::TSet<TVal> static As(const TVar &that) {
        TSet *ptr = dynamic_cast<TSet *>(that.Impl.get());
        if (ptr) {
          Rt::TSet<TVal> set_;
          for (auto iter = ptr->GetVal().begin(); iter != ptr->GetVal().end(); ++iter) {
            set_.insert(TVar::TDt<TVal>::As(*iter));
          }
          return set_;
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to set. Var is not a set.");
      }

    };  // TDt<Rt::TSet<TVal>>

  }  // Var

}  // Stig