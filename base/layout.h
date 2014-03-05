/* <base/layout.h>

   Attributes to control the layout of data structures.

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

/* This attribute, attached to a struct, class, or union, specifies a minimum alignment, in bytes, for instances of that type.
   The ISO standard requires that the alignment of any given struct or union type to be at least a perfect multiple of the lowest common
   multiple of the alignments of all of the members of the struct or union in question.  This attribute can increase that alignment, but cannot
   decrease it.  To decrease alignment, use PACKED. */
#define ALIGNED(size) __attribute__((__aligned__(size)))

/* This attribute, attached to a struct, class, or union, specifies that the type's alignment be the maximum useful for the host machine.  This can
   often make copy operations more efficient. */
#define ALIGNED_MAX __attribute__((__aligned__))

/* This attribute, attached to struct, class, or union type definition, specifies that each member (other than zero-width bitfields) of the
   aggregate must be placed to minimize the memory required.  When attached to an enum definition, it indicates that the smallest integral type
   should be used.

   For example:

      enum PACKED TFoo { A, B, C };

      struct PACKED TBar {
        TFoo X;
        uint32_t Y;
      };

      static_assert(sizeof(TFoo) == 1);
      static_assert(sizeof(TBar) == 5);

   Note that you will pay a performance price when accessing TBar::Y, as it is out of alignment. */
#define PACKED __attribute__((__packed__))
