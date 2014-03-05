/* <stig/key_generator.h>

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

#include <memory>

#include <stig/atom/kit2.h>
#include <stig/atom/suprena.h>
#include <stig/context_base.h>
#include <stig/rt/generator.h>

namespace Stig {

  /* TODO */
  class TKeyCursor {
    NO_COPY_SEMANTICS(TKeyCursor);
    public:

    /* TODO */
    virtual ~TKeyCursor() {}

    /* TODO */
    virtual operator bool() const = 0;

    /* TODO */
    virtual const Indy::TKey &operator*() const = 0;

    /* TODO */
    virtual const Indy::TKey *operator->() const = 0;

    /* TODO */
    virtual TKeyCursor &operator++() = 0;

    /* TODO */
    inline Atom::TCore::TArena *GetArena() {
      return Arena;
    }

    protected:

    /* TODO */
    TKeyCursor(Atom::TCore::TExtensibleArena *arena) : Arena(arena) {}

    /* TODO */
    Atom::TCore::TExtensibleArena *Arena;

  };  // TKeyCursor

  namespace L0 {

    /* TODO */
    class TPackageContext {
      NO_COPY_SEMANTICS(TPackageContext);
      public:

      /* TODO */
      virtual ~TPackageContext() {}

      /* TODO */
      virtual TKeyCursor *NewKeyCursor(TContextBase *context, const Indy::TIndexKey &pattern) const = 0;

      protected:

      TPackageContext() {}

    };  // TPackageContext

  }  // L0

  /* TODO */
  template <typename TRet>
  class TKeyGenerator
      : public Rt::TGenerator<TRet>,
        public std::enable_shared_from_this<TKeyGenerator<TRet>> {
    NO_COPY_SEMANTICS(TKeyGenerator);
    public:

    /* TODO */
    typedef std::shared_ptr<const TKeyGenerator> TPtr;

    //TODO: CODY: This cursor needs a rewrite. For now it is roughly copied from what was generated, as that seemed to work.
    class TCursor
        : public Base::TIter<const TRet> {
      public:

      /* TODO */
      typedef const TRet TVal;

      /* TODO */
      TCursor(TKeyGenerator::TPtr &ptr)
          : Cached(false), Valid(false), Item(0),
            /*Iter(&ptr->GetContext(), ptr->GetStart()),*/
            Iter(ptr->PackageContext->NewKeyCursor(&ptr->GetContext(), ptr->GetStart())),
            Ptr(ptr) {}

      /* TODO */
      TCursor(const TKeyGenerator::TPtr &ptr)
          : Cached(false), Valid(false), Item(0),
            /* Iter(&ptr->GetContext(), ptr->GetStart()), */
            Iter(ptr->PackageContext->NewKeyCursor(&ptr->GetContext(), ptr->GetStart())),
            Ptr(ptr) {}

      /* TODO */
      TCursor(const TCursor &that) = delete;

      /* TODO */
      TCursor(TCursor &&that)
          : Cached(that.Cached),
            Valid(that.Valid),
            Item(std::move(that.Item)),
            Iter(std::move(that.Iter)),
            Ptr(std::move(that.Ptr)) {
        that.Item = 0;
      }

      /* TODO */
      virtual ~TCursor() {
        if(Item) delete Item;
      }

      /* TODO */
      operator bool() const {
        assert(this);
        Refresh();
        assert(Cached);
        return Valid;
      }

      /* TODO */
      TVal &operator*() const {
        assert(this);
        Refresh();
        assert(Cached);
        assert(Valid);
        return *Item;
      }

      /* TODO */
      Base::TIter<TVal> &operator++() {
        assert(this);
        ++(*Iter);
        Cached = false;
        return *this;
      }

      private:

      /* TODO */
      inline void Refresh() const {
        assert(this);
        if (!Cached) {
          Cached = true;
          if (static_cast<bool>(*Iter)) {
            void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
            if (Item) {
              *Item = Sabot::AsNative<TRet>(*Sabot::State::TAny::TWrapper((*Iter)->GetState(state_alloc)));
            } else {
              Item = new TRet(Sabot::AsNative<TRet>(*Sabot::State::TAny::TWrapper((*Iter)->GetState(state_alloc))));
            }
            Valid = true;
          } else {
            Valid = false;
          }
        }
      }

      /* TODO */
      mutable bool Cached;

      /* TODO */
      mutable bool Valid;

      /* TODO */
      mutable TRet *Item;

      /* TODO */
      mutable std::unique_ptr<Stig::TKeyCursor> Iter;

      /* TODO */
      const TKeyGenerator::TPtr Ptr;

    }; // TCursor<TKeyGenerator<TRet>>

    /* TODO */
    virtual ~TKeyGenerator() {}

    /* TODO */
    TContextBase &GetContext() {
      assert(this);
      return Ctx;
    }

    /* TODO */
    TContextBase &GetContext() const {
      assert(this);
      return Ctx;
    }

    /* TODO */
    const Indy::TIndexKey &GetStart() const {
      assert(this);
      return Start;
    }

    /* TODO */
    virtual Base::TIterHolder<const TRet> NewCursor() const {
      assert(this);
      return MakeHolder(new TCursor(this->shared_from_this()));
    }

    /* TODO */
    TKeyGenerator(L0::TPackageContext *package_context, TContextBase &ctx,  const Sabot::State::TAny *start, const Base::TUuid &index_id)
        : PackageContext(package_context),
          Ctx(ctx),
          Start(index_id, Indy::TKey(Ctx.GetArena(), start)) {}

    private:

    /* TODO */
    L0::TPackageContext *PackageContext;

    /* TODO */
    TContextBase &Ctx;

    /* TODO */
    const Indy::TIndexKey Start;

  };  // TKeyGenerator

  namespace Var {

    template <typename TVal>
    struct TRead {

      static std::pair<bool, TVal> Do(const Indy::TKey &val) {
        void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
        return std::make_pair(true, Sabot::AsNative<TVal>(*Sabot::State::TAny::TWrapper(val.GetState(state_alloc))));
      }

    };  // TRead<TVal>

    template <typename TVal>
    struct TRead<Rt::TOpt<TVal>> {

      static std::pair<bool, Rt::TOpt<TVal>> Do(const Indy::TKey &val) {
        Atom::TCore void_comp;
        if (memcmp(&void_comp, &val.GetCore(), sizeof(void_comp)) != 0) {
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          return std::make_pair(true, Sabot::AsNative<TVal>(*Sabot::State::TAny::TWrapper(val.GetState(state_alloc))));
        } else {
          /* we have a void */
          return std::make_pair(false, *Rt::TOpt<TVal>::Unknown);
        }
        //syslog(LOG_ERR, "TODO: support reading optional");
        //throw std::runtime_error("TODO: support reading optional");
      }

    };  // TRead<TOpt<TVal>>
  }

  namespace Rt {

    /* TODO: This is an odd place for this. Oh well. */
    template <typename TRet, typename TAddr>
    TMutable<TAddr, TRet> Read(TContextBase &ctx, const TAddr &addr, const Base::TUuid &index_id) {
      assert(&ctx);
      assert(&addr);

      /* TODO: Copy copy copy copy copy! */
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      auto ret = Var::TRead<TRet>::Do(ctx[Indy::TIndexKey(index_id, Indy::TKey(Atom::TCore(ctx.GetArena(), Sabot::State::TAny::TWrapper(Native::State::New(addr, state_alloc))), ctx.GetArena()))]);
      /*
      std::cout << "Read [] = [";
      Sabot::State::TAny::TWrapper(Native::State::New(ret.second, state_alloc))->Accept(Sabot::TStateDumper(std::cout));
      std::cout << "]" << std::endl;
      */
      if  (ret.first) {
        return TMutable<TAddr, TRet>(TOpt<TAddr>(addr), ret.second);
      } else {
        return TMutable<TAddr, TRet>(TOpt<TAddr>(), ret.second);
      }
    };

    /* TODO: This is an odd place for this. Oh well. */
    template <typename TAddr>
    bool Exists(TContextBase &ctx, const TAddr &addr, const Base::TUuid &index_id) {
      assert(&ctx);
      assert(&addr);
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      return ctx.Exists(Indy::TIndexKey(index_id, Indy::TKey(Atom::TCore(ctx.GetArena(), Sabot::State::TAny::TWrapper(Native::State::New(addr, state_alloc))), ctx.GetArena())));
    }

  } // Rt

}  // Stig
