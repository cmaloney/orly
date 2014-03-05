/* <stig/indy/key.h>

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

#include <iostream> /* TODO GET RID OF */

#include <stig/atom/kit2.h>
#include <stig/sabot/get_hash.h>
#include <stig/sabot/order_states.h>
#include <stig/sabot/state_dumper.h>

namespace Stig {

  namespace Indy {

    /* TODO */
    class TKey {
      public:

      /* TODO */
      explicit inline TKey(Atom::TCore::TArena *arena = nullptr);

      /* TODO */
      explicit inline TKey(const Atom::TCore &core, Atom::TCore::TArena *arena = nullptr);

      /* TODO */
      inline TKey(const TKey &that);

      /* TODO */
      inline TKey(TKey &&that);

      /* TODO */
      template <typename TVal>
      inline TKey(const TVal &val, Atom::TCore::TExtensibleArena *fast_arena, void *state_alloc);

      /* Copy from the given key into my arena */
      inline TKey(Atom::TCore::TExtensibleArena *fast_arena, void *state_alloc, const TKey &that);

      /* Construct from sabot. */
      inline TKey(Atom::TCore::TExtensibleArena *fast_arena, const Sabot::State::TAny *state);

      /* Concat tuples */
      inline TKey(Atom::TCore::TExtensibleArena *fast_arena, const Sabot::State::TAny *lhs, const Sabot::State::TAny *rhs);

      /* TODO */
      inline TKey &operator=(TKey &&that);

      /* TODO */
      inline TKey &operator=(const TKey &that);

      /* TODO */
      inline bool operator==(const TKey &that) const;

      /* TODO */
      inline bool operator!=(const TKey &that) const;

      /* TODO */
      inline bool operator<(const TKey &that) const;

      /* TODO */
      inline bool operator<=(const TKey &that) const;

      /* TODO */
      inline bool operator>(const TKey &that) const;

      /* TODO */
      inline bool operator>=(const TKey &that) const;

      /* TODO */
      inline Atom::TComparison Compare(const TKey &that) const;

      /* TODO */
      inline void Dump(std::ostream &strm) const;

      /* TODO */
      inline Atom::TCore::TArena *GetArena() const;

      /* TODO */
      inline const Atom::TCore &GetCore() const;

      /* TODO */
      inline Atom::TCore &GetCore();

      /* TODO */
      inline Sabot::State::TAny *GetState(void *state_alloc) const;

      /* TODO */
      inline size_t GetHash() const;

      /* TODO */
      static inline bool EqEq(const Atom::TCore &lhs, Atom::TCore::TArena *lhs_arena, const Atom::TCore &rhs, Atom::TCore::TArena *rhs_arena);

      /* TODO */
      static inline bool TupleEqEq(const Atom::TCore &lhs, Atom::TCore::TArena *lhs_arena, const Atom::TCore &rhs, Atom::TCore::TArena *rhs_arena);

      /* TODO */
      static inline bool NeEq(const Atom::TCore &lhs, Atom::TCore::TArena *lhs_arena, const Atom::TCore &rhs, Atom::TCore::TArena *rhs_arena);

      /* TODO */
      static inline bool TupleNeEq(const Atom::TCore &lhs, Atom::TCore::TArena *lhs_arena, const Atom::TCore &rhs, Atom::TCore::TArena *rhs_arena);

      /* TODO */
      static inline Atom::TComparison Compare(const Atom::TCore &lhs, Atom::TCore::TArena *lhs_arena, const Atom::TCore &rhs, Atom::TCore::TArena *rhs_arena);

      private:

      /* TODO */
      Atom::TCore::TArena *Arena;

      /* TODO */
      Atom::TCore Core;

      /* TODO */
      mutable bool HashIsCached;
      mutable size_t CachedHash;

    };  // TKey

    /* TODO */
    class TIndexKey {
      public:

      /* TODO */
      TIndexKey() {}

      /* TODO */
      TIndexKey(const Base::TUuid &index_id, const TKey &key)
          : IndexId(index_id),
            Key(key) {}

      /* TODO */
      inline const Base::TUuid &GetIndexId() const {
        assert(this);
        return IndexId;
      }

      /* TODO */
      inline const TKey &GetKey() const {
        assert(this);
        return Key;
      }

      /* TODO */
      inline TKey &GetKey() {
        assert(this);
        return Key;
      }

      /* TODO */
      inline size_t GetHash() const {
        assert(this);
        return IndexId.GetHash() ^ Key.GetHash();
      }

