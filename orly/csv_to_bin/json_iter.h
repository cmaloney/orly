/* <orly/csv_to_bin/json_iter.h>

   An input stream of JSON objects.

   Copyright 2010-2014 OrlyAtomics, Inc.

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

#include <base/json.h>

namespace Orly {

  namespace CsvToBin {

    /* An input consumer that translates CSV level-1 syntax. */
    class TJsonIter final {
      public:

      /* Borrow the definition of a JSON object from Base. */
      using TJson = Base::TJson;

      /* Parses from the given input stream. */
      explicit TJsonIter(std::istream &strm)
          : Strm(strm), IsCached(false) {}

      /* True if we have not yet reached the end of the input. */
      operator bool() const {
        assert(this);
        TryRefresh();
        return IsCached;
      }

      /* Our cached state. */
      const TJson &operator*() const {
        assert(this);
        Refresh();
        return Cache;
      }

      /* Our cached state. */
      const TJson *operator->() const {
        assert(this);
        Refresh();
        return &Cache;
      }

      /* Dump our cached state and advance. */
      TJsonIter &operator++() {
        assert(this);
        Refresh();
        IsCached = false;
        return *this;
      }

      private:

      /* If our cache is fresh, do nothing; otherwise, read the next JSON
         object.  We must not be at the end of the input stream. */
      void Refresh() const {
        assert(this);
        TryRefresh();
        assert(IsCached);
      }

      /* If our cache is fresh, do nothing; otherwise, read the next JSON
         object, if there is one. */
      void TryRefresh() const {
        assert(this);
        if (!IsCached) {
          if (!ws(Strm).eof()) {
            Strm >> Cache;
            IsCached = true;
          }
        }
      }

      /* The input stream from which we parse. */
      std::istream &Strm;

      /* See TCache.  Valid only if IsCached is true. */
      mutable TJson Cache;

      /* If true, then Cache contains valid data. */
      mutable bool IsCached;

    };  // TJsonIter

  }  // Csv2Bin

}  // Orly
