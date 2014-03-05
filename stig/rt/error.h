/* <stig/rt/error.h>

   The 'error' runtime value.

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

#include <string>

namespace Stig {

  namespace Rt {

    template <typename TVal>
    class TError {
      public:

      /* TODO */
      TError(const char *err) : Err(err) {
        SetHash();
      }

      /* TODO */
      TError(const std::string &err) : Err(err) {
        SetHash();
      }

      /* TODO */
      TError(TError<TVal> &&that) : Hash(0) {
        assert(&that);
        std::swap(Err, that.Err);
        std::swap(Hash, that.Hash);
      }

      /* TODO */
      TError(const TError<TVal> &that) {
        assert(&that);
        Err = that.Err;
        Hash = that.Hash;
      }

      /* TODO */
      ~TError() {}

      /* TODO */
      TError &operator=(TError &&that) {
        assert(this);
        assert(&that);
        std::swap(Err, that.Err);
        std::swap(Hash, that.Hash);
        return *this;
      }

      /* TODO */
      TError &operator=(const TError &that) {
        assert(this);
        assert(&that);
        return *this = TVar(that);
      }

      /* TODO */
      bool operator==(const TError &that) const {
        assert(this);
        assert(&that);
        return Hash == that.Hash && Err == that.Err;
      }

      /* TODO */
      bool operator!=(const TError &that) const {
        assert(this);
        assert(&that);
        return Hash != that.Hash || Err != that.Err;
      }

      /* TODO */
      virtual size_t GetHash() const {
        assert(this);
        return Hash;
      }

      /* TODO */
      const std::string &GetErr() const {
        assert(this);
        return Err;
      }

      private:

      /* TODO */
      void SetHash() {
        assert(this);
        Hash = std::hash<std::string>()(Err);
      }

      /* TODO */
      std::string Err;

      /* TODO */
      size_t Hash;

    };  // TError

  }  // Rt

}  // Stig

namespace std {

  /* A standard hasher for Stig::Rt::TError<TVal>. */
  template <typename TVal>
  struct hash<Stig::Rt::TError<TVal>> {

    typedef size_t result_type;
    typedef Stig::Rt::TError<TVal> argument_type;

    result_type operator()(const argument_type &that) const {
      return that.GetHash();
    }

  };  // hash<Stig::Rt::TError<TVal>>

}  // std
