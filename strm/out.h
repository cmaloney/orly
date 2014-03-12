/* <strm/out.h>

   Base classes for the producers and consumers of out-flowing data.

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

#include <base/no_copy_semantics.h>

namespace Strm {

  namespace Out {

    /* So that TCons can be our friend. */
    class TProd;

    /* A consumer of out-flowing data. */
    class TCons {
      NO_COPY_SEMANTICS(TCons);
      protected:

      /* Start with no producer attached. */
      TCons() noexcept;

      /* It is not legal to destroy a consumer while it still has a producer
         attached.  Destroy the producer first. */
      virtual ~TCons();

      /* Called by our producer when it wants to get a new workspace and/or give us back the previous workspace with
         data in it for us to consume.

         If 'cursor' is non-null then
            cursor points into the workspace we most recently gave to the producer
            cursor marks the limit of the data we must now consume.
            If 'cursor' is at the start of the workspace we
              don't have to consume anything.

         If 'start' and 'limit' are non-null, then
            The producer wants a new workspace into which to begin producing data.
            We must set these such that 'start' is non-null and less-than 'limit'.

         We are guaranteed that the producer will call Cycle() again in the future, giving us back this workspace,
         possibly with data in it.

         If we cannot consume the data or if we cannot provide a workspace,
            we must throw.

         NOTE: It is acceptible (possibly desirable) for the consumer to reuse workspaces, as long as it has handled the
         data previously contained. */
      virtual void Cycle(char *cursor, char **start, char **limit) = 0;

      private:

      /* The producer currently attached to us.  If null, we don't currently
         have a producer. */
      TProd *Prod;

      /* For Cycle(). */
      friend class TProd;

    };  // TCons

    /* A producer of out-flowing data. */
    class TProd {
      NO_COPY_SEMANTICS(TProd);
      public:

      /* Abandon any data pending in our current workspace and return the
         workspace to the consumer.  If you call this function and then
         destroy the producer without any intervening calls to Write(), the
         producer's destructor is guaranteed not to throw.  NOTE: If we do not
         currently have a workspace, this function does nothing. */
      void Abandon() noexcept;

      /* Flush any data pending data to our consumer and return our workspace
         to our consumer.  If this function completes successfully and you then
         destroy the producer without any intervening calls to Write(), the
         producer's destructor is guaranteed not to throw.  NOTE: If we do not
         currently have a workspace, or our workspace contains no pending data,
         this function does nothing. */
      void Flush();

      protected:

      /* Attach to the given consumer, which must be non-null.  The consumer
         must not already have a producer attached. */
      TProd(TCons *cons) noexcept;

      /* Flush any data pending data to our consumer, return our workspace to
         our consumer, and detach from our consumer.  NOTE: If there is
         NOTE: If there is pending data and the consumer throws while trying
         to consume it, this destructor will abort the program.  If you want
         to be abort-proof, call Flush() manually and deal with any exception
         (possibly by calling Abandon()) before destroying your producer. */
      virtual ~TProd();

      /* Write the given bytes to our workspace, flushing to our consumer as
         necessary.  If we don't have a workspace, we'll get one now. */
      void Write(const void *data, size_t size);

      private:

      /* The consumer to which we are attached.  Never null. */
      TCons *Cons;

      /* Our current workspace and our position within it.  If we don't
         have a workspace, then these are all null.  If we do, then none of
         them is null, Start < Limit, and Start <= Cursor <= Limit. */
      char *Start, *Cursor, *Limit;

    };  // TProd

  }  // Out

}  // Strm
