/* <inv_con/orient.h>

   Classes for maintaining an invasive, ordered, double-linked ordered_list.

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

namespace InvCon {

  /* An orientation.  Used when inserting into an unordered list and when
     constructing a cursor over a collection. */
  enum TOrient {

    /* Forward, that is, toward the tail of the collection.
       This is the default. */
    Fwd,

    /* Reverse, that is, toward the head of the collection. */
    Rev

  };  // TOrient

}  // InvCon