      /* TODO */
      bool operator<(const TIndexKey &that) const {
        Atom::TComparison comp = Atom::CompareOrdered(IndexId, that.IndexId);
        switch (comp) {
          case Atom::TComparison::Lt: {
            return true;
          }
          case Atom::TComparison::Eq: {
            return Key < that.Key;
          }
          case Atom::TComparison::Gt: {
            return false;
          }
          case Atom::TComparison::Ne: {
            throw std::logic_error("CompareOrdered should not return Ne");
          }
        }
        throw;
      }

      /* TODO */
      bool operator==(const TIndexKey &that) const {
        return IndexId == that.IndexId && Key == that.Key;
      }

      /* TODO */
      bool operator!=(const TIndexKey &that) const {
        return IndexId != that.IndexId || Key != that.Key;
      }

      private:

      /* TODO */
      Base::TUuid IndexId;

      /* TODO */
      TKey Key;

    };  // TIndexKey

    /* A standard stream inserter for Stig::Indy::TKey. */
    inline std::ostream &operator<<(std::ostream &strm, const TKey &that) {
      assert(&that);
      that.Dump(strm);
      return strm;
    }

  }  // Indy

}  // Stig

namespace std {

  /* A standard hasher for Stig::Indy::TKey. */
  template <>
  struct hash<Stig::Indy::TKey> {
    typedef size_t result_type;
    typedef Stig::Indy::TKey argument_type;
    size_t operator()(const Stig::Indy::TKey &that) const {
      return that.GetHash();
    }
  };

  /* A standard hasher for Stig::Indy::TIndexKey. */
  template <>
  struct hash<Stig::Indy::TIndexKey> {
    typedef size_t result_type;
    typedef Stig::Indy::TIndexKey argument_type;
    size_t operator()(const Stig::Indy::TIndexKey &that) const {
      return that.GetHash();
    }
  };

}  // std

namespace Stig {

  namespace Indy {

    /* Inline */

    inline TKey::TKey(Atom::TCore::TArena *arena)
        : Arena(arena),
          HashIsCached(false),
          CachedHash(0UL) {}

    inline TKey::TKey(const Atom::TCore &core, Atom::TCore::TArena *arena)
        : Arena(arena),
          Core(core),
          HashIsCached(false),
          CachedHash(0UL) {}

    template <>
    inline TKey::TKey(const Atom::TCore &/*that*/, Atom::TCore::TExtensibleArena */*fast_arena*/, void */*state_alloc*/) = delete;

    template <typename TVal>
    inline TKey::TKey(const TVal &val, Atom::TCore::TExtensibleArena *fast_arena, void *state_alloc)
        : Arena(Base::AssertTrue(fast_arena)),
          Core(val, fast_arena, state_alloc),
          HashIsCached(false),
          CachedHash(0UL) {}

    inline TKey::TKey(Atom::TCore::TExtensibleArena *fast_arena, void *state_alloc, const TKey &that)
        : Arena(Base::AssertTrue(fast_arena)),
          Core(fast_arena, state_alloc, that.Arena, that.Core),
          HashIsCached(false),
          CachedHash(0UL) {}

    inline TKey::TKey(Atom::TCore::TExtensibleArena *fast_arena, const Sabot::State::TAny *state)
        : Arena(Base::AssertTrue(fast_arena)), Core(fast_arena, state),
          HashIsCached(false),
          CachedHash(0UL) {}

    inline TKey::TKey(Atom::TCore::TExtensibleArena *fast_arena, const Sabot::State::TAny *lhs, const Sabot::State::TAny *rhs)
        : Arena(Base::AssertTrue(fast_arena)),
          Core(fast_arena, lhs, rhs),
          HashIsCached(false),
          CachedHash(0UL) {}

    inline TKey::TKey(const TKey &that) : Arena(that.Arena), Core(that.Core), HashIsCached(that.HashIsCached), CachedHash(that.CachedHash) {}

    inline TKey::TKey(TKey &&that) : Arena(that.Arena), Core(that.Core), HashIsCached(that.HashIsCached), CachedHash(that.CachedHash) {}

    inline TKey &TKey::operator=(const TKey &that) {
      assert(this);
      assert(&that);
      Arena = that.Arena;
      Core = that.Core;
      HashIsCached = that.HashIsCached;
      CachedHash = that.CachedHash;
      return *this;
    }

    inline TKey &TKey::operator=(TKey &&that) {
      assert(this);
      assert(&that);
      std::swap(Arena, that.Arena);
      std::swap(Core, that.Core);
      std::swap(HashIsCached, that.HashIsCached);
      std::swap(CachedHash, that.CachedHash);
      return *this;
    }

