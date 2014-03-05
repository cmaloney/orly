/* <stig/var/dict.h>

   A Stig dict, which is a Rt::TDict.

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

    class TDict
        : public TVar::TImpl {
      public:

      /* TODO */
      typedef Rt::TDict<TVar, TVar> TDictType;
      typedef TDictType TElems;

      /* TODO */
      virtual Var::TVar &Index(const TVar &);

      /* TODO */
      virtual TDict &Add(const TVar &);

      /* TODO */
      virtual TDict &And(const TVar &);

      /* TODO */
      virtual TDict &Div(const TVar &);

      /* TODO */
      virtual TDict &Exp(const TVar &);

      /* TODO */
      virtual TDict &Intersection(const TVar &);

      /* TODO */
      virtual TDict &Mod(const TVar &);

      /* TODO */
      virtual TDict &Mult(const TVar &);

      /* TODO */
      virtual TDict &Or(const TVar &);

      /* TODO */
      virtual TDict &Sub(const TVar &);

      /* TODO */
      virtual TDict &SymmetricDiff(const TVar &);

      /* TODO */
      virtual TDict &Union(const TVar &);

      /* TODO */
      virtual TDict &Xor(const TVar &);

      /* TODO */
      const TDictType &GetVal() const {
        assert(this);
        return Val;
      }

      /* TODO */
      void Insert(const TDictType::const_iterator &begin, const TDictType::const_iterator &end);

      /* TODO */
      void Remove(const Rt::TSet<TVar> &keys);

      /* TODO */
      const Type::TType &GetKeyType() const {
        assert(this);
        return KeyType;
      }

      /* TODO */
      const Type::TType &GetValType() const {
        assert(this);
        return ValType;
      }

      /* TODO */
      virtual void Touch();

      /* TODO */
      virtual size_t GetHash() const;

      /* TODO */
      virtual Type::TType GetType() const;

      /* TODO */
      virtual void Write(std::ostream &) const;

      private:

      /* TODO */
      #if defined(STIG_HOST)
      template <typename TVal, typename TKey>
      TDict(const Rt::TDict<TKey, TVal> &that) : KeyType(Type::TDt<TKey>::GetType()), ValType(Type::TDt<TVal>::GetType()) {
        for (auto iter = that.begin(); iter != that.end(); ++iter) {
          Val[TVar(iter->first)] = TVar(iter->second);
        }
        SetHash();
      }
      #endif

      /* TODO */
      TDict(const Rt::TDict<TVar, TVar> &that, const Type::TType &key_type, const Type::TType &val_type);

      /* TODO */
      virtual ~TDict();

      /* TODO */
      virtual void Accept(const TVisitor &visitor) const;

      /* TODO */
      virtual TVar Copy() const;

      /* TODO */
      void SetHash();

      /* TODO */
      TDictType Val;

      /* TODO */
      Type::TType KeyType;

      /* TODO */
      Type::TType ValType;

      /* TODO */
      size_t Hash;

      /* TODO */
      friend class TVar;

    };  // TDict

    /* TODO */
    template <typename TKey, typename TVal>
    TVar TVar::Dict(const Rt::TDict<TKey, TVal> &that) {
      Rt::TDict<TVar, TVar> val;
      for (auto iter = that.begin(); iter != that.end(); ++iter) {
        val[TVar(iter->first)] = TVar(iter->second);
      }
      return (new TDict(val, Type::TDt<TKey>::GetType(), Type::TDt<TVal>::GetType()))->AsVar();
    }

    /* TODO */
    template<typename TKey, typename TVal>
    struct TVar::TDt<Rt::TDict<TKey, TVal>> {

      /* TODO */
      Rt::TDict<TKey, TVal> static As(const TVar &that) {
        TDict *ptr = dynamic_cast<TDict *>(that.Impl.get());
        if (ptr) {
          Rt::TDict<TKey, TVal> map_;
          for (auto iter = ptr->GetVal().begin(); iter != ptr->GetVal().end(); ++iter) {
            map_[TVar::TDt<TKey>::As(iter->first)] = TVar::TDt<TVal>::As(iter->second);
          }
          return map_;
        }
        std::cerr << "Var is a " << that.GetType() << std::endl;
        throw Rt::TSystemError(HERE, "Trying to cast dynamic Var to map. Var is not a map.");
      }

    };  // TDt<Rt::TDict<TKey, TVal>>

  }  // Var

}  // Stig