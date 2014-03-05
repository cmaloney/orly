/* <base/unique_ident.h>

   A macro for defining unique identifiers.

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

/* Use PASTE() to define an identifier that is made of two strings pasted together.
   PASTE_HELPER() exists only to facilitate PASTE().  You shouldn't need to invoke
   it directly. */
#define PASTE_HELPER(lhs, rhs) lhs##rhs
#define PASTE(lhs, rhs) PASTE_HELPER(lhs, rhs)

/* Creates a unique identifier by pasting the given prefix to unique number. */
#define UNIQUE_IDENT(prefix) PASTE(prefix, __COUNTER__)
