/* <stig/var/addr.h>

   A Stig addr, which is a Stig::Rt::TAddr.

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
#include <stig/shared_enum.h>
#include <stig/type/rt.h>
#include <stig/var/impl.h>

namespace Stig {

  namespace Var {

    class TAddr
        : public TVar::TImpl {
      public:

      typedef TAddrDir TDir;

      /* TODO */
      typedef std::vector<std::pair<TDir, TVar>> TAddrType;
      typedef TAddrType TElems;

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TAddr &Add(const TVar &);

      /* TODO */
      virtual TAddr &And(const TVar &);

      /* TODO */
      virtual TAddr &Div(const TVar &);

      /* TODO */
      virtual TAddr &Exp(const TVar &);

      /* TODO */
      virtual TAddr &Intersection(const TVar &);

      /* TODO */
      virtual TAddr &Mod(const TVar &);

      /* TODO */
      virtual TAddr &Mult(const TVar &);

      /* TODO */
      virtual TAddr &Or(const TVar &);

      /* TODO */
      virtual TAddr &Sub(const TVar &);

      /* TODO */
      virtual TAddr &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TAddr &Union(const TVar &);

      /* TODO */
      virtual TAddr &Xor(const TVar &);

      /* TODO */
      const TAddrType &GetVal() const {
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
      virtual void Write(std::ostream &strm) const;

      private:

      /* TODO
      template <typename... TElements>
      TAddr(const Rt::TAddr<TElements...> &that) {
        Unroll(that.GetSuper());
        SetHash();
      }*/

      /* TODO */
      TAddr(const TAddrType &that);

      /* TODO */
      virtual ~TAddr();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      void SetHash();

      /* TODO
      template <size_t Pos, typename... TElements>
      void Unroll(const Rt::TAddrImpl<Pos, TElements...> &that);

      template <size_t Pos>
      void Unroll(const Rt::TAddrImpl<Pos> &) {
        assert(this);
      }

      template <size_t Pos, typename THead, typename... TTail>
      void Unroll(const Rt::TAddrImpl<Pos, THead, TTail...> &that) {
        assert(this);
        Val.push_back(std::make_pair(that.GetDir(), TVar(that.GetHead())));
        TypeVec.push_back(Type::TDt<THead>::GetType());
        Unroll(that.GetTail());
      }

      template <size_t Pos>
      static void Unroll(TAddrType &, const Rt::TAddrImpl<Pos> &) {}

      template <size_t Pos, typename THead, typename... TTail>
      static void Unroll(TAddrType &val, const Rt::TAddrImpl<Pos, THead, TTail...> &that) {
        val.push_back(std::make_pair(that.GetDir(), TVar(that.GetHead())));
        Unroll(val, that.GetTail());
      }
      */

      /* TODO */
      TAddrType Val;

      /* TODO */
      std::vector<std::pair<TDir, Type::TType>> TypeVec; //TODO: Can I get this from TType?

      /* TODO */
      size_t Hash;

      /* TODO */
      friend class TVar;

    };  // TAddr

    /* TODO
    template <typename... TElements>
    TVar TVar::Addr(const Rt::TAddr<TElements...> &that) {
      TAddr::TAddrType val;
      TAddr::Unroll(val, that.GetSuper());
      return (new TAddr(val))->AsVar();
    } */

  }  // Var

  namespace Rt {

    /* TODO
    template <size_t Pos, typename THead_, typename... TTail>
    TAddrImpl<Pos, THead_, TTail...>::TAddrImpl(const Var::TAddr::TAddrType &vec)
        : TSuper(vec), Head(Var::TVar::TDt<typename THead_::TElem>::As(vec[Pos].second)) {}

    template <typename THead, typename... TTail>
    TAddr<THead, TTail...>::TAddr(const Var::TAddr::TAddrType &vec) : TSuper(vec) {}
    */

  }  // Rt

  namespace Var {

    /* TODO
    template<typename... TElements>
    struct TVar::TDt<Rt::TAddr<TElements...>> {

      static Rt::TAddr<TElements...> As(const TVar &that) {
        TAddr *ptr = dynamic_cast<TAddr *>(that.Impl.get());
        if (ptr) {
          return Rt::TAddr<TElements...>(ptr->GetVal());
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to addr. Var is not an addr.");
      }

    };  // TDt<std::vector<TVal>>
    */

  }  // Var

}  // Stig