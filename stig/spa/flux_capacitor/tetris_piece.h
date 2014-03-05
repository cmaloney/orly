/* <stig/spa/flux_capacitor/tetris_piece.h>

   A moderately brain-dead Tetris player.

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

#include <base/no_copy_semantics.h>

#include <cassert>
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include <stig/var.h>

namespace Stig {

  namespace Spa {

    namespace FluxCapacitor {

      /* TODO */
      class TContext;

      /* TODO */
      class TTetrisPiece {
        NO_COPY_SEMANTICS(TTetrisPiece);
        public:

        /* TODO */
        typedef Var::TVar TKey;

        /* TODO */
        virtual bool Assert(TContext &ctxt) const = 0;

        /* TODO */
        virtual void Fail() = 0;

        /* TODO */
        virtual bool ForEachKey(const std::function<bool (const TKey &)> &cb) const = 0;

        /* TODO */
        size_t GetAge() const {
          assert(this);
          return Age;
        }

        /* TODO */
        virtual size_t GetKeyCount() const = 0;

        /* TODO */
        bool IncrAge() {
          assert(this);
          return !MaxAge || (++Age <= MaxAge);
        }

        /* TODO */
        virtual bool Promote() = 0;

        /* TODO */
        static void PlayTetris(TContext &ctxt, const std::function<void (const std::function<void (TTetrisPiece *)> &)> &piece_generator);

        protected:

        /* TODO */
        TTetrisPiece(size_t max_age = 0)
            : Age(0), MaxAge(max_age) {}

        /* TODO */
        virtual ~TTetrisPiece();

        /* TODO */
        void ResetAge() {
          assert(this);
          Age = 0;
        }

        private:

        /* TODO */
        size_t Age, MaxAge;

      };  // TTetrisPiece

    }  // FluxCapacitor

  }  // Spa

}  // Stig