    inline bool TKey::EqEq(const Atom::TCore &lhs, Atom::TCore::TArena *lhs_arena, const Atom::TCore &rhs, Atom::TCore::TArena *rhs_arena) {
      assert(&lhs);
      assert(&rhs);
      size_t lhs_hash, rhs_hash;
      if (lhs.TryGetQuickHash(lhs_hash) && rhs.TryGetQuickHash(rhs_hash)) {
        return lhs_hash == rhs_hash && Atom::IsEq(TKey::Compare(lhs, lhs_arena, rhs, rhs_arena));
      }
      return Atom::IsEq(TKey::Compare(lhs, lhs_arena, rhs, rhs_arena));
    }

    inline bool TKey::TupleEqEq(const Atom::TCore &lhs, Atom::TCore::TArena *lhs_arena, const Atom::TCore &rhs, Atom::TCore::TArena *rhs_arena) {
      assert(&lhs);
      assert(&rhs);
      return lhs.ForceGetIndirectHash() == rhs.ForceGetIndirectHash() && Atom::IsEq(TKey::Compare(lhs, lhs_arena, rhs, rhs_arena));
    }

    inline bool TKey::operator==(const TKey &that) const {
      return EqEq(Core, Arena, that.Core, that.Arena);
    }

    inline bool TKey::NeEq(const Atom::TCore &lhs, Atom::TCore::TArena *lhs_arena, const Atom::TCore &rhs, Atom::TCore::TArena *rhs_arena) {
      assert(&lhs);
      assert(&rhs);
      size_t lhs_hash, rhs_hash;
      return (lhs.TryGetQuickHash(lhs_hash) && rhs.TryGetQuickHash(rhs_hash) && (lhs_hash != rhs_hash)) || Atom::IsNe(TKey::Compare(lhs, lhs_arena, rhs, rhs_arena));
    }

    inline bool TKey::TupleNeEq(const Atom::TCore &lhs, Atom::TCore::TArena *lhs_arena, const Atom::TCore &rhs, Atom::TCore::TArena *rhs_arena) {
      assert(&lhs);
      assert(&rhs);
      return (lhs.ForceGetIndirectHash() != rhs.ForceGetIndirectHash()) ||
              Atom::IsNe(TKey::Compare(lhs, lhs_arena, rhs, rhs_arena));
    }

    inline bool TKey::operator!=(const TKey &that) const {
      return NeEq(Core, Arena, that.Core, that.Arena);
    }

    inline bool TKey::operator<(const TKey &that) const {
      assert(this);
      assert(&that);
      return Atom::IsLt(Compare(that));
    }

    inline bool TKey::operator<=(const TKey &that) const {
      assert(this);
      assert(&that);
      return Atom::IsLe(Compare(that));
    }

    inline bool TKey::operator>(const TKey &that) const {
      assert(this);
      assert(&that);
      return Atom::IsGt(Compare(that));
    }

    inline bool TKey::operator>=(const TKey &that) const {
      assert(this);
      assert(&that);
      return Atom::IsGe(Compare(that));
    }

    inline Atom::TComparison TKey::Compare(const Atom::TCore &lhs, Atom::TCore::TArena *lhs_arena, const Atom::TCore &rhs, Atom::TCore::TArena *rhs_arena) {
      assert(&lhs);
      assert(&rhs);
      Atom::TComparison comp;
      if (lhs_arena && rhs_arena && lhs.TryQuickOrderComparison(lhs_arena, rhs, rhs_arena, comp)) {
        return comp;
      }
      void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
      void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
      return Sabot::OrderStates(*Sabot::State::TAny::TWrapper(lhs.NewState(lhs_arena, lhs_state_alloc)),
                                *Sabot::State::TAny::TWrapper(rhs.NewState(rhs_arena, rhs_state_alloc)));
    }

    inline Atom::TComparison TKey::Compare(const TKey &that) const {
      return Compare(Core, Arena, that.Core, that.Arena);
    }

    inline void TKey::Dump(std::ostream &strm) const {
      assert(this);
      void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
      Sabot::State::TAny::TWrapper(Core.NewState(Arena, state_alloc))->Accept(Sabot::TStateDumper(strm));
    }

    inline Atom::TCore::TArena *TKey::GetArena() const {
      assert(this);
      return Arena;
    }

    inline const Atom::TCore &TKey::GetCore() const {
      assert(this);
      return Core;
    }

    inline Atom::TCore &TKey::GetCore() {
      assert(this);
      return Core;
    }

    inline Sabot::State::TAny *TKey::GetState(void *state_alloc) const {
      assert(this);
      return Core.NewState(Arena, state_alloc);
    }

    inline size_t TKey::GetHash() const {
      assert(this);
      assert(Arena);
      if (!HashIsCached) {
        if (!Core.TryGetQuickHash(CachedHash)) {
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          CachedHash = Sabot::GetHash(*Sabot::State::TAny::TWrapper(Core.NewState(Arena, state_alloc)));
        }
        HashIsCached = true;
      }
      return CachedHash;
    }

  }  // Indy

}  // Stig