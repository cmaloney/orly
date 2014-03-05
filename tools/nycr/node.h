/* <tools/nycr/node.h>

   The base class for all CST nodes.

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

namespace Tools {

  namespace Nycr {

    /* TODO */
    class TNode {
      NO_COPY_SEMANTICS(TNode);
      public:

      /* TODO */
      virtual ~TNode();

      /* TODO */
      static void DeleteEach();

      protected:

      /* TODO */
      TNode();

      private:

      /* TODO */
      TNode *Next, *Prev;

      /* TODO */
      static TNode *First, *Last;

    };  // TNode

  }  // Nycr

}  // Tools
