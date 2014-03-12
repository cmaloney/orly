/* <strm/in.h>

   Base classes for the producers and consumers of in-flowing data.

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
#include <cstddef>
#include <stdexcept>

#include <base/no_copy_semantics.h>
#include <strm/past_end.h>

namespace Strm {

  namespace In {

    /* So that TProd can be our friend. */
    class TCons;

    /* A producer of in-flowing data. */
    class TProd {
      NO_COPY_SEMANTICS(TProd);
      protected:

      /* Start with no consumer attached. */
      TProd() noexcept;

      /* It is not legal to destroy a producer while it still has a consumer
         attached.  Destroy the consumer first. */
      virtual ~TProd();

      /* Called by our consumer when it wants to get a new workspace and/or
         give us back a previous workspace.  The 'release_count' indicates the
         number of workspaces, starting with the oldest, that the consumer is
         now done with.  We are free to reclaim these and the consumer will
         make no further mention of them.  If 'start' and 'limit' are non-null,
         then consumer wants a new workspace filled with data.  We must try to
         get that data and set 'start' 'limit' to point to it, or throw.  If
         we don't throw, we must ensure that we set the pointers non-null,
         with 'start' < 'limit', and return true.  If know there is no more
         data to get (meaning we're not an open-ended source of data and we
         know we have reached the end), we may ignore 'start' and 'limit' and
         just return false.  After that, the consumer will never ask for a new
         workspace again and any further calls to Cycle() would only be to
         release old workspaces.  If we do return true, we are guaranteed that
         the producer will call Cycle() again in the future, giving us back
         this workspace so we may reclaim it.  If 'start' and 'limit' are null,
         then the consumer doesn't want any additional data and we should just
         return false, which causes the client to never call back again.  In
         this case, we should definitely not throw. */
      virtual bool Cycle(
          size_t release_count, const char **start, const char **limit) = 0;

      private:

      /* The consumer currently attached to us.  If null, we don't currently
         have a consumer. */
      TCons *Cons;

      /* For Cycle(). */
      friend class TCons;

    };  // TProd

    /* A consumer of in-flowing data. */
    class TCons {
      NO_COPY_SEMANTICS(TCons);
      public:

      /* True iff. there is data to be consumed.  If the producer is open-
         ended (such as a socket), this will always be true. */
      operator bool() const {
        assert(this);
        TryRefresh();
        return !AtEnd;
      }

      protected:

      /* Attach to the given producer, which must be non-null.  The producer
         must not already have a consumer attached. */
      TCons(TProd *prod) noexcept;

      /* Returns any remaining workspaces to the producer and detaches from
         it. */
      virtual ~TCons();

      /* Peeks at the next byte of data. */
      char Peek() const {
        assert(this);
        Refresh();
        return *Cursor;
      }

      /* Peeks at the next bytes of data. */
      void Peek(const char *&start, const char *&limit) const {
        assert(this);
        assert(&start);
        assert(&limit);
        Refresh();
        start = Cursor;
        limit = Limit;
      }

      /* Pops the next byte of data. */
      char Pop() {
        assert(this);
        Refresh();
        return *Cursor++;
      }

      /* Reads the next bytes of data. */
      void Read(void *data, size_t size);

      /* Skips the next byte of data. */
      void Skip() {
        assert(this);
        Refresh();
        Cursor++;
      }

      /* Skips the next bytes of data. */
      void Skip(size_t size);

      private:

      /* Makes sure that our Cursor is positioned at at least one byte of
         readable data.  If our producer is open-ended, this will always
         succeed.  If not, and we have reached the end of our producer's data,
         we throw TPastEnd. */
      void Refresh() const {
        assert(this);
        TryRefresh();
        if (Cursor >= Limit) {
          throw TPastEnd();
        }
      }

      /* Tries to make sure our Cursor is positioned at at least one byte of
         readable data.  If our producer is open-ended, this will always
         succeed.  If not, and we have reached the end of our producer's data,
         we will note that we are at the end but not throw. */
      void TryRefresh() const;

      /* The producer to which we are attached.  Never null. */
      TProd *Prod;

      /* Our current workspace and our position within it.  If we don't
         have a workspace, then these are all null.  If we do, then none of
         them is null, Start < Limit, and Start <= Cursor <= Limit. */
      mutable const char *Start, *Cursor, *Limit;

      /* Initially false, but becomes true iff. our producer tells us it is
         out of data and we have consumed it all. */
      mutable bool AtEnd;

    };  // TCons

  }  // In

}  // Strm
