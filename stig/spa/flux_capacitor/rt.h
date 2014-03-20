/* <stig/spa/flux_capacitor/rt.h>

   a Stig::Rt::TGenerator which generates database keys.

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

#include <memory>
#include <utility>

#include <stig/rt/generator.h>
#include <stig/rt/opt.h>
#include <stig/spa/flux_capacitor/flux_capacitor.h>
#include <stig/var/impl.h>


namespace Stig {

  namespace Spa {

    namespace FluxCapacitor {

      template <typename TRet>
      class TKeyGenerator : public Rt::TGenerator<TRet>, public std::enable_shared_from_this<TKeyGenerator<TRet>> {
        NO_COPY_SEMANTICS(TKeyGenerator);
        public:

        typedef std::shared_ptr<const TKeyGenerator> TPtr;

        static TPtr New(TContext &ctx,  const Var::TVar &start) {
          return TPtr(new TKeyGenerator(ctx, start));
        }

        //TODO: This cursor needs a rewrite. For now it is roughly copied from what was generated, as that seemed to work.
        class TCursor : public Base::TIter<const TRet> {

          public:
          typedef const TRet TVal;

          TCursor(TKeyGenerator::TPtr &ptr) : Item(0), Iter(&ptr->GetContext(), ptr->GetStart()), Ptr(ptr) {}
          TCursor(const TKeyGenerator::TPtr &ptr) : Item(0), Iter(&ptr->GetContext(), ptr->GetStart()), Ptr(ptr) {}
          TCursor(const TCursor &that) = delete;
          TCursor(TCursor &&that) : Item(std::move(that.Item)), Iter(std::move(that.Iter)), Ptr(std::move(that.Ptr)) {
            that.Item = 0;
          }

          ~TCursor() {
            if(Item) delete Item;
          }

          operator bool() const {
            assert(this);
            if (Iter) {
              void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
              if (Item) {
                *Item = Sabot::AsNative<TRet>(Sabot::State::TAny::TWrapper(Iter->GetState(state_alloc)));
                //*Item = Var::DynamicCast<TRet>(Iter->GetKey());
              } else {
                Item = new TRet(Sabot::AsNative<TRet>(Sabot::State::TAny::TWrapper(Iter->GetState(state_alloc))));
                //Item = new TRet(Var::DynamicCast<TRet>(Iter->GetKey()));
              }
              return true;
            }
            return false;
          }

          TVal &operator*() const {
            assert(this);

            return *Item;
          }

          Base::TIter<TVal> &operator++() {
            assert(this);

            ++Iter;
            return *this;
          }

          private:

          mutable TRet *Item;
          mutable Spa::FluxCapacitor::TContext::TKeyCursor Iter;
          const TKeyGenerator::TPtr Ptr;
        }; // TCursor<TKeyGenerator<TRet>>

        TContext &GetContext() {
          assert(this);
          return Ctx;
        }

        TContext &GetContext() const {
          assert(this);
          return Ctx;
        }

        const Var::TVar &GetStart() const {
          assert(this);
          return Start;
        }

        Base::TIterHolder<const TRet> NewCursor() const {
          assert(this);
          return MakeHolder(new TCursor(this->shared_from_this()));
        }

        private:
        TKeyGenerator(TContext &ctx,  const Var::TVar &start) : Ctx(ctx), Start(start) {
          //NOTE: We can't dynamic cast start to TRet here to assert they match shape because start could contain
          //free statements, and that would cause problems.
        }

        TContext &Ctx;
        const Var::TVar Start;
      }; //TKeyGenerator<TReet>

    } // FluxCapacitor

  } // Spa

  /* moved to stig/key_generator.h */
  #if 0
  namespace Var {

    template <typename TVal>
    struct TRead {

      static std::pair<bool, TVal> Do(const Var::TVar &var) {
        if (!var) {
          throw Rt::TSystemError(HERE, "Reading key that does not exist or has been deleted.");
        }
        return std::make_pair(true, TVar::TDt<TVal>::As(var));
      }

    };  // TRead<TVal>

    template <typename TVal>
    struct TRead<Rt::TOpt<TVal>> {

      static std::pair<bool, Rt::TOpt<TVal>> Do(const Var::TVar &var) {
        if (!var) {
          return std::make_pair(false, *Rt::TOpt<TVal>::Unknown);
        }
        TOpt *ptr = dynamic_cast<TOpt *>(var.Impl.get());
        if (ptr) {
          return std::make_pair(true, TVar::TDt<Rt::TOpt<TVal>>::As(var));
        }
        return std::make_pair(true, TVar::TDt<TVal>::As(var));
      }

    };  // TRead<TOpt<TVal>>
  }

  namespace Rt {

    /* TODO: This is an odd place for this. Oh well. */
    template <typename TRet, typename TAddr>
    TMutable<TAddr, TRet> Read(Spa::FluxCapacitor::TContext &ctx, const TAddr &addr) {
      assert(&ctx);
      assert(&addr);

      /* TODO: Copy copy copy copy copy! */
      auto ret = Var::TRead<TRet>::Do(ctx[Spa::FluxCapacitor::TKV(Var::TVar::Addr(addr))]);
      if  (ret.first) {
        return TMutable<TAddr, TRet>(TOpt<TAddr>(addr), ret.second);
      } else {
        return TMutable<TAddr, TRet>(TOpt<TAddr>(), ret.second);
      }
    };

    /* TODO: This is an odd place for this. Oh well. */
    template <typename TAddr>
    bool Exists(Spa::FluxCapacitor::TContext &ctx, const TAddr &addr) {
      assert(&ctx);
      assert(&addr);

      return ctx.Exists(Spa::FluxCapacitor::TKV(Var::TVar::Addr(addr)));
    }

  } // Rt
  #endif

} // Stig