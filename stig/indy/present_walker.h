/* <stig/indy/present_walker.h>

   Walk the keys in a given line and, for each which exists or is tombstoned, return its most recent op.

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

#include <unordered_set>

#include <base/no_copy_semantics.h>
#include <base/no_throw.h>
#include <base/uuid.h>
#include <stig/atom/kit2.h>
#include <stig/indy/fiber/fiber.h>
#include <stig/indy/sequence_number.h>

namespace Stig {

  namespace Indy {

    /* Walk the keys in a given line and, for each which exists or is tombstoned, return its most recent op.
       Ignore any updates which occur on or after a given limiting sequence number. */
    class TPresentWalker {
      NO_COPY_SEMANTICS(TPresentWalker);
      public:

      /* An item returned by a walker, describing the relevant portions an update which was previously pushed to the repo. */
      class TItem {
        public:

        /* Do-little. */
        TItem() : SequenceNumber(0UL), KeyArena(nullptr), OpArena(nullptr) {}

        /* TODO */
        bool operator<(const TItem &that) const {
          assert(this);
          Atom::TComparison comp;
          if (KeyArena && that.KeyArena && Key.TryQuickOrderComparison(KeyArena, that.Key, that.KeyArena, comp)) {
          } else {
            void *lhs_state_alloc = alloca(Sabot::State::GetMaxStateSize() * 2);
            void *rhs_state_alloc = reinterpret_cast<uint8_t *>(lhs_state_alloc) + Sabot::State::GetMaxStateSize();
            comp = Sabot::OrderStates(*Sabot::State::TAny::TWrapper(Key.NewState(KeyArena, lhs_state_alloc)),
                                      *Sabot::State::TAny::TWrapper(that.Key.NewState(that.KeyArena, rhs_state_alloc)));
          }
          return Atom::IsLt(comp) || (Atom::IsEq(comp) && SequenceNumber >= that.SequenceNumber);
        }

        /* The sequence number of the update which contained the key-op pair. */
        TSequenceNumber SequenceNumber;

        /* The key being operated on. */
        Atom::TCore Key;

        /* The operation stored for the key. */
        Atom::TCore Op;

        /* The arena to look the key core up in. */
        Atom::TCore::TArena *KeyArena;

        /* The arena to look the op core up in. */
        Atom::TCore::TArena *OpArena;

      };  // TItem

      /* True iff. we have an item. */
      virtual operator bool() const = 0;

      /* The current item. */
      virtual const TItem &operator*() const = 0;

      /* Walk to the next item, if any. */
      virtual TPresentWalker &operator++() = 0;

      /* TODO */
      virtual ~TPresentWalker() {}

      protected:

      enum TSearchKind {
        Match,
        Range
      };

      /* Prepare to walk. */
      TPresentWalker(TSearchKind search_kind) : SearchKind(search_kind) {}

      /* TODO */
      TSearchKind SearchKind;

    };  // TPresentWalker

  }  // Indy

}  // Stig
