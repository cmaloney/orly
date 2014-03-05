/* <stig/rt/mutable.h>

   Mutable values. A mutable may or may not have an address, and will always have a value. People can talk about it
   as just it's value type, or can peer into the address by asking nicely.

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

#include <vector>
#include <utility>

#include <stig/rt/opt.h>

namespace Stig {

  namespace Var {

    class TVar;

  }  // Var

  namespace Rt {

    typedef std::vector<Var::TVar> TMutableParts;

    template <typename TAddr, typename TVal>
    class TMutable {
      public:

      typedef std::vector<Var::TVar> TParts;

      template <typename TCompatAddr, typename TCompatVal>
      TMutable(const TCompatAddr &addr, const TCompatVal &val)
          : Addr(addr), Val(val) {}

      template <typename TCompatAddr, typename TCompatVal>
      TMutable(const Rt::TOpt<TCompatAddr> &addr, const TCompatVal &val)
          : Addr(addr), Val(val) {}

      template <typename TCompatAddr, typename TCompatParts, typename TCompatVal>
      TMutable(const TCompatAddr &addr, const TCompatParts &parts, const TCompatVal &val)
          : Addr(addr), Parts(parts), Val(val) {}

      template <typename TCompatAddr, typename TCompatParts, typename TCompatVal>
      TMutable(const Rt::TOpt<TCompatAddr> &addr, const TCompatParts &parts, const TCompatVal &val)
          : Addr(addr), Parts(parts), Val(val) {}

      const TAddr &GetAddr() const {
        assert(this);
        return Addr.GetVal();
      }

      const Rt::TOpt<TAddr> &GetOptAddr() const {
        assert(this);
        return Addr;
      }

      const TParts &GetParts() const {
        assert(this);
        return Parts;
      }

      const TVal &GetVal() const {
        assert(this);
        return Val;
      }

      size_t GetHash() const {
        assert(this);
        //TODO: Write a better hash function.
        return std::hash<TVal>()(Val) ^ std::hash<Rt::TOpt<TAddr>>()(Addr);
      }

      bool HasAddr() const {
        assert(this);
        return Addr;
      }

      operator TVal() const {
        assert(this);
        return Val;
      }

      private:

      Rt::TOpt<TAddr> Addr;

      TParts Parts;

      TVal Val;

    };  // TMutable<TAddr, TVal>

    template <typename TAddr, typename TVal>
    TMutable<TAddr, TVal> MakeMutable(
          const TAddr &addr,
          const TMutableParts &parts,
          const TVal &val) {
      return TMutable<TAddr, TVal>(Rt::TOpt<TAddr>(addr), parts, val);
    }


    template <typename TAddr, typename TVal>
    TMutable<TAddr, TVal> MakeMutable(
          const TOpt<TAddr> &addr,
          const TMutableParts &parts,
          const TVal &val) {
      return TMutable<TAddr, TVal>(Rt::TOpt<TAddr>(addr), parts, val);
    }

    template <typename TAddr, typename TVal, typename TResVal>
    TMutable<TAddr, TResVal> RewrapMutable(const TMutable<TAddr, TVal> &src_mutable, const TMutableParts &parts,
          const TResVal &res_val) {

      assert(&src_mutable);
      assert(&parts);
      assert(&res_val);
      TMutableParts final_parts = src_mutable.GetParts();
      final_parts.insert(final_parts.end(), parts.begin(), parts.end());
      return TMutable<TAddr, TResVal>(src_mutable.GetOptAddr(), final_parts, res_val);
    }


    template <typename TAddr, typename TVal, typename TResVal>
    TMutable<TAddr, TResVal> RewrapMutable(const TMutable<TAddr, TVal> &src_mutable, const Var::TVar &part,
          const TResVal &res_val) {

      assert(&src_mutable);
      assert(&part);
      assert(&res_val);
      TMutableParts final_parts = src_mutable.GetParts();
      final_parts.push_back(part);
      return TMutable<TAddr, TResVal>(src_mutable.GetOptAddr(), final_parts, res_val);
    }


  }  // Rt

}  // Stig

namespace std {

  template <typename TAddr, typename TVal>
  struct hash<Stig::Rt::TMutable<TAddr, TVal>> {

    typedef size_t result_type;
    typedef Stig::Rt::TMutable<TAddr, TVal> argument_type;

    result_type operator()(const argument_type &that) const {
      return that.GetHash();
    }

  };  // hash<Stig::Rt::TMutable<TAddr, TVal>>

}  // std
