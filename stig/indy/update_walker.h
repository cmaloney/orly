/* <stig/indy/update_walker.h>

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

#include <base/no_copy_semantics.h>
#include <stig/atom/kit2.h>
#include <stig/indy/sequence_number.h>

namespace Stig {

  namespace Indy {

    /* TODO */
    class TUpdateWalker {
      NO_COPY_SEMANTICS(TUpdateWalker);
      public:

      /* TODO */
      class TItem {
        public:

        /* TODO */
        TSequenceNumber SequenceNumber;

        /* TODO */
        Atom::TCore Metadata;

        /* TODO */
        Atom::TCore Id;

        /* TODO */
        std::vector<std::pair<TIndexKey, Atom::TCore>> EntryVec;

        /* TODO */
        Atom::TCore::TArena *MainArena;

      };  // TItem

      /* True iff. we have an item. */
      virtual operator bool() const = 0;

      /* The current item. */
      virtual const TItem &operator*() const = 0;

      /* Walk to the next item, if any. */
      virtual TUpdateWalker &operator++() = 0;

      /* TODO */
      virtual ~TUpdateWalker() {}

      protected:

      /* Prepare to walk. */
      TUpdateWalker() {}

    };  // TUpdateWalker

  }  // Indy

}  // Stig
