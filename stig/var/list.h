/* <stig/var/list.h>

   A Stig list, which is a std::vector.

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

    class TList
        : public TVar::TImpl {
      public:

      /* TODO */
      typedef std::vector<TVar> TListType;
      typedef TListType TElems;

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TList &Add(const TVar &);

      /* TODO */
      virtual TList &And(const TVar &);

      /* TODO */
      virtual TList &Div(const TVar &);

      /* TODO */
      virtual TList &Exp(const TVar &);

      /* TODO */
      virtual TList &Intersection(const TVar &);

      /* TODO */
      virtual TList &Mod(const TVar &);

      /* TODO */
      virtual TList &Mult(const TVar &);

      /* TODO */
      virtual TList &Or(const TVar &);

      /* TODO */
      virtual TList &Sub(const TVar &);

      /* TODO */
      virtual TList &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TList &Union(const TVar &);

      /* TODO */
      virtual TList &Xor(const TVar &);

      /* TODO */
      const Type::TType &GetElemType() const {
        assert(this);
        return Type;
      }

      /* TODO */
      const TListType &GetVal() const {
        assert(this);
        return Val;
      }

      /* TODO */
      void Append(const TListType &other);

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
      #if defined(STIG_HOST)
      template <typename TVal>
      TList(const std::vector<TVal> &that) : Type(Type::TDt<TVal>::GetType()) {
        for (auto iter = that.begin(); iter != that.end(); ++iter) {
          Val.push_back(TVar(*iter));
        }
        SetHash();
      }
      #endif

      /* TODO */
      TList(const std::vector<TVar> &that, const Type::TType &type);

      /* TODO */
      virtual ~TList();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      void SetHash();

      /* TODO */
      TListType Val;

      /* TODO */
      Type::TType Type;

      /* TODO */
      size_t Hash;

      /* TODO */
      friend class TVar;

    };  // TList

    /* TODO */
    template <typename TVal>
    TVar TVar::List(const std::vector<TVal> &that) {
      std::vector<TVar> val;
      for (auto iter = that.begin(); iter != that.end(); ++iter) {
        val.push_back(TVar(*iter));
      }
      return (new TList(val, Type::TDt<TVal>::GetType()))->AsVar();
    }

    /* TODO */
    template<typename TVal>
    struct TVar::TDt<std::vector<TVal>> {

      /* TODO */
      std::vector<TVal> static As(const TVar &that) {
        TList *ptr = dynamic_cast<TList *>(that.Impl.get());
        if (ptr) {
          std::vector<TVal> vec;
          for (auto iter = ptr->GetVal().begin(); iter != ptr->GetVal().end(); ++iter) {
            vec.push_back(TVar::TDt<TVal>::As(*iter));
          }
          return vec;
        }
        std::cerr<< "TYPE NAME: " << typeid(*that.Impl.get()).name() << std::endl;
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to list. Var is not a list.");
      }

    };  // TDt<std::vector<TVal>>

  }  // Var

}  // Stig
