/* <stig/spa/flux_capacitor/kv.h>

   A value type representing an KV in the stored container.

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

#include <cassert>

#include <stig/var/impl.h>
#include <stig/var/mutation.h>

namespace Stig {

  namespace Spa {

    namespace FluxCapacitor {

      //TODO: Is this class actually needed anymore? It doesn't seem to add anything to Var.
      /* TODO */
      class TKV {
        public:

        typedef Var::TPtr<Var::TChange> TOp;

        /* Comparator for key ordering. */
        class TkComparator {
          public:

          /* Compare. */
          bool operator()(const TKV &lhs, const TKV &rhs) const {
            assert(&lhs);
            assert(&rhs);
            int result = lhs.Key.Compare(rhs.Key);
            return result < 0;
          }

        };  // TkComparator

        /* TODO */
        TKV() {
          SetHash();
        }

        /* TODO */
        TKV(const TKV &that) : Key(that.Key), IndexId(that.IndexId), Hash(that.Hash) {
        }

        /* TODO */
        TKV(TKV &&that) : Key(std::move(that.Key)), IndexId(std::move(that.IndexId)), Hash(that.Hash) {
          that.SetHash();
        }

        /* TODO */
        TKV(const Var::TVar &key, const Base::TUuid &index_id) : Key(key), IndexId(index_id) {
          SetHash();
        }

        /* TODO */
        TKV(const Var::TVar &&key, const Base::TUuid &&index_id) : Key(std::move(key)), IndexId(std::move(index_id)) {
          SetHash();
        }

        /* TODO */
        TKV &operator=(TKV &&that) {
          assert(this);
          assert(&that);
          std::swap(Key, that.Key);
          std::swap(IndexId, that.IndexId);
          std::swap(Hash, that.Hash);
          return *this;
        }

        /* TODO */
        TKV &operator=(const TKV &that) {
          assert(this);
          assert(&that);
          if (this != &that) {
            Key = that.Key;
            IndexId = that.IndexId;
            Hash = that.Hash;
          }
          return *this;
        }

        /* True iff. both fields are equal. */
        bool operator==(const TKV &that) const {
          assert(this);
          return Hash == that.Hash && Key == that.Key && IndexId == that.IndexId;
        }

        /* True iff. either field is unequal. */
        bool operator!=(const TKV &that) const {
          assert(this);
          return !(*this == that);
        }

        /* TODO */
        bool operator<(const TKV &that) const {
          assert(this);
          return Key < that.Key;
        }

        bool operator<=(const TKV &that) const {
          assert(this);
          return Key <= that.Key;
        }

        /* TODO */
        size_t GetHash() const {
          assert(this);
          return Hash;
        }

        /* The TVar which is our key. */
        const Var::TVar &GetKey() const {
          assert(this);
          return Key;
        }

        const Base::TUuid &GetIndexId() const {
          assert(this);
          return IndexId;
        }

        /* TODO */
        TKV &Reset() {
          assert(this);
          return *this = TKV();
        }

        private:

        /* TODO */
        void SetHash() {
          assert(this);
          Hash = Key.GetHash();
        }

        /* See accessor. */
        Var::TVar Key;

        /* TODO */
        Base::TUuid IndexId;

        /* See accessor. */
        size_t Hash;

      };  // TKV


    }  // FluxCapacitor

  }  // Spa

}  // Stig

namespace std {

  /* A standard hasher for Stig::Spa::FluxCapacitor::TKV. */
  template <>
  struct hash<Stig::Spa::FluxCapacitor::TKV> {

    typedef size_t result_type;
    typedef Stig::Spa::FluxCapacitor::TKV argument_type;

    result_type operator()(const argument_type &that) const {
      return that.GetHash();
    }

  };  // hash<Stig::Spa::FluxCapacitor::TKV>

}  // std
