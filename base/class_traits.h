/* <base/class_traits.h>

   Defines numerous macros which should be used to manage the general behaviors
  of a class.

   This is achieved by defaulting/deleting/defining constructors and member
  functions of the type in accordance with the
   general rules for dealing with a particular class of types.

   Most of these are fairly simple / straight forward. We provide these aliases
  to make it easy for someone glancing at
   the code to tell what the overarching behavior of a class is
  (NO_CONSTRUCTION, NO_COPY, etc.)

  TODO: Generic blurb about how to insert/use these in classes (Just after the '{')

   Copyright 2015 Theoretical Chaos.

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

/* Forces the compiler to emit the default move constructor*/
#define DEFAULT_MOVE(cls) \
  cls(cls &&) = default;  \
  cls &operator=(cls &&) = default;

/* Forces the compiler to emit the default copy constructor*/
#define DEFAULT_COPY(cls)     \
  cls(const cls &) = default; \
  cls &operator=(const cls &) = default;

/* Use this macro to make a class copyable only / not movable */
#define COPY_ONLY(cls) \
  NO_MOVE(cls)         \
  DEFAULT_COPY(cls)

/* Use this macro to make a class movable only / not copyable */
#define MOVE_ONLY(cls) \
  NO_COPY(cls)         \
  DEFAULT_MOVE(cls)

/* Use this macro to disable construction of a class, struct, or union,
   essentially turning the type into a namespace with accessibility
   controls.  This is particularly useful for creating namespace-like
   separation of declarations within classes.  For example:

      class TFoo {
        public:

        struct Alpha {
          NO_CONSTRUCTION(Alpha);
          static CallMe();
        };

        struct Beta {
          NO_CONSTRUCTION(Beta);
          static CallMe();
        };

      };

   A caller can now call TFoo::Alpha:CallMe() or TFoo::Beta::CallMe() without
   ambiguity.  Note that it is convention *not* to use the T-prefix with types
   without construction.  This makes them read more like namespaces, which they
   essentially are.

   All the members of a type without construction must obviously be static.  A
   per-instance member makes no sense in a type that will never be instantiated.

   If you attempt to default-construct, destruct, copy-construct or assign an
   instance of a type declared with this macro, you'll get a compile-time error
   telling you function is private.  If the attempt is made from within a scope
   friendly to the type, you'll get a link-time error telling you the function
   is undefined. */
#define NO_CONSTRUCTION(cls) \
  cls() = delete;            \
  ~cls() = delete;           \
  cls(const cls &) = delete; \
  cls &operator=(const cls &) = delete;

/* Use this macro to disable compiler-provided copy semantics in any
   class you intend to operate as a reference type.  Convention is to
   have this macro appear as the first thing in the class, before the
   first accessability declaration, like this:

      class TFoo {
        NO_COPY(TFoo);
        public:
        ...
      }

   If you attempt to copy-construct or assign an instance of a class
   declared with this macro, you'll get a compile-time error telling you
   that the copy constructor or assignment operator is private.  If the
   attempt is made from within a scope friendly to the class, you'll get
   a link-time error telling you the copy constructor or assignment
   operator is undefined. */
#define NO_COPY(cls)         \
  cls(const cls &) = delete; \
  cls &operator=(const cls &) = delete;

/* Use this macro to disable the compiler-provided move semantics in any class. */
#define NO_MOVE(cls)    \
  cls(cls &&) = delete; \
  cls &operator=(const cls &&) = delete;
