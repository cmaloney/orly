/* <inv_con/cursor.h>

   A cursor over an invasive collection.

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

#include <base/no_default_case.h>
#include <inv_con/orient.h>

namespace InvCon {

  namespace Impl {

    /* A cursor over an invasive collection. */
    template <typename TCollector, typename TCollection, typename TMember, typename TMembership>
    class TCursor {
      public:

      /* Starts at the given membership, if any. */
      TCursor(TMembership *membership = 0, TOrient orient = Fwd)
          : Orient(orient), Membership(membership) {}

      /* Start at the first (orient = Fwd) or last (orient = Rev) member of the given collection. */
      TCursor(TCollection *collection, TOrient orient = Fwd)
          : Orient(orient) {
        assert(collection);
        switch (orient) {
          case Fwd: {
            Membership = collection->TryGetFirstMembership();
            break;
          }
          case Rev: {
            Membership = collection->TryGetLastMembership();
            break;
          }
          NO_DEFAULT_CASE;
        }
      }

      /* True iff. we are currently pointing to a membership. */
      operator bool() const {
        assert(this);
        return Membership != 0;
      }

      /* Move in the direction of our orientation.
         Calling this function when we're not currently pointing at a membership is an error. */
      TCursor &operator++() {
        assert(this);
        assert(Membership);
        switch (Orient) {
          case Fwd: {
            Membership = Membership->TryGetNextMembership();
            break;
          }
          case Rev: {
            Membership = Membership->TryGetPrevMembership();
            break;
          }
          NO_DEFAULT_CASE;
        }
        return *this;
      }

      /* Move in the opposite direction of our orientation.
         Calling this function when we're not currently pointing at a membership is an error. */
      TCursor &operator--() {
        assert(this);
        assert(Membership);
        switch (Orient) {
          case Fwd: {
            Membership = Membership->TryGetPrevMembership();
            break;
          }
          case Rev: {
            Membership = Membership->TryGetNextMembership();
            break;
          }
          NO_DEFAULT_CASE;
        }
        return *this;
      }

      /* The member at which we're currently pointing.
         Calling this function when we're not currently pointing at a membership is an error. */
      TMember &operator*() const {
        assert(this);
        assert(Membership);
        return *(Membership->GetMember());
      }

      /* The member at which we're currently pointing.
         Calling this function when we're not currently pointing at a membership is an error. */
      TMember *operator->() const {
        assert(this);
        assert(Membership);
        return Membership->GetMember();
      }

      /* The membership at which we're currently pointing.
         Calling this function when we're not currently pointing at a membership is an error. */
      TMembership *GetMembership() const {
        assert(this);
        assert(Membership);
        return Membership;
      }

      private:

      /* Our orientation w.r.t. the collection. */
      TOrient Orient;

      /* The member at which we're currently pointing, or null if we're not so pointing. */
      TMembership *Membership;

    };  // TCursor

  }  // Impl

}  // InvCon
