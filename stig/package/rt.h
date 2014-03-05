/* <stig/package/rt.h>

   TODO

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

//To get the flux TContext
#include <stig/spa/flux_capacitor/api.h>
#include <base/chrono.h>
#include <base/scheduler.h>
#include <stig/atom/kit2.h>
#include <stig/atom/suprena.h>
#include <stig/context_base.h>
#include <stig/key_generator.h>
#include <stig/rt/lazy_random_list.h>
#include <stig/rt/opt.h>
#include <stig/var/mutation.h>

namespace Stig {

  namespace Package {

    //NOTE: Most of the functions here are inline because they are small and we want them to be optimized out by the
    //compiler whenever possible.
    class TContext
        : public L0::TPackageContext {
      NO_COPY_SEMANTICS(TContext);
      public:

      /* A map from database keys to changes to all/part of the key */
      typedef std::unordered_map<Indy::TIndexKey, std::shared_ptr<Var::TChange>> TEffects;

      /* All the results of if statements inside the program. */
      typedef std::vector<bool> TPredicateResults;

      /* TODO */
      virtual ~TContext() {}

      /* Add an effect toe the effect map. Will throw if the key has been changed in a way incompatible with the given
         change. */
      template <typename TVal>
      void AddEffect(const TVal &addr, const Base::TUuid &index_id, const Var::TPtr<Var::TChange> &change) {
        assert(&addr);
        assert(&change);
        assert(change);
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        Indy::TIndexKey index_key(index_id, Indy::TKey(Atom::TCore(Arena, Sabot::State::TAny::TWrapper(Native::State::New(addr, state_alloc))), Arena));

        auto it = Effects.find(index_key);

        if (it == Effects.end()) {
          Effects.insert(make_pair(index_key, change));
        } else {
          it->second->Augment(change);
        }
      }

      /* Add a new predicate result to the vector of predicate results */
      void AddPredicateResult(bool pred) {
        assert(this);
        PredicateResults.push_back(pred);
      }

      /* Get the predicate results of an execution */
      inline const TPredicateResults &GetPredicateResults() const {
        assert(this);
        return PredicateResults;
      }

      /* Get the effect map. We move this rather than copying because when used it is always to take the effect set
         and shove it into a FluxCapacitor::TUpdate */
      inline TEffects &&MoveEffects() {
        assert(this);
        return std::move(Effects);
      }

      /* TODO */
      inline const TEffects &GetEffects() {
        assert(this);
        return Effects;
      }

      /* TODO */
      template <typename TRet, typename... TArgs>
      std::shared_ptr<const TKeyGenerator<TRet>> New(TContextBase &ctx, const Base::TUuid &index_id, const std::tuple<TArgs...> &start) {
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        return std::make_shared<const TKeyGenerator<TRet>>(this, ctx, Sabot::State::TAny::TWrapper(Native::State::New(start, state_alloc)).get(), index_id);
      }

      /* Get the FluxCapacitor::TContext. Used by things like KeyGenerators and reading values out of the database. */
      virtual Stig::TContextBase &GetFlux() = 0;

      /* TODO */
      virtual TKeyCursor *NewKeyCursor(TContextBase *context, const Indy::TIndexKey &pattern) const = 0;

      /* TODO */
      virtual TKeyCursor *NewKeyCursor(TContextBase *context, const Indy::TIndexKey &from, const Indy::TIndexKey &to) const = 0;

      /* Get the current session id. */
      inline const TUUID &GetSessionId() const {
        assert(this);
        return SessionId;
      }

      inline int64_t GetRandomInt(int64_t gen_id, int64_t min, int64_t max, int64_t idx) const {
        assert(this);
        assert(gen_id >= 0);
        assert(max >= min);
        assert(idx >= 0);

        //TODO: Make seed bigger so we have more possible random sequences.
        if(OptRandomSeed.IsUnknown()) {
          OptRandomSeed = std::random_device()();
          //TODO: make_unique.

        }
        if(!GenSeeds) {
          GenSeeds = std::unique_ptr<Rt::TLazyRandomList>(new Rt::TLazyRandomList(OptRandomSeed.GetVal()));
        }
        assert(GenSeeds);

        // Get the requested generator (Using unique_ptr to insert if not exists)
        auto &gen_ptr = RandomGenerators[std::make_tuple(gen_id, min, max)];
        if(!gen_ptr) {
          //TODO: make_unique.
          gen_ptr = std::unique_ptr<Rt::TLazyRandomList>(new Rt::TLazyRandomList(GenSeeds->Get(gen_id), min, max));
        }

        assert(gen_ptr);

        return gen_ptr->Get(idx);
      }

      inline const Rt::TOpt<uint32_t> &GetOptRandomSeed() const {
        assert(this);

        return OptRandomSeed;
      }

      /* Get the current user id. */
      inline const Rt::TOpt<TUUID> &GetUserId() const {
        assert(this);
        return UserId;
      }

      /* Return true iff the effect set is empty. Useful for determining if a database update is needed. */
      bool HasEffects() const {
        assert(this);
        return !Effects.empty();
      }

      /* Get the current time. Note that this function will always return the __same__ time. This is because within a
         single program execution in Stig the time is assumed to be constant.

         TODO: If the time is used, it should really be rolled into the CheckAsserts function capture... */
      inline Base::Chrono::TTimePnt Now() const {
        if (OptNow.IsUnknown()) {
          OptNow = Base::Chrono::Now();
        }
        return OptNow.GetVal();
      }

      const Rt::TOpt<Base::Chrono::TTimePnt> &GetOptNow() const {
        assert(this);
        return OptNow;
      }

      /* TODO */
      inline Atom::TCore::TExtensibleArena *GetArena();

      /* TODO */
      inline Base::TScheduler *GetScheduler() const;

      protected:

        /* TODO */
      TContext(const Rt::TOpt<TUUID> &user_id,
        const TUUID &session_id,
        Atom::TCore::TExtensibleArena *arena,
        Base::TScheduler *scheduler,
        Rt::TOpt<Base::Chrono::TTimePnt> now,
        Rt::TOpt<uint32_t> random_seed)
            : Arena(arena), UserId(user_id), SessionId(session_id), Scheduler(scheduler),
              OptNow(now), OptRandomSeed(random_seed) {}

      /* TODO */
      mutable Atom::TCore::TExtensibleArena *Arena;

      private:

      /* TODO */
      TEffects Effects;

      /* TODO */
      TPredicateResults PredicateResults;

      /* TODO */
      Rt::TOpt<TUUID> UserId;

      /* TODO */
      TUUID SessionId;

      /* TODO */
      Base::TScheduler *Scheduler;

      // Lazily generated members.
      // Ideally we would just get these details from the compiler.
      /* TODO */
      mutable Rt::TOpt<Base::Chrono::TTimePnt> OptNow;

      mutable Rt::TOpt<uint32_t> OptRandomSeed;

      // Generators are aliased by id, min, and max.
      using TGenIdx = std::tuple<int64_t, int64_t, int64_t>;

      mutable std::unique_ptr<Rt::TLazyRandomList> GenSeeds;
      mutable std::unordered_map<TGenIdx, std::unique_ptr<Rt::TLazyRandomList>> RandomGenerators;

    };  // TContext

    /* TODO */
    class TSpaContext
          : public TContext {
      NO_COPY_SEMANTICS(TSpaContext);
      public:

      /* TODO */
      TSpaContext(
        const Rt::TOpt<TUUID> &user_id,
        const TUUID &session_id,
        Spa::FluxCapacitor::TContext &context,
        Atom::TCore::TExtensibleArena *arena,
        Base::TScheduler *scheduler,
        Rt::TOpt<Base::Chrono::TTimePnt> now,
        Rt::TOpt<uint32_t> random_seed)
          : TContext(user_id, session_id, arena, scheduler, now, random_seed), FluxContext(context) {}

      /* TODO */
      virtual Stig::TContextBase &GetFlux() override {
        assert(this);
        return FluxContext;
      }

      /* TODO */
      virtual TKeyCursor *NewKeyCursor(TContextBase *context, const Indy::TIndexKey &pattern) const override {
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        auto flux_context = dynamic_cast<Spa::FluxCapacitor::TContext *>(context);
        Var::TVar var_pattern = Var::ToVar(*Sabot::State::TAny::TWrapper(pattern.GetKey().GetState(state_alloc)));
        return new Spa::FluxCapacitor::TContext::TKeyCursor(flux_context, Arena, var_pattern, pattern.GetIndexId());
      }

      /* TODO */
      virtual TKeyCursor *NewKeyCursor(TContextBase * /*context*/, const Indy::TIndexKey &/*from*/, const Indy::TIndexKey &/*to*/) const override {
        throw std::logic_error("TODO: Implement TSpaContext NewKeyCursor range");
      }

      private:

      /* TODO */
      Spa::FluxCapacitor::TContext &FluxContext;

    };  // TSpaContext

    /*** Inline ***/
    inline Atom::TCore::TExtensibleArena *TContext::GetArena() {
      assert(this);
      return Arena;
    }

    inline Base::TScheduler *TContext::GetScheduler() const {
      assert(this);
      assert(Scheduler);
      return Scheduler;
    }

  } // Package

  namespace Rt {

    //TODO: This is a horrible place for this
    template <typename TRet, typename TFuncTrue, typename TFuncFalse>
    TRet Predicate(Package::TContext &ctx, bool predicate, const TFuncTrue &true_func,
      const TFuncFalse &false_func) {
      ctx.AddPredicateResult(predicate);
      return predicate ? true_func() : false_func();
    }

  } // Rt

} // Stig